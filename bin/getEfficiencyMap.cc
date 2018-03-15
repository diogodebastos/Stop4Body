#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <list>
#include <iterator>
#include <algorithm>
#include <iomanip>
#include <math.h>
#include <memory>
#include <cmath>
#include <cstdlib>
#include <regex>

#include "TROOT.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TChain.h"
#include "TObject.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TLegend.h"
#include "TGraph.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TTree.h"
#include "TF1.h"
#include "TCutG.h"
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
#include "TMultiGraph.h"
#include "TPaveText.h"
#include "THStack.h"
#include "TStyle.h"
#include "TLatex.h"
#include "TLorentzVector.h"
#include "TVectorD.h"

#include "UserCode/Stop4Body/interface/json.hpp"
#include "UserCode/Stop4Body/interface/SampleReader.h"
#include "UserCode/Stop4Body/interface/commonFunctions.h"
#include "UserCode/Stop4Body/interface/doubleWithUncertainty.h"
#include "UserCode/Stop4Body/interface/ValueWithSystematics.h"
#include "UserCode/Stop4Body/interface/LHEweightMAPs.h"

using json = nlohmann::json;

struct FileInfo
{
  std::string path;
  double crossSection;
  double branchingRatio;
  std::string tag;
};

int main(int argc, char** argv)
{
  std::string jsonFileName = "";
  std::string inputDirectory = "";
  std::string outputDirectory = "./OUT/";
  std::string suffix = "";
  bool verbose = false;
  bool isHighDM = false;
  double SRCut = 0.4;
  bool special = false;

  if(argc < 2)
  {
    std::cout << "You did not pass enough parameters" << std::endl;
    //printHelp();
    return 0;
  }

  for(int i = 1; i < argc; ++i)
  {
    std::string argument = argv[i];

    //if(argument == "--help")
    //{
    //  printHelp();
    //  return 0;
    //}

    if(argument == "--json")
      jsonFileName = argv[++i];

    if(argument == "--outDir")
      outputDirectory = argv[++i];

    if(argument == "--inDir")
      inputDirectory = argv[++i];

    if(argument == "--suffix")
      suffix = argv[++i];

    if(argument == "--signalRegionCut")
    {
      std::stringstream convert;
      convert << argv[++i];
      convert >> SRCut;
    }

    if(argument == "--isHighDeltaM")
    {
      isHighDM = true;
    }

    if(argument == "--verbose")
    {
      verbose = true;
    }

    if(argument == "--isSpecial")
    {
      special = true;
    }
  }

  if(SRCut < 0.2 && !special)
  {
    std::cout << "You have defined an invalid signal region cut." << std::endl;
    std::cout << "Using \"BDT > 0.2\" instead" << std::endl;
    SRCut = 0.2;
  }

  gStyle->SetOptStat(000000);
  gStyle->SetOptTitle(0);

  TFile puWeightFile((inputDirectory + "/puWeights.root").c_str(), "READ");

  std::cout << "Reading json files" << std::endl;
  SampleReader samples(jsonFileName, inputDirectory, suffix);

  std::string tightSelection = "(isTight == 1)";
  //std::string looseSelection = "(isLoose == 1) && !(isTight == 1)";
  //std::string promptSelection = "(isPrompt == 1)";
  //std::string fakeSelection = "!(isPrompt == 1)";
  std::string metSelection = " && (Met > 280)";
  std::string baseSelection = "(badCloneMuonMoriond2017 == 1) && (badMuonMoriond2017 == 1) && (DPhiJet1Jet2 < 2.5 || Jet2Pt < 60) && (HT > 200) && (Jet1Pt > 110)" + metSelection;
  std::string wjetsEnrich = "(NbLoose == 0)";
  std::string ttbarEnrich = "(NbTight > 0)";
  std::string controlRegion = "(BDT < 0.2)";
  if(special)
  {
    std::stringstream converter;
    converter << "(BDT < " << SRCut << ")";
    controlRegion = converter.str();
  }
  std::string signalRegion = "";
  {
    std::stringstream converter;
    converter << "(BDT > " << SRCut << ")";
    signalRegion = converter.str();
  }
  if(isHighDM)
  {
  }
  else
  {
    baseSelection += " && (LepPt < 30.)";
  }

  if(verbose)
    std::cout << "Splitting samples according to type" << std::endl;
  //auto MC = samples.getMCBkg();
  auto Sig = samples.getMCSig();
  //auto Data = samples.getData();

  if(verbose)
    std::cout << "Computing correct MC weight" << std::endl;
  std::string mcWeight;
  {
    std::stringstream converter;
    converter << "splitFactor*(genWeight/sumGenWeight)"; // The scale factors are not considered here
    converter >> mcWeight;
  }

  double minMass = 250;
  double maxMass = 800;
  double massStep = 25;
  int massBins = (maxMass-minMass)/massStep + 1;
  double minDM = 10;
  double maxDM = 80;
  double DMStep = 10;
  int dmBins = (maxDM - minDM) / DMStep + 1;

  //TFile outputTFile(outputFile, "RECREATE");
  TH2D effMap("efficiencyMap", "efficiencyMap", massBins, minMass - massStep/2, maxMass + massStep/2, dmBins, minDM - DMStep/2, maxDM + DMStep/2);

  std::regex estractSignalPointRE(".+\\((\\d+),(\\d+)\\)");
  std::smatch signalMatch;

  for(auto& process : Sig)
  {
    std::string tmpStr = process.label();
    std::cout << tmpStr << std::endl;
    if(std::regex_match(tmpStr, signalMatch, estractSignalPointRE))
    {
      std::cout << "Matches: " << signalMatch.size();
      std::cout << "; StopM: " << signalMatch[1].str();
      std::cout << "; NeutM: " << signalMatch[2].str();
      std::cout << std::endl;
    }
    else
    {
      std::cout << "A signal point was not found, skipping" << std::endl;
    }
  }

  return 0;
}
