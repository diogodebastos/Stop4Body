#include <iostream>
#include <algorithm>

#include <TROOT.h>
#include <TDirectory.h>
#include <TFile.h>
#include <TTree.h>

#include "UserCode/Stop4Body-nanoAOD//interface/json.hpp"
#include "UserCode/Stop4Body-nanoAOD//interface/SampleReader.h"

struct EventID
{
  ULong64_t evt;
  UInt_t run;
  UInt_t lumi;
  bool isTrain;
  bool isTest;
};

int main(int argc, char** argv)
{
  std::string fileName = "";
  std::string trainOutputFile = "";
  std::string testOutputFile = "";
  std::string trainTreeFile = "";
  std::string testTreeFile = "";

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

    if(argument == "--inFile")
      fileName = argv[++i];

    if(argument == "--testOutFile")
      testOutputFile = argv[++i];

    if(argument == "--trainOutFile")
      trainOutputFile = argv[++i];

    if(argument == "--trainTreeFile")
      trainTreeFile = argv[++i];

    if(argument == "--testTreeFile")
      testTreeFile = argv[++i];
  }

  if(fileName == "")
  {
    std::cout << "You must define a file to be splitted" << std::endl;
    return 1;
  }

  if(trainOutputFile == "")
  {
    std::cout << "You must define a training output file" << std::endl;
    return 1;
  }

  if(testOutputFile == "")
  {
    std::cout << "You must define a testing output file" << std::endl;
    return 1;
  }

  bool usePredefinedSplitting = false;
  if(trainTreeFile != "" && testTreeFile != "")
    usePredefinedSplitting = true;

  std::vector<EventID> eventInfo;
  if(usePredefinedSplitting)
  {
    std::cout << "Using the predefined splitting as defined in:" << std::endl;
    std::cout << "  - Train events: " << trainTreeFile << std::endl;
    std::cout << "  - Test events: " << testTreeFile << std::endl;
    TFile trainEventsFile((trainTreeFile).c_str(), "READ");
    TFile testEventsFile((testTreeFile).c_str(), "READ");
    TTree* inTrainTree = static_cast<TTree*>(trainEventsFile.Get("bdttree"));
    TTree* inTestTree = static_cast<TTree*>(testEventsFile.Get("bdttree"));
    UInt_t Run;
    ULong64_t Event;
    UInt_t LumiSec;
    inTrainTree->SetBranchAddress("Run", &Run);
    inTrainTree->SetBranchAddress("Event", &Event);
    inTrainTree->SetBranchAddress("LumiSec", &LumiSec);
    inTestTree->SetBranchAddress("Run", &Run);
    inTestTree->SetBranchAddress("Event", &Event);
    inTestTree->SetBranchAddress("LumiSec", &LumiSec);
    Long64_t nentries = inTrainTree->GetEntries();
    for(Long64_t i = 0; i < nentries; ++i)
    {
      inTrainTree->GetEntry(i);
      EventID tmp;
      tmp.evt = Event;
      tmp.run = Run;
      tmp.lumi = LumiSec;
      tmp.isTrain = true;
      tmp.isTest = false;
      eventInfo.push_back(std::move(tmp));
    }
    nentries = inTestTree->GetEntries();
    for(Long64_t i = 0; i < nentries; ++i)
    {
      inTestTree->GetEntry(i);
      EventID tmp;
      tmp.evt = Event;
      tmp.run = Run;
      tmp.lumi = LumiSec;
      tmp.isTrain = false;
      tmp.isTest = true;
      eventInfo.push_back(std::move(tmp));
    }
  }

  std::cout << "\t  Putting test events in: " << testOutputFile << std::endl;
  std::cout << "\t  Putting train events in: " << trainOutputFile << std::endl;



  auto cwd = gDirectory;

  TFile finput(fileName.c_str(), "READ");
  TTree* inTree = static_cast<TTree*>(finput.Get("bdttree"));
  inTree->SetBranchStatus("*", 1);

  // Event ID variables:
  UInt_t Run;
  ULong64_t Event;
  UInt_t LumiSec;
  bool isTight;
  inTree->SetBranchAddress("Run", &Run);
  inTree->SetBranchAddress("Event", &Event);
  inTree->SetBranchAddress("LumiSec", &LumiSec);
  inTree->SetBranchAddress("isTight", &isTight);

  auto EventCompare = [&Run, &Event, &LumiSec](EventID const& event) -> bool
  {
    return event.evt == Event && event.run == Run && event.lumi == LumiSec;
  };

  TFile testFile(testOutputFile.c_str(), "RECREATE");
  TTree* testTree = static_cast<TTree*>(inTree->CloneTree(0));
  Float_t testSplitFactor = 2;
  inTree->CopyAddresses(testTree);
  testTree->SetBranchAddress("splitFactor", &testSplitFactor);

  TFile trainFile(trainOutputFile.c_str(), "RECREATE");
  TTree* trainTree = static_cast<TTree*>(inTree->CloneTree(0));
  Float_t trainSplitFactor = 2;
  inTree->CopyAddresses(trainTree);
  trainTree->SetBranchAddress("splitFactor", &trainSplitFactor);

  cwd->cd();

  Long64_t nentries = inTree->GetEntries();
  Long64_t extraEvt = 0;
  for(Long64_t i = 0; i < nentries; ++i)
  {
    inTree->GetEntry(i);

    auto evtSearch = std::find_if(eventInfo.begin(), eventInfo.end(), EventCompare);
    if(usePredefinedSplitting && evtSearch != eventInfo.end())
    {
      if(evtSearch->isTest)
        testTree->Fill();
      else
        trainTree->Fill();
    }
    else
    {
      if(usePredefinedSplitting && isTight)
        ++extraEvt;
      if(Event%2 == 0)
        testTree->Fill();
      else
        trainTree->Fill();
    }
  }
  std::cout << "There were " << extraEvt << " extra events" << std::endl;

  testFile.cd();
  testTree->Write();
  trainFile.cd();
  trainTree->Write();
  cwd->cd();

  std::cout << "Done!" << std::endl << std::endl;

  return 0;
}
