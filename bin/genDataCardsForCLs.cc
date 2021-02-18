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

doubleUnc fakeDD(SampleReader &, SampleReader &, std::string, std::string)
doubleUnc fullDD(ProcessInfo &, SampleReader &, SampleReader &, std::string, std::string, std::string, std::string, std::string)
void makeBkgDataCard(SampleReader &, SampleReader &, std::string, std::string, std::string preSelection, std::string wjetsEnrich, std::string ttbarEnrich, std::string, std::string, std::string, std::string, std::string){

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
  if(!isPseudoData)
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

  // Create Background DataCards
  makeBkgDataCard();
  // Update Background DataCards with signal yields for Final DataCards


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

// create datacards for background per DM
void makeBkgDataCard(SampleReader &Data, SampleReader &MC, std::string tightSelection, std::string looseSelection, std::string preSelection, std::string wjetsEnrich, std::string ttbarEnrich, std::string controlRegion, std::string signalRegion, std::string wjetsControlRegion, std::string ttbarControlRegion, std::string mcWeight){

	auto wjets = MC.process(bkgMap["WJetsNLO"]);
	auto ttbar = MC.process(bkgMap["ttbar"]);
	//auto zinv = MC.process(bkgMap["ZInv"]);
	//auto qcd = MC.process(bkgMap["QCD"]);

    auto Wj   = fullDD(wjets, Data, MC, looseSelection, tightSelection, preSelection + " && " + signalRegion, preSelection + " && " + wjetsControlRegion, mcWeight);
    auto tt   = fullDD(outputTable, ttbar, Data, MC, looseSelection, tightSelection, preSelection + " && " + signalRegion, preSelection + " && " + ttbarControlRegion, mcWeight);
	auto Fake = fakeDD(Data,MC, looseSelection + " && " + preSelection + " && " + signalRegion, mcWeight);
	auto VV  = MC.process(bkgMap["VV"]);
	auto ST  = MC.process(bkgMap["SingleTop"]);
	auto DY  = MC.process(bkgMap["DYJets"]);
	auto TTX = MC.process(bkgMap["ttx"]);

	std::cout << "Wj:" << Wj << std::endl;
	std::cout << "tt:" << tt << std::endl;
	std::cout << "Fake:" << Fake << std::endl;
	std::cout << "VV:" << VV << std::endl;
	std::cout << "ST:" << ST << std::endl;
	std::cout << "DY:" << DY << std::endl;
	std::cout << "TTX:" << TTX << std::endl;

	return;
} 
// update background datacards per DM per SP



















