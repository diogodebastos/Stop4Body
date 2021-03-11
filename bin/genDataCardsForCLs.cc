#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <map>
#include <math.h>
#include <tuple>

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
std::tuple<double, double> fakeDD_varyXS(ProcessInfo &, SampleReader &, SampleReader &, std::string, std::string);
std::tuple<double, double> fullDD_varyXS(ProcessInfo &, ProcessInfo &, SampleReader &, SampleReader &, std::string, std::string, std::string, std::string, std::string);
std::string sysFromXSvar(ProcessInfo &, ProcessInfo &, doubleUnc, SampleReader &, SampleReader &, std::string, std::string, std::string, std::string, std::string, std::string);
std::string fullDD_sys(ProcessInfo &t, SampleReader &, SampleReader &, std::string, std::string, std::string, std::string, std::string, double, doubleUnc);
std::string getUpDownSysVar(ProcessInfo &, doubleUnc, std::string, double, std::string);
std::string Q2Sys(ProcessInfo &, doubleUnc, std::string, double);
std::string JesJerSys(ProcessInfo &, doubleUnc, std::string, std::string, double, double, std::string);
std::string xST(doubleUnc);
void makeDataCard(std::string, doubleUnc, doubleUnc, doubleUnc, doubleUnc, doubleUnc, doubleUnc, doubleUnc, doubleUnc, std::string, double, double, double, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string , std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string , std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string);

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
  double WjSys = 0.00;
  double ttSys = 0.00;
  double FakeSys = 0.00;

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

    if(argument == "--WjSys")
    {
      std::stringstream convert;
      convert << argv[++i];
      convert >> WjSys;
    }

    if(argument == "--ttSys")
    {
      std::stringstream convert;
      convert << argv[++i];
      convert >> ttSys;
    }

    if(argument == "--FakeSys")
    {
      std::stringstream convert;
      convert << argv[++i];
      convert >> FakeSys;
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
  std::string baseSelection = "(genWeight > 0) && (DPhiJet1Jet2 < 2.5 || Jet2Pt < 60)";
  std::string preSelection = baseSelection + " && (HT > 200) && (Jet1Pt > 110) && (Met>280)";
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

  std::string SR = tightSelection + " && " + preSelection + " && " + signalRegion;

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
    printSel("SR", SR);
    //printSel("SR",selMVA);
    for(auto& bkg : bkgMap)
    {
      std::cout << bkg.first << " | " << MC.process(bkg.second).getYield(SR, mcWeight) << std::endl;
      //std::cout << bkg.first << " | " << MC.process(bkg.second).getYield(selMVA, mcWeight) << std::endl;
    }
  }
  
  // Get yields
  auto wjets = MC.process(bkgMap["WJetsNLO"]);
  auto ttbar = MC.process(bkgMap["ttbar"]);
  auto zinv  = MC.process(bkgMap["ZInv"]);
  auto vv    = MC.process(bkgMap["VV"]);
  auto st    = MC.process(bkgMap["SingleTop"]);
  auto dy    = MC.process(bkgMap["DYJets"]);
  auto ttx   = MC.process(bkgMap["ttx"]);
  //auto qcd = MC.process(bkgMap["QCD"]);

  auto Wj   = fullDD(wjets, Data, MC, looseSelection, tightSelection, preSelection + " && " + signalRegion, preSelection + " && " + wjetsControlRegion, mcWeight);
  auto tt   = fullDD(ttbar, Data, MC, looseSelection, tightSelection, preSelection + " && " + signalRegion, preSelection + " && " + ttbarControlRegion, mcWeight);
  auto Fake = fakeDD(Data,MC, looseSelection + " && " + preSelection + " && " + signalRegion, mcWeight);
  auto VV   = MC.process(bkgMap["VV"]).getYield(SR, mcWeight);
  auto ST   = MC.process(bkgMap["SingleTop"]).getYield(SR, mcWeight);
  auto DY   = MC.process(bkgMap["DYJets"]).getYield(SR, mcWeight);
  auto TTX  = MC.process(bkgMap["ttx"]).getYield(SR, mcWeight);

  std::cout << "Wj:" << Wj << std::endl;
  std::cout << "tt:" << tt << std::endl;
  std::cout << "Fake:" << Fake << std::endl;

  double Wsy  = std::sqrt(std::pow(Wj.uncertainty()/Wj.value(),2) + WjSys*WjSys);
  double ttsy = std::sqrt(std::pow(tt.uncertainty()/tt.value(),2) + ttSys*ttSys);
  double Fksy = std::sqrt(std::pow(Fake.uncertainty()/Fake.value(),2) + FakeSys*FakeSys);

  std::string VVsyWj   = sysFromXSvar(vv, wjets, Wj, Data, MC, looseSelection, tightSelection, preSelection + " && " + signalRegion, preSelection + " && " + wjetsControlRegion, mcWeight, "mainBkg");  
  std::string VVsytt   = sysFromXSvar(vv, ttbar, tt, Data, MC, looseSelection, tightSelection, preSelection + " && " + signalRegion, preSelection + " && " + ttbarControlRegion, mcWeight, "mainBkg");
  std::string VVsyFake = sysFromXSvar(vv, zinv, Fake, Data, MC, "", "", looseSelection + " && " + preSelection + " && " + signalRegion, "", mcWeight, "fakes");

  std::string STsyWj   = sysFromXSvar(st, wjets, Wj, Data, MC, looseSelection, tightSelection, preSelection + " && " + signalRegion, preSelection + " && " + wjetsControlRegion, mcWeight, "mainBkg");  
  std::string STsytt   = sysFromXSvar(st, ttbar, tt, Data, MC, looseSelection, tightSelection, preSelection + " && " + signalRegion, preSelection + " && " + ttbarControlRegion, mcWeight, "mainBkg");
  std::string STsyFake = sysFromXSvar(st, zinv, Fake, Data, MC, "", "", looseSelection + " && " + preSelection + " && " + signalRegion, "", mcWeight, "fakes");

  std::string DYsyWj   = sysFromXSvar(dy, wjets, Wj, Data, MC, looseSelection, tightSelection, preSelection + " && " + signalRegion, preSelection + " && " + wjetsControlRegion, mcWeight, "mainBkg");  
  std::string DYsytt   = sysFromXSvar(dy, ttbar, tt, Data, MC, looseSelection, tightSelection, preSelection + " && " + signalRegion, preSelection + " && " + ttbarControlRegion, mcWeight, "mainBkg");
  std::string DYsyFake = sysFromXSvar(dy, zinv, Fake, Data, MC, "", "", looseSelection + " && " + preSelection + " && " + signalRegion, "", mcWeight, "fakes");

  std::string TTXsyWj   = sysFromXSvar(ttx, wjets, Wj, Data, MC, looseSelection, tightSelection, preSelection + " && " + signalRegion, preSelection + " && " + wjetsControlRegion, mcWeight, "mainBkg");  
  std::string TTXsytt   = sysFromXSvar(ttx, ttbar, tt, Data, MC, looseSelection, tightSelection, preSelection + " && " + signalRegion, preSelection + " && " + ttbarControlRegion, mcWeight, "mainBkg");
  std::string TTXsyFake = sysFromXSvar(ttx, zinv, Fake, Data, MC, "", "", looseSelection + " && " + preSelection + " && " + signalRegion, "", mcWeight, "fakes");

  std::string STQ2  = Q2Sys(st, ST, SR, luminosity);
  std::string DYQ2  = Q2Sys(dy, DY, SR, luminosity);
  std::string TTXQ2 = Q2Sys(ttx, TTX, SR, luminosity);

  std::string VVJES = JesJerSys(vv, VV, tightSelection, baseSelection, luminosity, bdtCut, "JES");
  std::string VVJER = JesJerSys(vv, VV, tightSelection, baseSelection, luminosity, bdtCut, "JER");
  std::string STJES = JesJerSys(st, ST, tightSelection, baseSelection, luminosity, bdtCut, "JES");
  std::string STJER = JesJerSys(st, ST, tightSelection, baseSelection, luminosity, bdtCut, "JER");
  std::string DYJES = JesJerSys(dy, DY, tightSelection, baseSelection, luminosity, bdtCut, "JES");
  std::string DYJER = JesJerSys(dy, DY, tightSelection, baseSelection, luminosity, bdtCut, "JER");
  std::string TTXJES = JesJerSys(ttx, TTX, tightSelection, baseSelection, luminosity, bdtCut, "JES");
  std::string TTXJER = JesJerSys(ttx, TTX, tightSelection, baseSelection, luminosity, bdtCut, "JER");

  std::string VVBTAG1 = getUpDownSysVar(vv, VV, SR, luminosity, "CFErr1");
  std::string VVBTAG2 = getUpDownSysVar(vv, VV, SR, luminosity, "CFErr2");
  std::string VVBTAG3 = getUpDownSysVar(vv, VV, SR, luminosity, "LF");
  std::string VVBTAG4 = getUpDownSysVar(vv, VV, SR, luminosity, "LFStats1");
  std::string VVBTAG5 = getUpDownSysVar(vv, VV, SR, luminosity, "LFStats2");
  std::string VVBTAG6 = getUpDownSysVar(vv, VV, SR, luminosity, "HF");
  std::string VVBTAG7 = getUpDownSysVar(vv, VV, SR, luminosity, "HFStats1");
  std::string VVBTAG8 = getUpDownSysVar(vv, VV, SR, luminosity, "HFStats2");

  std::string STBTAG1 = getUpDownSysVar(st, ST, SR, luminosity, "CFErr1");
  std::string STBTAG2 = getUpDownSysVar(st, ST, SR, luminosity, "CFErr2");
  std::string STBTAG3 = getUpDownSysVar(st, ST, SR, luminosity, "LF");
  std::string STBTAG4 = getUpDownSysVar(st, ST, SR, luminosity, "LFStats1");
  std::string STBTAG5 = getUpDownSysVar(st, ST, SR, luminosity, "LFStats2");
  std::string STBTAG6 = getUpDownSysVar(st, ST, SR, luminosity, "HF");
  std::string STBTAG7 = getUpDownSysVar(st, ST, SR, luminosity, "HFStats1");
  std::string STBTAG8 = getUpDownSysVar(st, ST, SR, luminosity, "HFStats2");

  std::string DYBTAG1 = getUpDownSysVar(dy, DY, SR, luminosity, "CFErr1");
  std::string DYBTAG2 = getUpDownSysVar(dy, DY, SR, luminosity, "CFErr2");
  std::string DYBTAG3 = getUpDownSysVar(dy, DY, SR, luminosity, "LF");
  std::string DYBTAG4 = getUpDownSysVar(dy, DY, SR, luminosity, "LFStats1");
  std::string DYBTAG5 = getUpDownSysVar(dy, DY, SR, luminosity, "LFStats2");
  std::string DYBTAG6 = getUpDownSysVar(dy, DY, SR, luminosity, "HF");
  std::string DYBTAG7 = getUpDownSysVar(dy, DY, SR, luminosity, "HFStats1");
  std::string DYBTAG8 = getUpDownSysVar(dy, DY, SR, luminosity, "HFStats2");

  std::string TTXBTAG1 = getUpDownSysVar(ttx, TTX, SR, luminosity, "CFErr1");
  std::string TTXBTAG2 = getUpDownSysVar(ttx, TTX, SR, luminosity, "CFErr2");
  std::string TTXBTAG3 = getUpDownSysVar(ttx, TTX, SR, luminosity, "LF");
  std::string TTXBTAG4 = getUpDownSysVar(ttx, TTX, SR, luminosity, "LFStats1");
  std::string TTXBTAG5 = getUpDownSysVar(ttx, TTX, SR, luminosity, "LFStats2");
  std::string TTXBTAG6 = getUpDownSysVar(ttx, TTX, SR, luminosity, "HF");
  std::string TTXBTAG7 = getUpDownSysVar(ttx, TTX, SR, luminosity, "HFStats1");
  std::string TTXBTAG8 = getUpDownSysVar(ttx, TTX, SR, luminosity, "HFStats2");
    
  std::string WjISR1 = fullDD_sys(wjets, Data, MC, looseSelection, tightSelection, preSelection + " && " + signalRegion, preSelection + " && " + wjetsControlRegion, "ISRweight_Bin1", luminosity, Wj);
  std::string WjISR2 = fullDD_sys(wjets, Data, MC, looseSelection, tightSelection, preSelection + " && " + signalRegion, preSelection + " && " + wjetsControlRegion, "ISRweight_Bin2", luminosity, Wj);
  std::string WjISR3 = fullDD_sys(wjets, Data, MC, looseSelection, tightSelection, preSelection + " && " + signalRegion, preSelection + " && " + wjetsControlRegion, "ISRweight_Bin3", luminosity, Wj);
  std::string WjISR4 = fullDD_sys(wjets, Data, MC, looseSelection, tightSelection, preSelection + " && " + signalRegion, preSelection + " && " + wjetsControlRegion, "ISRweight_Bin4", luminosity, Wj);
  std::string WjISR5 = fullDD_sys(wjets, Data, MC, looseSelection, tightSelection, preSelection + " && " + signalRegion, preSelection + " && " + wjetsControlRegion, "ISRweight_Bin5", luminosity, Wj);
  std::string WjISR6 = fullDD_sys(wjets, Data, MC, looseSelection, tightSelection, preSelection + " && " + signalRegion, preSelection + " && " + wjetsControlRegion, "ISRweight_Bin6", luminosity, Wj);
  std::string WjnST = fullDD_sys(wjets, Data, MC, looseSelection, tightSelection, preSelection + " && " + signalRegion, preSelection + " && " + wjetsControlRegion, "normStWeight", luminosity, Wj);

  std::string ttISR1 = fullDD_sys(ttbar, Data, MC, looseSelection, tightSelection, preSelection + " && " + signalRegion, preSelection + " && " + ttbarControlRegion, "ISRweight_Bin1", luminosity, tt);
  std::string ttISR2 = fullDD_sys(ttbar, Data, MC, looseSelection, tightSelection, preSelection + " && " + signalRegion, preSelection + " && " + ttbarControlRegion, "ISRweight_Bin2", luminosity, tt);
  std::string ttISR3 = fullDD_sys(ttbar, Data, MC, looseSelection, tightSelection, preSelection + " && " + signalRegion, preSelection + " && " + ttbarControlRegion, "ISRweight_Bin3", luminosity, tt);
  std::string ttISR4 = fullDD_sys(ttbar, Data, MC, looseSelection, tightSelection, preSelection + " && " + signalRegion, preSelection + " && " + ttbarControlRegion, "ISRweight_Bin4", luminosity, tt);
  std::string ttISR5 = fullDD_sys(ttbar, Data, MC, looseSelection, tightSelection, preSelection + " && " + signalRegion, preSelection + " && " + ttbarControlRegion, "ISRweight_Bin5", luminosity, tt);
  std::string ttISR6 = fullDD_sys(ttbar, Data, MC, looseSelection, tightSelection, preSelection + " && " + signalRegion, preSelection + " && " + ttbarControlRegion, "ISRweight_Bin6", luminosity, tt);
  std::string ttnST = fullDD_sys(ttbar, Data, MC, looseSelection, tightSelection, preSelection + " && " + signalRegion, preSelection + " && " + ttbarControlRegion, "normStWeight", luminosity, tt);

  std::string name;
  std::map<std::string, size_t> sigMap;
  for(size_t sig = 0; sig < Sig.nProcesses(); ++sig){
    // sigMap[Sig.process(sig).tag()] = sig;
  	if(verbose)
  	  std::cout << "Creating DataCard for: " << Sig.process(sig).tag() << std::endl;
    auto signal = Sig.process(sig);
    name = signal.tag();
    auto Sgn  = signal.getYield(SR, mcWeight);

    std::string FastS = getUpDownSysVar(signal, Sgn, SR, luminosity, "FullFast_ID_AltCorr");

    std::string SgnQ2 = Q2Sys(signal, Sgn, SR, luminosity);

    std::string SgnJES = JesJerSys(signal, Sgn, tightSelection, baseSelection, luminosity, bdtCut, "JES");
    std::string SgnJER = JesJerSys(signal, Sgn, tightSelection, baseSelection, luminosity, bdtCut, "JER");

    std::string SgnBTAG1 = getUpDownSysVar(signal, Sgn, SR, luminosity, "CFErr1");
    std::string SgnBTAG2 = getUpDownSysVar(signal, Sgn, SR, luminosity, "CFErr2");
    std::string SgnBTAG3 = getUpDownSysVar(signal, Sgn, SR, luminosity, "LF");
    std::string SgnBTAG4 = getUpDownSysVar(signal, Sgn, SR, luminosity, "LFStats1");
    std::string SgnBTAG5 = getUpDownSysVar(signal, Sgn, SR, luminosity, "LFStats2");
    std::string SgnBTAG6 = getUpDownSysVar(signal, Sgn, SR, luminosity, "HF");
    std::string SgnBTAG7 = getUpDownSysVar(signal, Sgn, SR, luminosity, "HFStats1");
    std::string SgnBTAG8 = getUpDownSysVar(signal, Sgn, SR, luminosity, "HFStats2");

    std::string SgnID = getUpDownSysVar(signal, Sgn, SR, luminosity, "LeptonIDSF_AltCorr");

    std::string SgnISR1 = getUpDownSysVar(signal, Sgn, SR, luminosity, "ISRweight_Bin1");
    std::string SgnISR2 = getUpDownSysVar(signal, Sgn, SR, luminosity, "ISRweight_Bin2");
    std::string SgnISR3 = getUpDownSysVar(signal, Sgn, SR, luminosity, "ISRweight_Bin3");
    std::string SgnISR4 = getUpDownSysVar(signal, Sgn, SR, luminosity, "ISRweight_Bin4");
    std::string SgnISR5 = getUpDownSysVar(signal, Sgn, SR, luminosity, "ISRweight_Bin5");
    std::string SgnISR6 = getUpDownSysVar(signal, Sgn, SR, luminosity, "ISRweight_Bin6");

    makeDataCard(name, Sgn, Wj, tt, Fake, VV, ST, DY, TTX, FastS, Wsy, ttsy, Fksy, VVsyWj, VVsytt, VVsyFake, STsyWj, STsytt, STsyFake, DYsyWj, DYsytt, DYsyFake, TTXsyWj, TTXsytt, TTXsyFake, SgnQ2, STQ2, DYQ2, TTXQ2, SgnJES, SgnJER, VVJES, VVJER, STJES, STJER, DYJES, DYJER, TTXJES, TTXJER, SgnBTAG1, SgnBTAG2, SgnBTAG3, SgnBTAG4, SgnBTAG5, SgnBTAG6, SgnBTAG7, SgnBTAG8, VVBTAG1, VVBTAG2, VVBTAG3, VVBTAG4, VVBTAG5, VVBTAG6, VVBTAG7, VVBTAG8, STBTAG1, STBTAG2, STBTAG3, STBTAG4, STBTAG5, STBTAG6, STBTAG7, STBTAG8, DYBTAG1, DYBTAG2, DYBTAG3, DYBTAG4, DYBTAG5, DYBTAG6, DYBTAG7, DYBTAG8, TTXBTAG1, TTXBTAG2, TTXBTAG3, TTXBTAG4, TTXBTAG5, TTXBTAG6, TTXBTAG7, TTXBTAG8, SgnID, SgnISR1, SgnISR2, SgnISR3, SgnISR4, SgnISR5, SgnISR6, WjISR1, WjISR2, WjISR3, WjISR4, WjISR5, WjISR6, WjnST, ttISR1, ttISR2, ttISR3, ttISR4, ttISR5, ttISR6, ttnST);
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

  doubleUnc estimate = NinSR/NinCR * (DatainCR - otherMC - fakes);

  return estimate;
}

std::tuple<double, double> fakeDD_varyXS(ProcessInfo &toVaryXS, SampleReader &LNTData, SampleReader &LNTMC, std::string signalRegion, std::string mcWeight)
{
  doubleUnc LNTMCinSR (0,0);
  doubleUnc LNTMCinSRvaryUp (0,0);
  doubleUnc LNTMCinSRvaryDown (0,0);
  doubleUnc LNTMCinSRup (0,0);
  doubleUnc LNTMCinSRdown (0,0);

  doubleUnc LNTinSR = LNTData.getYield(signalRegion, "weight");

  for(auto &process: LNTMC)
  {
    if(process.tag() != toVaryXS.tag()){
      LNTMCinSR += process.getYield(signalRegion + " && isPrompt == 1", mcWeight);
    }
    else if(process.tag() == toVaryXS.tag()){
      LNTMCinSRvaryUp += process.getYield(signalRegion + " && isPrompt == 1", mcWeight + "*1.5");
      LNTMCinSRvaryDown += process.getYield(signalRegion + " && isPrompt == 1", mcWeight + "*0.5");
    }
  }

  LNTMCinSRup   = LNTMCinSR + LNTMCinSRvaryUp;
  LNTMCinSRdown = LNTMCinSR + LNTMCinSRvaryDown;

  doubleUnc estimateUp = LNTinSR - LNTMCinSRup;
  doubleUnc estimateDown = LNTinSR - LNTMCinSRdown;
  /*
  std::cout << "Fakes estimateUp: " << estimateUp << std::endl;
  std::cout << "Fakes estimateDown: " << estimateDown << std::endl;
  */
  return std::make_tuple(estimateUp.value(), estimateDown.value());
}

std::tuple<double, double> fullDD_varyXS(ProcessInfo &toVaryXS, ProcessInfo &toEstimate, SampleReader &Data, SampleReader &MC, std::string looseSelection, std::string tightSelection, std::string signalRegion, std::string controlRegion, std::string mcWeight)
{
  doubleUnc NinSR = toEstimate.getYield(tightSelection + " && " + signalRegion + " && isPrompt == 1", mcWeight);
  doubleUnc NinCR = toEstimate.getYield(tightSelection + " && " + controlRegion + " && isPrompt == 1", mcWeight);
  doubleUnc DatainCR = Data.getYield(tightSelection + " && " + controlRegion, "1.0");
  doubleUnc otherMC (0,0);
  doubleUnc otherMCup (0,0);
  doubleUnc otherMCdown (0,0);
  doubleUnc varXSup (0,0);
  doubleUnc varXSdown (0,0);

  if(static_cast<double>(NinSR) == 0)
    NinSR = doubleUnc(4,2);
  for(auto &process: MC)
  {
    if(process.tag() != toEstimate.tag() && process.tag() != toVaryXS.tag()){
      otherMC += process.getYield(tightSelection + " && " + controlRegion + " && isPrompt == 1", mcWeight);
    }
    if(process.tag() == toVaryXS.tag()){
      varXSup = process.getYield(tightSelection + " && " + controlRegion + " && isPrompt == 1", mcWeight+"*1.5");
      varXSdown = process.getYield(tightSelection + " && " + controlRegion + " && isPrompt == 1", mcWeight+"*0.5");
    }
  }

  otherMCup = otherMC + varXSup;
  otherMCdown = otherMC + varXSdown;

  std::tuple<double, double> fakes = fakeDD_varyXS(toVaryXS, Data, MC, looseSelection + " && " + controlRegion, mcWeight);

  //doubleUnc estimate = NinSR/NinCR * (DatainCR - otherMC - fakes);
  double estimateUp   = NinSR.value()/NinCR.value() * (DatainCR.value() - otherMCup.value()   - std::get<0>(fakes));
  double estimateDown = NinSR.value()/NinCR.value() * (DatainCR.value() - otherMCdown.value() - std::get<1>(fakes));
  /*
  std::cout << "DD estimateUp: " << estimateUp << std::endl;
  std::cout << "DD estimateDown: " << estimateDown << std::endl;
  */
  return std::make_tuple(estimateUp, estimateDown);
}

std::string sysFromXSvar(ProcessInfo &toVaryXS, ProcessInfo &ddAffected, doubleUnc xDDcentral, SampleReader &Data, SampleReader &MC, std::string looseSelection, std::string tightSelection, std::string signalRegion, std::string controlRegion, std::string mcWeight, std::string ddMethod){

  std::tuple<double, double> toVaryXSsyddAffected;

  if (ddMethod == "mainBkg"){
    toVaryXSsyddAffected = fullDD_varyXS(toVaryXS,ddAffected, Data, MC, looseSelection, tightSelection, signalRegion, controlRegion, mcWeight);
  }
  else if (ddMethod == "fakes")
  {
    toVaryXSsyddAffected = fakeDD_varyXS(toVaryXS, Data, MC, signalRegion, mcWeight);
  }
  /*
  double syUp = std::abs(std::get<0>(toVaryXSsyddAffected)-xDDcentral.value());
  double syDown = std::abs(std::get<1>(toVaryXSsyddAffected)-xDDcentral.value());
  double syMax = std::max(syUp, syDown);
  
  return std::to_string(syMax/xDDcentral.value());
  */
  double syUp   = std::get<0>(toVaryXSsyddAffected)/xDDcentral.value();
  double syDown = std::get<1>(toVaryXSsyddAffected)/xDDcentral.value();

  return std::to_string(syUp) + "/" + std::to_string(syDown);
}

std::string fullDD_sys(ProcessInfo &toEstimate, SampleReader &Data, SampleReader &MC, std::string looseSelection, std::string tightSelection, std::string signalRegion, std::string controlRegion, std::string syst, double luminosity, doubleUnc xDDcentral)
{
  std::string UpDownVar = "1/1";
  std::string lumin = std::to_string(luminosity);
  std::string mcWeightVarUp   = "splitFactor*weight_"+syst+"_Up*"+lumin;
  std::string mcWeightVarDown = "splitFactor*weight_"+syst+"_Down*"+lumin;

  auto UpYield   = fullDD(toEstimate, Data, MC, looseSelection, tightSelection, signalRegion, controlRegion, mcWeightVarUp);
  auto DownYield = fullDD(toEstimate, Data, MC, looseSelection, tightSelection, signalRegion, controlRegion, mcWeightVarDown);

  UpDownVar = std::to_string(UpYield.value()/xDDcentral.value())+"/"+std::to_string(DownYield.value()/xDDcentral.value());

  return UpDownVar;
}

std::string getUpDownSysVar(ProcessInfo &toEstimate, doubleUnc centralYield, std::string selection, double luminosity, std::string systBase){
  std::string UpDownVar = "1/1";
  std::string lumin = std::to_string(luminosity);
  std::string mcWeightVarUp   = "splitFactor*weight_"+systBase+"_Up*"+lumin;
  std::string mcWeightVarDown = "splitFactor*weight_"+systBase+"_Down*"+lumin;

  auto UpYield = toEstimate.getYield(selection, mcWeightVarUp);
  auto DownYield = toEstimate.getYield(selection, mcWeightVarDown);

  UpDownVar = std::to_string(UpYield.value()/centralYield.value())+"/"+std::to_string(DownYield.value()/centralYield.value());

  return UpDownVar;
}

std::string xST(doubleUnc process){
  return std::to_string(1+process.uncertainty()/process.value());
}

std::string Q2Sys(ProcessInfo &toEstimate, doubleUnc centralYield, std::string selection, double luminosity){
  std::string lumin = std::to_string(luminosity);
  std::string mcWeight = "splitFactor*weight";
  doubleUnc yieldVar;
  double quadSumSys = 0;

  for (int i = 1; i <= 8; ++i)
  {
    std::string mcWeightVar = mcWeight + "_Q2_"+std::to_string(i)+"*"+lumin;
    yieldVar = toEstimate.getYield(selection, mcWeightVar);
    double percentVar = std::abs(yieldVar.value() - centralYield.value())/centralYield.value();
    quadSumSys += percentVar*percentVar;
  }

  return std::to_string(1+std::sqrt(quadSumSys));
}

std::string JesJerSys(ProcessInfo &toEstimate, doubleUnc centralYield, std::string tightSelection, std::string baseSelection, double luminosity, double bdtCut, std::string syst){
  std::string UpDownVar = "1/1";
  std::string lumin = std::to_string(luminosity);
  std::string bdt   = std::to_string(bdtCut);
  std::string varHTup   = "(HT_"+syst+"_Up > 200)";
  std::string varHTdown = "(HT_"+syst+"_Down > 200)";
  std::string varJet1Ptup   = "(Jet1Pt_"+syst+"_Up > 110)";
  std::string varJet1Ptdown = "(Jet1Pt_"+syst+"_Down > 110)";
  std::string varMetup   = "(Met_"+syst+"_Up > 280)";
  std::string varMetdown = "(Met_"+syst+"_Down > 280)";
  //std::string BDTup   = "(BDT_"+syst+"_Up > " + bdt + ")";
  //std::string BDTdown = "(BDT_"+syst+"_Down > " + bdt + ")";
  std::string BDTup   = "(BDT > " + bdt + ")";
  std::string BDTdown = "(BDT > " + bdt + ")";

  std::string selUp   = tightSelection + " && " + baseSelection + " && " + varHTup   + " && " + varJet1Ptup   + " && " + varMetup   + " && " + BDTup;
  std::string selDown = tightSelection + " && " + baseSelection + " && " + varHTdown + " && " + varJet1Ptdown + " && " + varMetdown + " && " + BDTdown;

  //std::cout << "selUp: " << selUp << std::endl;
  //std::cout << "selDown: " << selDown << std::endl;

  std::string mcWeightVarUp   = "splitFactor*weight_"+syst+"_Up*"+lumin;
  std::string mcWeightVarDown = "splitFactor*weight_"+syst+"_Down*"+lumin;

  auto UpYield   = toEstimate.getYield(selUp, mcWeightVarUp);
  auto DownYield = toEstimate.getYield(selDown, mcWeightVarDown);

  UpDownVar = std::to_string(UpYield.value()/centralYield.value())+"/"+std::to_string(DownYield.value()/centralYield.value());

  return UpDownVar;
}

void makeDataCard(std::string name, doubleUnc Sgn, doubleUnc Wj, doubleUnc tt, doubleUnc Fake, doubleUnc VV, doubleUnc ST, doubleUnc DY, doubleUnc TTX, std::string FastS, double Wsy, double ttsy, double Fksy, std::string VVsyWj, std::string VVsytt, std::string VVsyFake, std::string STsyWj, std::string STsytt, std::string STsyFake, std::string DYsyWj, std::string DYsytt, std::string DYsyFake, std::string TTXsyWj, std::string TTXsytt, std::string TTXsyFake, std::string SgnQ2, std::string STQ2, std::string DYQ2, std::string TTXQ2, std::string SgnJES, std::string SgnJER, std::string VVJES, std::string VVJER, std::string STJES, std::string STJER, std::string DYJES, std::string DYJER, std::string TTXJES, std::string TTXJER, std::string SgnBTAG1, std::string SgnBTAG2, std::string SgnBTAG3, std::string SgnBTAG4, std::string SgnBTAG5, std::string SgnBTAG6, std::string SgnBTAG7, std::string SgnBTAG8, std::string VVBTAG1, std::string VVBTAG2, std::string VVBTAG3, std::string VVBTAG4, std::string VVBTAG5, std::string VVBTAG6, std::string VVBTAG7, std::string VVBTAG8, std::string STBTAG1, std::string STBTAG2, std::string STBTAG3, std::string STBTAG4, std::string STBTAG5, std::string STBTAG6, std::string STBTAG7, std::string STBTAG8, std::string DYBTAG1, std::string DYBTAG2, std::string DYBTAG3, std::string DYBTAG4, std::string DYBTAG5, std::string DYBTAG6, std::string DYBTAG7, std::string DYBTAG8, std::string TTXBTAG1, std::string TTXBTAG2, std::string TTXBTAG3, std::string TTXBTAG4, std::string TTXBTAG5, std::string TTXBTAG6, std::string TTXBTAG7, std::string TTXBTAG8, std::string SgnID, std::string SgnISR1, std::string SgnISR2, std::string SgnISR3, std::string SgnISR4, std::string SgnISR5, std::string SgnISR6, std::string WjISR1, std::string WjISR2, std::string WjISR3, std::string WjISR4, std::string WjISR5, std::string WjISR6, std::string WjnST, std::string ttISR1, std::string ttISR2, std::string ttISR3, std::string ttISR4, std::string ttISR5, std::string ttISR6, std::string ttnST){
  name.replace(0,13,"");
  //name.replace(5,1,"N");
  std::ifstream dataCardIn("Templates/dataCardForCLs.txt");
  std::ofstream dataCardOut("DataCards/"+name+".txt");
  
  doubleUnc totalMC = Wj+tt+Fake+VV+ST+DY+TTX;

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
      strTemp = "observation " + std::to_string(std::round(totalMC.value()));
    }
    else if(i==8){
      strTemp = "bin "+bin+" "+bin+" "+bin+" "+bin+" "+bin+" "+bin+" "+bin+" "+bin;
    }
    else if(i==11){
      strTemp = "rate   "+std::to_string(Sgn.value())+" "+std::to_string(Wj.value())+" "+std::to_string(tt.value())+" "+std::to_string(Fake.value())+" "+std::to_string(VV.value())+" "+std::to_string(ST.value())+" "+std::to_string(DY.value())+" "+std::to_string(TTX.value());
    }
    else if(i==12){
      strTemp = "Sst   lnN " + xST(Sgn) + " - - - - - - -";
    }
    else if(i==13)
    {
      strTemp = "FastS lnN " + FastS + " - - - - - - -";
    }
    else if(i==14)
    {
      strTemp = "Wsy   lnN - " + std::to_string(1+Wsy) + " - - - - - -";
    }
    else if(i==15)
    {
      strTemp = "ttsy  lnN - - " + std::to_string(1+ttsy) + " - - - - -";
    }
    else if(i==16)
    {
      strTemp = "Fksy  lnN - - - " + std::to_string(1+Fksy) + " - - - -";
    }
    else if(i==17)
    {
      strTemp = "VVsy  lnN - " + VVsyWj + " "+ VVsytt + " " + VVsyFake + " 1.5/0.5 - - -";
    }
    else if(i==18)
    {
      strTemp = "VVst  lnN - - - - " + xST(VV) + " - - -";
    }
    else if(i==19)
    {
      strTemp = "STsy  lnN - " + STsyWj + " "+ STsytt + " " + STsyFake + " - 1.5/0.5 - -";
    }
    else if(i==20)
    {
      strTemp = "STst  lnN - - - - - " + xST(ST) + " - -";
    }
    else if(i==21)
    {
      strTemp = "DYsy  lnN - " + DYsyWj + " "+ DYsytt + " " + DYsyFake + " - - 1.5/0.5 -";
    }
    else if(i==22)
    {
      strTemp = "DYst  lnN - - - - - - " + xST(DY) + " -";
    }
    else if(i==23)
    {
      strTemp = "TTXsy lnN - " + TTXsyWj + " "+ TTXsytt + " " + TTXsyFake + " - - - 1.5/0.5";
    }
    else if(i==24)
    {
      strTemp = "TTXst lnN - - - - - - - " + xST(TTX);
    }
    else if(i==25)
    {
      strTemp = "Lum   lnN 1.023 - - - 1.023 1.023 1.023 1.023";
    }
    else if(i==26)
    {
      strTemp = "Trg   lnN 1.010 - - - 1.010 1.010 1.010 1.010";
    }
    else if(i==27)
    {
      strTemp = "PU    lnN 1.01 - - - 1.01 1.01 1.01 1.01";
    }
    else if(i==28)
    {
      strTemp = "Q2    lnN " + SgnQ2 + " - - - 1 " + STQ2 + " " + DYQ2 + " " + TTXQ2;
    }
    else if(i==29)
    {
      strTemp = "JES   lnN " + SgnJES + " - - - " + VVJES + " " + STJES + " " + DYJES + " " + TTXJES;
    }
    else if(i==30)
    {
      strTemp = "JER   lnN " + SgnJER + " - - - " + VVJER + " " + STJER + " " + DYJER + " " + TTXJER;
    }
    else if(i==31)
    {
      strTemp = "BTAG1 lnN " + SgnBTAG1 + " - - - " + VVBTAG1 + " " + STBTAG1 + " " + DYBTAG1 + " " + TTXBTAG1;
    }
    else if(i==32)
    {
      strTemp = "BTAG2 lnN " + SgnBTAG2 + " - - - " + VVBTAG2 + " " + STBTAG2 + " " + DYBTAG2 + " " + TTXBTAG2;
    }
    else if(i==33)
    {
      strTemp = "BTAG3 lnN " + SgnBTAG3 + " - - - " + VVBTAG3 + " " + STBTAG3 + " " + DYBTAG3 + " " + TTXBTAG3;
    }
    else if(i==34)
    {
      strTemp = "BTAG4 lnN " + SgnBTAG4 + " - - - " + VVBTAG4 + " " + STBTAG4 + " " + DYBTAG4 + " " + TTXBTAG4;
    }
    else if(i==35)
    {
      strTemp = "BTAG5 lnN " + SgnBTAG5 + " - - - " + VVBTAG5 + " " + STBTAG5 + " " + DYBTAG5 + " " + TTXBTAG5;
    }
    else if(i==36)
    {
      strTemp = "BTAG6 lnN " + SgnBTAG6 + " - - - " + VVBTAG6 + " " + STBTAG6 + " " + DYBTAG6 + " " + TTXBTAG6;
    }
    else if(i==37)
    {
      strTemp = "BTAG7 lnN " + SgnBTAG7 + " - - - " + VVBTAG7 + " " + STBTAG7 + " " + DYBTAG7 + " " + TTXBTAG7;
    }
    else if(i==38)
    {
      strTemp = "BTAG8 lnN " + SgnBTAG8 + " - - - " + VVBTAG8 + " " + STBTAG8 + " " + DYBTAG8 + " " + TTXBTAG8;
    }
    else if(i==39)
    {
      strTemp = "ID    lnN " + SgnID + " - - - 1.01 1.01 1.01 1.01";
    }/*
    else if(i==40)
    {
      strTemp = "";
      //strTemp = "RECO  lnN " + SgnRECO + " - - - 1.01 1.01 1.01 1.01";
    }*/
    else if(i==40)
    {
      strTemp = "ISR1  lnN " + SgnISR1 + " " + WjISR1 + " " + ttISR1 + " " + "-" + " - - - -";
    }
    else if(i==41)
    {
      strTemp = "ISR2  lnN " + SgnISR2 + " " + WjISR2 + " " + ttISR2 + " " + "-" + " - - - -";
    }
    else if(i==42)
    {
      strTemp = "ISR3  lnN " + SgnISR3 + " " + WjISR3 + " " + ttISR3 + " " + "-" + " - - - -";
    }
    else if(i==43)
    {
      strTemp = "ISR4  lnN " + SgnISR4 + " " + WjISR4 + " " + ttISR4 + " " + "-" + " - - - -";
    }
    else if(i==44)
    {
      strTemp = "ISR2  lnN " + SgnISR5 + " " + WjISR5 + " " + ttISR5 + " " + "-" + " - - - -";
    }
    else if(i==45)
    {
      strTemp = "ISR2  lnN " + SgnISR6 + " " + WjISR6 + " " + ttISR6 + " " + "-" + " - - - -";
    }
    else if(i==46)
    {
      strTemp = "nST   lnN - " + WjnST + " " + ttnST + " " + "-" + " - - - -";
    }

    strTemp += "\n";
    dataCardOut << strTemp;
  }

  return;
} 
// update background datacards per DM per SP

















