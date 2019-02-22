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
TEfficiency* getFakeRate(std::string, ProcessInfo &, VariableInfo&, std::string, std::string, std::string, std::string);
TH1D* getFakeRateRemovePrompt(std::string, ProcessInfo &, ProcessInfo &, SampleReader&,VariableInfo&, std::string, std::string, std::string, std::string, std::string);

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
  std::string debug;
  double luminosity = -1;
  // Placeholder for ${JSON_PATH}
  std::string jsonFileNameData = "/lstore/cms/dbastos/REPOS/Stop4Body/CMSSW_8_0_14/src/UserCode/Stop4Body/Macros/JSON/2017/DataJetHT.json";
  std::string jsonFileNameMC = "/lstore/cms/dbastos/REPOS/Stop4Body/CMSSW_8_0_14/src/UserCode/Stop4Body/Macros/JSON/2017/allMC.json";
  // Placeholder for ${INPUT}
  std::string inputDirectory = "/lstore/cms/dbastos/Stop4Body/nTuples_v2019-02-07-noSkim";
  std::string suffix = "";
  // Placeholder for ${variables} 
  std::string variablesJson = "/lstore/cms/dbastos/REPOS/Stop4Body/CMSSW_8_0_14/src/UserCode/Stop4Body/Macros/variables2017-fakeRateMethod.json";
  std::string cutsJson = "/lstore/cms/dbastos/REPOS/Stop4Body/CMSSW_8_0_14/src/UserCode/Stop4Body/Macros/variables2017-fakeRateMethod.json";

  std::cout << "Reading json files" << std::endl;
  VariableJsonLoader variables(variablesJson);
  SampleReader dataSamples(jsonFileNameData, inputDirectory, suffix);
  SampleReader MCSamples(jsonFileNameMC, inputDirectory, suffix);
  auto Data = dataSamples.getData();
  auto MC = MCSamples.getMCBkg();
  
  std::string selection = "";
  std::string dataSel = "";
  //std::string mcWeight = "XS*(genWeight/sumGenWeight)*puWeight*";

  std::string lowEta  = " && ((LepEta < 1.5) && (LepEta > -1.5))";
  std::string highEta = " && ((LepEta >= 1.5) || (LepEta <= -1.5))";
  std::string nonPrompt = " && (isPrompt == 0)";

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

  size_t jetHTIndex = 0;
  bool foundJetHT = false;
  for (size_t i = 0; i < Data.nProcesses(); i++) {
   // TODO: Check out to: If contains string -> "_JetHT"
   //if(Data.process(i).tag().find("_MetHt")) {
   if(Data.process(i).tag() == "Data") {
    jetHTIndex = i;
    foundJetHT = true;
   }
  }
  if(!foundJetHT)
  {
    std::cout << "There isn't a JetHT sample in the JSON file" << std::endl;
    return 1;
  }
  auto jetht = Data.process(jetHTIndex);
  
  size_t ttbarIndex = 0, wjetsIndex = 0, promptIndex = 0, qcdIndex = 0;
  bool foundTTbar = false, foundWJets = false, foundPrompt = false, foundQCD = false;
  for(size_t i = 0; i < MC.nProcesses(); ++i)
  {
    if(MC.process(i).tag() == "WJets")
    {
      wjetsIndex = i;
      foundWJets = true;
    }
    if(MC.process(i).tag().find("ttbar") != std::string::npos)
    {
      ttbarIndex = i;
      foundTTbar = true;
    }
    if(MC.process(i).tag() != "QCD" && MC.process(i).tag() != "ZInv" )
    {
     promptIndex = i;
     foundPrompt = true;
    }
    if(MC.process(i).tag() == "QCD")
    {
      qcdIndex = i;
      foundQCD = true;
    }
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
  if(!foundPrompt)
  {
    std::cout << "There isn't a prompt sample in the JSON file" << std::endl;
    return 1;
  }
  if(!foundQCD)
  {
    std::cout << "There isn't a qcd sample in the JSON file" << std::endl;
    return 1;
  }
  auto wjets = MC.process(wjetsIndex);
  auto ttbar = MC.process(ttbarIndex);
  auto prompt = MC.process(promptIndex);
  auto qcd = MC.process(qcdIndex);
  
  //DEBUG
  luminosity = jetht.getLumi();

  std::stringstream converter;
  converter << "luminosity: ";
  converter << luminosity/1000;
  std::cout << converter.str() << std::endl;
 
  std::string mcWeight;
  {
    std::stringstream converter2;
    converter2 << "splitFactor*weight";
    converter2 << "*" << luminosity;
    converter2 >> mcWeight;
  }
  
  std::cout << "Using mcWeight: " << mcWeight << std::endl;

//  TFile* pFile = new TFile("efficiency.root","recreate");

  // Measurement Region
  std::string mRegion_lep_tight = "";
  std::string mRegion_lep_loose = "";

  for(auto& cut : cutFlow)
  {
    if(cut.cut() != "")
    {
     selection = cut.cut();
     dataSel = selection + " && (HLT_PFHT1050)";
    }
    for(auto & variable : variables)
    {
     if(cut.name() == "electron") {
      std::cout << "\nelectron" << std::endl;
      mRegion_lep_tight = selection + " && (nGoodEl_cutId_veto)";
     }
     else if(cut.name() == "muon"){
      std::cout << "\nmuon" << std::endl;
      mRegion_lep_tight = selection + " && (nGoodMu_cutId_loose)";
     }
     
     std::string name = ("tightToLooseRatios_2017_"+cut.name()+"_"+variable.name()).c_str();
     /*
     auto pEff = getFakeRate(name, jetht, variable, dataSel, mRegion_lep_tight, mRegion_lep_loose, "weight");
     auto pEffLowEta = getFakeRate(name+"_LowEta", jetht, variable, dataSel + lowEta, mRegion_lep_tight, mRegion_lep_loose, "weight");
     auto pEffHighEta = getFakeRate(name+"_HighEta", jetht, variable, dataSel + highEta, mRegion_lep_tight, mRegion_lep_loose, "weight");
   
     auto pEffWjets = getFakeRate(name, wjets, variable, selection + nonPrompt, mRegion_lep_tight, mRegion_lep_loose, "weight");
     auto pEffTTbar = getFakeRate(name, ttbar, variable, selection + nonPrompt, mRegion_lep_tight, mRegion_lep_loose, "weight");
     auto pEffQCD   = getFakeRate(name, qcd, variable, selection, mRegion_lep_tight, mRegion_lep_loose, "weight");
     */

     auto pEffRemovePrompt = getFakeRateRemovePrompt(name, jetht, prompt, MC,variable, dataSel, selection, mRegion_lep_tight, mRegion_lep_loose, mcWeight);
     auto pEffRemovePromptLowEta = getFakeRateRemovePrompt(name + "_LowEta", jetht, prompt, MC,variable, dataSel + lowEta, selection + lowEta, mRegion_lep_tight, mRegion_lep_loose, mcWeight);
     auto pEffRemovePromptHightEta = getFakeRateRemovePrompt(name + "_HighEta", jetht, prompt, MC,variable, dataSel + highEta, selection + highEta, mRegion_lep_tight, mRegion_lep_loose, mcWeight);
     //Commented for DEBUG
     /*
     pEffWjets->SetLineColor(811);
     pEffTTbar->SetLineColor(614);
     pEffQCD->SetLineColor(809);
     pEff->Draw("");
     pEffWjets->Draw("same");
     pEffTTbar->Draw("same");
     pEffQCD->Draw("same");
     c1.BuildLegend(0.5,0.95,0.95,0.65,"");
     c1.SaveAs(("DataMCeff_" + name + ".png").c_str());
     
     pEff->Draw("");
     pEff->SetTitle((cut.name() + " efficiency").c_str());
     c1.SaveAs(("eff_" + name + ".png").c_str());
     pEff->SaveAs((name + ".root").c_str());
     
     pEffLowEta->Draw("");
     pEffLowEta->SetTitle((cut.name() + " efficiency (eta < 1.5)").c_str());
     c1.SaveAs(("eff_" + name + "_LowEta.png").c_str());
     pEffLowEta->SaveAs((name + "_LowEta.root").c_str());
     
     pEffHighEta->Draw("");
     pEffHighEta->SetTitle((cut.name() + " efficiency (eta > 1.5)").c_str());
     c1.SaveAs(("eff_" + name + "_HighEta.png").c_str());
     pEffHighEta->SaveAs((name + "_HighEta.root").c_str
     
//     pFile->Write();

     delete pEff;
     delete pEffLowEta;
     delete pEffHighEta;
     */
     delete pEffRemovePrompt;
    }
   }
  
//  system("hadd -f tightToLooseRatios_2017.root tightToLooseRatios_2017_electron_LepPt.root tightToLooseRatios_2017_muon_LepPt.root");
  system("hadd -f tightToLooseRatios_2017.root tightToLoose*.root");
    
  return 0;
}

TEfficiency* getFakeRate(std::string name, ProcessInfo &Process, VariableInfo& variable,std::string baseSelection, std::string tightSelection, std::string looseSelection, std::string Weight){
 TEfficiency* pEff = 0;
 TH1D* passTight = nullptr;
 TH1D* totalLoose = nullptr;
 doubleUnc yield = 0;
 bool checkConsistency = false;

 tightSelection = tightSelection + baseSelection;
 looseSelection = looseSelection + baseSelection;
 
 std::cout << "Getting variables and yields with selection (" << baseSelection << ")" << std::endl;
 
 yield = Process.getYield(baseSelection, Weight);
 std::cout << "Yield at:\n-Base selection: " << yield.value() << std::endl;
 yield = Process.getYield(tightSelection, Weight);
 std::cout << "-Tight: " << yield.value() << std::endl;
 yield = Process.getYield(looseSelection, Weight);
 std::cout << "-Loose: " << yield.value() << "\n" << std::endl;
 
 //auto lT = Process.getHist(variable.name().c_str(), variable, Weight + " * ( " + tightSelection + " )");
 //auto lL = Process.getHist(variable.name().c_str(), variable, Weight + " * ( " + looseSelection + " )");
 auto lT = Process.getHist(variable.name().c_str(), variable, "( " + tightSelection + " )");
 auto lL = Process.getHist(variable.name().c_str(), variable, "( " + looseSelection + " )");
 
 passTight = static_cast<TH1D*>(lT->Clone("tightlLeptons"));
 totalLoose = static_cast<TH1D*>(lL->Clone(name.c_str()));
 
 checkConsistency = TEfficiency::CheckConsistency(*passTight,*totalLoose);
 std::cout << "Consistency check: " << checkConsistency << std::endl;
 
 if(checkConsistency)
 {
  pEff = new TEfficiency(*passTight,*totalLoose);
 }
 return pEff;
 
 delete lL;
 delete lT;
 delete passTight;
 delete totalLoose;
 delete pEff;
}

TH1D* getFakeRateRemovePrompt(std::string name, ProcessInfo &Process, ProcessInfo &promptMC, SampleReader &MC, VariableInfo& variable, std::string dataSelection, std::string mcSelection, std::string tightSelection, std::string looseSelection, std::string mcWeight){
 std::string isPrompt = " && (isPrompt == 1)";
 printf("Canvas\n"); 
 TCanvas c1("DEBUG", "", 1200, 1350);
 gStyle->SetOptStat(0);  
 c1.cd();
 // Data
 auto lT = Process.getHist(variable.name().c_str(), variable, "weight * ( " + tightSelection + dataSelection + " )");
 auto lL = Process.getHist(variable.name().c_str(), variable, "weight * ( " + looseSelection + dataSelection + " )");
 //DEBUG
 //lL->Draw();
 //c1.SaveAs(("leptonLoose_" + name + ".png").c_str());
 //lL->SaveAs(("leptonLoose_" + name + ".root").c_str());
 //prompt MC
 TH1D* allPromptT = nullptr;
 TH1D* allPromptL = nullptr;

 for(size_t i = 0; i < MC.nProcesses(); ++i){ 
  if(MC.process(i).tag() != "QCD" && MC.process(i).tag() != "ZInv" ){
    auto tmpHistT = MC.process(i).getHist(variable.name().c_str(), variable, mcWeight + " * ( " + tightSelection + mcSelection + isPrompt + " )");
    auto tmpHistL = MC.process(i).getHist(variable.name().c_str(), variable, mcWeight + " * ( " + looseSelection + mcSelection + isPrompt + " )");
    //DEBUG
    //tmpHistL->Draw();
    //c1.SaveAs((MC.process(i).tag().c_str() + name + ".png").c_str());
    if(allPromptT == nullptr){
     allPromptT = tmpHistT;
    }
    else{
     allPromptT->Add(tmpHistT,1);
    }
    if(allPromptL == nullptr){
     allPromptL = tmpHistL;
    }
    else{
     allPromptL->Add(tmpHistL,1);
    }
    delete tmpHistT;
    delete tmpHistL;
  }
 }
 lT->Add(allPromptT,-1);
 lL->Add(allPromptL,-1);
 //DEBUG
 //lL->Draw();
 //c1.SaveAs(("leptonLooseDiff_" + name + ".png").c_str());
 //lL->SaveAs(("leptonLooseDiff_" + name + ".root").c_str());
 
 TGraphAsymmErrors* testEff = new TGraphAsymmErrors();
 testEff->Divide(lT,lL); 
 //DEBUG
 testEff->Draw();
 c1.SaveAs(("testEff_" + name + ".png").c_str());
 testEff->SaveAs(("testEff_" + name + ".root").c_str());
 
 auto ratio = static_cast<TH1D*>(lT->Clone((name+"EfficiencyAllEta").c_str()));
 ratio->Divide(lL);
 //DEBUG
 ratio->Draw();
 c1.SaveAs(("ratio_" + name + ".png").c_str());
 ratio->SaveAs(("ratio_" + name + ".root").c_str());

 return ratio;
 
 delete lL;
 delete lT;
 delete allPromptT;
 delete allPromptL;
 delete ratio;
}
