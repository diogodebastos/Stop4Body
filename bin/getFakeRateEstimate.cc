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
#include "UserCode/Stop4Body/interface/ValueWithSystematics.h"

using json = nlohmann::json;

int main(int argc, char** argv)
{
  std::string debug;
  double luminosity = -1;
  // Placeholder for ${JSON_PATH}
  //std::string jsonFileName = "/lstore/cms/dbastos/REPOS/Stop4Body/CMSSW_8_0_14/src/UserCode/Stop4Body/Macros/JSON/2017/DataJetHT.json";
  std::string jsonFileName = "/lstore/cms/dbastos/REPOS/Stop4Body/CMSSW_8_0_14/src/UserCode/Stop4Body/Macros/JSON/2017/DataMetHT.json";
  // Placeholder for ${INPUT}
  std::string inputDirectory = "/lstore/cms/dbastos/Stop4Body/nTuples_v2018-12-14";
  std::string suffix = "";
  
  SampleReader samples(jsonFileName, inputDirectory, suffix);
  auto Data = samples.getData();
  auto MC = samples.getMCBkg();
  
  //debug
  luminosity = Data.getLumi();

  std::stringstream converter;
  converter << "luminosity: ";
  converter << luminosity/1000;
  std::cout << converter.str() << std::endl;

  // Measurement Region
  std::string mRegion = "(HLT_PFHT1050 == 1) && (HT > 1200) && (Met < 40) && (mt < 30)";
  std::string tightEl = "(nGoodEl_cutId_veto)";
  std::string tightMu = "(nGoodMu_cutId_loose)";
  
  size_t jetHTIndex = 0, otherIndex = 0;
  bool foundJetHT = false, foundOtherIndex = false;
  for (size_t i = 0; i < Data.nProcesses(); i++) {
   // TODO: Check out to: If contains sting -> "_JetHT"
   if(Data.process(i).tag() == "_JetHT") {
    jetHTIndex = i;
    foundJetHT = true;
   }
  }
  for (size_t i = 0; i < MC.nProcesses(); i++) {
   if(MC.process(i).tag() != "ZInv" && MC.process(i).tag() != "QCD") {
    otherIndex = i;
    foundOtherIndex = true;
   }
  }
  if(!foundJetHT)
  {
    std::cout << "There isn't a JetHT sample in the JSON file" << std::endl;
    return 1;
  }
  if(!foundOtherIndex)
  {
    std::cout << "There isn't a other MC sample in the JSON file" << std::endl;
    return 1;
  }
  auto jetht = Data.process(jetHTIndex);
  auto other = MC.process(otherIndex);
  
  // Plot eTL
  //TO DO: Replace other with jetht when samples are available
  auto eL = other.getHist("LepPt", "LepPt;Evt.","weight * ()", 500, 0,500);
  auto eT = other.getHist("LepPt", "LepPt;Evt.", "weight * ( "+tightEl+")", 500, 0,500);
  
  
  
  
  //auto dataH = Data.process(0).getHist("BDT", "BDT;Evt.",               tightSelection+"&&"+baseSelection,     20, -1.0, 1.0);
  //auto mcH   = MC.getHist("MC", "BDT", "BDT;Evt.", mcWeight+"*("+tightSelection+"&&"+baseSelection+")", 20, -1.0, 1.0);
  

  
  return 0;
}