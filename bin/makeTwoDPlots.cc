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
#include <TLatex.h>

#include <iostream>
#include <vector>
#include <sstream>
#include <map>
#include <fstream>
#include <stdexcept>

#include "UserCode/Stop4Body/interface/json.hpp"
#include "UserCode/Stop4Body/interface/SampleReader.h"
#include "UserCode/Stop4Body/interface/VariableReader.h"
#include "UserCode/Stop4Body/interface/commonFunctions.h"

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

void adjustStyle(TH1* hist)
{
  hist->GetYaxis()->SetTitleOffset(1.0);

  int Xbin = hist->GetXaxis()->GetNbins();
  double xsize = std::min(800.0/(Xbin+1)/180, 0.12);
  hist->GetXaxis()->SetLabelSize(xsize);

  double scale = 1.0;
  hist->GetXaxis()->SetTitleSize(0.05*scale);
  hist->GetXaxis()->SetLabelSize(0.05*scale);
  hist->GetXaxis()->SetTitleOffset(1);
  hist->GetYaxis()->SetTitleSize(0.06*scale);
  hist->GetYaxis()->SetTitleOffset(1.25);
  hist->GetYaxis()->SetLabelSize(0.06*0.8*scale);
  hist->GetYaxis()->SetLabelOffset(0.01);

  return;
}

void drawInfo(TVirtualPad* c1, double luminosity, bool drawPreliminary = false)
{
  //double H = c1->GetWh();
  //double W = c1->GetWw();
  double l = c1->GetLeftMargin();
  double t = c1->GetTopMargin();
  double r = c1->GetRightMargin();
  double b = c1->GetBottomMargin();
  //double e = 0.025;

  char Buffer[1024];
  sprintf(Buffer, "%.1f fb^{-1} (%.1f TeV)", luminosity/1000, 13.0);
  TLatex *   tex  = new TLatex();
  tex->SetNDC();
  tex->SetTextAngle(0);
  tex->SetTextColor(kBlack);
  tex->SetTextFont(42);
  tex->SetTextAlign(31);
  tex->SetTextSize(0.6 * t);
  tex->DrawLatex(1-r,1-t+0.15*t,Buffer);

  TLatex *   tex2  = new TLatex();
  tex2->SetNDC();
  tex2->SetTextAngle(0);
  tex2->SetTextColor(kBlack);
  tex2->SetTextFont(61);
  tex2->SetTextSize(1.2*t);
  tex2->SetTextAlign(13);
  double posX = l + 0.045*(1-l-r);
  double posY = 1-t - 0.035*(1-t-b);
  char cmsText[] = "CMS";
  tex2->DrawLatex(posX, posY, cmsText);
  if(drawPreliminary)
  {
    std::cout << "Preliminary not implemented yet" << std::endl;
  }

  return;
}


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
  bool final = false;
  bool plotData = false;

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

    if(argument == "--final")
    {
      final = true;
    }

    if(argument == "--plotData")
    {
      plotData = true;
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

  TStyle* tdrstyle = getTDRStyle();
  tdrstyle->cd();

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

    if(jsonFile.count("prefilter") != 0)
    {
      std::string prefilter = jsonFile["prefilter"];
      std::cout << "Filtering the trees with: " << prefilter << std::endl;
      if(prefilter == "")
      {
        MC.filter("(weight_Q2_1 < 2) && (weight_Q2_2 < 2) && (weight_Q2_3 < 2) && (weight_Q2_4 < 2) && (weight_Q2_6 < 2) && (weight_Q2_8 < 2)");
        Sig.filter("(weight_Q2_1 < 2) && (weight_Q2_2 < 2) && (weight_Q2_3 < 2) && (weight_Q2_4 < 2) && (weight_Q2_6 < 2) && (weight_Q2_8 < 2)");
      }
      else
      {
        MC.filter(prefilter + " && (weight_Q2_1 < 2) && (weight_Q2_2 < 2) && (weight_Q2_3 < 2) && (weight_Q2_4 < 2) && (weight_Q2_6 < 2) && (weight_Q2_8 < 2)");
        Sig.filter(prefilter + " && (weight_Q2_1 < 2) && (weight_Q2_2 < 2) && (weight_Q2_3 < 2) && (weight_Q2_4 < 2) && (weight_Q2_6 < 2) && (weight_Q2_8 < 2)");
        Data.filter(prefilter);
      }
      std::cout << "Done filtering the trees" << std::endl;
    }

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
          TCanvas c1((cut.name()+"_"+variables.Get(j).name()+"_vs_"+variables.Get(i).name()).c_str(), "", ((plotData)?1200:800), 400);
          c1.SetLogz();
          if(plotData)
            c1.Divide(3,1,0,0);
          else
            c1.Divide(2,1,0,0);

          c1.SetRightMargin(0.0);
          c1.SetLeftMargin(0.0);

          TVirtualPad* thisPad = c1.cd(1);
          tdrstyle->cd();
          thisPad->SetLogz(true);
          //thisPad->SetTopMargin(0.05);
          //thisPad->SetBottomMargin(0.10);
          thisPad->SetRightMargin(0.16);
          thisPad->SetLeftMargin(0.16);

          auto sigHist = Sig.process(0).get2DHist(variables.Get(i).expression(),
                                                  variables.Get(j).expression(),
                                                  variables.Get(i).label()+";"+variables.Get(j).label()+";Events",
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
          adjustStyle(sigHist);

          TPaveText* sigLeg = new TPaveText(0.16,0.995,0.90,0.955, "NDC");
          sigLeg->SetFillColor(0);
          sigLeg->SetFillStyle(0);
          sigLeg->SetLineColor(0);
          sigLeg->SetTextAlign(12);
          sigLeg->SetTextFont(42);
          sigLeg->AddText(Sig.process(0).label().c_str());
          sigLeg->Draw("same");

          drawInfo(thisPad, luminosity, !final);

          thisPad = c1.cd(2);
          tdrstyle->cd();
          thisPad->SetLogz(true);
          //thisPad->SetTopMargin(0.05);
          //thisPad->SetBottomMargin(0.10);
          thisPad->SetRightMargin(0.16);
          thisPad->SetLeftMargin(0.16);

          auto bkgHist = MC.get2DHist(variables.Get(i).expression(),
                                      variables.Get(j).expression(),
                                      variables.Get(i).label()+";"+variables.Get(j).label()+";Events",
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
          adjustStyle(bkgHist);

          TPaveText* bkgLeg = new TPaveText(0.16,0.995,0.90,0.955, "NDC");
          bkgLeg->SetFillColor(0);
          bkgLeg->SetFillStyle(0);
          bkgLeg->SetLineColor(0);
          bkgLeg->SetTextAlign(12);
          bkgLeg->SetTextFont(42);
          bkgLeg->AddText("Simulated Background");
          bkgLeg->Draw("same");

          drawInfo(thisPad, luminosity, !final);

          if(plotData)
          {
            thisPad = c1.cd(3);
            tdrstyle->cd();
            thisPad->SetLogz(true);
            //thisPad->SetTopMargin(0.05);
            //thisPad->SetBottomMargin(0.10);
            thisPad->SetRightMargin(0.16);
            thisPad->SetLeftMargin(0.16);
          }

          auto dataHist =  Data.get2DHist(variables.Get(i).expression(),
                                          variables.Get(j).expression(),
                                          variables.Get(i).label()+";"+variables.Get(j).label()+";Events",
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
          if(plotData)
          {
            dataHist->Draw("COLZ");
            adjustStyle(dataHist);

            TPaveText* dataLeg = new TPaveText(0.16,0.995,0.90,0.955, "NDC");
            dataLeg->SetFillColor(0);
            dataLeg->SetFillStyle(0);
            dataLeg->SetLineColor(0);
            dataLeg->SetTextAlign(12);
            dataLeg->SetTextFont(42);
            dataLeg->AddText("Data");
            dataLeg->Draw("same");

            drawInfo(thisPad, luminosity, !final);
          }

          c1.cd(0);

          std::string plotName = cut.name() + "_" + variables.Get(j).name() + "_vs_" + variables.Get(i).name();
          c1.SaveAs((outputDirectory+"/"+plotName+".png").c_str());
          c1.SaveAs((outputDirectory+"/"+plotName+".pdf").c_str());
          c1.SaveAs((outputDirectory+"/"+plotName+".C").c_str());


          delete sigHist;
          delete bkgHist;
          delete dataHist;
        }

        correlationTable << sigCorr << "; " << bkgCorr << "; " << dataCorr;
      }

      correlationTable << "\\\\\n" << std::flush;
    }

    correlationTable << "\n\n\n";
  }



  return 0;
}








