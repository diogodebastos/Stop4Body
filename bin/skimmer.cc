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
#include <TStopwatch.h>

#include "TMVA/Factory.h"
#include "TMVA/Tools.h"
#include "TMVA/Reader.h"

#include <iostream>
#include <vector>
#include <sstream>
#include <map>
#include <fstream>

#include "UserCode/Stop4Body-nanoAOD/interface/json.hpp"
#include "UserCode/Stop4Body-nanoAOD/interface/SampleReader.h"

//#include "FWCore/PythonParameterSet/interface/MakeParameterSets.h"
//#include "FWCore/ParameterSet/interface/ParameterSet.h"

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
bool replace(std::string&, const std::string&, const std::string&);

int main(int argc, char** argv)
{
  std::string jsonFileName = "";
  std::string inputDirectory = "";
  std::string outputDirectory = "./OUT/";
  std::string suffix = "";
  bool limitEvents = false;
  Long64_t eventLimit = 0;

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

    if(argument == "--limit")
    {
      limitEvents = true;
      std::stringstream converter;
      converter << argv[++i];
      converter >> eventLimit;

      if(eventLimit <= 0)
        limitEvents = false;
    }
  }

  if(jsonFileName == "")
  {
    std::cout << "You must define a json file" << std::endl;
    return 1;
  }

  if(inputDirectory == "")
  {
    std::cout << "You must define an input directory" << std::endl;
    return 1;
  }

  std::cout << "Reading JSON file" << std::endl;
  SampleReader samples(jsonFileName, inputDirectory, suffix);

  std::vector<std::string> filesToProcess = samples.getAllFiles();

  std::cout << "Processing " << filesToProcess.size() << " files." << std::endl;

  for(auto & file : filesToProcess)
  {
    auto cwd = gDirectory;

    std::string outFile = file.substr(0, file.find_last_of("."))+"_skimmed.root";
    replace(outFile, inputDirectory, outputDirectory);
    std::cout << "  Processing file " << file << " and saving output in " << outFile << std::endl;

    TFile *inputFile = new TFile(file.c_str(), "READ");
    TFile *outputFile = new TFile(outFile.c_str(), "RECREATE");

    TTree* inTree = static_cast<TTree*>(inputFile->Get("bdttree"));
    inTree->SetBranchStatus("*",1);
    inTree->SetBranchStatus("*_Up",0);
    inTree->SetBranchStatus("*_Down",0);
    Long64_t inEntries = inTree->GetEntries();

    TTree* outTree = nullptr;

    if(!limitEvents || inEntries < eventLimit)
    {
      outTree = inTree->CloneTree();
      outTree->Write();
    }
    else
    {
      TTree* tmp = inTree->CloneTree(eventLimit);
      tmp->SetBranchStatus("*", 1);
      tmp->SetBranchStatus("weight", 0);
      outTree = tmp->CloneTree();
      tmp->SetBranchStatus("weight", 1);

      Float_t weight = 0;
      tmp->SetBranchAddress("weight", &weight);
      TBranch* newWeightBranch = outTree->Branch("weight", &weight);

      Float_t factor = inEntries;
      factor = factor / eventLimit;
      std::cout << "    The scaling factor is: " << factor << std::endl;

      for(Long64_t i = 0; i < eventLimit; ++i)
      {
        tmp->GetEntry(i);
        weight = weight * factor;
        newWeightBranch->Fill();
      }

      outTree->Write();
    }

    cwd->cd();
    outputFile->Close();
    inputFile->Close();
  }

  return 0;
}

void printHelp()
{
  std::cout << "The program has the following parameters:" << std::endl;
  std::cout << "\t--help\t- Print this message" << std::endl;

  return;
}

bool replace(std::string& str, const std::string& from, const std::string& to)
{
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

bool fileExists(std::string fileName)
{
  std::ifstream infile(fileName);
  return infile.good();
}
