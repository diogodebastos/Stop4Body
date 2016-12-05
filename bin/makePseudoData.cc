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

#include "UserCode/Stop4Body/interface/json.hpp"
#include "UserCode/Stop4Body/interface/SampleReader.h"

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
  std::string inputDirectory = "";
  std::string suffix = "";
  double luminosity = 10000;
  bool noPresel = false;
  bool includeSignal = false;

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

    if(argument == "--lumi")
    {
      std::stringstream conv;
      conv << argv[++i];
      conv >> luminosity;
    }

    if(argument == "--noPresel")
      noPresel = true;

    if(argument == "--injectSignal")
      includeSignal = true;
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

  std::cout << "Producing PseudoData from the file '" << jsonFileName << "' with an integrated luminosity of " << luminosity << " fb" <<std::endl;

  std::cout << "Reading JSON file" << std::endl;
  SampleReader samples(jsonFileName, inputDirectory, suffix);
  if(includeSignal)
    samples = samples.getMC();
  else
    samples = samples.getMCBkg();

  // Selection
  TCut muon = "(abs(LepID) == 13)";
  TCut electron = "(abs(LepID) == 11)";
  TCut singlep = (muon || electron) && "LepPt < 30";
  //TCut implicit = "(HT30 > 200) && (Met > 200) && (Jet1Pt > 90) && (LepPt < 30)";
  TCut met    = "Met > 300.";
  TCut ISRjet = "Jet1Pt > 110.";
  TCut dphij1j2 = "DPhiJet1Jet2_30 < 2.5 || Jet2Pt < 60";
  TCut presel = singlep+ISRjet+dphij1j2+met;
  if(noPresel)
    presel = "";

  TDirectory* cwd = gDirectory;

  auto fileSample = samples.getAllFiles()[0];
  TFile tmpFile(fileSample.c_str(), "READ");
  TTree* tmpTree = static_cast<TTree*>(tmpFile.Get("bdttree"));

  TFile outputFile((outputDirectory+"/PseudoData" + ((suffix=="")?(".root"):("_"+suffix+".root"))).c_str(), "RECREATE");
  outputFile.cd();
  TTree* PDtree = static_cast<TTree*>(tmpTree->CloneTree(0));

  delete tmpTree;
  tmpFile.Close();

  // Hard coded variables (because they make no sense being defined for Pseudo-data):
  Float_t XS = 1;
  Float_t Event = -999;
  Float_t LumiSec = -999;
  Float_t Nevt = -999;
  Float_t Run = -999;


  TRandom3 *randomizer = new TRandom3();

  for(auto &process : samples)
  {
    std::cout << "Processing process: " << process.tag() << std::endl;
    for(auto &sample : process)
    {
      std::cout << "\tProcessing sample: " << sample.tag() << std::endl;
      // TODO: if the sample is split into multiple files, this will not work
      for(auto &file : sample)
      {
        std::cout << "\t  Processing file: " << file << std::endl;

        TFile inputFile(file.c_str(), "READ");
        TTree* inputTree = static_cast<TTree*>(inputFile.Get("bdttree"));
        TTree* slimmedTree = inputTree;
        if(presel != "")
          slimmedTree = static_cast<TTree*>(inputTree->CopyTree(presel));
        outputFile.cd();

        float NEvt = 0;
        slimmedTree->SetBranchAddress("Nevt", &NEvt);
        slimmedTree->GetEntry(0);
        double readEvents = slimmedTree->GetEntries();
        double yield = readEvents/NEvt * sample.crossSection() * sample.branchingRatio() * luminosity;

        std::cout << "\t    The total number of initial events: " << NEvt << std::endl;
        std::cout << "\t    " << readEvents << " events were read from the file." << std::endl;
        std::cout << "\t    Yield: " << yield << std::endl;
        if(yield > readEvents)
        {
          std::cout << "ERROR: with the chosen luminosity, the expected yield is larger than the number of available events." << std::endl;
          std::cout << "It is not possible to generate the pseudo data, please choose a different value for the luminosity or different samples with more events" << std::endl;
          return 1;
        }

        std::vector<size_t> eventsToKeep;
        int numberOfEvents = randomizer->Poisson(yield);
        if(numberOfEvents > readEvents)
        {
          std::cout << "WARNING: The poisson smearing resulted in a number of events larger than the events available in the file." << std::endl;
          std::cout << "This may be just bad luck, or the total number of events and the expected yield have values very close to each other, please reevaluate the situation and try again." << std::endl;
          return 1;
        }

        inputTree->CopyAddresses(PDtree);
        PDtree->SetBranchAddress("XS", &XS);
        PDtree->SetBranchAddress("Event", &Event);
        PDtree->SetBranchAddress("LumiSec", &LumiSec);
        PDtree->SetBranchAddress("Nevt", &Nevt);
        PDtree->SetBranchAddress("Run", &Run);

        std::cout << "\t    Selecting events to keep" << std::endl;
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

        std::cout << "\t    Copying selected events from input to output" << std::endl;
        for(auto & i : eventsToKeep)
        {
          slimmedTree->GetEntry(i);
          PDtree->Fill();
        }

        delete slimmedTree;
        delete inputTree;
      }
    }
  }

  outputFile.cd();
  PDtree->Write("",TObject::kOverwrite);
  cwd->cd();

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
