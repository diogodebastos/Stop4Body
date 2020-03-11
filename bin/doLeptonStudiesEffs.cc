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
#include <TEfficiency.h>
#include <TGraphAsymmErrors.h>

#include "UserCode/Stop4Body/interface/json.hpp"
#include "UserCode/Stop4Body/interface/SampleReader.h"
#include "UserCode/Stop4Body/interface/doubleWithUncertainty.h"
#include "UserCode/Stop4Body/interface/ValueWithSystematics.h"

using json = nlohmann::json;

void getIDeffs(std::string, ProcessInfo&, VariableInfo&, std::string);

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
  std::string variablesJson = "";
  std::string cutsJson = "";

  std::string debug;
  double luminosity = -1;
  // Placeholder for ${JSON_PATH}
  std::string jsonFileNamData = "/lstore/cms/dbastos/REPOS/Stop4Body/CMSSW_8_0_14/src/UserCode/Stop4Body/Macros/JSON/2017/plot2017_DM30RP.json";
  // Placeholder for ${INPUT}
  std::string inputDirectory = "/lstore/cms/dbastos/Stop4Body/nTuples_v2019-06-05-EWK_IRS-low-Mu-SFs";
  std::string suffix = "";

  for(int i = 1; i < argc; ++i)
  {
    std::string argument = argv[i];

    if(argument == "--json")
      jsonFileName = argv[++i];

    if(argument == "--outDir")
      outputDirectory = argv[++i];

    if(argument == "--inDir")
      inputDirectory = argv[++i];

    if(argument == "--variables")
      variablesJson = argv[++i];

    if(argument == "--cuts")
      cutsJson = argv[++i];

    if(argument == "--verbose")
    {
      verbose = true;
    }
  }

  std::cout << "Reading json files" << std::endl;
  SampleReader samples(jsonFileName, inputDirectory, suffix);

  if(verbose)
    std::cout << "Splitting samples according to type" << std::endl;
  auto MC = samples.getMCBkg();
  auto Sig = samples.getMCSig();
  auto Data = samples.getData();

  std::string selection = "";

  std::vector<CutInfo> cutFlow;
  json jsonFile;
  std::ifstream inputFile(cutsJson);
  inputFile >> jsonFile;

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

  auto sig = Sig.process(0);

  for(auto& cut : cutFlow)
  {
    if(cut.cut() != "")
    {
      selection = cut.cut();
    }
    for(auto & variable : variables)
    {
      //LepPt
      //TH1D* veto = new  TH1D("veto","veto",100,0,100)
      //bdttree->Draw("LepPt>>veto","nGoodEl_cutId_veto")
      //r = (TH1D*)veto->Clone()
      //
      getIDeffs(cutName,sig,variable,selection);
    }
}

void getIDeffs(std::string name, ProcessInfo &Process, VariableInfo& variable, std::string selection)
{
  std::string step0lepID = "";
  std::string step1lepID = "";

  if(name == "electron") {
   //std::cout << "\nelectron" << std::endl;
   step0lepID = selection + " && (nGoodEl_cutId_veto == 1)";
   step1lepID = selection + " && (nGoodEl_cutId_loose == 1)";
  }
  else if(name == "muon"){
   //std::cout << "\nmuon" << std::endl;
   step0lepID = selection + " && (nGoodMu_cutId_soft == 1)";
   step1lepID = selection + " && (nGoodMu_cutId_loose == 1)";
  }

  auto pfLep = Process.getHist(variable.name().c_str(),variable,selection);
  auto pfStep0lepID = Process.getHist(variable.name().c_str(),variable,step0lepID);
  auto pfStep1lepID = Process.getHist(variable.name().c_str(),variable,step1lepID);

  step0Lepton = static_cast<TH1D*>(pfStep0lepID->Clone("Step0 lepton ID"));
  step1Lepton = static_cast<TH1D*>(pfStep1lepID->Clone("Step1 lepton ID"));

  step0Lepton->Divide(pfLep);
  step1Lepton->Divide(pfLep);

  TCanvas c1("Efficiencies", "", 1200, 1350);
  c1.cd();
  gStyle->SetOptStat(0);
  c1->SetGridx();
  c1->SetGridy();
  TLegend *lg1 = new TLegend(.85,.85,0.99,0.99);
  lg1->SetFillColor(0);

  step0Lepton->SetTitle((name+"_IDs_effs").c_str());
  step0Lepton->Draw();
  step1Lepton->SetLineColor(kRed);
  step1Lepton->Draw("same");
  lg1->AddEntry(step0Lepton,"step0","l");
  lg1->AddEntry(step1Lepton,"step1","l");

  c1.SaveAs(("lepton_cutID_effs.png").c_str());

  TCanvas c2("Ratio Lepton", "", 1200, 1350);
  step0Lepton->SetTitle((name+"step0/step1").c_str());
  step0Lepton->Divide(step1Lepton);
  step0Lepton->Draw();
  c2.SaveAs(("lepton_ratio.png").c_str());

  return;
}
