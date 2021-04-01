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

doubleUnc fakeDD(SampleReader &, SampleReader &, std::string, std::string);
doubleUnc fullDD(ProcessInfo &, SampleReader &, SampleReader &, std::string, std::string, std::string, std::string, std::string);
std::string reportYieldsUnc(double, double);

int main(int argc, char** argv)
{
  std::string jsonFileName = "";
  std::string inputDirectory = "";
  std::string outputDirectory = "./OUT/";
  std::string suffix = "";
  bool verbose = false;
  bool isHighDM = false;
  bool unblind = false;
  double luminosity = -1.0;
  double bdtCut = 0.20;
  int year = 2017;
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
    if(argument == "--unblind")
    {
      unblind = true;
    }
    if(argument == "--bdtCut")
    {
      std::stringstream convert;
      convert << argv[++i];
      convert >> bdtCut;
    }
    if(argument == "--year")
    {
      std::stringstream converter;
      converter << argv[++i];
      converter >> year;
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
    baseSelection += " && (LepPt < 30.)";
    preSelection  += " && (LepPt < 30.)";
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

  std::map<std::string, size_t> bkgMap;
  for(size_t i = 0; i < MC.nProcesses(); ++i){
    bkgMap[MC.process(i).tag()] = i;
  }

  if(verbose){
    printSel("SR", SR);
    //printSel("SR",selMVA);
    for(auto& bkg : bkgMap)
    {
      std::cout << bkg.first << " | " << MC.process(bkg.second).getYield(SR, mcWeight) << std::endl;
      //std::cout << bkg.first << " | " << MC.process(bkg.second).getYield(selMVA, mcWeight) << std::endl;
    }
  }

  doubleUnc DatainSR = Data.getYield(SR, "1.0");
  auto wjets = MC.process(bkgMap["WJetsNLO"]);
  auto ttbar = MC.process(bkgMap["ttbar"]);
  auto zinv  = MC.process(bkgMap["ZInv"]);
  auto vv    = MC.process(bkgMap["VV"]);
  auto st    = MC.process(bkgMap["SingleTop"]);
  auto dy    = MC.process(bkgMap["DYJets"]);
  auto ttx   = MC.process(bkgMap["ttx"]);
  auto Sgn   = Sig.getYield(SR, mcWeight);

  //auto qcd = MC.process(bkgMap["QCD"]);

  auto Wj   = fullDD(wjets, Data, MC, looseSelection, tightSelection, preSelection + " && " + signalRegion, preSelection + " && " + wjetsControlRegion, mcWeight);
  auto tt   = fullDD(ttbar, Data, MC, looseSelection, tightSelection, preSelection + " && " + signalRegion, preSelection + " && " + ttbarControlRegion, mcWeight);
  auto Fake = fakeDD(Data,MC, looseSelection + " && " + preSelection + " && " + signalRegion, mcWeight);
  auto VV   = MC.process(bkgMap["VV"]).getYield(SR, mcWeight);
  auto ST   = MC.process(bkgMap["SingleTop"]).getYield(SR, mcWeight);
  auto DY   = MC.process(bkgMap["DYJets"]).getYield(SR, mcWeight);
  auto TTX  = MC.process(bkgMap["ttx"]).getYield(SR, mcWeight);

  double Wsy  = std::sqrt(std::pow(Wj.uncertainty()/Wj.value(),2) + WjSys*WjSys);
  double ttsy = std::sqrt(std::pow(tt.uncertainty()/tt.value(),2) + ttSys*ttSys);
  double Fksy = std::sqrt(std::pow(Fake.uncertainty()/Fake.value(),2) + FakeSys*FakeSys);

  double VVsy = std::sqrt(std::pow(VV.uncertainty()/VV.value(),2) + 0.5*0.5);
  double STsy = std::sqrt(std::pow(ST.uncertainty()/ST.value(),2) + 0.5*0.5);
  double DYsy = std::sqrt(std::pow(DY.uncertainty()/DY.value(),2) + 0.5*0.5);
  double TTXsy = std::sqrt(std::pow(TTX.uncertainty()/TTX.value(),2) + 0.5*0.5);

  double totalMC = Wj.value() + tt.value() + Fake.value() + VV.value() + ST.value() + DY.value() + TTX.value();
  double totalMCsy = std::sqrt(Wsy*Wsy + ttsy*ttsy + Fksy*Fksy + VVsy*VVsy + STsy*STsy + DYsy*DYsy + TTXsy*TTXsy);

  std::cout << "WJets: " << reportYieldsUnc(Wj.value(), Wsy) << std::endl;
  std::cout << "TTbar: " << reportYieldsUnc(tt.value(), ttsy) << std::endl;
  std::cout << "Fakes: " << reportYieldsUnc(Fake.value(), Fksy) << std::endl;
  std::cout << "VV: " << reportYieldsUnc(VV.value(), VVsy) << std::endl;
  std::cout << "ST: " << reportYieldsUnc(ST.value(), STsy) << std::endl;
  std::cout << "DY: " << reportYieldsUnc(DY.value(), DYsy) << std::endl;
  std::cout << "TTX: " << reportYieldsUnc(TTX.value(), TTXsy) << std::endl;
  std::cout << "totalMC: " << reportYieldsUnc(totalMC, totalMCsy) << std::endl;

  if(unblind)
  {
  	std::cout << "Data: " << DatainSR << std::endl;
  }

	return;
}

doubleUnc fakeDD(SampleReader &LNTData, SampleReader &LNTMC, std::string signalRegion, std::string mcWeight)
{
  doubleUnc LNTinSR = LNTData.getYield(signalRegion, "weight");
  doubleUnc LNTMCinSR = LNTMC.getYield(signalRegion + " && isPrompt == 1", mcWeight);

  doubleUnc estimate = LNTinSR - LNTMCinSR;

  return estimate;
}

doubleUnc fullDD(ProcessInfo &toEstimate, SampleReader &Data, SampleReader &MC, std::string looseSelection, std::string tightSelection, std::string signalRegion, std::string controlRegion, std::string mcWeight)
{
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

std::string reportYieldsUnc(double Yield, double Unc){
	std::string report;

	report = std::to_string(Yield) + " +/- " + std::to_string(Unc);
  std::cout << "TTX: " << reportYieldsUnc(TTX.value(), TTXsy) << std::endl;

	return report;
}