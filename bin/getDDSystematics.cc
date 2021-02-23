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

#include "UserCode/Stop4Body-nanoAOD/interface/json.hpp"
#include "UserCode/Stop4Body-nanoAOD/interface/SampleReader.h"
#include "UserCode/Stop4Body-nanoAOD/interface/doubleWithUncertainty.h"
#define _USE_CERN_ROOT
#include "UserCode/Stop4Body-nanoAOD/interface/ValueWithSystematics.h"

using json = nlohmann::json;

doubleUnc getISRsystematicsSignal(SampleReader &, std::string, const ValueWithSystematics<std::string>&, bool);
doubleUnc getISRsystematicsDD(ProcessInfo &, SampleReader &, SampleReader &, std::string, std::string, std::string, std::string, const ValueWithSystematics<std::string>&, bool);

doubleUnc getFRsysClosure(SampleReader &, SampleReader &, std::string, std::string, std::string, std::string, std::string, bool);
doubleUnc getFRsysISR(SampleReader &, SampleReader &, std::string, std::string, std::string, std::string, const ValueWithSystematics<std::string>&, bool);
doubleUnc getFRsysNU(SampleReader &, SampleReader &, std::string, std::string, const double, bool);
//doubleUnc getFRsysNUalt(SampleReader &, SampleReader &, std::string, std::string, std::string, const double);

double methodOneDDSystematics(ProcessInfo &, SampleReader &, SampleReader &, std::string, std::string, std::string, std::string, std::string, std::string, std::string, bool);
double methodTwoDDSystematics(ProcessInfo &, SampleReader &, SampleReader &, std::string, std::string, std::string, std::string, std::string, bool);

doubleUnc fakeDD(SampleReader &, SampleReader &, std::string, std::string, std::string);
doubleUnc fullDD(ProcessInfo &, SampleReader &, SampleReader &, std::string, std::string, std::string, std::string, std::string);
doubleUnc naiveDD(ProcessInfo &, SampleReader &, SampleReader &, std::string, std::string, std::string, std::string, std::string);
void printSel(std::string, std::string);

int main(int argc, char** argv)
{
  std::string jsonFileName = "";
  std::string inputDirectory = "";
  std::string outputDirectory = "./OUT/";
  std::string suffix = "";
  double luminosity = -1.0;
  //bool isPseudoData = false;
  bool verbose = false;
  double SRCut = 0.4;
  double CRCut = 0.2;
  double TESTCut = 0.1;
  bool isHighDM = false;
  bool doVR1 = false; // Swap the Met and LepPt for this VR
  bool doVR2 = false; // Invert the Met for this VR
  bool doVR3 = false; // Invert the LepPt for this VR
  bool doLoosenBDT = false; // SRCut and CRCut == 0.1 -> When BDTcut is too high of VR SR
  bool unblind = false;

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
    }// */

    if(argument == "--json")
      jsonFileName = argv[++i];

    if(argument == "--outDir")
      outputDirectory = argv[++i];

    if(argument == "--inDir")
      inputDirectory = argv[++i];

    if(argument == "--suffix")
      suffix = argv[++i];

    if(argument == "--unblind")
      unblind = true;

    if(argument == "--lumi")
    {
      std::stringstream convert;
      convert << argv[++i];
      convert >> luminosity;
    }
/*
    if(argument == "--isPseudoData")
    {
      isPseudoData = true;
    }
*/
    if(argument == "--signalRegionCut")
    {
      std::stringstream convert;
      convert << argv[++i];
      convert >> SRCut;
    }

    if(argument == "--controlRegionCut")
    {
      std::stringstream convert;
      convert << argv[++i];
      convert >> CRCut;
    }
    if(argument == "--TestCut")
    {
      std::stringstream convert;
      convert << argv[++i];
      convert >> TESTCut;
    }

    if(argument == "--doVR1")
    {
      doVR1 = true;
    }

    if(argument == "--isHighDeltaM")
    {
      isHighDM = true;
    }

    if(argument == "--doVR2")
    {
      doVR2 = true;
    }

    if(argument == "--doVR3")
    {
      doVR3 = true;
    }

    if(argument == "--doLoosenBDT")
    {
      doLoosenBDT = true;
    }

    if(argument == "--verbose")
    {
      verbose = true;
    }
  }

  // Checks on arguments
  if(doVR3 && isHighDM)
  {
    std::cerr << "VR3 is not defined for the high DM region. Please revise your options" << std::endl;
    return 1;
  }

  if((doVR1 && doVR2) || (doVR1 && doVR3) || (doVR2 && doVR3))
  {
    std::cerr << "You can not simultaneously do multiple validation regions" << std::endl;
    return 1;
  }

  if(SRCut < CRCut)
  {
    std::cout << "The SR cut (BDT > " << SRCut << ") overlaps with the CR cut (BDT < " << CRCut << ")" << std::endl;
    std::cout << "Changing the CR cut so that they do not overlap." << std::endl;
    CRCut = SRCut;
  }

  std::cout << "Reading json files" << std::endl;
  SampleReader samples(jsonFileName, inputDirectory, suffix);

  //Defenition of all  the interesting regions
  std::string tightSelection = "(isTight == 1)";
  std::string looseSelection = "(isLoose == 1) && !(isTight == 1)";
  //std::string tightSelection = "(nGoodEl_cutId_loose+nGoodMu_cutId_medium == 1)";
  //std::string looseSelection = "(isLoose == 1) && !(nGoodEl_cutId_loose+nGoodMu_cutId_medium == 1)";
  std::string promptSelection = "(isPrompt == 1)";
  std::string fakeSelection = "!(isPrompt == 1)";
  std::string VR1Trigger = "(HLT_Mu == 1)";

  std::string metSelection = "";
  if(doVR2)
    metSelection = "(Met > 200) && (Met  < 280)";
  else
    metSelection = "(Met > 280)";

  std::string lepSelection = "(1)";
  if(isHighDM)
  {
    if(doVR1)
      lepSelection = "(LepPt < 280)";
  }
  else
  {
    if(doVR3)
      lepSelection = "(LepPt > 30)";
    else
      lepSelection = "(LepPt < 30)";
  }

  std::string baseSelection = "(DPhiJet1Jet2 < 2.5 || Jet2Pt < 60) && (HT > 200) && (Jet1Pt > 110)";
  //std::string baseSelection = "(METFilters==1) && (DPhiJet1Jet2 < 2.5 || Jet2Pt < 60) && (HT > 200) && (Jet1Pt > 110)";
  //std::string baseSelection = "(DPhiJet1Jet2 < 2.5 || Jet2Pt < 60) && (HT > 200) && (Jet1Pt > 110)";
  //std::string baseSelection = "(badCloneMuonMoriond2017 == 1) && (badMuonMoriond2017 == 1) && (DPhiJet1Jet2 < 2.5 || Jet2Pt < 60) && (HT > 200) && (Jet1Pt > 110)";
  std::string preSelection = baseSelection + " && (Met > 280) && " + lepSelection;
  std::string wjetsEnrich = "(NbLoose == 0)";
  std::string ttbarEnrich = "(NbTight > 0)";
  baseSelection += " && " + metSelection;
  baseSelection += " && " + lepSelection;
  std::string crSelection = "(BDT < "+ std::to_string(CRCut) +" )";
  std::string srSelection = "(BDT > "+ std::to_string(SRCut) +" )";
  std::string testCrSelection = "(BDT < "+ std::to_string(TESTCut) +" )";
  std::string testSrSelection = "(BDT > "+ std::to_string(TESTCut) +" )";
  if(doVR1)
    baseSelection += " && " + VR1Trigger;

  // crSelection += " && " + baseSelection;
  // srSelection += " && " + baseSelection;
  // wjetsEnrich += " && " + baseSelection;
  // ttbarEnrich += " && " + baseSelection;
/*
  if(verbose) {
    printSel("base selection", baseSelection);
    printSel("CR selection", crSelection);
    printSel("SR selection", srSelection);
    printSel("wjets enrichment", wjetsEnrich);
    printSel("ttbar enrichment", ttbarEnrich);
    printSel("WJets SR-VR", wjetsEnrich);
  }
*/
  if(verbose)
    std::cout << "Splitting samples according to type" << std::endl;
  auto MC = samples.getMCBkg();
  auto Sig = samples.getMCSig();
  auto Data = samples.getData();

  if(verbose)
    std::cout << "Building background process map" << std::endl;
  std::map<std::string, size_t> bkgMap;
  bool foundTTbar = false, foundWJets = false;
  for(size_t i = 0; i < MC.nProcesses(); ++i)
  {
    if(MC.process(i).tag().find("ttbar") != std::string::npos)
    {
      bkgMap["ttbar"] = i;
      foundTTbar = true;
    }
    else
      bkgMap[MC.process(i).tag()] = i;
    if(MC.process(i).tag() == "WJetsNLO")
    //if(MC.process(i).tag() == "WJets")
      foundWJets = true;
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

  if(verbose)
    std::cout << "Preparing to compute all the DD systematics" << std::endl;

  if(verbose)
    std::cout << "Doing method 1 DD systematics" << std::endl;

  for(auto& bkg : bkgMap)
  {
    //std::cout << bkg.first << "," << bkg.second << std::endl;
    // if(!(bkg.first == "WJets"))
    // {
    // }
    // if(!(bkg.first == "ttbar"))
    // {
    // }
  }
  if(verbose)
    std::cout << "Retrieving luminosity" << std::endl;
  if(luminosity <= 0)
    luminosity = Data.getLumi();
  // Comment this part for debugging of FRsys
/*
  std::string mcWeight;
  {
    std::stringstream converter;
    if(isPseudoData)
      //converter << "splitFactor*XS*filterEfficiency*(genWeight/sumGenWeight)"; // The scale factors are not considered here
      converter << "splitFactor*XS*filterEfficiency*(genWeight/sumGenWeight)*puWeight"; // The scale factors are not considered here
    else
      converter << "splitFactor*weight";
    converter << "*" << luminosity;
    converter >> mcWeight;
  }
*/
  auto wjets = MC.process(bkgMap["WJetsNLO"]);
  //auto wjets = MC.process(bkgMap["WJets"]);
  auto ttbar = MC.process(bkgMap["ttbar"]);

  // Load Systematics
  std::vector<std::string> systBase;
  {
    systBase.push_back("ISRweight_Bin1");
    systBase.push_back("ISRweight_Bin2");
    systBase.push_back("ISRweight_Bin3");
    systBase.push_back("ISRweight_Bin4");
    systBase.push_back("ISRweight_Bin5");
    systBase.push_back("ISRweight_Bin6");
    systBase.push_back("normSTandHTweight");
    /*
    systBase.push_back("EWKISRweight_Bin1");
    systBase.push_back("EWKISRweight_Bin2");
    systBase.push_back("EWKISRweight_Bin3");
    systBase.push_back("EWKISRweight_Bin4");
    systBase.push_back("EWKISRweight_Bin5");
    systBase.push_back("EWKISRweight_Bin6");
    systBase.push_back("EWKISRweight_Bin7");
    */
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

  if(verbose){
    std::cout << "Using mcWeight: " << mcWeight.Value() << std::endl;
    std::cout << "With variations:" << std::endl;
    for(auto& syst: systematics)
    {
      std::cout << "  " << syst << " - " << mcWeight.Systematic(syst) << std::endl;
    }
    std::cout << std::endl;
  }


  if(doVR2 || doVR3){
  //Systematics for the DD methods of WJets and TTbar
    if(verbose){
      std::cout << "Doing DD sys" << std::endl;
    }
    srSelection = "(BDT > 0.2)";
    //methodOneDDSystematics(wjets, Data, MC, baseSelection, looseSelection, tightSelection, srSelection, crSelection, wjetsEnrich, mcWeight.Value(), verbose);
    //methodTwoDDSystematics(wjets, Data, MC, baseSelection, srSelection, crSelection, wjetsEnrich, mcWeight.Value(), verbose);
    if (doLoosenBDT){
      srSelection = "(BDT > 0.1)";
      crSelection = "(BDT < 0.1)";
      srSelection = testSrSelection;
      crSelection = testCrSelection;
    }
    methodOneDDSystematics(wjets, Data, MC, baseSelection, looseSelection, tightSelection, srSelection, crSelection, wjetsEnrich, mcWeight.Value(), verbose);
    methodTwoDDSystematics(wjets, Data, MC, baseSelection, srSelection, crSelection, wjetsEnrich, mcWeight.Value(), verbose);

    methodOneDDSystematics(ttbar, Data, MC, baseSelection, looseSelection, tightSelection, srSelection, crSelection, ttbarEnrich, mcWeight.Value(), verbose);
    methodTwoDDSystematics(ttbar, Data, MC, baseSelection, srSelection, crSelection, ttbarEnrich, mcWeight.Value(), verbose);

    std::cout << std::endl;
  }
  else{
    // SYS ISR
    if(verbose){
      std::cout << "Doing ISR sys variations" << std::endl;
    }

//    getISRsystematicsSignal(Sig, preSelection + " && " + srSelection, mcWeight,verbose);

//    getISRsystematicsDD(ttbar, Data, MC, looseSelection, tightSelection, preSelection + " && " + srSelection, preSelection + " && " + crSelection + " && " + ttbarEnrich, mcWeight, verbose);

//    getISRsystematicsDD(wjets, Data, MC, looseSelection, tightSelection, preSelection + " && " + srSelection, preSelection + " && " + crSelection + " && " + wjetsEnrich, mcWeight, verbose);

    // SYS Fake-Rate
    if(verbose){
      std::cout << "Doing Fake-Rate systematics" << std::endl;
    }
    // SysFR 1) Closure test for the prediction of the background with a non-prompt lepton

    getFRsysClosure(Data, MC, looseSelection, tightSelection, fakeSelection, preSelection + "&&" + srSelection, mcWeight.Value(), verbose);
/*
    std::string tightSelectionMu = "(nGoodEl == 0 && nGoodMu_cutId_medium == 1)";
    //std::string looseSelectionMu = "(isLoose == 1) && !(isTight == 1) && nGoodEl == 0 && nGoodMu == 1";
    std::string looseSelectionMu = "(isLoose == 1) && !(isTight) && nGoodEl == 0";
    std::string tightSelectionEl = "(nGoodEl_cutId_loose == 1 && nGoodMu == 0)";
    std::string looseSelectionEl = "(isLoose == 1) && !(isTight) && nGoodMu == 0";
    //std::string looseSelectionEl = "(isLoose == 1) && !(isTight == 1) && nGoodEl == 1 && nGoodMu == 0";
    
    std::cout << "Mu closure" << std::endl;
    getFRsysClosure(Data, MC, looseSelectionMu, tightSelectionMu, fakeSelection, preSelection + "&&" + srSelection, mcWeight.Value(), verbose);
    std::cout << "El closure" << std::endl;
    getFRsysClosure(Data, MC, looseSelectionEl, tightSelectionEl, fakeSelection, preSelection + "&&" + srSelection, mcWeight.Value(), verbose);
*/
    // SysFR 2) ISR on fakes prediction

    getFRsysISR(Data, MC, looseSelection, tightSelection, fakeSelection, preSelection + "&&" + srSelection, mcWeight, verbose);

    // SysFR 3) Non-universality

    getFRsysNU(Data, MC, looseSelection, preSelection + "&&" + srSelection, luminosity, verbose);
  }

  if(unblind){
    std::cout<<"placeholder"<<std::endl;
  }
}

// Ported from getDDEstimate.cc -> Might want to incorporate this function on commonFunctions.cc
double methodOneDDSystematics(ProcessInfo &toEstimate, SampleReader &Data, SampleReader &MC, std::string baseSelection, std::string looseSelection, std::string tightSelection, std::string signalRegion, std::string controlRegion, std::string xEnrich, std::string mcWeight, bool verbose)
{
  std::string SR = signalRegion + " && " + tightSelection + " && " + xEnrich  + " && " + baseSelection;
  std::string CR = controlRegion + " && " + tightSelection + " && " + xEnrich  + " && " + baseSelection;

  doubleUnc NinSR = toEstimate.getYield(SR, mcWeight);
  doubleUnc NinCR = toEstimate.getYield(CR, mcWeight);
  doubleUnc DatainSR = Data.getYield(SR, "1.0");
  doubleUnc DatainCR = Data.getYield(CR, "1.0");
  doubleUnc otherMC (0,0);
  doubleUnc otherMCinSR (0,0);
  doubleUnc NPredinSR (0,0);
  doubleUnc NDDinSR = naiveDD(toEstimate, Data, MC, baseSelection, signalRegion, controlRegion, xEnrich, mcWeight);
  //doubleUnc NDDinSR = fullDD(toEstimate, Data, MC, looseSelection, tightSelection, baseSelection + " && " + signalRegion, baseSelection + " && " + controlRegion + " && " + xEnrich, mcWeight);
  doubleUnc rareMC (0,0);
  doubleUnc contamMC (0,0);

  for(auto &process: MC)
  {
    if(process.tag() != toEstimate.tag()){
      otherMC += process.getYield(CR, mcWeight);
      otherMCinSR += process.getYield(SR, mcWeight);
    }
    if((process.tag() != "WJetsNLO") && (process.tag().find("ttbar") == std::string::npos)){
      rareMC += process.getYield(SR, mcWeight);
      //std::cout << process.tag() << ": " << process.getYield(SR, mcWeight) << std::endl;
    }
    if((toEstimate.tag() == "WJetsNLO") && (process.tag().find("ttbar") != std::string::npos)){
      contamMC = process.getYield(SR, mcWeight);
      //std::cout << "Getting contamination Process" << std::endl;
      //std::cout << process.tag() << ": " << contamMC << std::endl;
    }
    if((toEstimate.tag().find("ttbar") != std::string::npos) && (process.tag() == "WJetsNLO")){
      contamMC = process.getYield(SR, mcWeight);
      //std::cout << "Getting contamination Process" << std::endl;
      //std::cout << process.tag() << ": " << contamMC << std::endl;
    }
  }

//  std::cout << "DatainSR Unc : " << DatainSR. << std::endl;
  NPredinSR = NDDinSR + otherMCinSR;

  doubleUnc sysDD = DatainSR - NPredinSR;
  doubleUnc precDD = std::sqrt(std::pow(DatainSR.uncertainty(),2) + std::pow(0.5*rareMC.value(),2) + std::pow(0.2*contamMC.value(),2));
  doubleUnc SysUnDD = std::sqrt(std::max(std::pow(sysDD.value(),2),std::pow(precDD.value(),2)));
  doubleUnc RelSysDD = SysUnDD/NDDinSR;

  if(verbose){
    std::cout << std::endl;
    std::cout << "/* Method 1 */" << std::endl;
    std::cout << "==== " << toEstimate.label() << " ====" <<std::endl;
    printSel("CR selection", controlRegion);
    printSel("SR selection", signalRegion);
    std::cout << "NinCR: " << NinCR << std::endl;
    std::cout << "NinSR: " << NinSR << std::endl;
    std::cout << "DatainCR: " << DatainCR << std::endl;
    std::cout << "DatainSR: " << DatainSR << std::endl;
    std::cout << "otherMCinCR: " << otherMC << std::endl;
    std::cout << "otherMCinSR: " << otherMCinSR << std::endl;
    std::cout << "NDDinSR: " << NDDinSR << std::endl;
    std::cout << "NPredinSR: " << NPredinSR << std::endl;
    std::cout << std::endl;
    std::cout << "sysDD: " << sysDD << std::endl;
    std::cout << "precDD: " << precDD << std::endl;
    std::cout << "SysUnDD: " << SysUnDD << std::endl;
    std::cout << "RelSysDD: " << RelSysDD.value()*100 << std::endl;
    std::cout << std::endl;
  }
  return RelSysDD.value()*100;
}

double methodTwoDDSystematics(ProcessInfo &toEstimate, SampleReader &Data, SampleReader &MC, std::string baseSelection, std::string signalRegion, std::string controlRegion, std::string xEnrich, std::string mcWeight, bool verbose){
  std::string SR = signalRegion + " && (isTight ==1) && " + xEnrich  + " && " + baseSelection;
  std::string CR = controlRegion + " && (isTight ==1) && " + xEnrich  + " && " + baseSelection;
  //std::string SR = signalRegion + " && (nGoodEl_cutId_loose+nGoodMu_cutId_medium == 1) && " + xEnrich  + " && " + baseSelection;
  //std::string CR = controlRegion + " && (nGoodEl_cutId_loose+nGoodMu_cutId_medium == 1) && " + xEnrich  + " && " + baseSelection;

  doubleUnc NinSR = toEstimate.getYield(SR, mcWeight);
  doubleUnc NinCR = toEstimate.getYield(CR, mcWeight);
  doubleUnc DatainSR = Data.getYield(SR, "1.0");
  doubleUnc DatainCR = Data.getYield(CR, "1.0");
  doubleUnc otherMCinSR (0,0);
  doubleUnc otherMCinCR (0,0);

  for(auto &process: MC)
  {
    if(process.tag() != toEstimate.tag()){
      otherMCinSR += process.getYield(SR, mcWeight);
      otherMCinCR += process.getYield(CR, mcWeight);
    }
  }

  doubleUnc RinSR = (DatainSR - otherMCinSR) / NinSR;
  doubleUnc RinCR = (DatainCR - otherMCinCR) / NinCR;

  doubleUnc D = RinSR - RinCR;
  doubleUnc NDDinSR = naiveDD(toEstimate, Data, MC, baseSelection, signalRegion, controlRegion, xEnrich, mcWeight);

  double diffD = std::pow(D.value(),2) - std::pow(D.uncertainty(),2);
  doubleUnc SysUnDD = std::sqrt(std::max(diffD, std::pow(NDDinSR.uncertainty(),2)));

  doubleUnc RelSysDD = SysUnDD/NDDinSR;

  if(verbose){
    std::cout << std::endl;
    std::cout << "/* Method 2 */" << std::endl;
    std::cout << "==== " << toEstimate.label() << " ====" <<std::endl;
    printSel("CR selection", CR);
    printSel("SR selection", SR);
    std::cout << "NinCR: " << NinCR << std::endl;
    std::cout << "NinSR: " << NinSR << std::endl;
    std::cout << "DatainCR: " << DatainCR << std::endl;
    std::cout << "DatainSR: " << DatainSR << std::endl;
    std::cout << "otherMCinCR: " << otherMCinCR << std::endl;
    std::cout << "otherMCinSR: " << otherMCinSR << std::endl;
    std::cout << std::endl;
    std::cout << "RinCR: " << RinCR << std::endl;
    std::cout << "RinSR: " << RinSR << std::endl;
    std::cout << "D: " << D << std::endl;
    std::cout << "diffD: " << diffD << std::endl;
    std::cout << "NDDinSR: " << NDDinSR << std::endl;
    std::cout << "NDDinSRUncPow2: " << std::pow(NDDinSR.uncertainty(),2) << std::endl;
    std::cout << "SysUnDD: " << SysUnDD << std::endl;
    std::cout << "RelSysDD: " << RelSysDD.value()*100 << std::endl;
    std::cout << std::endl;
  }

  return RelSysDD.value()*100;
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
  doubleUnc fakes = fakeDD(Data, MC, looseSelection + " && " + controlRegion, "weight", mcWeight);

  doubleUnc estimate = NinSR/NinCR * (DatainCR - otherMC - fakes);

  return estimate;
}

doubleUnc fakeDD(SampleReader &LNTData, SampleReader &LNTMC, std::string signalRegion, std::string dataWeight ,std::string mcWeight)
{
  doubleUnc LNTinSR = LNTData.getYield(signalRegion, dataWeight);
  doubleUnc LNTMCinSR = LNTMC.getYield(signalRegion + " && isPrompt == 1", mcWeight);

  doubleUnc estimate = LNTinSR - LNTMCinSR;

  return estimate;
}

doubleUnc naiveDD(ProcessInfo &toEstimate, SampleReader &Data, SampleReader &MC, std::string baseSelection, std::string signalRegion, std::string controlRegion, std::string xEnrich, std::string mcWeight)
{
  std::string fakeSelection = "(isLoose == 1) && !(isTight == 1) && " + baseSelection + " && " + signalRegion + " && " + xEnrich;
  signalRegion += " && (isTight==1) && " + xEnrich + " && " + baseSelection;
  controlRegion += " && (isTight==1) && " + xEnrich + " && " + baseSelection;

  doubleUnc NinSR = toEstimate.getYield(signalRegion, mcWeight);
  doubleUnc NinCR = toEstimate.getYield(controlRegion, mcWeight);
  doubleUnc DatainCR = Data.getYield(controlRegion, "1.0");
  doubleUnc otherMC (0,0);
  // for debuggind
  /*
  doubleUnc DatainSR = Data.getYield(signalRegion, "1.0");
  doubleUnc otherSingleProcessMCinSR (0,0);
  doubleUnc otherMCinSR (0,0);
  doubleUnc otherMCinSRprompt (0,0);
  */
  if(static_cast<double>(NinSR) == 0)
    NinSR = doubleUnc(4,2);

  // std::cout << "== Printing other MC processes Yield for debug: " << std::endl;
  for(auto &process: MC)
  {
    if(process.tag() != toEstimate.tag()){
      otherMC += process.getYield(controlRegion, mcWeight);
      /*
      otherSingleProcessMCinSR = process.getYield(signalRegion, mcWeight);
      std::cout << process.tag() << ": " << otherSingleProcessMCinSR << std::endl;
      otherMCinSR += otherSingleProcessMCinSR;
      otherMCinSRprompt += process.getYield("(isPrompt == 1) && " + signalRegion, mcWeight);
      */
    }
  }

  doubleUnc estimate = NinSR/NinCR * (DatainCR - otherMC);
  /*
  doubleUnc fakes = fakeDD(Data, MC, fakeSelection, mcWeight);
  doubleUnc otherMCinSRwithFakesAndPrompt = otherMCinSRprompt + fakes;

  std::cout << std::endl;
  std::cout << "==== " << toEstimate.label() << " ===="  <<std::endl;
  printSel("CR selection", controlRegion);
  printSel("SR selection", signalRegion);
  std::cout << "NinSR: " << NinSR << std::endl;
  std::cout << "NinCR: " << NinCR << std::endl;
  std::cout << "DatainCR: " << DatainCR << std::endl;
  std::cout << "DatainSR: " << DatainSR << std::endl;
  std::cout << "otherMCinCR: " << otherMC << std::endl;
  std::cout << "otherMCinSR: " << otherMCinSR << std::endl;
  std::cout << "fakes: " << fakes << std::endl;
  std::cout << "otherMCinSR(prompt + DD fakes): " << otherMCinSRwithFakesAndPrompt << std::endl;
  std::cout << "estimate: " << estimate << std::endl;
  std::cout << std::endl;
  */
  return estimate;
}

doubleUnc getISRsystematicsDD(ProcessInfo &toEstimate, SampleReader &Data, SampleReader &MC, std::string looseSelection, std::string tightSelection, std::string signalRegion, std::string controlRegion, const ValueWithSystematics<std::string>& mcWeight, bool verbose){


  doubleUnc xDDCentral = fullDD(toEstimate, Data, MC, looseSelection, tightSelection, signalRegion, controlRegion, mcWeight.Value());


  doubleUnc xDDVar;
  doubleUnc diff;
  doubleUnc relSys;

  if(verbose){
    std::cout << "/* ISR Systematics */" << std::endl;
    std::cout << "==== " << toEstimate.label() << " ====" <<std::endl;
    std::cout << "xDDCentral: " << xDDCentral <<std::endl;
  }

  for(auto& syst: mcWeight.Systematics())
  {
    xDDVar = fullDD(toEstimate, Data, MC, looseSelection, tightSelection, signalRegion, controlRegion, mcWeight.Systematic(syst));
    diff = xDDVar-xDDCentral;
    relSys = diff/xDDCentral;

    if(verbose){
//    std::cout << "  " << syst << " - " << mcWeight.Systematic(syst) << std::endl;
      std::cout << " > " << syst  << std::endl;
      std::cout << "  xDDVar: " << xDDVar <<std::endl;
      std::cout << "  = relSys: " << relSys.value()*100 <<std::endl;
    }
  }
  if(verbose){
    std::cout << "" <<std::endl;
  }
  return relSys;
}

doubleUnc getISRsystematicsSignal(SampleReader &Sig, std::string signalRegion, const ValueWithSystematics<std::string>& mcWeight, bool verbose)
{
  doubleUnc relSys = 0;
  if(verbose){
    std::cout << "/* Signal ISR Systematics */" << std::endl;
  }
  for(auto& process : Sig)
  {
    doubleUnc sigCentral = process.getYield(signalRegion, mcWeight.Value());
    doubleUnc sigVar;
    doubleUnc diff;

    if(verbose){
      std::cout << "==== " << process.label() << " ====" <<std::endl;
      std::cout << "sigCentral: " << sigCentral <<std::endl;

    }

    for(auto& syst: mcWeight.Systematics())
    {
      sigVar = process.getYield(signalRegion, mcWeight.Systematic(syst));
      diff = sigVar-sigCentral;
      relSys = diff/sigCentral;
      // = sigVar/sigCentral
      if(verbose){
        //std::cout << "  " << syst << " - " << mcWeight.Systematic(syst) << std::endl;
        std::cout << " > " << syst << std::endl;
        std::cout << "  sigVar: " << sigVar <<std::endl;
        std::cout << "  = relSys: " << relSys.value()*100 <<std::endl;
      }
    }
    if(verbose){
      std::cout << "" <<std::endl;
    }
  }
  return relSys;
}

doubleUnc getFRsysClosure(SampleReader &Data, SampleReader &MC, std::string looseSelection, std::string tightSelection, std::string nonPrompt, std::string signalRegion, std::string mcWeight, bool verbose)
{
  doubleUnc NTightNonPrompt = MC.getYield(tightSelection + "&&" + signalRegion + "&&" + nonPrompt, mcWeight);

  doubleUnc NDDnonPromptMC = MC.getYield(looseSelection + "&&" + signalRegion + "&&" + nonPrompt, mcWeight);

  doubleUnc diff = NDDnonPromptMC-NTightNonPrompt;
  doubleUnc sigDiff = std::sqrt(std::pow(NTightNonPrompt.uncertainty(),2)+std::pow(NDDnonPromptMC.uncertainty(),2));
  //doubleUnc SysClosure = std::sqrt(std::max(std::pow(diff.value(),2)-std::pow(sigDiff.value(),2),0));
  double diffSquare = std::pow(diff.value(),2)-std::pow(sigDiff.value(),2);
  doubleUnc SysClosure = std::sqrt(std::max(diffSquare ,0.0));
  doubleUnc relSys = SysClosure/NDDnonPromptMC;
  

  //doubleUnc fakes = fakeDD(Data, MC, looseSelection + " && " + signalRegion, mcWeight);

  if(verbose){
    std::cout << "/* Fake-rate Systematics: Non-Closure */" << std::endl;
    std::cout << "NTightNonPrompt: " << NTightNonPrompt <<std::endl;
    std::cout << "NDDnonPromptMC: " << NDDnonPromptMC <<std::endl;
    std::cout << "diff: " << diff <<std::endl;
    std::cout << "sigDiff: " << sigDiff <<std::endl;
    std::cout << "diffSquare: " << diffSquare <<std::endl;
    std::cout << "SysClosure: " << SysClosure <<std::endl;
    std::cout << "relSys: " << relSys.value()*100 <<std::endl;
    std::cout << "" <<std::endl;
  }
  return relSys;
}

doubleUnc getFRsysISR(SampleReader &Data, SampleReader &MC, std::string looseSelection, std::string tightSelection, std::string nonPrompt, std::string signalRegion, const ValueWithSystematics<std::string>& mcWeight, bool verbose)
{
  doubleUnc relSys = 0;

  doubleUnc fakesCentral = fakeDD(Data, MC, looseSelection + " && " + signalRegion, "weight",mcWeight.Value());

  doubleUnc fakesVar;
  doubleUnc diff;

  if(verbose){
    std::cout << "/* Fake-rate Systematics: ISR variations */" << std::endl;
    std::cout << "fakesCentral: " << fakesCentral <<std::endl;
  }

  for(auto& syst: mcWeight.Systematics())
  {
    fakesVar = fakeDD(Data, MC, looseSelection + " && " + signalRegion,"weight", mcWeight.Systematic(syst));
    diff = fakesVar-fakesCentral;
    relSys = diff/fakesCentral;
    if(verbose){
    //std::cout << "  " << syst << " - " << mcWeight.Systematic(syst) << std::endl;
      std::cout << " > " << syst << std::endl;
      std::cout << "  fakesVar: " << fakesVar <<std::endl;
      std::cout << "  = relSys: " << relSys.value()*100 <<std::endl;
    }
  }
  if(verbose){
    std::cout << "" <<std::endl;
  }
  return relSys;
}

doubleUnc getFRsysNU(SampleReader &Data, SampleReader &MC, std::string looseSelection, std::string signalRegion, const double luminosity, bool verbose){
  std::cout << "/* Fake-rate Systematics: Non-universality */" << std::endl;
  doubleUnc relSys = 0;

  // Load Systematics
  std::vector<std::string> systBase;
  {
    for(int i = 1; i <= 5; ++i)
    {
      std::stringstream converter;
      std::string tmp;
      // 2016
      converter << "TightLoose_NU_Bin" << i;
      // 2017
      //converter << "2017_TightLoose_NU_Bin" << i;
      converter >> tmp;
      systBase.push_back(tmp);
    }
    // 2016
    systBase.push_back("TightLoose_NU_AltCorr");
    //systBase.push_back("2017_TightLoose_NU_AltCorr");
  }

  std::vector<std::string> systematics;
  for(auto& base: systBase)
  {
    systematics.push_back(base + "_Up");
    systematics.push_back(base + "_Down");
  }

  auto valueLoop = systematics;
  valueLoop.push_back("CentralValue");

  ValueWithSystematics<std::string> dataWeight = std::string("weight");

  ValueWithSystematics<std::string> mcWeight = std::string("splitFactor*weight");

  for(auto& syst: systematics){
    dataWeight.Systematic(syst) = dataWeight.Value() + "_" + syst;
    mcWeight.Systematic(syst) = mcWeight.Value() + "_" + syst;
  }
  {
    std::stringstream converter;
    converter << "*" << luminosity;
    mcWeight += converter.str();
  }

  /* For DEBUG
  if(verbose){
    std::cout << "Using mcWeight: " << mcWeight.Value() << std::endl;
    std::cout << "With variations:" << std::endl;
    for(auto& syst: systematics)
    {
      std::cout << "  " << syst << " - " << dataWeight.Systematic(syst) << "  |  "<< mcWeight.Systematic(syst) << std::endl;
    }
    std::cout << std::endl;
  }
  */
  doubleUnc fakesCentral = fakeDD(Data, MC, looseSelection + " && " + signalRegion, dataWeight.Value(),mcWeight.Value());
  doubleUnc fakesVar;
  doubleUnc diff;

  if(verbose){
    std::cout << "fakesCentral: " << fakesCentral <<std::endl;
  }

  for(auto& syst: mcWeight.Systematics())
  {
    fakesVar = fakeDD(Data, MC, looseSelection + " && " + signalRegion,dataWeight.Systematic(syst) , mcWeight.Systematic(syst));
    std::cout << "  fakesVar: " << fakesVar <<std::endl;
    diff = fakesVar-fakesCentral;
    relSys = diff/fakesCentral;
    if(verbose){
    //std::cout << "  " << syst << " - " << mcWeight.Systematic(syst) << std::endl;
      std::cout << " > " << syst << std::endl;
      std::cout << "  = relSys: " << relSys.value()*100 <<std::endl;
    }
  }
  if(verbose){
    std::cout << "" <<std::endl;
  }
  return relSys;
}

/*
doubleUnc getFRsysNUalt(SampleReader &Data, SampleReader &MC, std::string looseSelection, std::string signalRegion, std::string altRegion,const double luminosity){
  doubleUnc relSys = 0;

  // Load Systematics
  std::vector<std::string> systBase;
  {
    for(int i = 1; i <= 5; ++i)
    {
      std::stringstream converter;
      std::string tmp;
      converter << "TightLoose_NU_Bin" << i;
      converter >> tmp;
      systBase.push_back(tmp);
    }
    systBase.push_back("TightLoose_NU_AltCorr");
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
  {
    std::cout << "  " << syst << " - " << mcWeight.Systematic(syst) << std::endl;
  }
  std::cout << std::endl;

  doubleUnc fakesCentral = fakeDD(Data, MC, looseSelection + " && " + signalRegion, mcWeight.Value());

  std::cout << "fakesCentral: " << fakesCentral <<std::endl;

  doubleUnc fakesVar;
  doubleUnc diff;

  for(auto& syst: mcWeight.Systematics())
  {
    std::cout << "  " << syst << " - " << mcWeight.Systematic(syst) << std::endl;

    fakesVar = fakeDD(Data, MC, looseSelection + " && " + signalRegion + " && " + altRegion, mcWeight.Systematic(syst));
    std::cout << "  fakesVar: " << fakesVar <<std::endl;
    diff = fakesVar-fakesCentral;
    relSys = diff/fakesCentral;

    std::cout << " = relSys: " << relSys.value()*100 <<std::endl;
  }
  std::cout << "" <<std::endl;
  return relSys;
}
*/

void printSel(std::string name, std::string selection)
{
  std::cout << "The used " << name << ":" << std::endl;
  std::cout << "  " << selection << std::endl;
  std::cout << std::endl;
  return;
}
