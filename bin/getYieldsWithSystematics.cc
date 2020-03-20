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

#include "UserCode/Stop4Body-nanoAOD//interface/json.hpp"
#include "UserCode/Stop4Body-nanoAOD//interface/SampleReader.h"
#include "UserCode/Stop4Body-nanoAOD//interface/doubleWithUncertainty.h"
#define _USE_CERN_ROOT
#include "UserCode/Stop4Body-nanoAOD//interface/ValueWithSystematics.h"

using json = nlohmann::json;

class NullBuffer : public std::streambuf
{
  public:
    int overflow(int c) { return c; }
};

class NullStream : public std::ostream
{
  public:
    NullStream():std::ostream(&m_sb) {}
  private:
    NullBuffer m_sb;
};

int main(int argc, char** argv)
{
  std::string jsonFileName = "";
  std::string inputDirectory = "";
  std::string outputDirectory = "./OUT/";
  std::string suffix = "";
  double luminosity = -1.0;
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

  gStyle->SetOptStat(000000);
  gStyle->SetOptTitle(0);

  std::cout << "Reading json files" << std::endl;
  SampleReader samples(jsonFileName, inputDirectory, suffix);

  std::string tightSelection = "(isTight == 1)";
  std::string looseSelection = "(isLoose == 1) && !(isTight == 1)";
  std::string promptSelection = "(isPrompt == 1)";
  std::string fakeSelection = "!(isPrompt == 1)";
  std::string VR1Trigger = "(HLT_Mu == 1)";

  // Build selection strings, with the systematic variations
  ValueWithSystematics<std::string> Met          = std::string("Met");
  ValueWithSystematics<std::string> DPhiJet1Jet2 = std::string("DPhiJet1Jet2");
  ValueWithSystematics<std::string> Jet1Pt       = std::string("Jet1Pt");
  ValueWithSystematics<std::string> Jet2Pt       = std::string("Jet2Pt");
  ValueWithSystematics<std::string> HT           = std::string("HT");
  ValueWithSystematics<std::string> NbLoose      = std::string("NbLoose");
  ValueWithSystematics<std::string> NbTight      = std::string("NbTight");
  ValueWithSystematics<std::string> BDT          = std::string("BDT");
  ValueWithSystematics<std::string> LepPt        = std::string("LepPt");
  { // Loading the systematic variations of the quantities above
    std::vector<std::string> variations = {"JES_Up", "JES_Down", "JER_Up", "JER_Down"};
    for(auto& syst : variations)
    {
      Met.Systematic(syst)          = Met.Value() + "_" + syst;
      DPhiJet1Jet2.Systematic(syst) = DPhiJet1Jet2.Value() + "_" + syst;
      Jet1Pt.Systematic(syst)       = Jet1Pt.Value() + "_" + syst;
      Jet2Pt.Systematic(syst)       = Jet2Pt.Value() + "_" + syst;
      HT.Systematic(syst)           = HT.Value() + "_" + syst;
      NbLoose.Systematic(syst)      = NbLoose.Value() + "_" + syst;
      NbTight.Systematic(syst)      = NbTight.Value() + "_" + syst;
      BDT.Systematic(syst)          = BDT.Value() + "_" + syst;
    }
  }

  ValueWithSystematics<std::string> metSelection = std::string("(");
  if(doVR2)
    metSelection += Met + " > 200 && " + Met + " < 280)";
  else
    metSelection += Met + " > 280)";

  ValueWithSystematics<std::string> lepSelection = std::string("(");
  if(isHighDM)
  {
    if(doVR1)
      lepSelection += LepPt + " < 280";
    else
      lepSelection += "1)";
  }
  else
  {
    if(doVR3)
      lepSelection += LepPt + " > 30)";
    else
      lepSelection += LepPt + " < 30)";
  }

  ValueWithSystematics<std::string> wjetsEnrich = std::string("(");
  wjetsEnrich += NbLoose + " == 0)";

  ValueWithSystematics<std::string> ttbarEnrich = std::string("(");
  ttbarEnrich += NbTight + " > 0)";

  ValueWithSystematics<std::string> crSelection = std::string("(");
  {
    std::stringstream converter;
    converter << CRCut;
    crSelection += BDT + " < " + converter.str() + ")";
  }

  ValueWithSystematics<std::string> srSelection = std::string("(");
  {
    std::stringstream converter;
    converter << SRCut;
    srSelection += BDT + " > " + converter.str() + ")";
  }

  ValueWithSystematics<std::string> baseSelection = std::string("(");
  baseSelection += DPhiJet1Jet2 + " < 2.5 || " + Jet2Pt + " < 60) && (" + HT + " > 200) && (" + Jet1Pt + " > 110) && ";
  baseSelection += metSelection + " && ";
  baseSelection += lepSelection;
  if(doVR1)
    baseSelection += " && " + VR1Trigger;

  auto printSel = [&](std::string name, ValueWithSystematics<std::string> selection) -> void
  {
    std::cout << "The used " << name << ":" << std::endl;
    std::cout << "  " << selection.Value() << std::endl;
    for(auto& syst : selection.Systematics())
      std::cout << "   - " << syst << ": " << selection.Systematic(syst) << std::endl;
    std::cout << std::endl;
    return;
  };

  printSel("base selection", baseSelection);
  printSel("CR selection", crSelection);
  printSel("SR selection", srSelection);
  printSel("ttbar enrichment", ttbarEnrich);
  printSel("wjets enrichment", wjetsEnrich);
  printSel("tight selection", tightSelection);
  printSel("loose selection", looseSelection);
  printSel("prompt selection", promptSelection);
  printSel("fake selection", fakeSelection);


  std::vector<std::string> systematics = {"JES", "JER"};
  {
    auto loadSystName = [&systematics](std::string baseName, int bins, int min = 1) -> void
    {
      for(int i = min; i <= bins; ++i)
      {
        std::stringstream converter;
        converter << baseName << i;
        systematics.push_back(converter.str());
      }
      return;
    };

    systematics.push_back("PU");

    //loadSystName("Q2_", 8);
    systematics.push_back("Q2");

    systematics.push_back("CFErr1");
    systematics.push_back("CFErr2");
    systematics.push_back("HF");
    systematics.push_back("HFStats1");
    systematics.push_back("HFStats2");
    systematics.push_back("LF");
    systematics.push_back("LFStats1");
    systematics.push_back("LFStats2");

    systematics.push_back("FullFast");
    systematics.push_back("FullFast_HIIP_AltCorr");
    systematics.push_back("FullFast_ID_AltCorr");
    loadSystName("FullFast_HIIP_Electron_Bin", 40);
    loadSystName("FullFast_HIIP_Muon_Bin", 48);
    loadSystName("FullFast_ID_Electron_Bin", 35);
    loadSystName("FullFast_ID_Muon_Bin", 42);

    systematics.push_back("LeptonIDSF_AltCorr");
    systematics.push_back("LeptonISOSF_AltCorr");
    loadSystName("LeptonIDSF_Electron_Bin", 98);
    loadSystName("LeptonIDSF_Muon_Bin", 56);
    loadSystName("LeptonISOSF_Electron_Bin", 12);
    loadSystName("LeptonISOSF_Muon_Bin", 6);

    systematics.push_back("ISRweight_AltCorr");
    loadSystName("ISRweight_Bin", 6);

    systematics.push_back("EWKISRweight_AltCorr");
    loadSystName("EWKISRweight_Bin", 7);

    systematics.push_back("TightLoose_AltCorr");
    loadSystName("TightLoose_Electron_Bin", 16);
    loadSystName("TightLoose_Muon_Bin", 18);

    systematics.push_back("TightLoose_NU_AltCorr");
    loadSystName("TightLoose_NU_Bin", 5);

    systematics.push_back("triggerEfficiency");

    systematics.push_back("triggerEfficiency_stat");
  }
  std::vector<std::string> variations;
  {
    for(auto& syst : systematics)
    {
      if(syst == "Q2")
      {
        variations.push_back(syst+"_1");
        variations.push_back(syst+"_2");
        variations.push_back(syst+"_3");
        variations.push_back(syst+"_4");
        variations.push_back(syst+"_5");
        variations.push_back(syst+"_6");
        variations.push_back(syst+"_7");
        variations.push_back(syst+"_8");
        continue;
      }
      variations.push_back(syst+"_Up");
      variations.push_back(syst+"_Down");
    }
  }
  ValueWithSystematics<std::string> weight = std::string("weight");
  for(auto& syst : variations)
  {
    weight.Systematic(syst) = weight.Value() + "_" + syst;
  }

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


  // The output file
  TFile outFile((outputDirectory + "/yields.root").c_str(), "RECREATE");
  // Moving the file here means that the temporary filtered trees will be saved to disk... might help alleviate the memory pressure

  if(verbose)
    std::cout << "Filtering the trees" << std::endl;

  TChain* tmpChain = Data.getChain();
  TTree* dataTree = tmpChain->CopyTree(baseSelection.Value().c_str());
  delete tmpChain;

  tmpChain = MC.getChain();
  ValueWithSystematics<TTree*> bkgTree = tmpChain->CopyTree(baseSelection.Value().c_str()); // Do we even need a specific tree for background?
  for(auto& syst : baseSelection.Systematics())
  {
    bkgTree.Systematic(syst) = tmpChain->CopyTree(baseSelection.Systematic(syst).c_str());
  }
  delete tmpChain;

  std::vector<ValueWithSystematics<TTree*>> sigTree;
  for(size_t i = 0; i < Sig.nProcesses(); ++i)
  {
    tmpChain = Sig.process(i).getChain();
    sigTree.push_back(tmpChain->CopyTree(baseSelection.Value().c_str()));
    for(auto& syst : baseSelection.Systematics())
    {
      sigTree[i].Systematic(syst) = tmpChain->CopyTree(baseSelection.Systematic(syst).c_str());
    }
    delete tmpChain;
  }

  std::vector<ValueWithSystematics<TTree*>> mcTree;
  for(size_t i = 0; i < MC.nProcesses(); ++i)
  {
    tmpChain = MC.process(i).getChain();
    mcTree.push_back(tmpChain->CopyTree(baseSelection.Value().c_str()));
    for(auto& syst : baseSelection.Systematics())
    {
      mcTree[i].Systematic(syst) = tmpChain->CopyTree(baseSelection.Systematic(syst).c_str());
    }
    delete tmpChain;
  }

  if(verbose)
    std::cout << "Done filtering" << std::endl << std::endl;

  if(verbose)
    std::cout << "Preparing to compute all the analysis yields" << std::endl;

  auto getYield = [&](ValueWithSystematics<TTree*> tree, ValueWithSystematics<std::string> weight) -> ValueWithSystematics<double>
  {
    ValueWithSystematics<double> retVal = 0.0;
    std::vector<std::string> mySystematics = {"central"};
    loadSystematics(mySystematics, tree);
    loadSystematics(mySystematics, weight);

    for(auto& syst : mySystematics)
    {
      TH1D tmpHist("tmpHist", "tmpHist", 1, 0.0, 20.0);
      tmpHist.Sumw2();

      tree.GetSystematicOrValue(syst)->Draw("weight>>tmpHist", weight.GetSystematicOrValue(syst).c_str());

      double val = tmpHist.GetBinContent(0) + tmpHist.GetBinContent(1) + tmpHist.GetBinContent(2);
      double unc = tmpHist.GetBinError(0) + tmpHist.GetBinError(1) + tmpHist.GetBinError(2);

      if(syst == "central")
      {
        retVal.Value() = val;
        retVal.Systematic("Stat") = unc;
      }
      else
      {
        retVal.Systematic(syst) = val;
      }
    }

    return retVal;
  };

  // The selection cuts for the several regions of interest
  ValueWithSystematics<std::string> theSRSelection = std::string("(");
  theSRSelection += srSelection + " && " + tightSelection + ") * " + weight;
  ValueWithSystematics<std::string> theSRWJetsSelection = std::string("(");
  theSRWJetsSelection += srSelection + " && " + wjetsEnrich + " && " + tightSelection + ") * " + weight;
  ValueWithSystematics<std::string> theSRTTbarSelection = std::string("(");
  theSRTTbarSelection += srSelection + " && " + ttbarEnrich + " && " + tightSelection + ") * " + weight;
  ValueWithSystematics<std::string> theCRWJetsSelection = std::string("(");
  theCRWJetsSelection += crSelection + " && " + wjetsEnrich + " && " + tightSelection + ") * " + weight;
  ValueWithSystematics<std::string> theCRTTbarSelection = std::string("(");
  theCRTTbarSelection += crSelection + " && " + ttbarEnrich + " && " + tightSelection + ") * " + weight;

  ValueWithSystematics<std::string> theSRPromptSelection = std::string("(");
  theSRPromptSelection += srSelection + " && " + tightSelection + " && " + promptSelection + ") * " + weight;
  ValueWithSystematics<std::string> theSRFakeSelection = std::string("(");
  theSRFakeSelection += srSelection + " && " + tightSelection + " && " + fakeSelection + ") * " + weight;
  ValueWithSystematics<std::string> theCRWJetsPromptSelection = std::string("(");
  theCRWJetsPromptSelection += crSelection + " && " + wjetsEnrich + " && " + tightSelection + " && " + promptSelection + ") * " + weight;
  ValueWithSystematics<std::string> theCRTTbarPromptSelection = std::string("(");
  theCRTTbarPromptSelection += crSelection + " && " + ttbarEnrich + " && " + tightSelection + " && " + promptSelection + ") * " + weight;
  ValueWithSystematics<std::string> theCRWJetsFakeSelection = std::string("(");
  theCRWJetsFakeSelection += crSelection + " && " + wjetsEnrich + " && " + tightSelection + " && " + fakeSelection + ") * " + weight;
  ValueWithSystematics<std::string> theCRTTbarFakeSelection = std::string("(");
  theCRTTbarFakeSelection += crSelection + " && " + ttbarEnrich + " && " + tightSelection + " && " + fakeSelection + ") * " + weight;


  ValueWithSystematics<std::string> theLNTSRSelection = std::string("(");
  theLNTSRSelection += srSelection + " && " + looseSelection + ") * " + weight;
  ValueWithSystematics<std::string> theLNTSRPromptSelection = std::string("(");
  theLNTSRPromptSelection += srSelection + " && " + looseSelection + " && " + promptSelection + ") * " + weight;
  ValueWithSystematics<std::string> theLNTSRFakeSelection = std::string("(");
  theLNTSRFakeSelection += srSelection + " && " + looseSelection + " && " + fakeSelection + ") * " + weight;
  ValueWithSystematics<std::string> theLNTSRRawSelection = std::string("(");
  theLNTSRRawSelection += srSelection + " && " + looseSelection + ") * (" + weight + "/looseNotTightWeight)";
  ValueWithSystematics<std::string> theLNTSRRawPromptSelection = std::string("(");
  theLNTSRRawPromptSelection += srSelection + " && " + looseSelection + " && " + promptSelection + ") * (" + weight + "/looseNotTightWeight)";
  ValueWithSystematics<std::string> theLNTSRRawFakeSelection = std::string("(");
  theLNTSRRawFakeSelection += srSelection + " && " + looseSelection + " && " + fakeSelection + ") * (" + weight + "/looseNotTightWeight)";


  ValueWithSystematics<std::string> theLNTCRWJetsSelection = std::string("(");
  theLNTCRWJetsSelection += crSelection + " && " + wjetsEnrich + " && " + looseSelection + ") * " + weight;
  ValueWithSystematics<std::string> theLNTCRTTbarSelection = std::string("(");
  theLNTCRTTbarSelection += crSelection + " && " + ttbarEnrich + " && " + looseSelection + ") * " + weight;
  ValueWithSystematics<std::string> theLNTCRWJetsPromptSelection = std::string("(");
  theLNTCRWJetsPromptSelection += crSelection + " && " + wjetsEnrich + " && " + looseSelection + " && " + promptSelection + ") * " + weight;
  ValueWithSystematics<std::string> theLNTCRTTbarPromptSelection = std::string("(");
  theLNTCRTTbarPromptSelection += crSelection + " && " + ttbarEnrich + " && " + looseSelection + " && " + promptSelection + ") * " + weight;
  ValueWithSystematics<std::string> theLNTCRWJetsFakeSelection = std::string("(");
  theLNTCRWJetsFakeSelection += crSelection + " && " + wjetsEnrich + " && " + looseSelection + " && " + fakeSelection + ") * " + weight;
  ValueWithSystematics<std::string> theLNTCRTTbarFakeSelection = std::string("(");
  theLNTCRTTbarFakeSelection += crSelection + " && " + ttbarEnrich + " && " + looseSelection + " && " + fakeSelection + ") * " + weight;


  auto saveYields = [&](std::string regionName, ValueWithSystematics<std::string> regionSelection, bool unblind) -> void
  {
    if(unblind)
    {
      auto dataYield = getYield(dataTree, regionSelection.Value());
      dataYield.SaveTTree(regionName + "_data", &outFile);
    }

    //auto bkgYield = getYield(bkgTree, regionSelection);
    //bkgYield.SaveTTree(regionName + "_bkg", &outFile);

    ValueWithSystematics<double> bkgYield;
    double stat = 0;
    for(auto& bkg : bkgMap)
    {
      auto thisBkgYield = getYield(mcTree[bkg.second], regionSelection);

      if(!(bkg.first == "WJets" || bkg.first == "ttbar"))
      {
        thisBkgYield.Systematic("xsec_" + bkg.first + "_Up") = thisBkgYield.Value() * 1.5;
        thisBkgYield.Systematic("xsec_" + bkg.first + "_Down") = thisBkgYield.Value() * 0.5;
      }

      thisBkgYield.SaveTTree(regionName + "_" + bkg.first, &outFile);
      bkgYield += thisBkgYield;
      stat += std::pow(thisBkgYield.Systematic("Stat"), 2);
    }
    bkgYield.Systematic("Stat") = std::sqrt(stat);
    bkgYield.SaveTTree(regionName + "_bkg", &outFile);

    for(size_t i = 0; i < Sig.nProcesses(); ++i)
    {
      auto thisSigYield = getYield(sigTree[i], regionSelection);
      thisSigYield.SaveTTree(regionName + "_" + Sig.process(i).tag(), &outFile);
    }

    return;
  };

  bool unblindSRYields = unblind || doVR1 || doVR2 || doVR3;

  saveYields("SR", theSRSelection, unblindSRYields);
  saveYields("SR_WJets", theSRWJetsSelection, unblindSRYields);
  saveYields("SR_TTbar", theSRTTbarSelection, unblindSRYields);
  saveYields("CR_WJets", theCRWJetsSelection, true); // No need to blind the control regions
  saveYields("CR_TTbar", theCRTTbarSelection, true);

  saveYields("SR_prompt", theSRPromptSelection, false); // Do not do prompt/fake for data, it is not possible to tag
  saveYields("SR_fake", theSRFakeSelection, false);
  saveYields("CR_WJets_prompt", theCRWJetsPromptSelection, false);
  saveYields("CR_WJets_fake", theCRWJetsFakeSelection, false);
  saveYields("CR_TTbar_prompt", theCRTTbarPromptSelection, false);
  saveYields("CR_TTbar_fake", theCRTTbarFakeSelection, false);

  saveYields("LNT_SR", theLNTSRSelection, true); // Do not blind control regions, but do not do prompt/fake for data
  saveYields("LNT_SR_prompt", theLNTSRPromptSelection, false);
  saveYields("LNT_SR_fake", theLNTSRFakeSelection, false);
  saveYields("LNT_SR_Raw", theLNTSRRawSelection, true);
  saveYields("LNT_SR_Raw_prompt", theLNTSRRawPromptSelection, false);
  saveYields("LNT_SR_Raw_fake", theLNTSRRawFakeSelection, false);

  saveYields("LNT_CR_WJets", theLNTCRWJetsSelection, true); // No need to blind the control regions
  saveYields("LNT_CR_TTbar", theLNTCRTTbarSelection, true);
  saveYields("LNT_CR_WJets_prompt", theLNTCRWJetsPromptSelection, false); // Do not do prompt/fake for data
  saveYields("LNT_CR_WJets_fake", theLNTCRWJetsFakeSelection, false);
  saveYields("LNT_CR_TTbar_prompt", theLNTCRTTbarPromptSelection, false);
  saveYields("LNT_CR_TTbar_fake", theLNTCRTTbarFakeSelection, false);

  // Do data-driven estimates
  auto DDFake = [](TFile* outFile, std::string baseName) -> void
  {
    ValueWithSystematics<double> dataYield;
    dataYield.LoadTTree("LNT_" + baseName + "_data", outFile);

    ValueWithSystematics<double> promptBkgYield;
    promptBkgYield.LoadTTree("LNT_" + baseName + "_prompt_bkg", outFile);

    ValueWithSystematics<double> fakeYield = dataYield - promptBkgYield;
    fakeYield.Systematic("Stat") = std::sqrt(std::pow(dataYield.Systematic("Stat"), 2) + std::pow(promptBkgYield.Systematic("Stat"), 2));
    fakeYield.SaveTTree(baseName + "_DDfake", outFile);

    return;
  };

  DDFake(&outFile, "SR");
  DDFake(&outFile, "CR_WJets");
  DDFake(&outFile, "CR_TTbar");

  auto DDPrompt = [&](TFile* outFile, std::string baseName, bool removeFake = true) -> void
  {
    ValueWithSystematics<double> dataCRWJets;
    ValueWithSystematics<double> dataCRTTbar;
    dataCRWJets.LoadTTree("CR_WJets_data", outFile);
    dataCRTTbar.LoadTTree("CR_TTbar_data", outFile);

    ValueWithSystematics<double> wjetsCRWJets;
    ValueWithSystematics<double> ttbarCRTTbar;
    if(removeFake)
    {
      wjetsCRWJets.LoadTTree("CR_WJets_prompt_WJets", outFile);
      ttbarCRTTbar.LoadTTree("CR_TTbar_prompt_ttbar", outFile);
    }
    else
    {
      wjetsCRWJets.LoadTTree("CR_WJets_WJets", outFile);
      ttbarCRTTbar.LoadTTree("CR_TTbar_ttbar", outFile);
    }

    ValueWithSystematics<double> wjetsSR;
    ValueWithSystematics<double> ttbarSR;
    if(removeFake)
    {
      wjetsSR.LoadTTree(baseName + "_prompt_WJets", outFile);
      ttbarSR.LoadTTree(baseName + "_prompt_ttbar", outFile);
    }
    else
    {
      wjetsSR.LoadTTree(baseName + "_WJets", outFile);
      ttbarSR.LoadTTree(baseName + "_ttbar", outFile);
    }

    ValueWithSystematics<double> fakeCRWJets = 0;
    ValueWithSystematics<double> fakeCRTTbar = 0;
    if(removeFake)
    {
      fakeCRWJets.LoadTTree("CR_WJets_DDfake", outFile);
      fakeCRTTbar.LoadTTree("CR_TTbar_DDfake", outFile);
    }

    ValueWithSystematics<double> otherMCCRWJets;
    ValueWithSystematics<double> otherMCCRTTbar;
    double statCRWJets = 0;
    double statCRTTbar = 0;

    for(auto& bkg : bkgMap)
    {
      ValueWithSystematics<double> mcCRWJets;
      ValueWithSystematics<double> mcCRTTbar;

      if(!(bkg.first == "WJets"))
      {
        if(removeFake)
          mcCRWJets.LoadTTree("CR_WJets_prompt_" + bkg.first, outFile);
        else
          mcCRWJets.LoadTTree("CR_WJets_" + bkg.first, outFile);
        statCRWJets += std::pow(mcCRWJets.Systematic("Stat"), 2);
        otherMCCRWJets += mcCRWJets;
      }

      if(!(bkg.first == "ttbar"))
      {
        if(removeFake)
          mcCRTTbar.LoadTTree("CR_TTbar_prompt_" + bkg.first, outFile);
        else
          mcCRTTbar.LoadTTree("CR_TTbar_" + bkg.first, outFile);
        statCRTTbar += std::pow(mcCRTTbar.Systematic("Stat"), 2);
        otherMCCRTTbar += mcCRTTbar;
      }
    }
    otherMCCRWJets.Systematic("Stat") = std::sqrt(statCRWJets);
    otherMCCRTTbar.Systematic("Stat") = std::sqrt(statCRTTbar);

    ValueWithSystematics<double> subCRWJets = dataCRWJets - fakeCRWJets - otherMCCRWJets;
    ValueWithSystematics<double> subCRTTbar = dataCRTTbar - fakeCRTTbar - otherMCCRTTbar;
    subCRWJets.Systematic("Stat") = std::sqrt(
      std::pow(   dataCRWJets.Systematic("Stat"), 2) +
      std::pow(   fakeCRWJets.Systematic("Stat"), 2) +
      std::pow(otherMCCRWJets.Systematic("Stat"), 2)
    );
    subCRTTbar.Systematic("Stat") = std::sqrt(
      std::pow(   dataCRTTbar.Systematic("Stat"), 2) +
      std::pow(   fakeCRTTbar.Systematic("Stat"), 2) +
      std::pow(otherMCCRTTbar.Systematic("Stat"), 2)
    );

    ValueWithSystematics<double> ratioCRWJets = wjetsSR / wjetsCRWJets;
    ValueWithSystematics<double> ratioCRTTbar = ttbarSR / ttbarCRTTbar;
    ratioCRWJets.Systematic("Stat") = std::sqrt(
      std::pow(wjetsSR.Systematic("Stat")/wjetsCRWJets.Value(), 2) +
      std::pow(wjetsCRWJets.Systematic("Stat")*wjetsSR.Value()/std::pow(wjetsCRWJets.Value(), 2), 2)
    );
    ratioCRTTbar.Systematic("Stat") = std::sqrt(
      std::pow(ttbarSR.Systematic("Stat")/ttbarCRTTbar.Value(), 2) +
      std::pow(ttbarCRTTbar.Systematic("Stat")*ttbarSR.Value()/std::pow(ttbarCRTTbar.Value(), 2), 2)
    );

    ValueWithSystematics<double> wjetsEstimate = ratioCRWJets * subCRWJets;
    ValueWithSystematics<double> ttbarEstimate = ratioCRTTbar * subCRTTbar;
    wjetsEstimate.Systematic("Stat") = std::sqrt(
      std::pow(ratioCRWJets.Value() * subCRWJets.Systematic("Stat"), 2) +
      std::pow(ratioCRWJets.Systematic("Stat") * subCRWJets.Value(), 2)
    );
    ttbarEstimate.Systematic("Stat") = std::sqrt(
      std::pow(ratioCRTTbar.Value() * subCRTTbar.Systematic("Stat"), 2) +
      std::pow(ratioCRTTbar.Systematic("Stat") * subCRTTbar.Value(), 2)
    );

    wjetsEstimate.SaveTTree(baseName + "_DDWJets", outFile);
    ttbarEstimate.SaveTTree(baseName + "_DDTTbar", outFile);

    return;
  };

  DDPrompt(&outFile, "SR");
  DDPrompt(&outFile, "SR_WJets", false); // Do not remove fake component from the CRs for testing closure in VRs
  DDPrompt(&outFile, "SR_TTbar", false);

  if(verbose)
    std::cout << "We are verbose" << std::endl;

  return 0;
}
