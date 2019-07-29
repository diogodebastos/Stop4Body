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
#define _USE_CERN_ROOT
#include "UserCode/Stop4Body/interface/ValueWithSystematics.h"

using json = nlohmann::json;
doubleUnc methodOneDDSystematics(ProcessInfo &, SampleReader &, SampleReader &, std::string, std::string, std::string, std::string, std::string, std::string, std::string);
doubleUnc fakeDD(SampleReader &, SampleReader &, std::string, std::string);
doubleUnc fullDD(ProcessInfo &, SampleReader &, SampleReader &, std::string, std::string, std::string, std::string, std::string);

int main(int argc, char** argv)
{
  std::string jsonFileName = "";
  std::string inputDirectory = "";
  std::string outputDirectory = "./OUT/";
  std::string suffix = "";
  double luminosity = -1.0;
  bool isPseudoData = false;
  bool verbose = false;
  double SRCut = 0.4;
  double CRCut = 0.2;
  bool isHighDM = false;
  bool doVR1 = false; // Swap the Met and LepPt for this VR
  bool doVR2 = false; // Invert the Met for this VR
  bool doVR3 = false; // Invert the LepPt for this VR
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

    if(argument == "--isPseudoData")
    {
      isPseudoData = true;
    }

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

  //std::string baseSelection = "(DPhiJet1Jet2 < 2.5 || Jet2Pt < 60) && (HT > 200) && (Jet1Pt > 110)";
  std::string baseSelection = "(badCloneMuonMoriond2017 == 1) && (badMuonMoriond2017 == 1) && (DPhiJet1Jet2 < 2.5 || Jet2Pt < 60) && (HT > 200) && (Jet1Pt > 110)";
  std::string preSelection = baseSelection + " && (Met > 280) && " + lepSelection;
  std::string wjetsEnrich = "(NbLoose == 0)";
  std::string ttbarEnrich = "(NbTight > 0)";
  baseSelection += " && " + metSelection;
  baseSelection += " && " + lepSelection;
  std::string crSelection = "(BDT < "+ std::to_string(CRCut) +" )";
  std::string srSelection = "(BDT > "+ std::to_string(SRCut) +" )";
  if(doVR1)
    baseSelection += " && " + VR1Trigger;

  auto printSel  = [&](std::string name, std::string selection) -> void
  {
    std::cout << "The used " << name << ":" << std::endl;
    std::cout << "  " << selection << std::endl;
    std::cout << std::endl;
  };

  // crSelection += " && " + baseSelection;
  // srSelection += " && " + baseSelection;
  // wjetsEnrich += " && " + baseSelection;
  // ttbarEnrich += " && " + baseSelection;

  std::string WJetsSR_VR = wjetsEnrich + " && "  + srSelection;

  printSel("base selection", baseSelection);
  printSel("CR selection", crSelection);
  printSel("SR selection", srSelection);
  printSel("wjets enrichment", wjetsEnrich);
  printSel("ttbar enrichment", ttbarEnrich);
  printSel("WJets SR-VR", wjetsEnrich);


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

    if(MC.process(i).tag() == "WJets")
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
    std::cout << bkg.first << "," << bkg.second << std::endl;
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
  std::string mcWeight;
  {
    std::stringstream converter;
    if(isPseudoData)
      converter << "splitFactor*XS*filterEfficiency*(genWeight/sumGenWeight)"; // The scale factors are not considered here
    else
      converter << "splitFactor*weight";
    converter << "*" << luminosity;
    converter >> mcWeight;
  }

  auto wjets = MC.process(bkgMap["WJets"]);
  doubleUnc SysDD = methodOneDDSystematics(wjets, Data, MC, baseSelection, looseSelection, tightSelection, srSelection, crSelection, wjetsEnrich, mcWeight);

  //doubleUnc nDD = fullDD(wjets, Data, MC, looseSelection, tightSelection, baseSelection + " && " + srSelection, baseSelection + " && " + crSelection + " && " + wjetsEnrich, mcWeight); // Do this without setting any VR and you should get DD estiamtion for given process == to AN

  if(verbose)
    std::cout << "Estimate on DD method: " << SysDD << std::endl;
  if(unblind){
    std::cout<<"placeholder"<<std::endl;
  }
}

// Ported from getDDEstimate.cc -> Might want to incorporate this function on commonFunctions.cc

doubleUnc methodOneDDSystematics(ProcessInfo &toEstimate, SampleReader &Data, SampleReader &MC, std::string baseSelection, std::string looseSelection, std::string tightSelection, std::string signalRegion, std::string controlRegion, std::string xEnrich, std::string mcWeight)
{
  doubleUnc DatainSR = Data.getYield(baseSelection + " && " + signalRegion + " && "  + xEnrich, "1.0");

  doubleUnc NDDinSR = fullDD(toEstimate, Data, MC, looseSelection, tightSelection, baseSelection + " && " + signalRegion, baseSelection + " && " + controlRegion + " && " + xEnrich, mcWeight);

  doubleUnc otherMC (0,0);
  for(auto &process: MC)
  {
    if(process.tag() != toEstimate.tag())
      otherMC += process.getYield(baseSelection + " && " + signalRegion + " && " + xEnrich , mcWeight);
  }
  std::cout << "DatainSR: " << DatainSR << std::endl;
  std::cout << "NDDinSR: " << NDDinSR << std::endl;
  std::cout << "otherMC: " << otherMC << std::endl;
  
  doubleUnc sysDD = DatainSR - (NDDinSR + otherMC);

  return sysDD;
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

doubleUnc fakeDD(SampleReader &LNTData, SampleReader &LNTMC, std::string signalRegion, std::string mcWeight)
{
  doubleUnc LNTinSR = LNTData.getYield(signalRegion, "weight");
  doubleUnc LNTMCinSR = LNTMC.getYield(signalRegion + " && isPrompt == 1", mcWeight);

  doubleUnc estimate = LNTinSR - LNTMCinSR;

  return estimate;
}
