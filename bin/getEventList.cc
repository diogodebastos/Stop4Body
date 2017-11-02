#include <iostream>
#include <vector>
#include <sstream>
#include <map>
#include <fstream>

#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TChain.h>
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
#include "UserCode/Stop4Body/interface/ValueWithSystematics.h"

using json = nlohmann::json;

int main(int argc, char** argv)
{
  std::string jsonFileName = "";
  std::string inputDirectory = "";
  std::string suffix = "";
  std::string preselection = "";
  std::string bdtCut = "";

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

    if(argument == "--inDir")
      inputDirectory = argv[++i];

    if(argument == "--suffix")
      suffix = argv[++i];

    if(argument == "--preselection")
      preselection = argv[++i];

    if(argument == "--bdtCut")
      bdtCut = argv[++i];
  }

  std::cout << "Reading json files" << std::endl;
  SampleReader samples(jsonFileName, inputDirectory, suffix);

  if(preselection == "")
    preselection = "(isTight == 1) && (Met > 280) && (DPhiJet1Jet2 < 2.5 || Jet2Pt < 60) && (HT > 200) && (Jet1Pt > 110) && (LepPt < 30.)";

  //std::cout << "Splitting samples according to type" << std::endl;
  //auto MC = samples.getMCBkg();
  //auto Sig = samples.getMCSig();
  //auto Data = samples.getData();

  TChain* tmpChain = samples.getChain();
  tmpChain->SetBranchStatus("*", 0);
  tmpChain->SetBranchStatus("Run", 1);
  tmpChain->SetBranchStatus("LumiSec", 1);
  tmpChain->SetBranchStatus("Event", 1);

  TTree* dataTree = tmpChain->CopyTree((preselection + ((bdtCut != "")?(" && BDT > " + bdtCut):(""))).c_str());

  size_t nentries = static_cast<size_t>(dataTree->GetEntries());
  std::cout << "There are " << nentries << " events." << std::endl;

  UInt_t Run;
  UInt_t LumiSec;
  ULong64_t Event;
  dataTree->SetBranchAddress("Run", &Run);
  dataTree->SetBranchAddress("LumiSec", &LumiSec);
  dataTree->SetBranchAddress("Event", &Event);

  for(size_t i = 0; i < nentries; i++)
  {
    dataTree->GetEntry(i);

    std::cout << "  " << Run << ":" << LumiSec << ":" << Event << std::endl;
  }

  return 0;
}
