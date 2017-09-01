#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TStyle.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TLegend.h>
#include <TPaveText.h>
#include <TGraphErrors.h>
#include <TList.h>
#include <TCollection.h>
#include <TObject.h>

#include <iostream>
#include <vector>
#include <sstream>
#include <map>
#include <fstream>
#include <stdexcept>

#include "UserCode/Stop4Body/interface/json.hpp"
#include "UserCode/Stop4Body/interface/SampleReader.h"
#include "UserCode/Stop4Body/interface/VariableReader.h"

using json = nlohmann::json;

class CutInfo
{
public:
  CutInfo(std::string name, std::string cut, std::string latex): name_(name), cut_(cut), latex_(latex) {}

  std::string name()  const {return name_;}
  std::string cut()   const {return cut_;}
  std::string latex() const {return latex_;}

private:
protected:
  std::string name_;
  std::string cut_;
  std::string latex_;
};


int main(int argc, char** argv)
{
  std::string jsonFileName = "";
  std::string inputDirectory = "";
  std::string outputDirectory = "./plots/";
  std::string suffix = "";
  std::string variablesJson = "";
  std::string cutsJson = "";
  double luminosity = -1.0;
  bool verbose = false;

  if(argc < 2)
  {
    std::cout << "You did not pass enough parameters" << std::endl;
    //printHelp();
    return 0;
  }

  for(int i = 1; i < argc; ++i)
  {
    std::string argument = argv[i];

    if(argument == "--help")
    {
      //printHelp();
      return 0;
    }

    if(argument == "--json")
      jsonFileName = argv[++i];

    if(argument == "--outDir")
      outputDirectory = argv[++i];

    if(argument == "--inDir")
      inputDirectory = argv[++i];

    if(argument == "--suffix")
      suffix = argv[++i];

    if(argument == "--variables")
      variablesJson = argv[++i];

    if(argument == "--cuts")
      cutsJson = argv[++i];

    if(argument == "--lumi")
    {
      std::stringstream convert;
      convert << argv[++i];
      convert >> luminosity;
    }

    if(argument == "--verbose")
    {
      verbose = true;
    }
  }

  if(jsonFileName == "")
  {
    std::cout << "You must define a json file" << std::endl;
    return 1;
  }

  if(variablesJson == "")
  {
    std::cout << "You must define a json file with the variables to plot" << std::endl;
    return 1;
  }

  if(cutsJson == "")
  {
    std::cout << "You must define a json file with the cuts" << std::endl;
    return 1;
  }

  if(inputDirectory == "")
  {
    std::cout << "You must define an input directory" << std::endl;
    return 1;
  }


  gStyle->SetOptStat(000000);
  gStyle->SetOptTitle(0);

  std::cout << "Reading json files" << std::endl;
  VariableJsonLoader variables(variablesJson);
  SampleReader samples(jsonFileName, inputDirectory, suffix);

  auto MC = samples.getMCBkg();
  auto Sig = samples.getMCSig();
  auto Data = samples.getData();

  if(luminosity <= 0)
    luminosity = Data.getLumi();

  std::string mcWeight = "splitFactor*weight";
  std::cout << "Using mcWeight: " << mcWeight << std::endl;

  std::vector<CutInfo> cutFlow;
  {
    json jsonFile;
    std::ifstream inputFile(cutsJson);
    inputFile >> jsonFile;

    if(jsonFile.count("cuts") == 0)
      throw MissingJSONParam("The JSON file does not contain the 'cuts' entry. It is not a valid file.");

    for(auto& cut : jsonFile["cuts"])
    {
      try
      {
        std::string cutName, cutString, cutLatex;

        if(cut.count("name") == 0)
          throw MissingJSONParam("The cut does not have a name.");
        cutName = cut["name"];

        if(cut.count("expression") == 0)
          throw MissingJSONParam("The cut does not have an expression.");
        cutString = cut["expression"];

        if(cut.count("latex") == 0)
        {
          cutLatex = cutName;
        }
        else
        {
          cutLatex = cut["latex"];
        }

        cutFlow.push_back(CutInfo(cutName, cutString, cutLatex));
      }
      catch(MissingJSONParam& exception)
      {
        std::cout << "Incomplete cut found, skipping it." << std::endl;
        std::cout << "The message was: " << exception.what() << std::endl;
      }
    }
  }

  std::ofstream correlationTable(outputDirectory+"/correlation.tex");
  if(verbose)
    std::cout << "This just here to avoid a warning which is treated as an error by CMSSW" << std::endl;

  std::string selection = "";
  for(auto& cut : cutFlow)
  {
    if(cut.cut() != "")
      selection = cut.cut();
    else
      continue;

    std::cout << "Getting plots with selection (" << selection << ") and weight (" << mcWeight << ")" << std::endl;

    correlationTable << "Cut: " << cut.name() << "\n";
    correlationTable << "\\begin{tabular}{";
    correlationTable << "c|";
    for(size_t i = 0; i < variables.size(); ++i)
    {
      correlationTable << "c";
    }
    correlationTable << "}\n";

    for(size_t i = 0; i < variables.size(); ++i)
    {
      correlationTable << " & " << variables.Get(i).label();
    }
    correlationTable << "\\\\\n\\hline\n";

    for(size_t i = 0; i < variables.size(); ++i)
    {
      correlationTable << variables.Get(i).label();
      for(size_t j = 0; j < variables.size(); ++j)
      {
        correlationTable << " & ";

        if(j < i)
          continue;

        double sigCorr = 0;
        double bkgCorr = 0;
        double dataCorr = 0;

        if(j == i)
        {
          sigCorr = 1;
          bkgCorr = 1;
          dataCorr = 1;
        }
        else
        {
          TCanvas c1((cut.name()+"_"+variables.Get(j).name()+"_vs_"+variables.Get(i).name()).c_str(), "", 1200, 400);
          c1.SetLogz();
          c1.Divide(3,1,0,0);

          TVirtualPad* thisPad = c1.cd(1);
          thisPad->SetLogz(true);
          thisPad->SetTopMargin(0.10);
          thisPad->SetBottomMargin(0.10);
          thisPad->SetRightMargin(0.20);

          auto sigHist = Sig.process(0).get2DHist(variables.Get(i).expression(),
                                                  variables.Get(j).expression(),
                                                  variables.Get(i).label()+";"+variables.Get(j).label()+";Evt.",
                                                  mcWeight+"*("+selection+")",
                                                  variables.Get(i).bins(),
                                                  variables.Get(i).min(),
                                                  variables.Get(i).max(),
                                                  variables.Get(j).bins(),
                                                  variables.Get(j).min(),
                                                  variables.Get(j).max());
          sigCorr = sigHist->GetCorrelationFactor();

          sigHist->SetTitle("");
          sigHist->SetStats(kFALSE);
          sigHist->Draw("COLZ");

          TPaveText* sigLeg = new TPaveText(0.10,0.995,0.90,0.90, "NDC");
          sigLeg->SetFillColor(0);
          sigLeg->SetFillStyle(0);
          sigLeg->SetLineColor(0);
          sigLeg->SetTextAlign(12);
          sigLeg->AddText(Sig.process(0).label().c_str());
          sigLeg->Draw("same");

          thisPad = c1.cd(2);
          thisPad->SetLogz(true);
          thisPad->SetTopMargin(0.10);
          thisPad->SetBottomMargin(0.10);
          thisPad->SetRightMargin(0.20);

          auto bkgHist = MC.get2DHist(variables.Get(i).expression(),
                                      variables.Get(j).expression(),
                                      variables.Get(i).label()+";"+variables.Get(j).label()+";Evt.",
                                      mcWeight+"*("+selection+")",
                                      variables.Get(i).bins(),
                                      variables.Get(i).min(),
                                      variables.Get(i).max(),
                                      variables.Get(j).bins(),
                                      variables.Get(j).min(),
                                      variables.Get(j).max());
          bkgCorr = bkgHist->GetCorrelationFactor();

          bkgHist->SetTitle("");
          bkgHist->SetStats(kFALSE);
          bkgHist->Draw("COLZ");

          TPaveText* bkgLeg = new TPaveText(0.10,0.995,0.90,0.90, "NDC");
          bkgLeg->SetFillColor(0);
          bkgLeg->SetFillStyle(0);
          bkgLeg->SetLineColor(0);
          bkgLeg->SetTextAlign(12);
          bkgLeg->AddText("Background");
          bkgLeg->Draw("same");

          thisPad = c1.cd(3);
          thisPad->SetLogz(true);
          thisPad->SetTopMargin(0.10);
          thisPad->SetBottomMargin(0.10);
          thisPad->SetRightMargin(0.20);

          auto dataHist =  Data.get2DHist(variables.Get(i).expression(),
                                          variables.Get(j).expression(),
                                          variables.Get(i).label()+";"+variables.Get(j).label()+";Evt.",
                                          "("+selection+")",
                                          variables.Get(i).bins(),
                                          variables.Get(i).min(),
                                          variables.Get(i).max(),
                                          variables.Get(j).bins(),
                                          variables.Get(j).min(),
                                          variables.Get(j).max());
          dataCorr = dataHist->GetCorrelationFactor();

          dataHist->SetTitle("");
          dataHist->SetStats(kFALSE);
          dataHist->Draw("COLZ");

          TPaveText* dataLeg = new TPaveText(0.10,0.995,0.90,0.90, "NDC");
          dataLeg->SetFillColor(0);
          dataLeg->SetFillStyle(0);
          dataLeg->SetLineColor(0);
          dataLeg->SetTextAlign(12);
          dataLeg->AddText("Data");
          dataLeg->Draw("same");

          c1.cd(0);

          std::string plotName = cut.name() + "_" + variables.Get(j).name() + "_vs_" + variables.Get(i).name();
          c1.SaveAs((outputDirectory+"/"+plotName+".png").c_str());
          c1.SaveAs((outputDirectory+"/"+plotName+".C").c_str());


          delete sigHist;
          delete bkgHist;
          delete dataHist;
        }

        correlationTable << sigCorr << "; " << bkgCorr << "; " << dataCorr;
      }

      correlationTable << "\\\\\n";
    }

    correlationTable << "\n\n\n";
  }



  return 0;
}








