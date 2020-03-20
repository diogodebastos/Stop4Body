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

#include "UserCode/Stop4Body-nanoAOD/interface/json.hpp"
#include "UserCode/Stop4Body-nanoAOD/interface/SampleReader.h"
#include "UserCode/Stop4Body-nanoAOD/interface/doubleWithUncertainty.h"
#include "UserCode/Stop4Body-nanoAOD/interface/ValueWithSystematics.h"

using json = nlohmann::json;

void getIDeffs(std::string, std::string, ProcessInfo &, VariableInfo& , std::string, std::string, std::string);

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
  // double luminosity = -1;
  // Placeholder for ${JSON_PATH}
  std::string jsonFileName = "/lstore/cms/dbastos/REPOS/Stop4Body/CMSSW_8_0_14/src/UserCode/Stop4Body-nanoAOD/Macros/JSON/2017/plot2017_DM30RP.json";
  // Placeholder for ${INPUT}
  std::string inputDirectory = "/lstore/cms/dbastos/Stop4Body/nTuples_v2019-06-05-EWK_IRS-low-Mu-SFs";
  std::string outputDirectory = "./OUT/";
  bool verbose = false;

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
  VariableJsonLoader variables(variablesJson);
  SampleReader samples(jsonFileName, inputDirectory, suffix);

  if(verbose)
    std::cout << "Splitting samples according to type" << std::endl;
  auto MC = samples.getMCBkg();
  auto Sig = samples.getMCSig();
  auto Data = samples.getData();

  std::map<std::string, size_t> bkgMap;
  bool foundTTbar = false, foundWJets = false, foundZInv = false;
  for(size_t i = 0; i < MC.nProcesses(); ++i)
  {
    if(MC.process(i).tag().find("ttbar") != std::string::npos)
    {
      bkgMap["ttbar"] = i;
      foundTTbar = true;
    }
    else
      bkgMap[MC.process(i).tag()] = i;

    if(MC.process(i).tag() == "WJets")
      foundWJets = true;
    if(MC.process(i).tag() == "ZInv")
      foundZInv = true;
  }
  if(!foundTTbar)
  {
    std::cout << "There isn't a ttbar sample in the JSON file" << std::endl;
    return 1;
  }
  if(!foundWJets)
  {
    std::cout << "There isn't a wjets sample in the JSON file" << std::endl;
    return 1;
  }
  if(!foundZInv)
  {
    std::cout << "There isn't a wjets sample in the JSON file" << std::endl;
    return 1;
  }

  auto wjets = MC.process(bkgMap["WJets"]);
  auto ttbar = MC.process(bkgMap["ttbar"]);
  auto zinv = MC.process(bkgMap["ZInv"]);

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
      std::string subName = cut.latex().substr(0,6);
      getIDeffs(cut.name(), sig.tag(), sig, variable, selection, subName, outputDirectory);
      getIDeffs(cut.name(), zinv.tag(), zinv, variable, selection, subName, outputDirectory);
      getIDeffs(cut.name(), ttbar.tag(), ttbar, variable, selection, subName, outputDirectory);
      getIDeffs(cut.name(), wjets.tag(), wjets, variable, selection, subName, outputDirectory);
    }
  }
}

void getIDeffs(std::string name, std::string label, ProcessInfo &Process, VariableInfo& variable, std::string selection, std::string subName, std::string outputDirectory)
{
  std::string step0lepID = "";
  std::string step1lepID = "";
  TH1D* step0Lepton = nullptr;
  TH1D* step1Lepton = nullptr;

  if(name == "electron") {
   //std::cout << "\nelectron" << std::endl;
   step0lepID = selection + " && (nGoodEl_cutId_veto == 1)";
   step1lepID = selection + " && (nGoodEl_cutId_loose == 1)";
  }
  else if(name == "muon"){
   //std::cout << "\nmuon" << std::endl;
   step0lepID = selection + " && (nGoodMu_cutId_loose == 1)";
   step1lepID = selection + " && (nGoodMu_cutId_medium == 1)";
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
  c1.SetGridx();
  c1.SetGridy();
  TLegend *lg1 = new TLegend(.85,.85,0.99,0.99);
  lg1->SetFillColor(0);

  step0Lepton->SetTitle((name+"_IDs_effs").c_str());
  step0Lepton->GetYaxis()->SetRangeUser(0,1.01);
  step1Lepton->GetYaxis()->SetRangeUser(0,1.01);
  step0Lepton->Draw();
  step1Lepton->SetLineColor(kRed);
  step1Lepton->Draw("same");
  lg1->AddEntry(step0Lepton,"step0","l");
  lg1->AddEntry(step1Lepton,"step1","l");
  lg1->Draw();

  c1.SaveAs((outputDirectory+"/"+label+"_"+name+"_"+subName+"_cutID_effs.png").c_str());

  TCanvas c2("Ratio Lepton", "", 1200, 1350);
  step1Lepton->SetTitle((name+"_step1/step0").c_str());
  step1Lepton->Divide(step0Lepton);
  step1Lepton->Draw();
  c2.SaveAs((outputDirectory+"/"+label+"_"+name+"_"+subName+"_ratio.png").c_str());

  delete pfLep;
  delete pfStep0lepID;
  delete pfStep1lepID;
  delete step0Lepton;
  delete step1Lepton;
  return;
}
