#include <iostream>
#include <vector>
#include <sstream>
#include <map>
#include <fstream>

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

#include "UserCode/Stop4Body/interface/json.hpp"
#include "UserCode/Stop4Body/interface/SampleReader.h"
#include "UserCode/Stop4Body/interface/doubleWithUncertainty.h"
#include "UserCode/Stop4Body/interface/ValueWithSystematics.h"

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
  bool doNonUniversality = false;

  if(argc < 2)
  {
    std::cout << "You did not pass enough parameters" << std::endl;
    //printHelp();
    return 0;
  }

  for(int i = 1; i < argc; ++i)
  {
    std::string argument = argv[i];

    /*if(argument == "--help")
    {
      printHelp();
      return 0;
    }*/

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

    /*if(argument == "--lumi")
    {
      std::stringstream convert;
      convert << argv[++i];
      convert >> luminosity;
    }*/

    if(argument == "--verbose")
    {
      verbose = true;
    }

    if(argument == "--doNonUniversality")
    {
      doNonUniversality = true;
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
  //auto Sig = samples.getMCSig();
  auto Data = samples.getData();

  if(luminosity <= 0)
    luminosity = Data.getLumi();

  std::vector<std::string> systBase;
  if(doNonUniversality)
  {
    for(int i = 1; i <= 18; ++i)
    {
      std::stringstream converter;
      std::string tmp;
      converter << "TightLoose_Muon_NU_Bin" << i;
      converter >> tmp;
      systBase.push_back(tmp);
    }
    for(int i = 1; i <= 16; ++i)
    {
      std::stringstream converter;
      std::string tmp;
      converter << "TightLoose_Electron_NU_Bin" << i;
      converter >> tmp;
      systBase.push_back(tmp);
    }
  }
  else
  {
    systBase.push_back("ISRweight_Bin1");
    systBase.push_back("ISRweight_Bin2");
    systBase.push_back("ISRweight_Bin3");
    systBase.push_back("ISRweight_Bin4");
    systBase.push_back("ISRweight_Bin5");
    systBase.push_back("ISRweight_Bin6");
    systBase.push_back("EWKISRweight_Bin1");
    systBase.push_back("EWKISRweight_Bin2");
    systBase.push_back("EWKISRweight_Bin3");
    systBase.push_back("EWKISRweight_Bin4");
    systBase.push_back("EWKISRweight_Bin5");
    systBase.push_back("EWKISRweight_Bin6");
    systBase.push_back("EWKISRweight_Bin7");
  }

  std::vector<std::string> systematics;
  for(auto& base: systBase)
  {
    systematics.push_back(base + "_Up");
    systematics.push_back(base + "_Down");
  }

  auto valueLoop = systematics;
  valueLoop.push_back("CentralValue");

  ValueWithSystematics<std::string> mcWeight = std::string("splitFactor*weight");
  for(auto& syst: systematics)
    mcWeight.Systematic(syst) = mcWeight.Value() + "_" + syst;
  {
    std::stringstream converter;
    converter << "*" << luminosity;
    mcWeight += converter.str();
  }

  std::cout << "Using mcWeight: " << mcWeight.Value() << std::endl;
  std::cout << "With variations:" << std::endl;
  for(auto& syst: systematics)
    std::cout << "  " << syst << " - " << mcWeight.Systematic(syst) << std::endl;
  std::cout << std::endl;

  std::vector<CutInfo> cutFlow;
  if(cutsJson == "")
  {
    cutFlow.push_back(CutInfo("Preselection", "HT > 200", "$H_T > 200$"));
    //cutFlow.push_back(CutInfo("JetPt110", "Jet1Pt > 110", "$p_T\\left(j_1\\right) > 110$"));
    //cutFlow.push_back(CutInfo("MET300", "Met > 300", "$MET > 300$"));
    //cutFlow.push_back(CutInfo("LepPt30", "LepPt < 30", "$p_T\\left(l\\right) < 30$"));
    cutFlow.push_back(CutInfo("Selection", "(DPhiJet1Jet2 < 2.5 || Jet2Pt < 60) && (HT > 200) && (Met > 280) && (Jet1Pt > 110) && (nGoodEl+nGoodMu <= 2)", "Selection"));
    cutFlow.push_back(CutInfo("Test", "(DPhiJet1Jet2 < 2.5 || Jet2Pt < 60) && (HT > 200) && (Met > 280) && (Jet1Pt > 110) && (nGoodEl+nGoodMu <= 2) && (Njet60 < 3)", "$N(jet_{60}) < 3$"));
  }
  else
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

  std::string selection = "";
  std::ofstream outSummary(outputDirectory+"/summary.txt", std::ios_base::binary | std::ios_base::trunc);
  for(auto& cut : cutFlow)
  {
    selection = cut.cut();

    std::cout << "Getting variables with selection (" << selection << ") and weight (" << mcWeight.Value() << ") and variations" << std::endl;

    for(auto & variable : variables)
    {
      outSummary << "Cut: " << cut.name() << std::endl;
      outSummary << "Variable: " << variable.name() << std::endl;

      if(verbose)
        std::cout << "  Doing variable: " << variable.name() << std::endl;
      std::map<std::string, TH1D*> variationHistograms;
      std::string dataSel;
      ValueWithSystematics<std::string> mcSel = std::string("");

      /*{
        std::string tmp = mcWeight.Value()+"*( ((isLoose == 1) && (isTight == 0) && (isPrompt == 1)) && "+selection+")";
        mcSel = tmp;
      }*/
      mcSel = mcWeight.Value()+"*( ((isLoose == 1) && (isTight == 0) && (isPrompt == 1)) && "+selection+")";
      for(auto& syst: mcWeight.Systematics())
        mcSel.Systematic(syst) = mcWeight.Systematic(syst)+"*( ((isLoose == 1) && (isTight == 0) && (isPrompt == 1)) && "+selection+")";
      dataSel = "weight * ( ((isLoose == 1) && (isTight == 0)) && " + selection + ")";

      for(auto& syst: valueLoop)
      {
        auto dataH = Data.process(0).getHist(cut.name(), variable, dataSel);
        auto mcH   =   MC.getHist(cut.name(), variable, mcSel.GetSystematicOrValue(syst));

        auto fakeEstimate = static_cast<TH1D*>(dataH->Clone((cut.name()+"_"+variable.name()+"_"+syst).c_str()));
        fakeEstimate->Add(mcH, -1.0);
        fakeEstimate->SetTitle((";" + variable.label() + ";fake DD").c_str());

        variationHistograms[syst] = fakeEstimate;

        TCanvas c1((cut.name()+"_"+variable.name()+"_"+syst).c_str(), "", 1200, 1350); // 800x900 in original, then scaled by 1.5
        gStyle->SetOptStat(0);

        if(variable.logx())
          c1.SetLogx();
        if(variable.logy())
          c1.SetLogy();
        c1.SetTopMargin(0.07);
        //t1->SetBottomMargin(0.10);
        //t1->SetRightMargin(0.20);
        fakeEstimate->Draw("hist");

        TPaveText* T = new TPaveText(0.1,0.995,0.84,0.95, "NDC");
        T->SetFillColor(0);
        T->SetFillStyle(0);
        T->SetLineColor(0);
        T->SetTextAlign(12);
        char Buffer[1024];
        sprintf(Buffer, "CMS preliminary, #sqrt{s}=%.1f TeV, #scale[0.5]{#int} L=%.1f fb^{-1}", 13.0, luminosity/1000);
        T->AddText(Buffer);
        T->Draw("same");
        T->SetBorderSize(0);

        c1.SaveAs((outputDirectory+"/"+cut.name()+"_"+variable.name()+"_"+syst+".png").c_str());
        c1.SaveAs((outputDirectory+"/"+cut.name()+"_"+variable.name()+"_"+syst+".C").c_str());
        c1.SaveAs((outputDirectory+"/"+cut.name()+"_"+variable.name()+"_"+syst+".root").c_str());

        delete dataH;
        delete mcH;
        delete T;
      }

      for(auto& syst: systematics)
      {
        variationHistograms[syst]->Add(variationHistograms["CentralValue"], -1);
        TH1D* tmpHist = static_cast<TH1D*>(variationHistograms[syst]->Clone(("tmp_"+cut.name()+"_"+variable.name()+"_"+syst).c_str()));
        tmpHist->Divide(variationHistograms["CentralValue"]);

        TCanvas c1((cut.name()+"_"+variable.name()+"_"+syst+"_Var").c_str(), "", 1200, 1350); // 800x900 in original, then scaled by 1.5
        gStyle->SetOptStat(0);

        c1.SetTopMargin(0.07);
        //t1->SetBottomMargin(0.10);
        //t1->SetRightMargin(0.20);
        tmpHist->Draw("hist");

        TPaveText* T = new TPaveText(0.1,0.995,0.84,0.95, "NDC");
        T->SetFillColor(0);
        T->SetFillStyle(0);
        T->SetLineColor(0);
        T->SetTextAlign(12);
        char Buffer[1024];
        sprintf(Buffer, "CMS preliminary, #sqrt{s}=%.1f TeV, #scale[0.5]{#int} L=%.1f fb^{-1}", 13.0, luminosity/1000);
        T->AddText(Buffer);
        T->Draw("same");
        T->SetBorderSize(0);

        c1.SaveAs((outputDirectory+"/"+cut.name()+"_"+variable.name()+"_"+syst+"_Var.png").c_str());
        c1.SaveAs((outputDirectory+"/"+cut.name()+"_"+variable.name()+"_"+syst+"_Var.C").c_str());
        c1.SaveAs((outputDirectory+"/"+cut.name()+"_"+variable.name()+"_"+syst+"_Var.root").c_str());

        delete T;
        delete tmpHist;
      }

      outSummary << "Bins:" << std::endl;
      TH1D* totalSyst = static_cast<TH1D*>(variationHistograms["CentralValue"]->Clone((cut.name()+"_"+variable.name()+"_Var").c_str()));
      for(int xbin=1; xbin <= totalSyst->GetXaxis()->GetNbins(); xbin++)
      {
        if(totalSyst->GetBinContent(xbin)==0)
          continue;

        double unc = 0;

        for(auto& base: systBase)
        {
          double down = variationHistograms[base + "_Down"]->GetBinContent(xbin);
          double up = variationHistograms[base + "_Up"]->GetBinContent(xbin);

          if(std::abs(down) > std::abs(up))
          {
            unc += down*down;
          }
          else
          {
            unc += up*up;
          }
        }

        unc = std::sqrt(unc);
        unc = unc / variationHistograms["CentralValue"]->GetBinContent(xbin);

        totalSyst->SetBinContent(xbin,0);
        totalSyst->SetBinError(xbin,unc);

        outSummary << "  " << xbin << ": " << unc << std::endl;
      }
      outSummary << std::endl;

      TCanvas c1((cut.name()+"_"+variable.name()+"_Var").c_str(), "", 1200, 1350); // 800x900 in original, then scaled by 1.5
      gStyle->SetOptStat(0);

      c1.SetTopMargin(0.07);
      //t1->SetBottomMargin(0.10);
      //t1->SetRightMargin(0.20);
      c1.SetGridy(true);

      TGraphErrors *totalSystArea = new TGraphErrors(totalSyst);
      totalSystArea->SetLineColor(1);
      totalSystArea->SetFillStyle(3001);
      totalSystArea->SetFillColor(kGray);
      totalSystArea->SetMarkerColor(1);
      totalSystArea->SetMarkerStyle(1);
      totalSyst->Reset("ICE");
      totalSyst->Draw();
      totalSystArea->Draw("3");
      double minErr = -0.3;//-1.1;
      double maxErr = 0.3;//1.1;
      //double yscale = (1.0-0.2)/(0.18-0);
      totalSyst->GetYaxis()->SetTitle("Total Uncertainty");
      totalSyst->SetMinimum(minErr);
      totalSyst->SetMaximum(maxErr);
      totalSyst->GetXaxis()->SetTitle("");
      totalSyst->GetXaxis()->SetTitleOffset(1.3);
      //totalSyst->GetXaxis()->SetLabelSize(0.033*yscale);
      //totalSyst->GetXaxis()->SetTitleSize(0.036*yscale);
      //totalSyst->GetXaxis()->SetTickLength(0.03*yscale);
      totalSyst->GetYaxis()->SetTitleOffset(0.3);
      totalSyst->GetYaxis()->SetNdivisions(25);
      //totalSyst->GetYaxis()->SetLabelSize(0.033*yscale);
      //totalSyst->GetYaxis()->SetTitleSize(0.036*yscale);

      TPaveText* T = new TPaveText(0.1,0.995,0.84,0.95, "NDC");
      T->SetFillColor(0);
      T->SetFillStyle(0);
      T->SetLineColor(0);
      T->SetTextAlign(12);
      char Buffer[1024];
      sprintf(Buffer, "CMS preliminary, #sqrt{s}=%.1f TeV, #scale[0.5]{#int} L=%.1f fb^{-1}", 13.0, luminosity/1000);
      T->AddText(Buffer);
      T->Draw("same");
      T->SetBorderSize(0);

      c1.SaveAs((outputDirectory+"/"+cut.name()+"_"+variable.name()+"_Var.png").c_str());
      c1.SaveAs((outputDirectory+"/"+cut.name()+"_"+variable.name()+"_Var.C").c_str());
      c1.SaveAs((outputDirectory+"/"+cut.name()+"_"+variable.name()+"_Var.root").c_str());

      double integral = variationHistograms["CentralValue"]->Integral();
      double quadSum = 0;
      for(auto& base: systBase)
      {
        outSummary << base;
        double down = variationHistograms[base + "_Down"]->Integral();
        double up = variationHistograms[base + "_Up"]->Integral();

        outSummary << " -  Up: " << up/integral << " (" << up << "); Down: " << down/integral << " (" << down << ")";

        double max = std::max(std::abs(down), std::abs(up));
        outSummary << "; Max: " << max/integral << " (" << max << ")";

        quadSum += max*max;
        outSummary << std::endl;
      }
      outSummary << "Total - " << std::sqrt(quadSum)/integral << " (" << std::sqrt(quadSum) << ")" << std::endl;

      delete totalSyst;
      delete totalSystArea;
      delete T;

      outSummary << std::endl << std::endl;
    }
  }

  std::cout << "Done" << std::endl;

  return 0;
}

