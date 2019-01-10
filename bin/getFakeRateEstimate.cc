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
  std::string jsonFileName = "/lstore/cms/dbastos/REPOS/Stop4Body/CMSSW_8_0_14/src/UserCode/Stop4Body/Macros/JSON/2017/plot2017.json";
  // Placeholder for ${INPUT}
  std::string inputDirectory = "/lstore/cms/dbastos/Stop4Body/nTuples_v2019-01-04-test";
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
   // TODO: Check out to: If contains string -> "_JetHT"
   //if(Data.process(i).tag().find("_JetHt") != std::string::npos) {
   if(Data.process(i).tag() == "Data") {
    jetHTIndex = i;
    foundJetHT = true;
   }
  }
  for (size_t i = 0; i < MC.nProcesses(); i++) {
   if(MC.process(i).tag() != "ZInv" || MC.process(i).tag() != "QCD") {
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
  //auto eL = jetht.getHist("LepPt", "LepPt;Evt.","weight * ()", 500, 0,500);
  //auto eT = jetht.getHist("LepPt", "LepPt;Evt.", "weight * ( "+tightEl+")", 500, 0,500);
  auto eT = jetht.getHist("LepPt", "LepPt;Ratio", tightEl, 40, 0,200);
  auto eL = jetht.getHist("LepPt", "LepPt;Ratio","" ,40, 0,200);
  
  auto ratio = static_cast<TH1D*>(eT->Clone("Ratio"));
  ratio->SetTitle("Tight to Loose ratio");
  ratio->Divide(eL);
  printf("Canvas\n"); 
  TCanvas c1("tmp_canv", "", 800, 800);
  gStyle->SetOptStat(0);

  TPad* t1 = new TPad("t1","t1", 0.0, 0.20, 1.0, 1.0);
  t1->Draw();
  t1->cd();
  //t1->SetLogy(true);
  ratio->Draw();
  c1.SaveAs("getFakeRate_test.png");

  
  return 0;
}
