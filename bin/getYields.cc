#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TChain.h>
#include <TCut.h>
#include <TString.h>
#include <TObject.h>
#include <TRandom3.h>
#include <TDirectory.h>
#include <TStyle.h>

#include "TMVA/Factory.h"
#include "TMVA/Tools.h"

#include <iostream>
#include <vector>
#include <sstream>
#include <map>
#include <fstream>

#include "UserCode/Stop4Body/interface/json.hpp"
#include "UserCode/Stop4Body/interface/SampleReader.h"

using json = nlohmann::json;

void printHelp();
bool fileExists(std::string);

int main(int argc, char** argv)
{
  std::string jsonFileName = "";
  std::string inputDirectory = "";
  std::string outputDirectory = "./OUT/";
  std::string suffix = "";
  std::string variablesJson = "";
  double luminosity = 5000;

  if(argc < 2)
  {
    std::cout << "You did not pass enough parameters" << std::endl;
    printHelp();
    return 0;
  }

  for(int i = 1; i < argc; ++i)
  {
    std::string argument = argv[i];

    if(argument == "--help")
    {
      printHelp();
      return 0;
    }

    if(argument == "--json")
      jsonFileName = argv[++i];

    if(argument == "--outDir")
      outputDirectory = argv[++i];

    if(argument == "--inDir")
      inputDirectory = argv[++i];

    if(argument == "--suffix")
      suffix = argv[++i];

    if(argument == "--variables")
      variablesJson = argv[++i];

    if(argument == "--lumi")
    {
      std::stringstream convert;
      convert << argv[++i];
      convert >> luminosity;
    }
  }

  if(jsonFileName == "")
  {
    std::cout << "You must define a json file" << std::endl;
    return 1;
  }

  if(variablesJson == "")
  {
    std::cout << "You must define a json file with the variables to plot" << std::endl;
    return 1;
  }

  if(inputDirectory == "")
  {
    std::cout << "You must define an input directory" << std::endl;
    return 1;
  }

  std::cout << "Reading json file" << std::endl;
  SampleReader samples(jsonFileName, inputDirectory, suffix);


  gStyle->SetOptStat(000000);
  TCut muon = "(nGoodMu==1)";
  TCut electron = "(nGoodEl==1)";
  TCut singlep = muon||electron;
  TCut ISRjet = "Jet1Pt > 110.";
  TCut met    = "Met>300.";
  TCut dphij1j2 = "DPhiJet1Jet2 < 2.5";
  TCut presel = singlep+ISRjet+dphij1j2+met;

  std::string mcWeight;
  {
    std::stringstream converter;
    converter << "XS*" << luminosity << "/Nevt";
    converter >> mcWeight;
  }

  auto MC = samples.getMCBkg();
  auto Sig = samples.getMCSig();
  auto Data = samples.getData();

  std::cout << "Total MC yield: " << MC.getYield(presel.GetTitle(), mcWeight) << std::endl;
  for(auto & sample : MC)
    std::cout << "\t" << sample.label() << " yield: " << sample.getYield(presel.GetTitle(), mcWeight) << std::endl;
  std::cout << "Total Data yield: " << Data.getYield(presel.GetTitle(), "") << std::endl;
  std::cout << "Total Signal yield: " << Sig.getYield(presel.GetTitle(), mcWeight) << std::endl;





  return 0;
}

void printHelp()
{
  std::cout << "The program has the following parameters:" << std::endl;
  std::cout << "\t--help\t- Print this message" << std::endl;

  return;
}

bool fileExists(std::string fileName)
{
  std::ifstream infile(fileName);
  return infile.good();
}
