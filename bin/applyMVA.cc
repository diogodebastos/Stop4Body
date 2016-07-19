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
  std::string inputDirectory = "./IN/";
  std::string outputDirectory = "./OUT/";
  std::vector<std::string> methods;
  methods.clear();

  // Default MVA methods to be trained + tested
  std::map<std::string,int> Use;
  // --- Boosted Decision Trees
  Use["BDT"]             = 0; // uses Adaptive Boost
  Use["BDTG"]            = 0; // uses Gradient Boost
  Use["BDTB"]            = 0; // uses Bagging
  Use["BDTD"]            = 0; // decorrelation + Adaptive Boost
  Use["BDTF"]            = 0; // allow usage of fisher discriminant for node splitting
  //
  // --- Friedman's RuleFit method, ie, an optimised series of cuts ("rules")
  Use["RuleFit"]         = 0;

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

    if(argument == "--method")
      methods.push_back(argv[++i]);
  }

  if(jsonFileName == "")
  {
    std::cout << "You must define a json file" << std::endl;
    return 0;
  }

  for(auto & method : methods)
  {
    if(Use.count(method) != 1)
    {
      std::cout << "Unknown method \"" << method << "\", ignoring the request" << std::endl;
    }
    else
      Use[method] = 1;
  }

  std::cout << "Reading json file" << std::endl;
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
        std::string path = "";
        path += file["tag"];
        path += "_bdt.root";
        std::string identifier = process["tag"];
        identifier += ":";
        identifier += file["tag"];
        if(path == "")
          emptyPath.push_back(identifier);
        else
        {
          if(fileExists(inputDirectory+"/"+path))
          {
            FileInfo tmpFile;

            tmpFile.path = path;
            tmpFile.crossSection = file["xsec"];
            tmpFile.branchingRatio = file["br"];
            tmpFile.tag = file["tag"];

            filesToProcess.push_back(tmpFile);
          }
          else
            invalidPath.push_back(identifier + "  ->  " + inputDirectory + "/" + path);
        }
      }
    }
    else
      emptyLines.push_back(process["tag"]);
  }

  std::cout << "Processing " << filesToProcess.size() << " files." << std::endl;

  // Load TMVA
  TMVA::Tools::Instance();

  std::cout << std::endl;
  std::cout << "==> Start TMVAClassificationApplication" << std::endl;

  TMVA::Reader *reader = new TMVA::Reader( "!Color:!Silent" );

  Float_t LepPt, LepEta, LepDxy, LepDz, Met, mt, Q80, CosDeltaPhi;
  Float_t Jet1Pt,Jet1Eta,Jet2Pt,Jet2Eta,JetHBpt,DrJet1Lep,DrJet2Lep,DrJetHBLep,DrJet1Jet2,JetLepMass;
  Float_t J3Mass,HT20,XS;
  Int_t Njet, NbLoose30, NbTight30, LepID, LepChg, Nevt, Event;
  //Int_t nGoodMu, nGoodEl;
  //Float_t LepSip3, LepIso03, LepIso04, HT25, HT30, DPhiJet1Jet2,

  Float_t NbLoose30f, LepChgf, Njetf;

  // Base BDT
  reader->AddVariable("Jet1Pt", &Jet1Pt);
  reader->AddVariable("mt", &mt);
  reader->AddVariable("Met", &Met);
  reader->AddVariable("HT20", &HT20);
  reader->AddVariable("NbLoose30", &NbLoose30f);
  reader->AddVariable("LepChg", &LepChgf);
  reader->AddVariable("LepEta", &LepEta);
  reader->AddVariable("LepPt", &LepPt);
  //reader->AddVariable("Q80", &Q80);
  //reader->AddVariable("CosDeltaPhi", &CosDeltaPhi);
  //reader->AddVariable("Jet1Pt", &Jet1Pt);
  //reader->AddVariable("Jet2Pt", &Jet2Pt);
  reader->AddVariable("JetHBpt", &JetHBpt);
  reader->AddVariable("Njet", &Njetf);
  //reader->AddVariable("JetLepMass",&JetLepMass);
  //reader->AddVariable("J3Mass",&J3Mass);
  //reader->AddVariable("DrJet1Lep", &DrJet1Lep);
  reader->AddVariable("DrJetHBLep",&DrJetHBLep);
  //reader->AddVariable("DrJet1Jet2",&DrJet1Jet2);
  //reader->AddVariable("HT25",&HT25);

  TString dir    = "weights/";
  TString prefix = "TMVAClassification";

  for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++)
  {
    if (it->second)
    {
      TString methodName = TString(it->first) + TString(" method");
      std::cout << "method being used " << methodName << std::endl;
      TString weightfile = dir + prefix + TString("_") + TString(it->first) + TString(".weights.xml");
      std::cout << "weight file being used : "    <<  weightfile << std::endl;
      reader->BookMVA( methodName, weightfile );
    }
  }

  for(auto & file : filesToProcess)
  {
    auto cwd = gDirectory;

    std::cout << "Processing file " << inputDirectory + "/" + file.path << std::endl;
    TFile *inputFile = new TFile((inputDirectory + "/" + file.path).c_str(), "READ");
    TFile *outputFile = new TFile((outputDirectory + "/" + file.path).c_str(), "RECREATE");

    TTree* OutputTree = static_cast<TTree*>(inputFile->Get("bdttree"))->CloneTree();

    OutputTree->SetBranchAddress("LepID", &LepID);
    OutputTree->SetBranchAddress("LepChg", &LepChg);
    OutputTree->SetBranchAddress("LepPt", &LepPt);
    OutputTree->SetBranchAddress("LepEta", &LepEta);
    OutputTree->SetBranchAddress("LepDxy", &LepDxy);
    OutputTree->SetBranchAddress("LepDz", &LepDz);
    //OutputTree->SetBranchAddress("LepIso", &LepIso);
    OutputTree->SetBranchAddress("Met", &Met);
    OutputTree->SetBranchAddress("mt",  &mt);
    OutputTree->SetBranchAddress("Q80", &Q80);
    OutputTree->SetBranchAddress("CosDeltaPhi",&CosDeltaPhi);
    OutputTree->SetBranchAddress("NbLoose30", &NbLoose30);
    OutputTree->SetBranchAddress("NbTight30", &NbTight30);
    OutputTree->SetBranchAddress("Njet", &Njet);
    OutputTree->SetBranchAddress("Jet1Pt", &Jet1Pt);
    OutputTree->SetBranchAddress("Jet1Eta", &Jet1Eta);
    OutputTree->SetBranchAddress("Jet2Pt", &Jet2Pt);
    OutputTree->SetBranchAddress("Jet2Eta", &Jet2Eta);
    OutputTree->SetBranchAddress("JetHBpt", &JetHBpt);
    OutputTree->SetBranchAddress("DrJet1Lep", &DrJet1Lep);
    OutputTree->SetBranchAddress("DrJet2Lep", &DrJet2Lep);
    OutputTree->SetBranchAddress("DrJetHBLep",&DrJetHBLep);
    OutputTree->SetBranchAddress("DrJet1Jet2",&DrJet1Jet2);
    OutputTree->SetBranchAddress("JetLepMass",&JetLepMass);
    OutputTree->SetBranchAddress("J3Mass",&J3Mass);
    OutputTree->SetBranchAddress("HT20",&HT20);
    //OutputTree->SetBranchAddress("HT25",&HT25);
    //OutputTree->SetBranchAddress("HT30",&HT30);
    OutputTree->SetBranchAddress("XS", &XS);
    OutputTree->SetBranchAddress("Event", &Event);
    OutputTree->SetBranchAddress("Nevt", &Nevt);

    std::map<std::string, Float_t> MVAs;
    for(auto & mva : Use)
      if(mva.second)
        MVAs[mva.first] = 0;

    std::map<std::string, TBranch* > MVABranches;
    for(auto & mva : MVAs)
    {
      std::cout << "Creating branch for \"" << mva.first << "\"" << std::endl;
      MVABranches[mva.first] = OutputTree->Branch(mva.first.c_str(), &mva.second);
    }

    std::cout << "--- Processing: " << OutputTree->GetEntries() << " events" << std::endl;
    TStopwatch sw;
    sw.Start();

    for (Long64_t ievt=0; ievt < OutputTree->GetEntries(); ievt++)
    {
      if (ievt%1000 == 0) std::cout << "--- ... Processing event: " << ievt << std::endl;

      OutputTree->GetEntry(ievt);
      NbLoose30f = NbLoose30;
      LepChgf = LepChg;
      Njetf = Njet;
      for(auto & mva : MVAs)
      {
        //if (ievt%1000 == 0) std::cout << "Evaluating method: \"" << mva.first << "\"" << std::endl;
        mva.second = reader->EvaluateMVA((mva.first + " method").c_str());
        MVABranches[mva.first]->Fill();
      }
    }

    sw.Stop();
    std::cout << "--- End of event loop: ";
    sw.Print();

    OutputTree->Write();
    //outputFile->Write();
    cwd->cd();
    outputFile->Close();
    inputFile->Close();
  }




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

  return;
}

bool fileExists(std::string fileName)
{
  std::ifstream infile(fileName);
  return infile.good();
}
