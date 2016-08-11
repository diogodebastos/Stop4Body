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

#include <iostream>
#include <vector>
#include <sstream>
#include <map>
#include <fstream>

#include "json.hpp"

using json = nlohmann::json;

struct FileInfo
{
  std::string path;
  double crossSection;
  double branchingRatio;
  std::string tag;
};

void printHelp();
bool fileExists(std::string);

int main(int argc, char** argv)
{
  std::string jsonFileName = "";
  std::string outputDirectory = "./OUT/";
  std::string inputDirectory = "./OUT/";
  double luminosity = 10000;
  bool noPresel = false;

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

    if(argument == "--lumi")
    {
      std::stringstream conv;
      conv << argv[++i];
      conv >> luminosity;
    }

    if(argument == "--noPresel")
      noPresel = true;
  }

  if(jsonFileName == "")
  {
    std::cout << "You must define a json file" << std::endl;
  }
  std::cout << "Producing PseudoData from the file '" << jsonFileName << "' with an integrated luminosity of " << luminosity << " fb" <<std::endl;

  json jsonFile;
  std::ifstream inputFile(jsonFileName);
  inputFile >> jsonFile;

  if(jsonFile.find("lines") == jsonFile.end())
  {
    std::cout << "The specified json file is not a valid sample descriptor" << std::endl;
    return 1;
  }

  std::vector<FileInfo> filesToProcess;
  std::vector<std::string> emptyLines;
  std::vector<std::string> emptyPath;
  std::vector<std::string> invalidPath;

  for(auto &process : jsonFile["lines"])
  {
    if(process.find("files") != process.end())
    {
      for(auto &file : process["files"])
      {
        //std::string path = file["path"];
        std::string path = inputDirectory + "/";
        path += file["tag"];
        path += "_bdt.root";
        std::string identifier = process["tag"];
        identifier += ":";
        identifier += file["tag"];
        if(path == "")
          emptyPath.push_back(identifier);
        else
        {
          if(fileExists(path))
          {
            FileInfo tmpFile;

            tmpFile.path = path;
            tmpFile.crossSection = file["xsec"];
            tmpFile.branchingRatio = file["br"];
            tmpFile.tag = file["tag"];

            bool processFile = true;
            if(process.count("isdata") == 1)
              if(process["isdata"] == true)
                processFile = false;
            if(process.count("issignal") == 1)
              if(process["issignal"] == true)
                processFile = false;

            if(processFile)
              filesToProcess.push_back(tmpFile);
          }
          else
            invalidPath.push_back(identifier + "  ->  " + path);
        }
      }
    }
    else
      emptyLines.push_back(process["tag"]);
  }


  // Selection
  TCut muon = "nGoodMu == 1";
  TCut electron = "nGoodEl == 1";
  TCut singlep = muon || electron;
  TCut met    = "Met > 300.";
  TCut ISRjet = "Jet1Pt > 110.";
  TCut dphij1j2 = "DPhiJet1Jet2 < 2.5";
  TCut presel = singlep+ISRjet+dphij1j2+met;
  if(noPresel)
    presel = "";

  //TDirectory* curDir = gDirectory;

  TFile outputFile((outputDirectory+"/PseudoData_bdt.root").c_str(), "RECREATE");
  outputFile.cd();
  TTree* PDtree = new TTree("bdttree", "Same tree");
  std::map<TString, float> VarMap;
  std::map<TString, int> VarMapI;
  std::map<TString, float> DummyVars;
  VarMap["LepPt"] = 0.;
  VarMap["LepEta"] = 0.;
  VarMap["LepDxy"] = -999;
  VarMap["LepDz"] = -999;
  VarMap["LepSip3"] = -999;
  VarMap["LepIso03"] = -999;
  VarMap["LepIso04"] = -999;
  VarMap["Met"] = 0.;
  VarMap["mt"] = -999;
  VarMap["HT20"] = 0.;
  VarMap["HT25"] = 0.;
  VarMap["HT30"] = 0.;
  VarMap["Jet1Pt"] = 0.;
  VarMap["Jet1Eta"] = -999;
  //VarMap["Jet1Phi"] = -999;
  VarMap["Jet2Pt"] = 0.;
  VarMap["Jet2Eta"] = -999;
  //VarMap["Jet2Phi"] = -999;
  VarMap["DPhiJet1Jet2"] = 0;
  VarMap["JetHBpt"] = 0.;
  VarMap["Q80"] = 0.;
  VarMap["CosDeltaPhi"] = 0.;
  VarMap["DrJet1Lep"] = 0.;
  VarMap["DrJet2Lep"] = 0.;
  VarMap["DrJet1Jet2"] = 0.;
  VarMap["DrJetHBLep"] = 0.;
  VarMap["JetLepMass"] = 0.;
  VarMap["J3Mass"] = 0.;
  VarMapI["Njet"] = 0.;
  VarMapI["NbLoose30"] = 0.;
  VarMapI["NbTight30"] = 0.;
  VarMapI["nGoodMu"] = 0;
  VarMapI["nGoodEl"] = 0;
  VarMapI["LepID"] = -999;
  VarMapI["LepChg"] = -999;
  DummyVars["XS"] = -999;
  DummyVars["Event"] = -999;
  DummyVars["LumiSec"] = -999;
  DummyVars["Nevt"] = -999;

  for(auto & var : VarMap)
    PDtree->Branch(var.first, &var.second);
  for(auto & var : VarMapI)
    PDtree->Branch(var.first, &var.second);
  for(auto & var : DummyVars)
    PDtree->Branch(var.first, &var.second);

  //curDir->cd();
  TRandom3 *randomizer = new TRandom3();

  for(auto &file : filesToProcess)
  {
    std::cout << "Processing " << file.tag << std::endl;

    TFile inputFile(file.path.c_str(), "READ");
    outputFile.cd();
    TTree* inputTree = static_cast<TTree*>(inputFile.Get("bdttree"));
    TTree* slimmedTree = static_cast<TTree*>(inputTree->CopyTree(presel));
    int NEvt = 0;
    slimmedTree->SetBranchAddress("Nevt", &NEvt);
    slimmedTree->GetEntry(0);
    double readEvents = slimmedTree->GetEntries();
    double yield = readEvents/NEvt * file.crossSection * file.branchingRatio * luminosity;

    std::cout << "\tThe total number of initial events: " << NEvt << std::endl;
    std::cout << "\t" << readEvents << " events were read from the file." << std::endl;
    std::cout << "\tYield: " << yield << std::endl;
    if(yield > readEvents)
    {
      std::cout << "ERROR: with the chosen luminosity, the expected yield is larger than the number of available events." << std::endl;
      std::cout << "It is not possible to generate the pseudo data, please choose a different value for the luminosity or different samples with more events" << std::endl;
      return 1;
    }

    for(auto & var : VarMap)
      slimmedTree->SetBranchAddress(var.first, &var.second);
    for(auto & var : VarMapI)
      slimmedTree->SetBranchAddress(var.first, &var.second);

    std::vector<size_t> eventsToKeep;
    int numberOfEvents = randomizer->Poisson(yield);
    if(numberOfEvents > readEvents)
    {
      std::cout << "WARNING: The poisson smearing resulted in a number of events larger than the events available in the file." << std::endl;
      std::cout << "This may be just bad luck, or the total number of events and the expected yield have values very close to each other, please reevaluate the situaation and try again." << std::endl;
      return 1;
    }

    std::cout << "\tSelecting events to keep" << std::endl;
    for(int i = 0; i < numberOfEvents; ++i)
    {
      size_t evtindex = readEvents;
      while(evtindex == readEvents)
        evtindex = randomizer->Uniform(readEvents);
      if(std::find(eventsToKeep.begin(), eventsToKeep.end(), evtindex) == eventsToKeep.end())
        eventsToKeep.push_back(evtindex);
      else
        --i;
    }

    std::cout << "\tCopying selected events from input to output" << std::endl;
    for(auto & i : eventsToKeep)
    {
      slimmedTree->GetEntry(i);
      PDtree->Fill();
    }

    delete slimmedTree;
  }

  outputFile.cd();
  PDtree->Write();


  if(emptyLines.size() != 0)
  {
    std::cout << "The following lines did not have any files associated to them: ";
    for(auto &process : emptyLines)
      std::cout << process << "; ";
    std::cout << std::endl;
  }

  if(emptyPath.size() != 0)
  {
    std::cout << "The following files did not have any path defined, or the defined path was empty: ";
    for(auto &file : emptyPath)
      std::cout << file << "; ";
    std::cout << std::endl;
  }

  if(invalidPath.size() != 0)
  {
    std::cout << "The following files could not be found: " << std::endl;
    for(auto &file : invalidPath)
      std::cout << "\t" << file << std::endl;
    std::cout << std::endl;
  }

  return 0;
}

void printHelp()
{
  std::cout << "The program has the following parameters:" << std::endl;
  std::cout << "\t--help\t- Print this message" << std::endl;
  std::cout << "\t--json\t- Used to specify the json file which describes the samples, this option is obligatory. The parameter immediately after this one should be the desired json file" << std::endl;
  std::cout << "\t--inDir\t- Used to specify the input directory. The parameter immediately after this one should be the desired directory. By default the directory './OUT'" << std::endl;
  std::cout << "\t--outDir\t- Used to specify the output directory. The parameter immediately after this one should be the desired directory. By default the directory './OUT/' is used" << std::endl;

  return;
}

bool fileExists(std::string fileName)
{
  std::ifstream infile(fileName);
  return infile.good();
}