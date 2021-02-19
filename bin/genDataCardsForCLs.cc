#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <map>
#include <math.h>

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

#include "UserCode/Stop4Body-nanoAOD/interface/json.hpp"
#include "UserCode/Stop4Body-nanoAOD/interface/SampleReader.h"
#include "UserCode/Stop4Body-nanoAOD/interface/doubleWithUncertainty.h"
#define _USE_CERN_ROOT
#include "UserCode/Stop4Body-nanoAOD/interface/ValueWithSystematics.h"

using json = nlohmann::json;

void printSel(std::string, std::string);
doubleUnc fakeDD(SampleReader &, SampleReader &, std::string, std::string);
doubleUnc fullDD(ProcessInfo &, SampleReader &, SampleReader &, std::string, std::string, std::string, std::string, std::string);
void makeDataCard(std::string, doubleUnc, doubleUnc, doubleUnc, doubleUnc, doubleUnc, doubleUnc, doubleUnc, doubleUnc);
//void makeDataCard(ProcessInfo &, SampleReader &, SampleReader &, std::map<std::string, size_t>, std::string, std::string, std::string preSelection, std::string wjetsEnrich, std::string ttbarEnrich, std::string, std::string, std::string, std::string, std::string);

int main(int argc, char** argv)
{
  std::string jsonFileName = "";
  std::string inputDirectory = "";
  std::string outputDirectory = "./OUT/";
  std::string suffix = "";
  bool verbose = false;
  bool isHighDM = false;
  double luminosity = -1.0;
  double bdtCut = 0.20;

  if(argc < 2)
  {
    std::cout << "You did not pass enough parameters" << std::endl;
    //printHelp();
    return 0;
  }

  for(int i = 1; i < argc; ++i)
  {
    std::string argument = argv[i];

    if(argument == "--json")
      jsonFileName = argv[++i];

    if(argument == "--outDir")
      outputDirectory = argv[++i];

    if(argument == "--inDir")
      inputDirectory = argv[++i];

    if(argument == "--suffix")
      suffix = argv[++i];

    if(argument == "--isHighDeltaM")
    {
      isHighDM = true;
    }

    if(argument == "--bdtCut")
    {
      std::stringstream convert;
      convert << argv[++i];
      convert >> bdtCut;
    }

    if(argument == "--verbose")
    {
      verbose = true;
    } 
  }

  std::cout << "Reading json files" << std::endl;
  SampleReader samples(jsonFileName, inputDirectory, suffix);

  // Kinematic regions definition

  std::string tightSelection = "(isTight == 1)";
  std::string looseSelection = "(isLoose == 1) && !(isTight == 1)";
  std::string preSelection = "(genWeight > 0) && (DPhiJet1Jet2 < 2.5 || Jet2Pt < 60) && (HT > 200) && (Jet1Pt > 110) && (Met>280)";
  std::string wjetsEnrich = "(NbLoose == 0)";
  std::string ttbarEnrich = "(NbTight > 0)";
  std::string controlRegion = "(BDT < 0.2)";

  if(!isHighDM)
  {
    preSelection +=" && (LepPt < 30.)";
  }

  std::string signalRegion = "";
  {
    std::stringstream converter;
    converter << "(BDT > " << bdtCut << ")";
    signalRegion = converter.str();
  }

  std::string wjetsControlRegion = controlRegion + " && " + wjetsEnrich;
  std::string ttbarControlRegion = controlRegion + " && " + ttbarEnrich;

  if(verbose)
    std::cout << "Splitting samples according to type" << std::endl;
  auto MC = samples.getMCBkg();
  auto Sig = samples.getMCSig();
  auto Data = samples.getData();

  if(verbose)
    std::cout << "Retrieving luminosity" << std::endl;
  if(luminosity <= 0)
    luminosity = Data.getLumi();

  std::string mcWeight;
  {
    std::stringstream converter;
    converter << "splitFactor*weight";
    converter << "*" << luminosity;
    converter >> mcWeight;
  }

  // Add systematics variations

  ValueWithSystematics<std::string> mcWeightSys(mcWeight);
  {
    std::vector<std::string> systVars;
    systVars.push_back("ISRweight_Bin1_Up");
    systVars.push_back("ISRweight_Bin1_Down");
    systVars.push_back("ISRweight_Bin2_Up");
    systVars.push_back("ISRweight_Bin2_Down");
    systVars.push_back("ISRweight_Bin3_Up");
    systVars.push_back("ISRweight_Bin3_Down");
    systVars.push_back("ISRweight_Bin4_Up");
    systVars.push_back("ISRweight_Bin4_Down");
    systVars.push_back("ISRweight_Bin5_Up");
    systVars.push_back("ISRweight_Bin5_Down");
    systVars.push_back("ISRweight_Bin6_Up");
    systVars.push_back("ISRweight_Bin6_Down");
    systVars.push_back("TightLoose_NU_Bin1_Up");
    systVars.push_back("TightLoose_NU_Bin1_Down");
    systVars.push_back("TightLoose_NU_Bin2_Up");
    systVars.push_back("TightLoose_NU_Bin2_Down");
    systVars.push_back("TightLoose_NU_Bin3_Up");
    systVars.push_back("TightLoose_NU_Bin3_Down");
    systVars.push_back("TightLoose_NU_Bin4_Up");
    systVars.push_back("TightLoose_NU_Bin4_Down");
    systVars.push_back("TightLoose_NU_Bin5_Up");
    systVars.push_back("TightLoose_NU_Bin5_Down");
    systVars.push_back("HF_Up");
    systVars.push_back("HF_Down");
    systVars.push_back("LF_Up");
    systVars.push_back("LF_Down");
    systVars.push_back("HFStats1_Up");
    systVars.push_back("HFStats1_Down");
    systVars.push_back("LFStats1_Up");
    systVars.push_back("LFStats1_Down");
    systVars.push_back("HFStats2_Up");
    systVars.push_back("HFStats2_Down");
    systVars.push_back("LFStats2_Up");
    systVars.push_back("LFStats2_Down");
    systVars.push_back("CFErr1_Up");
    systVars.push_back("CFErr1_Down");
    systVars.push_back("CFErr2_Up");
    systVars.push_back("CFErr2_Down");

    for(auto& syst: systVars)
    {
      std::stringstream converter;
      converter << "splitFactor*weight_" << syst << "*" << luminosity;
      mcWeightSys.Systematic(syst) = converter.str();
    }
  }
  if(verbose)
    std::cout << "Using mcWeight: " << mcWeight << std::endl;
  if(mcWeightSys.Systematics().size() > 0)
  {
    std::cout << "And variations:" << std::endl;
    for(auto& syst: mcWeightSys.Systematics())
      if(verbose)
        std::cout << "  " << syst << ": " << mcWeightSys.Systematic(syst) << std::endl;
  }
  std::cout << std::endl;

  if(verbose)
    std::cout << "Building background process map" << std::endl;
  std::map<std::string, size_t> bkgMap;
  for(size_t i = 0; i < MC.nProcesses(); ++i){
    bkgMap[MC.process(i).tag()] = i;
  }

  //std::string selMVA = "(DPhiJet1Jet2 < 2.5 || Jet2Pt < 60) && (HT > 200) && (Met > 280) && (nGoodEl_cutId_loose+nGoodMu_cutId_medium == 1) && (Jet1Pt > 110) && (BDT > 0.45) && (LepPt < 30)";

  if(verbose){
    printSel("SR",tightSelection + " && " + preSelection + " && " + signalRegion);
    //printSel("SR",selMVA);
    for(auto& bkg : bkgMap)
    {
      std::cout << bkg.first << " | " << MC.process(bkg.second).getYield(tightSelection + " && " + preSelection + " && " + signalRegion, mcWeight) << std::endl;
      //std::cout << bkg.first << " | " << MC.process(bkg.second).getYield(selMVA, mcWeight) << std::endl;
    }
  }
  
  // Get yields
  auto wjets = MC.process(bkgMap["WJetsNLO"]);
  auto ttbar = MC.process(bkgMap["ttbar"]);
  //auto zinv = MC.process(bkgMap["ZInv"]);
  //auto qcd = MC.process(bkgMap["QCD"]);

  auto Wj   = fullDD(wjets, Data, MC, looseSelection, tightSelection, preSelection + " && " + signalRegion, preSelection + " && " + wjetsControlRegion, mcWeight);
  auto tt   = fullDD(ttbar, Data, MC, looseSelection, tightSelection, preSelection + " && " + signalRegion, preSelection + " && " + ttbarControlRegion, mcWeight);
  auto Fake = fakeDD(Data,MC, looseSelection + " && " + preSelection + " && " + signalRegion, mcWeight);
  auto VV   = MC.process(bkgMap["VV"]).getYield(tightSelection + " && " + preSelection + " && " + signalRegion, mcWeight);
  auto ST   = MC.process(bkgMap["SingleTop"]).getYield(tightSelection + " && " + preSelection + " && " + signalRegion, mcWeight);
  auto DY   = MC.process(bkgMap["DYJets"]).getYield(tightSelection + " && " + preSelection + " && " + signalRegion, mcWeight);
  auto TTX  = MC.process(bkgMap["ttx"]).getYield(tightSelection + " && " + preSelection + " && " + signalRegion, mcWeight);

  std::cout << "Wj:" << Wj << std::endl;
  std::cout << "tt:" << tt << std::endl;
  std::cout << "Fake:" << Fake << std::endl;

  std::string name;
  std::map<std::string, size_t> sigMap;
  for(size_t sig = 0; sig < Sig.nProcesses(); ++sig){
   // sigMap[Sig.process(sig).tag()] = sig;
     if(verbose)
       std::cout << "Creating DataCard for: " << Sig.process(sig).tag() << std::endl;
     auto signal = Sig.process(sig);
     name = signal.tag();
     auto Sgn  = signal.getYield(tightSelection + " && " + preSelection + " && " + signalRegion, mcWeight);
     makeDataCard(name, Sgn, Wj, tt, Fake, VV, ST, DY, TTX);
     //makeDataCard(signal, Data, MC, bkgMap, tightSelection, looseSelection, preSelection, wjetsEnrich, ttbarEnrich, controlRegion, signalRegion, wjetsControlRegion, ttbarControlRegion, mcWeight);

  }

}

void printSel(std::string name, std::string selection)
{
  std::cout << "The used " << name << ":" << std::endl;
  std::cout << "  " << selection << std::endl;
  std::cout << std::endl;
  return;
}

doubleUnc fakeDD(SampleReader &LNTData, SampleReader &LNTMC, std::string signalRegion, std::string mcWeight)
{
  //printSel("FakeinSR:", signalRegion);

  doubleUnc LNTinSR = LNTData.getYield(signalRegion, "weight");
  doubleUnc LNTMCinSR = LNTMC.getYield(signalRegion + " && isPrompt == 1", mcWeight);

  doubleUnc estimate = LNTinSR - LNTMCinSR;

  return estimate;
}


doubleUnc fullDD(ProcessInfo &toEstimate, SampleReader &Data, SampleReader &MC, std::string looseSelection, std::string tightSelection, std::string signalRegion, std::string controlRegion, std::string mcWeight)
{
  //printSel("DDinSR: ",tightSelection + " && " + signalRegion + " && isPrompt == 1");
  //printSel("DDinCR: ",tightSelection + " && " + controlRegion + " && isPrompt == 1");

  doubleUnc NinSR = toEstimate.getYield(tightSelection + " && " + signalRegion + " && isPrompt == 1", mcWeight);
  doubleUnc NinCR = toEstimate.getYield(tightSelection + " && " + controlRegion + " && isPrompt == 1", mcWeight);
  doubleUnc DatainCR = Data.getYield(tightSelection + " && " + controlRegion, "1.0");
  doubleUnc otherMC (0,0);

  if(static_cast<double>(NinSR) == 0)
    NinSR = doubleUnc(4,2);
  for(auto &process: MC)
  {
    if(process.tag() != toEstimate.tag())
      otherMC += process.getYield(tightSelection + " && " + controlRegion + " && isPrompt == 1", mcWeight);
  }

  doubleUnc fakes = fakeDD(Data, MC, looseSelection + " && " + controlRegion, mcWeight);

  //printSel("FakeSel:", looseSelection + " && " + controlRegion);
  doubleUnc estimate = NinSR/NinCR * (DatainCR - otherMC - fakes);

  return estimate;
}

// create datacards for background per DM
//void makeDataCard(ProcessInfo &Signal, SampleReader &Data, SampleReader &MC, std::map<std::string, size_t> bkgMap, std::string tightSelection, std::string looseSelection, std::string preSelection, std::string wjetsEnrich, std::string ttbarEnrich, std::string controlRegion, std::string signalRegion, std::string wjetsControlRegion, std::string ttbarControlRegion, std::string mcWeight){
void makeDataCard(std::string name, doubleUnc Sgn, doubleUnc Wj, doubleUnc tt, doubleUnc Fake, doubleUnc VV, doubleUnc ST, doubleUnc DY, doubleUnc TTX){
  name.replace(0,13,"");
  //name.replace(5,1,"N");
  std::ifstream dataCardIn("Templates/dataCardForCLs.txt");
  std::ofstream dataCardOut("DataCards/"+name+".txt");
  
  if(!dataCardIn || !dataCardOut)
  {
    std::cout << "Error opening files!" << std::endl;
    return;
  }

  std::string bin = "B_"+name+"_SR";

  std::string strTemp;
  int i = 0;

  while(getline(dataCardIn,strTemp))
  {
    i++;
    if(i==5){
      strTemp = "bin "+bin;
    }
    else if(i==6){
      //strTemp = "observation " + std::to_string(std::round(totalMC.value()));
    }
    else if(i==8){
      strTemp = "bin "+bin+" "+bin+" "+bin+" "+bin+" "+bin+" "+bin+" "+bin+" "+bin;
    }
    else if(i==11){
      strTemp = "rate "+std::to_string(Sgn.value())+" "+std::to_string(Wj.value())+" "+std::to_string(tt.value())+" "+std::to_string(Fake.value())+" "+std::to_string(VV.value())+" "+std::to_string(ST.value())+" "+std::to_string(DY.value())+" "+std::to_string(TTX.value());
    }
    strTemp += "\n";
    dataCardOut << strTemp;
  }

  return;
} 
// update background datacards per DM per SP



















