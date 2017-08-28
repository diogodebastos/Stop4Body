#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TH1F.h>
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
#include "UserCode/Stop4Body/interface/commonFunctions.h"
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

extern TH2D* centralElectronSFHist;
extern TH2D* centralMuonSFHist;
extern TH1F* hephyElectronIDSFHistBarrel;
extern TH1F* hephyElectronIDSFHistEndcap;
extern TH1F* hephyMuonIDSFHist;
extern TH1F* hephyElectronISOSFHistBarrel;
extern TH1F* hephyElectronISOSFHistEndcap;
extern TH1F* hephyMuonISOSFHist;
extern TH1F* electronTightToLooseLowEta;
extern TH1F* electronTightToLooseHighEta;
extern TH1F* muonTightToLooseLowEta;
extern TH1F* muonTightToLooseHighEta;

int main(int argc, char** argv)
{
  std::string jsonFileName = "";
  std::string inputDirectory = "";
  std::string outputDirectory = "./OUT/";
  std::string suffix = "";

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

  TDirectory* cwd = gDirectory;
  TFile puWeightFile((outputDirectory + "/puWeights.root").c_str(), "READ");
  TFile centralElectronSFFile("../data/scaleFactors.root", "READ");
  centralElectronSFHist = static_cast<TH2D*>(centralElectronSFFile.Get("GsfElectronToCutBasedSpring15V"));
  TFile centralMuonSFFile("../data/TnP_NUM_LooseID_DENOM_generalTracks_VAR_map_pt_eta.root", "READ");
  centralMuonSFHist = static_cast<TH2D*>(centralMuonSFFile.Get("SF"));
  TFile hephySFFile("../data/hephy_scale_factors_10p0binning.root", "READ");
  hephyElectronIDSFHistBarrel = static_cast<TH1F*>(hephySFFile.Get("ele_SF_IdSpec_barrel"));
  hephyElectronIDSFHistEndcap = static_cast<TH1F*>(hephySFFile.Get("ele_SF_IdSpec_endcap"));
  hephyMuonIDSFHist = static_cast<TH1F*>(hephySFFile.Get("muon_SF_Id_all"));
  hephyElectronISOSFHistBarrel = static_cast<TH1F*>(hephySFFile.Get("ele_SF_IpIso_barrel"));
  hephyElectronISOSFHistEndcap = static_cast<TH1F*>(hephySFFile.Get("ele_SF_IpIso_endcap"));
  hephyMuonISOSFHist = static_cast<TH1F*>(hephySFFile.Get("muon_SF_IpIsoSpec_all"));
  TFile tightToLooseRatios("../data/tightToLooseRatios.root", "READ");
  electronTightToLooseLowEta = static_cast<TH1F*>(tightToLooseRatios.Get("electronEfficiencyLowEta"));
  electronTightToLooseHighEta = static_cast<TH1F*>(tightToLooseRatios.Get("electronEfficiencyHighEta"));
  muonTightToLooseLowEta = static_cast<TH1F*>(tightToLooseRatios.Get("muonEfficiencyLowEta"));
  muonTightToLooseHighEta = static_cast<TH1F*>(tightToLooseRatios.Get("muonEfficiencyHighEta"));
  cwd->cd();

  Float_t triggerEfficiency;
  Float_t EWKISRweight;
  Float_t ISRweight;
  Float_t puWeight;
  Float_t leptonIDSF;
  Float_t leptonISOSF;
  Float_t weight;
  //Float_t nTrueInt;
  Float_t Met;
  Float_t LepPt;
  Float_t nIsr;

  ValueWithSystematics<Float_t> triggerEfficiencySys;
  ValueWithSystematics<Float_t> EWKISRweightSys;
  ValueWithSystematics<Float_t> ISRweightSys;
  ValueWithSystematics<Float_t> puWeightSys;
  ValueWithSystematics<Float_t> leptonIDSFSys;
  ValueWithSystematics<Float_t> leptonISOSFSys;
  ValueWithSystematics<Float_t> weightSys;

  triggerEfficiencySys = triggerEfficiencyFromMETSys(200.0);
  EWKISRweightSys = EWKISRweightFromISRpTSys(20, 1.0, 40, 0.7);
  ISRweightSys = ISRweightFromNISRJetSys(2);
  puWeightSys = 1;
  //puWeightSys.Systematic("PU_UP");
  //puWeightSys.Systematic("PU_DOWN");
  leptonIDSFSys = getLeptonIDSF(LepID, LepPt, LepEta);
  leptonISOSFSys = getLeptonISOSF(LepID, LepPt, LepEta);

  weightSys = puWeightSys * triggerEfficiencySys * EWKISRweightSys * ISRweightSys * leptonIDSFSys * leptonISOSFSys;

  triggerEfficiencySys.Lock();
  EWKISRweightSys.Lock();
  ISRweightSys.Lock();
  puWeightSys.Lock();
  leptonIDSFSys.Lock();
  leptonISOSFSys.Lock();
  weightSys.Lock();

  for(auto &process : samples)
  {
    std::cout << "Processing process: " << process.tag() << std::endl;

    double ISRCParam = 1;
    double EWKISRCParam = 1;

    TH1D* puWeightDistrib     = static_cast<TH1D*>(puWeightFile.Get( ("process_"+process.tag()+"_puWeight").c_str())->Clone("puWeightDistrib"));
    TH1D* puWeightDistribUp   = static_cast<TH1D*>(puWeightFile.Get( ("process_"+process.tag()+"_puWeight_up").c_str())->Clone("puWeightDistribUp"));
    TH1D* puWeightDistribDown = static_cast<TH1D*>(puWeightFile.Get( ("process_"+process.tag()+"_puWeight_down").c_str())->Clone("puWeightDistribDown"));

    TVectorD* processISRWeights = static_cast<TVectorD*>(puWeightFile.Get(("process_"+process.tag()+"_ISRCParams").c_str())->Clone("processISRWeights"));
    ISRCParam =    (*processISRWeights)[0];
    EWKISRCParam = (*processISRWeights)[1];
    delete processISRWeights;

    for(auto &sample : process)
    {
      std::cout << "  Processing sample: " << sample.tag() << std::endl;

      for(auto &file : sample)
      {
        cwd = gDirectory;

        //std::string outFile = file.substr(0, file.find_last_of("."))+"_bdt.root";
        std::string outFile = file;
        replace(outFile, inputDirectory, outputDirectory);
        std::cout << "    Processing file " << file << " and saving output in " << outFile << std::endl;

        TFile *inputFile = new TFile(file.c_str(), "READ");
        TFile *outputFile = new TFile(outFile.c_str(), "RECREATE");

        TTree* OutputTree = static_cast<TTree*>(inputFile->Get("bdttree"))->CloneTree();

        OutputTree->SetBranchAddress("triggerEfficiency", &triggerEfficiency);
        OutputTree->SetBranchAddress("EWKISRweight", &EWKISRweight);
        OutputTree->SetBranchAddress("ISRweight", &ISRweight);
        OutputTree->SetBranchAddress("puWeight", &puWeight);
        OutputTree->SetBranchAddress("leptonIDSF", &leptonIDSF);
        OutputTree->SetBranchAddress("leptonISOSF", &leptonISOSF);
        OutputTree->SetBranchAddress("weight", &weight);
        //OutputTree->SetBranchAddress("nTrueInt", &nTrueInt);
        OutputTree->SetBranchAddress("Met", &Met);
        OutputTree->SetBranchAddress("LepPt", &LepPt);
        OutputTree->SetBranchAddress("nIsr", &nIsr);

        std::vector<TBranch*> branchesToSave;

        if(!process.isdata())
        {
          for(auto& syst: triggerEfficiencySys.Systematics())
          {
            TBranch* thisBranch = OutputTree->Branch(("triggerEfficiency_" + syst).c_str(), &(triggerEfficiencySys.Systematic(syst)));
            branchesToSave.push_back(thisBranch);
          }
          for(auto& syst: EWKISRweightSys.Systematics())
          {
            TBranch* thisBranch = OutputTree->Branch(("EWKISRweight_" + syst).c_str(), &(EWKISRweightSys.Systematic(syst)));
            branchesToSave.push_back(thisBranch);
          }
          for(auto& syst: ISRweightSys.Systematics())
          {
            TBranch* thisBranch = OutputTree->Branch(("ISRweight_" + syst).c_str(), &(ISRweightSys.Systematic(syst)));
            branchesToSave.push_back(thisBranch);
          }
          for(auto& syst: puWeightSys.Systematics())
          {
            TBranch* thisBranch = OutputTree->Branch(("puWeight_" + syst).c_str(), &(puWeightSys.Systematic(syst)));
            branchesToSave.push_back(thisBranch);
          }
          for(auto& syst: leptonIDSFSys.Systematics())
          {
            TBranch* thisBranch = OutputTree->Branch(("leptonIDSF_" + syst).c_str(), &(leptonIDSFSys.Systematic(syst)));
            branchesToSave.push_back(thisBranch);
          }
          for(auto& syst: leptonISOSFSys.Systematics())
          {
            TBranch* thisBranch = OutputTree->Branch(("leptonISOSF_" + syst).c_str(), &(leptonISOSFSys.Systematic(syst)));
            branchesToSave.push_back(thisBranch);
          }
          for(auto& syst: weightSys.Systematics())
          {
            TBranch* thisBranch = OutputTree->Branch(("weight_" + syst).c_str(), &(weightSys.Systematic(syst)));
            branchesToSave.push_back(thisBranch);
          }
        }

        std::cout << "--- Processing: " << OutputTree->GetEntries() << " events" << std::endl;
        TStopwatch sw;
        sw.Start();

        for (Long64_t ievt=0; ievt < OutputTree->GetEntries(); ++ievt)
        {
          if (ievt%1000 == 0) std::cout << "--- ... Processing event: " << ievt << std::endl;

          OutputTree->GetEntry(ievt);
          triggerEfficiencySys = triggerEfficiency;
          EWKISRweightSys = EWKISRweight;
          ISRweightSys = ISRweight;
          puWeightSys = puWeight;
          leptonIDSFSys = leptonIDSF;
          leptonISOSFSys = leptonISOSF;

          if(!process.isdata())
          {
            //puWeightSys = puWeightDistrib->GetBinContent(puWeightDistrib->FindBin(nTrueInt));
            //puWeightSys.Systematic("PU_UP") = puWeightDistribUp->GetBinContent(puWeightDistrib->FindBin(nTrueInt));
            //puWeightSys.Systematic("PU_DOWN") = puWeightDistribDown->GetBinContent(puWeightDistrib->FindBin(nTrueInt));
            triggerEfficiencySys = triggerEfficiencyFromMETSys(Met);
            // For EWK ISR, assume syst 100%
            if(process.tag() == "WJets")
              EWKISRweightSys = EWKISRweightFromISRpTSys(LepPt, lep_phi, Met, met_phi) * EWKISRCParam;
            // For ISR, assume syst 50%
            if(process.tag() == "ttbar" || process.tag() == "ttbar_lep" || process.tag() == "ttbar_lo" || process.issignal())
              ISRweightSys = ISRweightFromNISRJetSys(nIsr) * ISRCParam;
          }
          else
          {
            puWeightSys = 1;
            triggerEfficiencySys = 1;
            EWKISRweightSys = 1;
            ISRweightSys = 1;
            leptonIDSFSys = 1;
            leptonISOSFSys = 1;
            weightSys = 1;
          }



          for(auto &branch : branchesToSave)
          {
            branch->Fill();
          }
        }

        sw.Stop();
        std::cout << "--- End of event loop: ";
        sw.Print();

        OutputTree->Write();
        cwd->cd();
        outputFile->Close();
        inputFile->Close();
      }
    }
  }

          /*
          if(!process.isdata())
          {
            triggerEfficiency = static_cast<double>(triggerEfficiencyFromMET(met_pt));
            // For EWK ISR, assume syst 100%
            if(process.tag() == "WJets")
              EWKISRweight = EWKISRCParam * static_cast<double>(EWKISRweightFromISRpT(LepPt, lep_phi, met_pt, met_phi));
            // For ISR, assume syst 50%
            if(process.tag() == "ttbar" || process.tag() == "ttbar_lep" || process.tag() == "ttbar_lo" || process.issignal())
              ISRweight = ISRCParam * static_cast<double>(ISRweightFromNISRJet(nIsr));
            // TODO: missing tt_pow reweighting https://twiki.cern.ch/twiki/bin/viewauth/CMS/SUSRecommendationsMoriond17
          }
          if(!process.isdata())
            weight = puWeight*XS*filterEfficiency*(genWeight/sumGenWeight)*triggerEfficiency*EWKISRweight*ISRweight*leptonIDSF*leptonISOSF;
          else
            weight = 1;

            if(!process.isdata())
            {
              leptonIDSF = static_cast<double>(getLeptonIDSF(LepID, LepPt, LepEta));
              leptonISOSF = static_cast<double>(getLeptonISOSF(LepID, LepPt, LepEta));
            }*/

  for(auto & file : filesToProcess)
  {
    auto cwd = gDirectory;

    //std::string outFile = file.substr(0, file.find_last_of("."))+"_bdt.root";
    std::string outFile = file;
    replace(outFile, inputDirectory, outputDirectory);
    std::cout << "Processing file " << file << " and saving output in " << outFile << std::endl;

    TFile *inputFile = new TFile(file.c_str(), "READ");
    TFile *outputFile = new TFile(outFile.c_str(), "RECREATE");

    TTree* OutputTree = static_cast<TTree*>(inputFile->Get("bdttree"))->CloneTree();

    OutputTree->SetBranchAddress("triggerEfficiency", &triggerEfficiency);
    OutputTree->SetBranchAddress("EWKISRweight", &EWKISRweight);
    OutputTree->SetBranchAddress("ISRweight", &ISRweight);
    OutputTree->SetBranchAddress("puWeight", &puWeight);
    OutputTree->SetBranchAddress("leptonIDSF", &leptonIDSF);
    OutputTree->SetBranchAddress("leptonISOSF", &leptonISOSF);
    OutputTree->SetBranchAddress("weight", &weight);

    std::vector<TBranch*> branchesToSave;

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

  std::cout << "Done!" << std::endl;

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
