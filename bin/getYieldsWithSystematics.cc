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
  printSel("VR1Trigger", VR1Trigger);

  return 0;
}
