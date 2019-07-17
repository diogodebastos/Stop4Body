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

#include "UserCode/Stop4Body/interface/json.hpp"
#include "UserCode/Stop4Body/interface/SampleReader.h"
#include "UserCode/Stop4Body/interface/ValueWithSystematics.h"

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
  std::string fileName = "";
  std::string outputDirectory = "./OUT/";
  std::string MVAsuffix = "";
  std::string weightDirectory = "";

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

    if(argument == "--inFile")
      fileName = argv[++i];

    if(argument == "--outDir")
      outputDirectory = argv[++i];

    if(argument == "--mvaSuffix")
      MVAsuffix = argv[++i];

    if(argument == "--weightDir")
      weightDirectory = argv[++i];
  }

  if(fileName == "")
  {
    std::cout << "You must define an input file" << std::endl;
    return 1;
  }

  std::vector<std::string> filesToProcess;
  filesToProcess.push_back(fileName);

  std::cout << "Processing " << filesToProcess.size() << " files." << std::endl;

  // Load TMVA
  TMVA::Tools::Instance();

  std::cout << std::endl;
  std::cout << "==> Start TMVAClassificationApplication" << std::endl;

  TMVA::Reader *reader = new TMVA::Reader( "!Color:!Silent" );

  Float_t LepPt, LepEta, LepDxy, LepDz, Met, mt, Q80, CosDeltaPhi;
  Float_t Jet1Pt,Jet1Eta,Jet2Pt,Jet2Eta,JetHBpt,DrJet1Lep,DrJet2Lep,DrJetHBLep,DrJet1Jet2,JetLepMass;
  Float_t J3Mass,XS,HT;
  Float_t Njet, NbLoose, NbTight, LepID, LepChg, Nevt;
  ULong64_t Event;
  Float_t JetHBCSV;
  //Int_t nGoodMu, nGoodEl;
  //Float_t LepSip3, LepIso03, LepIso04, DPhiJet1Jet2,
  ValueWithSystematics<float> Jet1PtSys;
  ValueWithSystematics<float> mtSys;
  ValueWithSystematics<float> MetSys;
  ValueWithSystematics<float> HTSys;
  ValueWithSystematics<float> NbLooseSys;
  ValueWithSystematics<float> NjetSys;
  ValueWithSystematics<float> JetHBptSys;
  ValueWithSystematics<float> DrJetHBLepSys;
  ValueWithSystematics<float> JetHBCSVSys;
  std::vector<std::string> variations = {"JES_Up", "JES_Down", "JER_Up", "JER_Down"};
  if(fileName.find("Data") != std::string::npos) variations = {};
  for(auto& syst : variations)
  {
    Jet1PtSys.Systematic(syst);
    mtSys.Systematic(syst);
    MetSys.Systematic(syst);
    HTSys.Systematic(syst);
    NbLooseSys.Systematic(syst);
    NjetSys.Systematic(syst);
    JetHBptSys.Systematic(syst);
    DrJetHBLepSys.Systematic(syst);
    JetHBCSVSys.Systematic(syst);
  }

  // SET0
  reader->AddVariable("Jet1Pt", &Jet1Pt);
  reader->AddVariable("mt", &mt);
  reader->AddVariable("Met", &Met);
  reader->AddVariable("LepChg", &LepChg);
  reader->AddVariable("LepEta", &LepEta);
  reader->AddVariable("LepPt", &LepPt);
  reader->AddVariable("JetHBpt", &JetHBpt);
  reader->AddVariable("DrJetHBLep",&DrJetHBLep);
  reader->AddVariable("Njet", &Njet);
  reader->AddVariable("HT", &HT);
  reader->AddVariable("NbLoose", &NbLoose);
  // SET1
  // SET2
  reader->AddVariable("JetHBCSV",&JetHBCSV);
  // SET3
  //reader->AddVariable("JetLepMass",&JetLepMass);
  //reader->AddVariable("J3Mass",&J3Mass);

  //reader->AddVariable("NbLooseTo50",&NbLooseTo50);
  //reader->AddVariable("NbLoose50",&NbLoose50);
  //reader->AddVariable("NbMediumTo50",&NbMediumTo50);
  //reader->AddVariable("NbMedium50",&NbMedium50);
  //reader->AddVariable("NbTightTo50",&NbTightTo50);
  //reader->AddVariable("NbTight50",&NbTight50);
  //reader->AddVariable("nGoodTrack", &nGoodTrack);
  //reader->AddVariable("Jet1Eta", &Jet1Eta);
  //reader->AddVariable("Jet2Pt", &Jet2Pt);
  //reader->AddVariable("DrJet1Lep", &DrJet1Lep);
  //reader->AddVariable("Q80", &Q80);
  //reader->AddVariable("CosDeltaPhi", &CosDeltaPhi);
  //reader->AddVariable("DrJet1Jet2",&DrJet1Jet2);
  //reader->AddVariable("Jet1CSV", &Jet1CSV);
  //reader->AddVariable("Jet2CSV", &Jet2CSV);
  //reader->AddVariable("Jet3CSV", &Jet3CSV);

  TString dir    = "weights/";
  if(weightDirectory != "")
  {
    dir = (weightDirectory+"/").c_str();
  }
  TString prefix = "TMVAClassification";

  TString methodName = TString("BDT method");
  std::cout << "method being used " << methodName << std::endl;
  TString weightfile = dir + prefix + TString("_BDT");
  if(MVAsuffix != "")
    weightfile += TString("_"+MVAsuffix);
  weightfile += TString(".weights.xml");
  std::cout << "weight file being used : "    <<  weightfile << std::endl;
  reader->BookMVA( methodName, weightfile );

  for(auto & file : filesToProcess)
  {
    auto cwd = gDirectory;

    std::string outFile = file.substr(0, file.find_last_of("."))+"_bdt.root";
    std::string inputDirectory = file.substr(0, file.find_last_of("/"));
    replace(outFile, inputDirectory, outputDirectory);
    std::cout << "Processing file " << file << " and saving output in " << outFile << std::endl;

    TFile *inputFile = new TFile(file.c_str(), "READ");
    TFile *outputFile = new TFile(outFile.c_str(), "RECREATE");

    TTree* OutputTree = static_cast<TTree*>(inputFile->Get("bdttree"))->CloneTree();

    OutputTree->SetBranchAddress("LepID", &LepID);
    OutputTree->SetBranchAddress("LepChg", &LepChg);
    OutputTree->SetBranchAddress("LepPt", &LepPt);
    OutputTree->SetBranchAddress("LepEta", &LepEta);
    OutputTree->SetBranchAddress("LepDxy", &LepDxy);
    OutputTree->SetBranchAddress("LepDz", &LepDz);
    //OutputTree->SetBranchAddress("LepIso", &LepIso);
    OutputTree->SetBranchAddress("XS", &XS);
    OutputTree->SetBranchAddress("Event", &Event);
    OutputTree->SetBranchAddress("Nevt", &Nevt);

    // These quantities are affected by systematics
    OutputTree->SetBranchAddress("Q80", &Q80);
    OutputTree->SetBranchAddress("CosDeltaPhi",&CosDeltaPhi);
    OutputTree->SetBranchAddress("NbTight", &NbTight);
    OutputTree->SetBranchAddress("Jet1Eta", &Jet1Eta);
    OutputTree->SetBranchAddress("Jet2Pt", &Jet2Pt);
    OutputTree->SetBranchAddress("Jet2Eta", &Jet2Eta);
    OutputTree->SetBranchAddress("DrJet1Lep", &DrJet1Lep);
    OutputTree->SetBranchAddress("DrJet2Lep", &DrJet2Lep);
    OutputTree->SetBranchAddress("DrJet1Jet2",&DrJet1Jet2);
    OutputTree->SetBranchAddress("JetLepMass",&JetLepMass);
    OutputTree->SetBranchAddress("J3Mass",&J3Mass);

    OutputTree->SetBranchAddress("Jet1Pt",     &(Jet1PtSys.Value()));
    OutputTree->SetBranchAddress("mt",         &(mtSys.Value()));
    OutputTree->SetBranchAddress("Met",        &(MetSys.Value()));
    OutputTree->SetBranchAddress("HT",         &(HTSys.Value()));
    OutputTree->SetBranchAddress("NbLoose",    &(NbLooseSys.Value()));
    OutputTree->SetBranchAddress("Njet",       &(NjetSys.Value()));
    OutputTree->SetBranchAddress("JetHBpt",    &(JetHBptSys.Value()));
    OutputTree->SetBranchAddress("DrJetHBLep", &(DrJetHBLepSys.Value()));
    OutputTree->SetBranchAddress("JetHBCSV",   &(JetHBCSVSys.Value()));
    for(auto& syst : variations)
    {
      OutputTree->SetBranchAddress(("Jet1Pt_" + syst).c_str(),     &(Jet1PtSys.Systematic(syst)));
      OutputTree->SetBranchAddress(("mt_" + syst).c_str(),         &(mtSys.Systematic(syst)));
      OutputTree->SetBranchAddress(("Met_" + syst).c_str(),        &(MetSys.Systematic(syst)));
      OutputTree->SetBranchAddress(("HT_" + syst).c_str(),         &(HTSys.Systematic(syst)));
      OutputTree->SetBranchAddress(("NbLoose_" + syst).c_str(),    &(NbLooseSys.Systematic(syst)));
      OutputTree->SetBranchAddress(("Njet_" + syst).c_str(),       &(NjetSys.Systematic(syst)));
      OutputTree->SetBranchAddress(("JetHBpt_" + syst).c_str(),    &(JetHBptSys.Systematic(syst)));
      OutputTree->SetBranchAddress(("DrJetHBLep_" + syst).c_str(), &(DrJetHBLepSys.Systematic(syst)));
      OutputTree->SetBranchAddress(("JetHBCSV_" + syst).c_str(),   &(JetHBCSVSys.Systematic(syst)));
    }

    std::map<std::string, ValueWithSystematics<Float_t>> MVAs;
    MVAs["BDT"] = 0;
    for(auto& syst : variations)
    {
      MVAs["BDT"].Systematic(syst) = 0;
    }

    std::map<std::string, TBranch* > MVABranches;
    for(auto & mva : MVAs)
    {
      std::cout << "Creating branch for \"" << mva.first << "\"" << std::endl;
      MVABranches[mva.first] = OutputTree->Branch(mva.first.c_str(), &(mva.second.Value()));
      std::cout << "Creating branches for the systematic variations:" << std::endl;
      for(auto& syst : variations)
      {
        std::cout << "  - " << syst << std::endl;
        MVABranches[mva.first + "_" + syst] = OutputTree->Branch((mva.first + "_" + syst).c_str(), &(mva.second.Systematic(syst)));
      }
    }

    std::cout << "--- Processing: " << OutputTree->GetEntries() << " events" << std::endl;
    TStopwatch sw;
    sw.Start();

    for (Long64_t ievt=0; ievt < OutputTree->GetEntries(); ievt++)
    {
      if (ievt%1000 == 0) std::cout << "--- ... Processing event: " << ievt << std::endl;

      OutputTree->GetEntry(ievt);
      for(auto & mva : MVAs)
      {
        Jet1Pt = Jet1PtSys.Value();
        mt = mtSys.Value();
        Met = MetSys.Value();
        HT = HTSys.Value();
        NbLoose = NbLooseSys.Value();
        Njet = NjetSys.Value();
        JetHBpt = JetHBptSys.Value();
        DrJetHBLep = DrJetHBLepSys.Value();
        JetHBCSV = JetHBCSVSys.Value();
        //if (ievt%1000 == 0) std::cout << "Evaluating method: \"" << mva.first << "\"" << std::endl;
        mva.second.Value() = reader->EvaluateMVA((mva.first + " method").c_str());
        MVABranches[mva.first]->Fill();

        for(auto& syst : variations)
        {
          Jet1Pt = Jet1PtSys.Systematic(syst);
          mt = mtSys.Systematic(syst);
          Met = MetSys.Systematic(syst);
          HT = HTSys.Systematic(syst);
          NbLoose = NbLooseSys.Systematic(syst);
          Njet = NjetSys.Systematic(syst);
          JetHBpt = JetHBptSys.Systematic(syst);
          DrJetHBLep = DrJetHBLepSys.Systematic(syst);
          JetHBCSV = JetHBCSVSys.Systematic(syst);

          mva.second.Systematic(syst) = reader->EvaluateMVA((mva.first + " method").c_str());
          MVABranches[mva.first + "_" + syst]->Fill();
        }
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

  std::cout << "Done!" << std::endl << std::endl;

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
