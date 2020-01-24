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

void printSel(std::string, std::string);

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

  std::string placeHolderSelection = "(badCloneMuonMoriond2017 == 1) && (badMuonMoriond2017 == 1) && (DPhiJet1Jet2 < 2.5 || Jet2Pt < 60) && (HT > 200) && (Met > 280) && (Jet1Pt > 110) && (isTight == 1)";

  if(isHighDM)
  {
  }

  std::string srSelection = "(BDT > " + std::to_string(bdtCut) + " )";

  std::string selection = placeHolderSelection  + " && " + srSelection;

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

  std::cout << "mcWeight: " << mcWeight << std::endl;

  if(verbose)
    std::cout << "Building background process map" << std::endl;
  std::map<std::string, size_t> bkgMap;
  bool foundTTbar = false, foundWJets = false;
  for(size_t i = 0; i < MC.nProcesses(); ++i)
  {
  //  std::cout << MC.process(i).tag() << ": " << MC.process(i).getYield(selection, mcWeight) << std::endl;

    if(MC.process(i).tag().find("ttbar") != std::string::npos)
    {
      bkgMap["ttbar"] = i;
      foundTTbar = true;
    }
    else
      bkgMap[MC.process(i).tag()] = i;

    if(MC.process(i).tag() == "WJets")
    {
 //     bkgMap["WJets"] = i;
      foundWJets = true;
    }
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

  for(auto& bkg : bkgMap)
  {
   // std::cout << bkg.first << " | " << MC.process(bkg.second).getYield(selection, mcWeight) << std::endl;
    
  }


  auto wjets = MC.process(bkgMap["WJets"]);
  auto ttbar = MC.process(bkgMap["ttbar"]);
  auto zinv = MC.process(bkgMap["ZInv"]);
  auto vv = MC.process(bkgMap["VV"]);
  auto singleT = MC.process(bkgMap["SingleTop"]);
  auto dy = MC.process(bkgMap["DYJets"]);
  auto qcd = MC.process(bkgMap["QCD"]);
  auto ttx = MC.process(bkgMap["ttx"]);

  auto sig = Sig.process(0);
  std::cout << "Sig tag : " <<  sig.tag() << std::endl;

  printSel("sel", selection);
  
  doubleUnc wjetsY = wjets.getYield(selection, mcWeight);
  doubleUnc ttbarY = ttbar.getYield(selection, mcWeight);
  doubleUnc zinvY = zinv.getYield(selection, mcWeight);
  doubleUnc vvY = vv.getYield(selection, mcWeight);
  doubleUnc singleTY = singleT.getYield(selection, mcWeight);
  doubleUnc dyY = dy.getYield(selection, mcWeight);
  doubleUnc qcdY = qcd.getYield(selection, mcWeight);
  doubleUnc ttxY = ttx.getYield(selection, mcWeight);
  doubleUnc sigY = sig.getYield(selection, mcWeight);
  doubleUnc totalMC = MC.getYield(selection, mcWeight);
  doubleUnc dataY = Data.getYield(selection, "weight");


  // Get Yields

  if(verbose){
    std::cout << "/* Yields report */" << '\n';
    std::cout << "Wjets: " << wjetsY << std::endl;
    std::cout << "tt: " << ttbarY << std::endl;
    std::cout << "ZInv: " << zinvY << std::endl;
    std::cout << "VV: " << vvY << std::endl;
    std::cout << "Single tops: " << singleTY << std::endl;
    std::cout << "DY: " << dyY << std::endl;
    std::cout << "QCD: " << qcdY << std::endl;
    std::cout << "ttX: " << ttxY << std::endl;
    std::cout << "Signal: " << sigY << std::endl;
    std::cout << "Total MC: " << totalMC << std::endl;
    std::cout << "Data: " << dataY << std::endl;
  }



  //doubleUnc NinSR = toEstimate.getYield(SR, mcWeight);


  // Get FOM
  // Get Efficiency
  // Make DataCards
  // Run combine (send job?)

}

void printSel(std::string name, std::string selection)
{
  std::cout << "The used " << name << ":" << std::endl;
  std::cout << "  " << selection << std::endl;
  std::cout << std::endl;
  return;
}
