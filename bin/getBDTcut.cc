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

#include "UserCode/Stop4Body-nanoAOD//interface/json.hpp"
#include "UserCode/Stop4Body-nanoAOD//interface/SampleReader.h"
#include "UserCode/Stop4Body-nanoAOD//interface/doubleWithUncertainty.h"
#define _USE_CERN_ROOT
#include "UserCode/Stop4Body-nanoAOD//interface/ValueWithSystematics.h"

using json = nlohmann::json;

void printSel(std::string, std::string);
double FOM(doubleUnc, doubleUnc, float);
double eff(doubleUnc, doubleUnc);
void saveCSV(std::string, std::string, double, double, double, double);
void makeDataCard(std::string, std::string, double, doubleUnc, doubleUnc, doubleUnc, doubleUnc, doubleUnc, doubleUnc, doubleUnc, doubleUnc);

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

  std::string preSelection = "(badCloneMuonMoriond2017 == 1) && (badMuonMoriond2017 == 1) && (DPhiJet1Jet2 < 2.5 || Jet2Pt < 60) && (HT > 200) && (Met > 280) && (Jet1Pt > 110) && (isTight == 1)";

  if(isHighDM)
  {
  }

  std::string srSelection = "(BDT > " + std::to_string(bdtCut) + " )";

  std::string selection = preSelection  + " && " + srSelection;

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
  std::string name = sig.tag();
  name.replace(0,13,"ST");
  name.replace(5,1,"N");

  printSel("sel", selection);

  // Get Yields
  doubleUnc totalMCinit = MC.getYield(preSelection, mcWeight);
  doubleUnc sigYinit = sig.getYield(preSelection, mcWeight);

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

  // Get Efficiency
  double effSig = eff(sigYinit, sigY);
  double effBckg = eff(totalMCinit, totalMC);
  // Get FOM
  double fom = FOM(sigY,totalMC,0.2);

  //Save CSV with FOM and EFFs
  saveCSV(outputDirectory, name, bdtCut, effSig, effBckg, fom);
  // Make DataCards
  makeDataCard(outputDirectory, name, bdtCut, sigY, totalMC,wjetsY, ttbarY, zinvY, vvY, singleTY, dyY);
  // Run combine (send job?)

  if(verbose){
    std::cout << " Yields report " << '\n';
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
    std::cout << "Signal efficiency: " << effSig << std::endl;
    std::cout << "Background efficiency: " << effBckg << std::endl;
    std::cout << "FOM: " << fom << std::endl;
//    std::cout << "Size: " << size << std::endl;
  }
}

void printSel(std::string name, std::string selection)
{
  std::cout << "The used " << name << ":" << std::endl;
  std::cout << "  " << selection << std::endl;
  std::cout << std::endl;
  return;
}

double FOM(doubleUnc signal, doubleUnc background, float f)
{
  double fom;

  double S = signal.value();
  double B = background.value();
  std::cout << "Signal yield: " << S << std::endl;
  std::cout << "Background yield: " << B << std::endl;

  double sigmaB2 = pow(f*B, 2);

  double term1=(S+B)*log(((S+B)*(B+sigmaB2))/(pow(B,2)+(S+B)*sigmaB2));
  double term2=pow(B,2)/sigmaB2*log(1+(sigmaB2*S)/(B*(B+sigmaB2)));

  fom = std::sqrt(2*term1-term2);

  return fom;
}

double eff(doubleUnc yieldAtPreSel, doubleUnc yieldAtCut)
{
  double eff;
  double yInit = yieldAtPreSel.value();
  double yCut = yieldAtCut.value();

  eff = yCut/yInit;

  return eff;
}

void saveCSV(std::string outputDirectory, std::string name, double bdtCut, double effSig, double effBckg, double fom){
  std::string fileName = outputDirectory+"/"+name + "_eff_fom.csv";
  std::fstream efffomFile;
  efffomFile.open(fileName, std::ios::app );
  /* TODO: Fix this
  std::streampos size;
  size = efffomFile.tellg();
  if (size==0){
    efffomFile << "bdt,effSig,effBckg,fom\n";
  }
  */
  efffomFile << bdtCut <<"," << effSig << "," << effBckg <<","<< fom <<"\n";
  efffomFile.close();
}

std::string toDC(doubleUnc yield){
  std::string toDC;
  toDC = std::to_string(1 + yield.uncertainty()/yield.value());
  return toDC;
}

void makeDataCard(std::string outputDirectory, std::string bin, double bdtCut, doubleUnc sigY,doubleUnc totalMC, doubleUnc wjetsY, doubleUnc ttbarY, doubleUnc zinvY, doubleUnc vvY, doubleUnc singleTY, doubleUnc dyY){
  std::ifstream dataCardIn("Templates/dataCardForCuts.txt");
  std::ofstream dataCardOut(outputDirectory+"/DataCards/"+bin+"_BDT"+std::to_string(bdtCut)+".txt");

  if(!dataCardIn || !dataCardOut)
   {
    std::cout << "Error opening files!" << std::endl;
    return;
  }

  //std::string strReplace = "ST500N420";
  //std::string strNew = bin;

  std::string strTemp;
  int i = 0;
  //while(dataCardIn >> strTemp)
  while(getline(dataCardIn,strTemp))
  {
    i++;
    if(i==7){
      strTemp = "bin "+bin;
    }
    else if(i==8){
      // TODO: Code the number of observations
      strTemp = "observation " + std::to_string(std::round(totalMC.value()));
    }
    else if(i==10){
      strTemp = "bin "+bin+" "+bin+" "+bin+" "+bin+" "+bin+" "+bin+" "+ bin;
    }
    else if(i==13){
      strTemp = "rate "+std::to_string(sigY.value())+" "+std::to_string(wjetsY.value())+" "+std::to_string(ttbarY.value())+" "+std::to_string(zinvY.value())+" "+std::to_string(vvY.value())+" "+std::to_string(singleTY.value())+" "+std::to_string(dyY.value());

    }
    else if(i==15){
      strTemp = "Stat_" + bin + " lnN " + toDC(sigY) + " - - - - - -";
    }
    else if(i==22){
      strTemp = "SysBckg\t lnN - "+toDC(wjetsY)+" "+toDC(ttbarY)+" 1.50 1.50 1.50 1.50";
                                //toDC(wjetsY)+" "+toDC(ttbarY)+" "+toDC(zinvY)+" "+toDC(vvY)+" "+toDC(singleTY)+" "+toDC(dyY);
    }
//    if(strTemp == strReplace)
//    {
//      strTemp = strNew;
//    }

    // std::cout << strTemp << std::endl;
    //std::cout << strTemp << "\t " << i  <<std::endl;
    strTemp += "\n";
    dataCardOut << strTemp;
  }
  return;
}
