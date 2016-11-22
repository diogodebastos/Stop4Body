#include <iostream>

#include <TROOT.h>
#include <TDirectory.h>
#include <TFile.h>
#include <TTree.h>

#include "UserCode/Stop4Body/interface/json.hpp"
#include "UserCode/Stop4Body/interface/SampleReader.h"

int main(int argc, char** argv)
{
  std::string jsonFileName = "";
  std::string inputDirectory = "";
  std::string trainOutputDirectory = "";
  std::string testOutputDirectory = "";
  std::string suffix = "";

  if(argc < 2)
  {
    std::cout << "You did not pass enough parameters" << std::endl;
    //printHelp();
    return 0;
  }

  for(int i = 1; i < argc; ++i)
  {
    std::string argument = argv[i];

    if(argument == "--help")
    {
      //printHelp();
      return 0;
    }

    if(argument == "--json")
      jsonFileName = argv[++i];

    if(argument == "--testOutDir")
      testOutputDirectory = argv[++i];

    if(argument == "--trainOutDir")
      trainOutputDirectory = argv[++i];

    if(argument == "--inDir")
      inputDirectory = argv[++i];

    if(argument == "--suffix")
      suffix = argv[++i];
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

  if(trainOutputDirectory == "")
  {
    std::cout << "You must define a training output directory" << std::endl;
    return 1;
  }

  if(testOutputDirectory == "")
  {
    std::cout << "You must define a testing output directory" << std::endl;
    return 1;
  }

  std::cout << "Reading json file" << std::endl;
  SampleReader samples(jsonFileName, inputDirectory, suffix);

  for(auto &process : samples)
  {
    std::cout << "Processing process: " << process.tag() << std::endl;
    for(auto &sample : process)
    {
      std::cout << "\tProcessing sample: " << sample.tag() << std::endl;
      std::string testOutputFile = testOutputDirectory + "/" + sample.tag();
      std::string trainOutputFile = trainOutputDirectory + "/" + sample.tag();
      if(suffix != "")
      {
        testOutputFile += "_" + suffix;
        trainOutputFile += "_" + suffix;
      }
      testOutputFile += ".root";
      trainOutputFile += ".root";
      std::cout << "\t  Putting test events in: " << testOutputFile << std::endl;
      std::cout << "\t  Putting train events in: " << trainOutputFile << std::endl;

      auto inFiles = sample.getAllFiles();
      if(inFiles.size() > 1)
      {
        std::cout << "There was a serious problem" << std::endl;
        return 2;
      }
      if(inFiles.size() == 0)
      {
        std::cout << "There was no input file for this sample" << std::endl;
        continue;
      }

      auto cwd = gDirectory;

      TFile finput(inFiles[0].c_str(), "READ");
      TTree* inTree = static_cast<TTree*>(finput.Get("bdttree"));
      inTree->SetBranchStatus("*", 1);

      TFile testFile(testOutputFile.c_str(), "RECREATE");
      TTree* testTree = static_cast<TTree*>(inTree->CloneTree(0));
      inTree->CopyAddresses(testTree);

      TFile trainFile(trainOutputFile.c_str(), "RECREATE");
      TTree* trainTree = static_cast<TTree*>(inTree->CloneTree(0));
      inTree->CopyAddresses(trainTree);

      cwd->cd();

      Long64_t nentries = inTree->GetEntries();
      for(Long64_t i = 0; i < nentries; ++i)
      {
        inTree->GetEntry(i);
        if(i%2 == 0)
          testTree->Fill();
        else
          trainTree->Fill();
      }

      testFile.cd();
      testTree->Write();
      trainFile.cd();
      trainTree->Write();
      cwd->cd();
    }
  }

  return 0;
}
