#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <list>
#include <iterator>
#include <algorithm>
#include <iomanip>
#include <math.h>
#include <memory>
#include <cmath>
#include <cstdlib>

#include "TROOT.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TChain.h"
#include "TObject.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TLegend.h"
#include "TGraph.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TTree.h"
#include "TF1.h"
#include "TCutG.h"
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
#include "TMultiGraph.h"
#include "TPaveText.h"
#include "THStack.h"
#include "TStyle.h"
#include "TLatex.h"
#include "TLorentzVector.h"
#include "TVectorD.h"

#include "UserCode/Stop4Body/interface/json.hpp"
#include "UserCode/Stop4Body/interface/SampleReader.h"
#include "UserCode/Stop4Body/interface/commonFunctions.h"
#include "UserCode/Stop4Body/interface/doubleWithUncertainty.h"
#include "UserCode/Stop4Body/interface/ValueWithSystematics.h"
#include "UserCode/Stop4Body/interface/LHEweightMAPs.h"

#include "CondFormats/BTauObjects/interface/BTagCalibration.h"
#include "CondTools/BTau/interface/BTagCalibrationReader.h"

#define GENPART_LIMIT  40
#define JETCOLL_LIMIT  40
#define LEPCOLL_LIMIT  40
#define TAUCOLL_LIMIT  40
#define TRACOLL_LIMIT 200
#define LHEWEIGHT_LIMIT  500

#define ISR_JET_PT 90
#define SECOND_LEPTON_PT 20
#define MIN_MET 100

using json = nlohmann::json;

struct FileInfo
{
  std::string path;
  double crossSection;
  double branchingRatio;
  std::string tag;
};

void printHelp();

const double ECALGap_MinEta =  1.4442; // ECAL gap parameters
const double ECALGap_MaxEta =  1.5660;
const double CSV_Loose = 0.5426;
const double CSV_Medium = 0.800; // TODO: Update
const double CSV_Tight = 0.935;

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
  std::string outputDirectory = "./OUT/";
  std::string lheScaleDir = "";
  bool doSync = false;
  bool noSkim = false;
  std::string suffix = "";
  size_t max_sync_count = 0;
  double jetPtThreshold = 30;
  bool overrideXSec = false;
  bool swap = false;
  bool looseNotTight = false;
  bool preemptiveDropEvents = false;
  bool doLooseLeptons = false;
  bool doPromptTagging = false;
  bool noTrim = false;
  int part = -1;
  std::string bTagCalibrationFile = "";

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

    if(argument == "--lheScaleDir")
      lheScaleDir = argv[++i];

    if(argument == "--doSync")
    {
      std::stringstream converter;
      doSync = true;
      converter << argv[++i];
      converter >> max_sync_count;
    }

    if(argument == "--suffix")
      suffix = argv[++i];

    if(argument == "--noSkim")
      noSkim = true;

    if(argument == "--noTrim")
      noTrim = true;

    if(argument == "--jetPtThreshold")
    {
      std::stringstream converter;
      converter << argv[++i];
      converter >> jetPtThreshold;
    }

    if(argument == "--overrideXSec")
      overrideXSec = true;

    if(argument == "--swap")
      swap = true;

    if(argument == "--looseNotTight")
      looseNotTight = true;

    if(argument == "--preemptiveDropEvents")
      preemptiveDropEvents = true;

    if(argument == "--doLooseLeptons")
      doLooseLeptons = true;

    if(argument == "--doPromptTagging")
      doPromptTagging = true;

    if(argument == "--bTagCalibrationFile")
      bTagCalibrationFile = argv[++i];

    if(argument == "--part")
    {
      std::stringstream converter;
      converter << argv[++i];
      converter >> part;
    }
  }

  if(jsonFileName == "")
  {
    std::cout << "You must define a json file" << std::endl;
    return 1;
  }

  if(lheScaleDir == "")
  {
    std::cout << "You must define the lhe scale directory" << std::endl;
    return 1;
  }

  if(bTagCalibrationFile == "")
    bTagCalibrationFile = "../data/CSVv2_Moriond17_B_H.csv";

  if(!fileExists(outputDirectory+"/puWeights.root"))
  {
    std::cout << "You must first obtain the PU weights before trying to process the samples" << std::endl;
    return 1;
  }

  if(!fileExists(lheScaleDir+"/lheWeights.root"))
  {
    std::cout << "You must first obtain the LHE scales before trying to process the samples" << std::endl;
    return 1;
  }

  if(!fileExists(bTagCalibrationFile))
  {
    std::cout << "You must define a valid b-tag calibration file" << std::endl;
    return 1;
  }

  gInterpreter->GenerateDictionary("map<int, double>","map");

  TDirectory* cwd = gDirectory;
  TFile puWeightFile((outputDirectory + "/puWeights.root").c_str(), "READ");
  TFile lheScaleFile((lheScaleDir + "/lheWeights.root").c_str(), "READ");
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
  TFile FullFastElVeto("../data/sf_el_vetoCB.root", "READ");
  electronFullFastSFIDHist = static_cast<TH2D*>(FullFastElVeto.Get("histo2D"));
  TFile FullFastMuLoose("../data/sf_mu_looseID.root", "READ");
  muonFullFastSFIDHist = static_cast<TH2D*>(FullFastMuLoose.Get("histo2D"));
  TFile FullFastElHIIP("../data/Full-FastSimSFs_2D_el_HI+IP.root", "READ");
  TCanvas* elc5 = static_cast<TCanvas*>(FullFastElHIIP.Get("c5"));
  electronFullFastSFHIIPHist = static_cast<TH2D*>(elc5->GetPrimitive("Full-Fast_ratios_2D"));
  TFile FullFastMuHIIP("../data/Full-FastSimSFs_2D_mu_HI+IP.root", "READ");
  TCanvas* muc5 = static_cast<TCanvas*>(FullFastMuHIIP.Get("c5"));
  muonFullFastSFHIIPHist = static_cast<TH2D*>(muc5->GetPrimitive("Full-Fast_ratios_2D"));
  cwd->cd();

  BTagCalibration bCalib("csvv2", bTagCalibrationFile);
  BTagCalibrationReader bReader (BTagEntry::OP_RESHAPING,  // operating point
                                  "central",               // central sys type
                                  {                        // other sys types
                                    "up_jes",
                                    "down_jes",
                                    "up_lf",
                                    "down_lf",
                                    "up_hf",
                                    "down_hf",
                                    "up_hfstats1",
                                    "down_hfstats1",
                                    "up_hfstats2",
                                    "down_hfstats2",
                                    "up_lfstats1",
                                    "down_lfstats1",
                                    "up_lfstats2",
                                    "down_lfstats2",
                                    "up_cferr1",
                                    "down_cferr1",
                                    "up_cferr2",
                                    "down_cferr2",
                                  }
                                );
  bReader.load(bCalib, BTagEntry::FLAV_B, "iterativefit");
  bReader.load(bCalib, BTagEntry::FLAV_C, "iterativefit");
  bReader.load(bCalib, BTagEntry::FLAV_UDSG, "iterativefit");

  Float_t identity[100];
  for(int i = 0; i < 100; ++i)
    identity[i] = i;

  std::map<int, double>* lheScaleMap = nullptr;
  //TClass *mapClass =
  gROOT->FindSTLClass("std::map<int,double>", true);

  std::cout << "Reading JSON file" << std::endl;
  SampleReader samples(jsonFileName);

  for(auto &process : samples)
  {
    std::cout << "Processing process: " << process.tag() << std::endl;

    TH1D* puWeightDistrib     = static_cast<TH1D*>(puWeightFile.Get( ("process_"+process.tag()+"_puWeight").c_str())->Clone("puWeightDistrib"));
    TH1D* puWeightDistribUp   = nullptr;
    TH1D* puWeightDistribDown = nullptr;
    {
      auto tmp = puWeightFile.Get( ("process_"+process.tag()+"_puWeight_Up").c_str());
      if(tmp != nullptr)
      {
        puWeightDistribUp   = static_cast<TH1D*>(tmp->Clone("puWeightDistribUp"));
        tmp = puWeightFile.Get( ("process_"+process.tag()+"_puWeight_Down").c_str());
        puWeightDistribDown = static_cast<TH1D*>(tmp->Clone("puWeightDistribDown"));
      }
    }

    if(!process.isdata())
    {
      lheScaleFile.GetObject(("process_"+process.tag()+"_lhemap").c_str(), lheScaleMap);
      std::cout << "Loaded LHEScaleMap with " << lheScaleMap->size() << " entries:" << std::endl;
      for(auto& kv: *lheScaleMap)
        std::cout << "  " << kv.first << ": " << kv.second << std::endl;
    }

    for(auto &sample : process)
    {
      std::cout << "\tProcessing sample: " << sample.tag() << std::endl;
      std::string outputFile = outputDirectory + "/" + sample.tag();
      if(suffix != "")
        outputFile += "_" + suffix;
      if(part >= 0)
      {
        outputFile += "_Part";
        std::stringstream converter;
        converter << part;
        outputFile += converter.str();
      }
      outputFile += ".root";
      std::cout << "\t  Putting output in: " << outputFile << std::endl;

      std::ofstream SyFile;
      if(doSync)
      {
        SyFile.open(outputDirectory + "/" + sample.tag() + "_synch.txt", std::ofstream::out | std::ofstream::trunc);
        SyFile.setf(std::ios::fixed);
        SyFile.precision(3);
      }

      size_t sync_count = 0;

      std::cout << "\t  Opening output file and creating tree" << std::endl;
      TFile foutput(outputFile.c_str(), "RECREATE");
      TTree *bdttree= new TTree("bdttree","bdttree");
      std::cout << "\t    The tree pointer: " << bdttree << std::endl;

      // New branch in bdt tree
      std::cout << "\t  Creating branches in output tree" << std::endl;
      std::cout << "\t    Creating event metadata branches" << std::endl;
      UInt_t Run;  bdttree->Branch("Run",&Run);
      ULong64_t Event;  bdttree->Branch("Event",&Event);
      UInt_t LumiSec;  bdttree->Branch("LumiSec",&LumiSec);
      Float_t Nevt;  bdttree->Branch("Nevt",&Nevt,"Nevt/F");
      Float_t XS; bdttree->Branch("XS",&XS,"XS/F");
      Float_t nVert; bdttree->Branch("nVert", &nVert, "nVert/F");

      std::cout << "\t    Creating signal gen info" << std::endl;
      Float_t genGravitinoM; bdttree->Branch("genGravitinoM", &genGravitinoM, "genGravitinoM/F");
      Float_t genStopM; bdttree->Branch("genStopM", &genStopM, "genStopM/F");
      Float_t genSbottomM; bdttree->Branch("genSbottomM", &genSbottomM, "genSbottomM/F");
      Float_t genNeutralinoM; bdttree->Branch("genNeutralinoM", &genNeutralinoM, "genNeutralinoM/F");

      std::cout << "\t    Creating branches for systematic variations:" << std::endl;
      std::cout << "\t      Creating systematic variation variables" << std::endl;
      ValueWithSystematics<float> triggerEfficiency;
      ValueWithSystematics<float> EWKISRweight;
      ValueWithSystematics<float> ISRweight;
      ValueWithSystematics<float> puWeight;
      ValueWithSystematics<float> leptonIDSF;
      ValueWithSystematics<float> leptonISOSF;
      ValueWithSystematics<float> leptonFullFastSF;
      ValueWithSystematics<float> looseNotTightWeight;
      ValueWithSystematics<float> Q2Var;
      ValueWithSystematics<float> bTagSF;
      ValueWithSystematics<float> weight;

      std::cout << "\t      Creating the variations if needed" << std::endl;
      if(!process.isdata())
      {
        // Performing dummy computations just so that the placeholders for all the uncertainties are created
        std::cout << "\t        trigger" << std::endl;
        triggerEfficiency = triggerEfficiencyFromMETSys(200.0);
        triggerEfficiency.Systematic("JES_Up");
        triggerEfficiency.Systematic("JES_Down");
        triggerEfficiency.Systematic("JER_Up");
        triggerEfficiency.Systematic("JER_Down");
        std::cout << "\t        EWK" << std::endl;
        EWKISRweight = EWKISRweightFromISRpTSys(20, 1.0, 40, 0.7);
        EWKISRweight.Systematic("JES_Up");
        EWKISRweight.Systematic("JES_Down");
        EWKISRweight.Systematic("JER_Up");
        EWKISRweight.Systematic("JER_Down");
        std::cout << "\t        ISR" << std::endl;
        ISRweight = ISRweightFromNISRJetSys(2);
        std::cout << "\t        pu" << std::endl;
        puWeight = 1.0f;
        puWeight.Systematic("PU_Up");
        puWeight.Systematic("PU_Down");
        std::cout << "\t        lID" << std::endl;
        leptonIDSF = getLeptonIDSFSys(11, 20, 1.1);
        std::cout << "\t        lISO" << std::endl;
        leptonISOSF = getLeptonISOSFSys(11, 20, 1.1);
        std::cout << "\t        FullFastSim" << std::endl;
        leptonFullFastSF = getFullFastSFSys(11, 20, 1.1);
        std::cout << "\t        Q^2" << std::endl;
        Q2Var = 1;
        Q2Var.Systematic("Q2_1");
        Q2Var.Systematic("Q2_2");
        Q2Var.Systematic("Q2_3");
        Q2Var.Systematic("Q2_4");
        Q2Var.Systematic("Q2_5");
        Q2Var.Systematic("Q2_6");
        Q2Var.Systematic("Q2_7");
        Q2Var.Systematic("Q2_8");
        std::cout << "\t        B-Tag SF" << std::endl;
        bTagSF = 1;
        bTagSF.Systematic("JES_Up");
        bTagSF.Systematic("JES_Down");
        bTagSF.Systematic("LF_Up");
        bTagSF.Systematic("LF_Down");
        bTagSF.Systematic("HF_Up");
        bTagSF.Systematic("HF_Down");
        bTagSF.Systematic("LFStats1_Up");
        bTagSF.Systematic("LFStats1_Down");
        bTagSF.Systematic("HFStats1_Up");
        bTagSF.Systematic("HFStats1_Down");
        bTagSF.Systematic("LFStats2_Up");
        bTagSF.Systematic("LFStats2_Down");
        bTagSF.Systematic("HFStats2_Up");
        bTagSF.Systematic("HFStats2_Down");
        bTagSF.Systematic("CFErr1_Up");
        bTagSF.Systematic("CFErr1_Down");
        bTagSF.Systematic("CFErr2_Up");
        bTagSF.Systematic("CFErr2_Down");
      }

      std::cout << "\t        looseNotTight" << std::endl;
      looseNotTightWeight = getLeptonTightLooseRatioSys(11, 20, 1.1);

      std::cout << "\t        weight" << std::endl;
      weight = puWeight * triggerEfficiency * EWKISRweight * ISRweight * leptonIDSF * leptonISOSF * leptonFullFastSF * looseNotTightWeight * Q2Var * bTagSF;

      std::cout << "\t        locking" << std::endl;
      if(!process.isdata())
      {
        // Then lock the variables so that the placeholders are not removed or new ones are created
        triggerEfficiency.Lock();
        EWKISRweight.Lock();
        ISRweight.Lock();
        puWeight.Lock();
        leptonIDSF.Lock();
        leptonISOSF.Lock();
        leptonFullFastSF.Lock();
        bTagSF.Lock();
        Q2Var.Lock();
      }
      looseNotTightWeight.Lock();
      weight.Lock();


      triggerEfficiency = 1.0;
      EWKISRweight = 1.0;
      ISRweight = 1.0;
      puWeight = 1.0;
      leptonIDSF = 1.0;
      leptonISOSF = 1.0;
      leptonFullFastSF = 1.0;
      looseNotTightWeight = 1.0;
      bTagSF = 1.0;
      Q2Var = 1.0;
      weight = 1.0;

      Float_t genWeight=1;
      Float_t sumGenWeight=1;
      Float_t filterEfficiency=1;
      Float_t splitFactor=1;
      bool isLooseNotTight=false;

      std::cout << "\t      Creating the base branches" << std::endl;
      bdttree->Branch("genWeight", &genWeight, "genWeight/F");
      bdttree->Branch("sumGenWeight", &sumGenWeight, "sumGenWeight/F");
      bdttree->Branch("filterEfficiency", &filterEfficiency, "filterEfficiency/F");
      bdttree->Branch("splitFactor", &splitFactor, "splitFactor/F");
      bdttree->Branch("triggerEfficiency", &(triggerEfficiency.Value()), "triggerEfficiency/F");
      bdttree->Branch("EWKISRweight", &(EWKISRweight.Value()), "EWKISRweight/F");
      bdttree->Branch("ISRweight", &(ISRweight.Value()), "ISRweight/F");
      bdttree->Branch("puWeight", &(puWeight.Value()), "puWeight/F");
      bdttree->Branch("leptonIDSF", &(leptonIDSF.Value()), "leptonIDSF/F");
      bdttree->Branch("leptonISOSF", &(leptonISOSF.Value()), "leptonISOSF/F");
      bdttree->Branch("leptonFullFastSF", &(leptonFullFastSF.Value()), "leptonFullFastSF/F");
      bdttree->Branch("bTagSF", &(bTagSF.Value()), "bTagSF/F");
      bdttree->Branch("weight", &(weight.Value()), "weight/F");
      bdttree->Branch("isLooseNotTight", &isLooseNotTight);
      bdttree->Branch("looseNotTightWeight", &looseNotTightWeight.Value(), "looseNotTightWeight/F");

      if(!process.isdata() && noTrim)
      {
        for(auto& systematic: triggerEfficiency.Systematics())
          bdttree->Branch(("triggerEfficiency_"+systematic).c_str(), &(triggerEfficiency.Systematic(systematic)));
        for(auto& systematic: EWKISRweight.Systematics())
          bdttree->Branch(("EWKISRweight_"+systematic).c_str(), &(EWKISRweight.Systematic(systematic)));
        for(auto& systematic: ISRweight.Systematics())
          bdttree->Branch(("ISRweight_"+systematic).c_str(), &(ISRweight.Systematic(systematic)));
        for(auto& systematic: puWeight.Systematics())
          bdttree->Branch(("puWeight_"+systematic).c_str(), &(puWeight.Systematic(systematic)));
        for(auto& systematic: leptonIDSF.Systematics())
          bdttree->Branch(("leptonIDSF_"+systematic).c_str(), &(leptonIDSF.Systematic(systematic)));
        for(auto& systematic: leptonISOSF.Systematics())
          bdttree->Branch(("leptonISOSF_"+systematic).c_str(), &(leptonISOSF.Systematic(systematic)));
        for(auto& systematic: leptonFullFastSF.Systematics())
          bdttree->Branch(("leptonFullFastSF_"+systematic).c_str(), &(leptonFullFastSF.Systematic(systematic)));
        for(auto& systematic: bTagSF.Systematics())
          bdttree->Branch(("bTagSF_"+systematic).c_str(), &(bTagSF.Systematic(systematic)));
      }
      if(noTrim)
        for(auto& systematic: looseNotTightWeight.Systematics())
          bdttree->Branch(("looseNotTightWeight_"+systematic).c_str(), &(looseNotTightWeight.Systematic(systematic)));
      for(auto& systematic: weight.Systematics())
        bdttree->Branch(("weight_"+systematic).c_str(), &(weight.Systematic(systematic)));
      std::cout << "\t      Finished." << std::endl;

      bool isTight;      bdttree->Branch("isTight",   &isTight);
      bool isLoose;      bdttree->Branch("isLoose",   &isLoose);
      Float_t LepID;     bdttree->Branch("LepID",     &LepID,     "LepID/F");
      Float_t LepChg;    bdttree->Branch("LepChg",    &LepChg,    "LepChg/F");
      Float_t LepPt;     bdttree->Branch("LepPt",     &LepPt,     "LepPt/F");
      Float_t LepEta;    bdttree->Branch("LepEta",    &LepEta,    "LepEta/F");
      Float_t LepDxy;    bdttree->Branch("LepDxy",    &LepDxy,    "LepDxy/F");
      Float_t LepDz;     bdttree->Branch("LepDz",     &LepDz,     "LepDz/F");
      Float_t LepIso03;  bdttree->Branch("LepIso03",  &LepIso03,  "LepIso03/F");
      Float_t LepHybIso03; bdttree->Branch("LepHybIso03",  &LepHybIso03,  "LepHybIso03/F");
      Float_t isPrompt;  bdttree->Branch("isPrompt",  &isPrompt,  "isPrompt/F");
      Float_t Lep2ID;    bdttree->Branch("Lep2ID",    &Lep2ID,    "Lep2ID/F");
      Float_t Lep2Chg;   bdttree->Branch("Lep2Chg",   &Lep2Chg,   "Lep2Chg/F");
      Float_t Lep2Pt;    bdttree->Branch("Lep2Pt",    &Lep2Pt,    "Lep2Pt/F");
      Float_t Lep2Eta;   bdttree->Branch("Lep2Eta",   &Lep2Eta,   "Lep2Eta/F");
      Float_t Lep2Dxy;   bdttree->Branch("Lep2Dxy",   &Lep2Dxy,   "Lep2Dxy/F");
      Float_t Lep2Dz;    bdttree->Branch("Lep2Dz",    &Lep2Dz,    "Lep2Dz/F");
      Float_t Lep2Iso03; bdttree->Branch("Lep2Iso03", &Lep2Iso03, "Lep2Iso03/F");
      Float_t nGoodMu;   bdttree->Branch("nGoodMu",&nGoodMu,"nGoodMu/F");
      Float_t nGoodEl;   bdttree->Branch("nGoodEl",&nGoodEl,"nGoodEl/F");
      Float_t nGoodMu_loose;
      Float_t nGoodEl_loose;


      ValueWithSystematics<float> Met;
      ValueWithSystematics<float> mt;
      ValueWithSystematics<float> Q80;
      ValueWithSystematics<float> CosDeltaPhi;

      ValueWithSystematics<float> nIsr_out;
      ValueWithSystematics<float> Njet;
      ValueWithSystematics<float> Njet30;
      ValueWithSystematics<float> Njet40;
      ValueWithSystematics<float> Njet50;
      ValueWithSystematics<float> Njet60;
      ValueWithSystematics<float> Njet70;
      ValueWithSystematics<float> Njet80;
      ValueWithSystematics<float> Njet90;
      ValueWithSystematics<float> Njet100;
      ValueWithSystematics<float> Jet1Pt;
      ValueWithSystematics<float> Jet1Eta;
      ValueWithSystematics<float> Jet1CSV;
      ValueWithSystematics<float> Jet2Pt;
      ValueWithSystematics<float> Jet2Eta;
      ValueWithSystematics<float> Jet2CSV;
      ValueWithSystematics<float> Jet3Pt;
      ValueWithSystematics<float> Jet3Eta;
      ValueWithSystematics<float> Jet3CSV;
      ValueWithSystematics<float> DPhiJet1Jet2;
      ValueWithSystematics<float> HT;

      ValueWithSystematics<float> NbLoose;
      ValueWithSystematics<float> NbTight;
      ValueWithSystematics<float> NbLooseTo50;
      ValueWithSystematics<float> NbLoose50;
      ValueWithSystematics<float> NbMediumTo50;
      ValueWithSystematics<float> NbMedium50;
      ValueWithSystematics<float> NbTightTo50;
      ValueWithSystematics<float> NbTight50;
      ValueWithSystematics<float> JetHBpt;
      ValueWithSystematics<float> JetHBeta;
      ValueWithSystematics<float> JetHBindex;
      ValueWithSystematics<float> JetHBCSV;
      ValueWithSystematics<float> JetB2pt;
      ValueWithSystematics<float> JetB2eta;
      ValueWithSystematics<float> JetB2index;
      ValueWithSystematics<float> JetB2CSV;

      ValueWithSystematics<float> DrJet1Lep;
      ValueWithSystematics<float> DrJet2Lep;
      ValueWithSystematics<float> DrJetHBLep;
      ValueWithSystematics<float> DrJet1Jet2;
      ValueWithSystematics<float> JetLepMass;
      ValueWithSystematics<float> J3Mass;

      if(!process.isdata())
      {
        Met.Systematic("JES_Up");
        Met.Systematic("JES_Down");
        Met.Systematic("JER_Up");
        Met.Systematic("JER_Down");
        mt.Systematic("JES_Up");
        mt.Systematic("JES_Down");
        mt.Systematic("JER_Up");
        mt.Systematic("JER_Down");
        Q80.Systematic("JES_Up");
        Q80.Systematic("JES_Down");
        Q80.Systematic("JER_Up");
        Q80.Systematic("JER_Down");
        CosDeltaPhi.Systematic("JES_Up");
        CosDeltaPhi.Systematic("JES_Down");
        CosDeltaPhi.Systematic("JER_Up");
        CosDeltaPhi.Systematic("JER_Down");
        Njet.Systematic("JES_Up");
        Njet.Systematic("JES_Down");
        Njet.Systematic("JER_Up");
        Njet.Systematic("JER_Down");
        Njet30.Systematic("JES_Up");
        Njet30.Systematic("JES_Down");
        Njet30.Systematic("JER_Up");
        Njet30.Systematic("JER_Down");
        Njet40.Systematic("JES_Up");
        Njet40.Systematic("JES_Down");
        Njet40.Systematic("JER_Up");
        Njet40.Systematic("JER_Down");
        Njet50.Systematic("JES_Up");
        Njet50.Systematic("JES_Down");
        Njet50.Systematic("JER_Up");
        Njet50.Systematic("JER_Down");
        Njet60.Systematic("JES_Up");
        Njet60.Systematic("JES_Down");
        Njet60.Systematic("JER_Up");
        Njet60.Systematic("JER_Down");
        Njet70.Systematic("JES_Up");
        Njet70.Systematic("JES_Down");
        Njet70.Systematic("JER_Up");
        Njet70.Systematic("JER_Down");
        Njet80.Systematic("JES_Up");
        Njet80.Systematic("JES_Down");
        Njet80.Systematic("JER_Up");
        Njet80.Systematic("JER_Down");
        Njet90.Systematic("JES_Up");
        Njet90.Systematic("JES_Down");
        Njet90.Systematic("JER_Up");
        Njet90.Systematic("JER_Down");
        Njet100.Systematic("JES_Up");
        Njet100.Systematic("JES_Down");
        Njet100.Systematic("JER_Up");
        Njet100.Systematic("JER_Down");
        Jet1Pt.Systematic("JES_Up");
        Jet1Pt.Systematic("JES_Down");
        Jet1Pt.Systematic("JER_Up");
        Jet1Pt.Systematic("JER_Down");
        Jet1Eta.Systematic("JES_Up");
        Jet1Eta.Systematic("JES_Down");
        Jet1Eta.Systematic("JER_Up");
        Jet1Eta.Systematic("JER_Down");
        Jet1CSV.Systematic("JES_Up");
        Jet1CSV.Systematic("JES_Down");
        Jet1CSV.Systematic("JER_Up");
        Jet1CSV.Systematic("JER_Down");
        Jet2Pt.Systematic("JES_Up");
        Jet2Pt.Systematic("JES_Down");
        Jet2Pt.Systematic("JER_Up");
        Jet2Pt.Systematic("JER_Down");
        Jet2Eta.Systematic("JES_Up");
        Jet2Eta.Systematic("JES_Down");
        Jet2Eta.Systematic("JER_Up");
        Jet2Eta.Systematic("JER_Down");
        Jet2CSV.Systematic("JES_Up");
        Jet2CSV.Systematic("JES_Down");
        Jet2CSV.Systematic("JER_Up");
        Jet2CSV.Systematic("JER_Down");
        Jet3Pt.Systematic("JES_Up");
        Jet3Pt.Systematic("JES_Down");
        Jet3Pt.Systematic("JER_Up");
        Jet3Pt.Systematic("JER_Down");
        Jet3Eta.Systematic("JES_Up");
        Jet3Eta.Systematic("JES_Down");
        Jet3Eta.Systematic("JER_Up");
        Jet3Eta.Systematic("JER_Down");
        Jet3CSV.Systematic("JES_Up");
        Jet3CSV.Systematic("JES_Down");
        Jet3CSV.Systematic("JER_Up");
        Jet3CSV.Systematic("JER_Down");
        DPhiJet1Jet2.Systematic("JES_Up");
        DPhiJet1Jet2.Systematic("JES_Down");
        DPhiJet1Jet2.Systematic("JER_Up");
        DPhiJet1Jet2.Systematic("JER_Down");
        HT.Systematic("JES_Up");
        HT.Systematic("JES_Down");
        HT.Systematic("JER_Up");
        HT.Systematic("JER_Down");
        NbLoose.Systematic("JES_Up");
        NbLoose.Systematic("JES_Down");
        NbLoose.Systematic("JER_Up");
        NbLoose.Systematic("JER_Down");
        NbTight.Systematic("JES_Up");
        NbTight.Systematic("JES_Down");
        NbTight.Systematic("JER_Up");
        NbTight.Systematic("JER_Down");
        NbLooseTo50.Systematic("JES_Up");
        NbLooseTo50.Systematic("JES_Down");
        NbLooseTo50.Systematic("JER_Up");
        NbLooseTo50.Systematic("JER_Down");
        NbLoose50.Systematic("JES_Up");
        NbLoose50.Systematic("JES_Down");
        NbLoose50.Systematic("JER_Up");
        NbLoose50.Systematic("JER_Down");
        NbMediumTo50.Systematic("JES_Up");
        NbMediumTo50.Systematic("JES_Down");
        NbMediumTo50.Systematic("JER_Up");
        NbMediumTo50.Systematic("JER_Down");
        NbMedium50.Systematic("JES_Up");
        NbMedium50.Systematic("JES_Down");
        NbMedium50.Systematic("JER_Up");
        NbMedium50.Systematic("JER_Down");
        NbTightTo50.Systematic("JES_Up");
        NbTightTo50.Systematic("JES_Down");
        NbTightTo50.Systematic("JER_Up");
        NbTightTo50.Systematic("JER_Down");
        NbTight50.Systematic("JES_Up");
        NbTight50.Systematic("JES_Down");
        NbTight50.Systematic("JER_Up");
        NbTight50.Systematic("JER_Down");
        JetHBpt.Systematic("JES_Up");
        JetHBpt.Systematic("JES_Down");
        JetHBpt.Systematic("JER_Up");
        JetHBpt.Systematic("JER_Down");
        JetHBeta.Systematic("JES_Up");
        JetHBeta.Systematic("JES_Down");
        JetHBeta.Systematic("JER_Up");
        JetHBeta.Systematic("JER_Down");
        JetHBindex.Systematic("JES_Up");
        JetHBindex.Systematic("JES_Down");
        JetHBindex.Systematic("JER_Up");
        JetHBindex.Systematic("JER_Down");
        JetHBCSV.Systematic("JES_Up");
        JetHBCSV.Systematic("JES_Down");
        JetHBCSV.Systematic("JER_Up");
        JetHBCSV.Systematic("JER_Down");
        JetB2pt.Systematic("JES_Up");
        JetB2pt.Systematic("JES_Down");
        JetB2pt.Systematic("JER_Up");
        JetB2pt.Systematic("JER_Down");
        JetB2eta.Systematic("JES_Up");
        JetB2eta.Systematic("JES_Down");
        JetB2eta.Systematic("JER_Up");
        JetB2eta.Systematic("JER_Down");
        JetB2index.Systematic("JES_Up");
        JetB2index.Systematic("JES_Down");
        JetB2index.Systematic("JER_Up");
        JetB2index.Systematic("JER_Down");
        JetB2CSV.Systematic("JES_Up");
        JetB2CSV.Systematic("JES_Down");
        JetB2CSV.Systematic("JER_Up");
        JetB2CSV.Systematic("JER_Down");
        DrJet1Lep.Systematic("JES_Up");
        DrJet1Lep.Systematic("JES_Down");
        DrJet1Lep.Systematic("JER_Up");
        DrJet1Lep.Systematic("JER_Down");
        DrJet2Lep.Systematic("JES_Up");
        DrJet2Lep.Systematic("JES_Down");
        DrJet2Lep.Systematic("JER_Up");
        DrJet2Lep.Systematic("JER_Down");
        DrJetHBLep.Systematic("JES_Up");
        DrJetHBLep.Systematic("JES_Down");
        DrJetHBLep.Systematic("JER_Up");
        DrJetHBLep.Systematic("JER_Down");
        DrJet1Jet2.Systematic("JES_Up");
        DrJet1Jet2.Systematic("JES_Down");
        DrJet1Jet2.Systematic("JER_Up");
        DrJet1Jet2.Systematic("JER_Down");
        JetLepMass.Systematic("JES_Up");
        JetLepMass.Systematic("JES_Down");
        JetLepMass.Systematic("JER_Up");
        JetLepMass.Systematic("JER_Down");
        J3Mass.Systematic("JES_Up");
        J3Mass.Systematic("JES_Down");
        J3Mass.Systematic("JER_Up");
        J3Mass.Systematic("JER_Down");

        Met.Lock();
        mt.Lock();
        Q80.Lock();
        CosDeltaPhi.Lock();
        nIsr_out.Lock();
        Njet.Lock();
        Njet30.Lock();
        Njet40.Lock();
        Njet50.Lock();
        Njet60.Lock();
        Njet70.Lock();
        Njet80.Lock();
        Njet90.Lock();
        Njet100.Lock();
        Jet1Pt.Lock();
        Jet1Eta.Lock();
        Jet1CSV.Lock();
        Jet2Pt.Lock();
        Jet2Eta.Lock();
        Jet2CSV.Lock();
        Jet3Pt.Lock();
        Jet3Eta.Lock();
        Jet3CSV.Lock();
        DPhiJet1Jet2.Lock();
        HT.Lock();
        NbLoose.Lock();
        NbTight.Lock();
        NbLooseTo50.Lock();
        NbLoose50.Lock();
        NbMediumTo50.Lock();
        NbMedium50.Lock();
        NbTightTo50.Lock();
        NbTight50.Lock();
        JetHBpt.Lock();
        JetHBeta.Lock();
        JetHBindex.Lock();
        JetHBCSV.Lock();
        JetB2pt.Lock();
        JetB2eta.Lock();
        JetB2index.Lock();
        JetB2CSV.Lock();
        DrJet1Lep.Lock();
        DrJet2Lep.Lock();
        DrJetHBLep.Lock();
        DrJet1Jet2.Lock();
        JetLepMass.Lock();
        J3Mass.Lock();
      }


      bdttree->Branch("Met",&Met.Value(),"Met/F");
      bdttree->Branch("mt",&mt.Value(),"mt/F");
      bdttree-> Branch("Q80",&Q80.Value(),"Q80/F");
      bdttree->Branch("CosDeltaPhi",&CosDeltaPhi.Value(),"CosDeltaPhi/F");

      bdttree->Branch("nIsr", &nIsr_out.Value(), "nIsr/F");
      bdttree->Branch("Njet",&Njet.Value(),"Njet/F");
      bdttree->Branch("Njet30",&Njet30.Value(),"Njet30/F");
      bdttree->Branch("Njet40",&Njet40.Value(),"Njet40/F");
      bdttree->Branch("Njet50",&Njet50.Value(),"Njet50/F");
      bdttree->Branch("Njet60",&Njet60.Value(),"Njet60/F");
      bdttree->Branch("Njet70",&Njet70.Value(),"Njet70/F");
      bdttree->Branch("Njet80",&Njet80.Value(),"Njet80/F");
      bdttree->Branch("Njet90",&Njet90.Value(),"Njet90/F");
      bdttree->Branch("Njet100",&Njet100.Value(),"Njet100/F");
      bdttree->Branch("Jet1Pt",&Jet1Pt.Value(),"Jet1Pt/F");
      bdttree->Branch("Jet1Eta",&Jet1Eta.Value(),"Jet1Eta/F");
      bdttree->Branch("Jet1CSV",&Jet1CSV.Value(),"Jet1CSV/F");
      bdttree->Branch("Jet2Pt",&Jet2Pt.Value(),"Jet2Pt/F");
      bdttree->Branch("Jet2Eta",&Jet2Eta.Value(),"Jet2Eta/F");
      bdttree->Branch("Jet2CSV",&Jet2CSV.Value(),"Jet2CSV/F");
      bdttree->Branch("Jet3Pt",&Jet3Pt.Value(),"Jet3Pt/F");
      bdttree->Branch("Jet3Eta",&Jet3Eta.Value(),"Jet3Eta/F");
      bdttree->Branch("Jet3CSV",&Jet3CSV.Value(),"Jet3CSV/F");
      bdttree->Branch("DPhiJet1Jet2",&DPhiJet1Jet2.Value(),"DPhiJet1Jet2/F");
      bdttree->Branch("HT",&HT.Value(),"HT/F");

      bdttree->Branch("NbLoose",&NbLoose.Value(),"NbLoose/F");
      bdttree->Branch("NbTight",&NbTight.Value(),"NbTight/F");
      bdttree->Branch("NbLooseTo50",&NbLooseTo50.Value(),"NbLooseTo50/F");
      bdttree->Branch("NbLoose50",&NbLoose50.Value(),"NbLoose50/F");
      bdttree->Branch("NbMediumTo50",&NbMediumTo50.Value(),"NbMediumTo50/F");
      bdttree->Branch("NbMedium50",&NbMedium50.Value(),"NbMedium50/F");
      bdttree->Branch("NbTightTo50",&NbTightTo50.Value(),"NbTightTo50/F");
      bdttree->Branch("NbTight50",&NbTight50.Value(),"NbTight50/F");
      bdttree->Branch("JetHBpt",&JetHBpt.Value(),"JetHBpt/F");
      bdttree->Branch("JetHBeta",&JetHBeta.Value(),"JetHBeta/F");
      bdttree->Branch("JetHBindex", &JetHBindex.Value(), "JetHBindex/F");
      bdttree->Branch("JetHBCSV", &JetHBCSV.Value(), "JetHBCSV/F");
      bdttree->Branch("JetB2pt",&JetB2pt.Value(),"JetB2pt/F");
      bdttree->Branch("JetB2eta",&JetB2eta.Value(),"JetB2eta/F");
      bdttree->Branch("JetB2index", &JetB2index.Value(), "JetB2index/F");
      bdttree->Branch("JetB2CSV", &JetB2CSV.Value(), "JetB2CSV/F");

      bdttree->Branch("DrJet1Lep",&DrJet1Lep.Value(),"DrJet1Lep/F");
      bdttree->Branch("DrJet2Lep",&DrJet2Lep.Value(),"DrJet2Lep/F");
      bdttree->Branch("DrJetHBLep",&DrJetHBLep.Value(),"DrJetHBLep/F");
      bdttree->Branch("DrJet1Jet2",&DrJet1Jet2.Value(),"DrJet1Jet2/F");
      bdttree->Branch("JetLepMass",&JetLepMass.Value(),"JetLepMass/F");
      bdttree->Branch("J3Mass",&J3Mass.Value(),"J3Mass/F");

      if(!process.isdata())
      {
        for(auto& systematic: Met.Systematics())
          bdttree->Branch(("Met_"+systematic).c_str(), &(Met.Systematic(systematic)));
        for(auto& systematic: mt.Systematics())
          bdttree->Branch(("mt_"+systematic).c_str(), &(mt.Systematic(systematic)));
        for(auto& systematic: Q80.Systematics())
          bdttree->Branch(("Q80_"+systematic).c_str(), &(Q80.Systematic(systematic)));
        for(auto& systematic: CosDeltaPhi.Systematics())
          bdttree->Branch(("CosDeltaPhi_"+systematic).c_str(), &(CosDeltaPhi.Systematic(systematic)));
        for(auto& systematic: nIsr_out.Systematics())
          bdttree->Branch(("nIsr_"+systematic).c_str(), &(nIsr_out.Systematic(systematic)));
        for(auto& systematic: Njet.Systematics())
          bdttree->Branch(("Njet_"+systematic).c_str(), &(Njet.Systematic(systematic)));
        for(auto& systematic: Njet30.Systematics())
          bdttree->Branch(("Njet30_"+systematic).c_str(), &(Njet30.Systematic(systematic)));
        for(auto& systematic: Njet40.Systematics())
          bdttree->Branch(("Njet40_"+systematic).c_str(), &(Njet40.Systematic(systematic)));
        for(auto& systematic: Njet50.Systematics())
          bdttree->Branch(("Njet50_"+systematic).c_str(), &(Njet50.Systematic(systematic)));
        for(auto& systematic: Njet60.Systematics())
          bdttree->Branch(("Njet60_"+systematic).c_str(), &(Njet60.Systematic(systematic)));
        for(auto& systematic: Njet70.Systematics())
          bdttree->Branch(("Njet70_"+systematic).c_str(), &(Njet70.Systematic(systematic)));
        for(auto& systematic: Njet80.Systematics())
          bdttree->Branch(("Njet80_"+systematic).c_str(), &(Njet80.Systematic(systematic)));
        for(auto& systematic: Njet90.Systematics())
          bdttree->Branch(("Njet90_"+systematic).c_str(), &(Njet90.Systematic(systematic)));
        for(auto& systematic: Njet100.Systematics())
          bdttree->Branch(("Njet100_"+systematic).c_str(), &(Njet100.Systematic(systematic)));
        for(auto& systematic: Jet1Pt.Systematics())
          bdttree->Branch(("Jet1Pt_"+systematic).c_str(), &(Jet1Pt.Systematic(systematic)));
        for(auto& systematic: Jet1Eta.Systematics())
          bdttree->Branch(("Jet1Eta_"+systematic).c_str(), &(Jet1Eta.Systematic(systematic)));
        for(auto& systematic: Jet1CSV.Systematics())
          bdttree->Branch(("Jet1CSV_"+systematic).c_str(), &(Jet1CSV.Systematic(systematic)));
        for(auto& systematic: Jet2Pt.Systematics())
          bdttree->Branch(("Jet2Pt_"+systematic).c_str(), &(Jet2Pt.Systematic(systematic)));
        for(auto& systematic: Jet2Eta.Systematics())
          bdttree->Branch(("Jet2Eta_"+systematic).c_str(), &(Jet2Eta.Systematic(systematic)));
        for(auto& systematic: Jet2CSV.Systematics())
          bdttree->Branch(("Jet2CSV_"+systematic).c_str(), &(Jet2CSV.Systematic(systematic)));
        for(auto& systematic: Jet3Pt.Systematics())
          bdttree->Branch(("Jet3Pt_"+systematic).c_str(), &(Jet3Pt.Systematic(systematic)));
        for(auto& systematic: Jet3Eta.Systematics())
          bdttree->Branch(("Jet3Eta_"+systematic).c_str(), &(Jet3Eta.Systematic(systematic)));
        for(auto& systematic: Jet3CSV.Systematics())
          bdttree->Branch(("Jet3CSV_"+systematic).c_str(), &(Jet3CSV.Systematic(systematic)));
        for(auto& systematic: DPhiJet1Jet2.Systematics())
          bdttree->Branch(("DPhiJet1Jet2_"+systematic).c_str(), &(DPhiJet1Jet2.Systematic(systematic)));
        for(auto& systematic: HT.Systematics())
          bdttree->Branch(("HT_"+systematic).c_str(), &(HT.Systematic(systematic)));
        for(auto& systematic: NbLoose.Systematics())
          bdttree->Branch(("NbLoose_"+systematic).c_str(), &(NbLoose.Systematic(systematic)));
        for(auto& systematic: NbTight.Systematics())
          bdttree->Branch(("NbTight_"+systematic).c_str(), &(NbTight.Systematic(systematic)));
        for(auto& systematic: NbLooseTo50.Systematics())
          bdttree->Branch(("NbLooseTo50_"+systematic).c_str(), &(NbLooseTo50.Systematic(systematic)));
        for(auto& systematic: NbLoose50.Systematics())
          bdttree->Branch(("NbLoose50_"+systematic).c_str(), &(NbLoose50.Systematic(systematic)));
        for(auto& systematic: NbMediumTo50.Systematics())
          bdttree->Branch(("NbMediumTo50_"+systematic).c_str(), &(NbMediumTo50.Systematic(systematic)));
        for(auto& systematic: NbMedium50.Systematics())
          bdttree->Branch(("NbMedium50_"+systematic).c_str(), &(NbMedium50.Systematic(systematic)));
        for(auto& systematic: NbTightTo50.Systematics())
          bdttree->Branch(("NbTightTo50_"+systematic).c_str(), &(NbTightTo50.Systematic(systematic)));
        for(auto& systematic: NbTight50.Systematics())
          bdttree->Branch(("NbTight50_"+systematic).c_str(), &(NbTight50.Systematic(systematic)));
        for(auto& systematic: JetHBpt.Systematics())
          bdttree->Branch(("JetHBpt_"+systematic).c_str(), &(JetHBpt.Systematic(systematic)));
        for(auto& systematic: JetHBeta.Systematics())
          bdttree->Branch(("JetHBeta_"+systematic).c_str(), &(JetHBeta.Systematic(systematic)));
        for(auto& systematic: JetHBindex.Systematics())
          bdttree->Branch(("JetHBindex_"+systematic).c_str(), &(JetHBindex.Systematic(systematic)));
        for(auto& systematic: JetHBCSV.Systematics())
          bdttree->Branch(("JetHBCSV_"+systematic).c_str(), &(JetHBCSV.Systematic(systematic)));
        for(auto& systematic: JetB2pt.Systematics())
          bdttree->Branch(("JetB2pt_"+systematic).c_str(), &(JetB2pt.Systematic(systematic)));
        for(auto& systematic: JetB2eta.Systematics())
          bdttree->Branch(("JetB2eta_"+systematic).c_str(), &(JetB2eta.Systematic(systematic)));
        for(auto& systematic: JetB2index.Systematics())
          bdttree->Branch(("JetB2index_"+systematic).c_str(), &(JetB2index.Systematic(systematic)));
        for(auto& systematic: JetB2CSV.Systematics())
          bdttree->Branch(("JetB2CSV_"+systematic).c_str(), &(JetB2CSV.Systematic(systematic)));
        for(auto& systematic: DrJet1Lep.Systematics())
          bdttree->Branch(("DrJet1Lep_"+systematic).c_str(), &(DrJet1Lep.Systematic(systematic)));
        for(auto& systematic: DrJet2Lep.Systematics())
          bdttree->Branch(("DrJet2Lep_"+systematic).c_str(), &(DrJet2Lep.Systematic(systematic)));
        for(auto& systematic: DrJetHBLep.Systematics())
          bdttree->Branch(("DrJetHBLep_"+systematic).c_str(), &(DrJetHBLep.Systematic(systematic)));
        for(auto& systematic: DrJet1Jet2.Systematics())
          bdttree->Branch(("DrJet1Jet2_"+systematic).c_str(), &(DrJet1Jet2.Systematic(systematic)));
        for(auto& systematic: JetLepMass.Systematics())
          bdttree->Branch(("JetLepMass_"+systematic).c_str(), &(JetLepMass.Systematic(systematic)));
        for(auto& systematic: J3Mass.Systematics())
          bdttree->Branch(("J3Mass_"+systematic).c_str(), &(J3Mass.Systematic(systematic)));
      }

      Float_t HLT_PFMET90_PFMHT90;   bdttree->Branch("HLT_PFMET90_PFMHT90",   &HLT_PFMET90_PFMHT90,   "HLT_PFMET90_PFMHT90/F"  );
      Float_t HLT_PFMET100_PFMHT100; bdttree->Branch("HLT_PFMET100_PFMHT100", &HLT_PFMET100_PFMHT100, "HLT_PFMET100_PFMHT100/F");
      Float_t HLT_PFMET110_PFMHT110; bdttree->Branch("HLT_PFMET110_PFMHT110", &HLT_PFMET110_PFMHT110, "HLT_PFMET110_PFMHT110/F");
      Float_t HLT_PFMET120_PFMHT120; bdttree->Branch("HLT_PFMET120_PFMHT120", &HLT_PFMET120_PFMHT120, "HLT_PFMET120_PFMHT120/F");
      Float_t HLT_Ele;               bdttree->Branch("HLT_Ele",               &HLT_Ele,               "HLT_Ele/F"              );
      Float_t HLT_Mu;                bdttree->Branch("HLT_Mu",                &HLT_Mu,                "HLT_Mu/F"               );

      Float_t METFilters; bdttree->Branch("METFilters", &METFilters, "METFilters/F");
      Float_t HBHENoiseFilter; bdttree->Branch("HBHENoiseFilter", &HBHENoiseFilter,"HBHENoiseFilter/F");
      Float_t HBHENoiseIsoFilter; bdttree->Branch("HBHENoiseIsoFilter", &HBHENoiseIsoFilter,"HBHENoiseIsoFilter/F");
      Float_t eeBadScFilter; bdttree->Branch("eeBadScFilter", &eeBadScFilter,"eeBadScFilter/F");
      Float_t EcalDeadCellTriggerPrimitiveFilter; bdttree->Branch("EcalDeadCellTriggerPrimitiveFilter", &EcalDeadCellTriggerPrimitiveFilter,"EcalDeadCellTriggerPrimitiveFilter/F");
      Float_t goodVertices; bdttree->Branch("goodVertices", &goodVertices,"goodVertices/F");
      Float_t globalTightHalo2016Filter; bdttree->Branch("globalTightHalo2016Filter", &globalTightHalo2016Filter, "globalTightHalo2016Filter/F");
      Float_t badMuonMoriond2017; bdttree->Branch("badMuonMoriond2017", &badMuonMoriond2017, "badMuonMoriond2017/F");
      Float_t badCloneMuonMoriond2017; bdttree->Branch("badCloneMuonMoriond2017", &badCloneMuonMoriond2017, "badCloneMuonMoriond2017/F");
      Float_t badMuonFilter; bdttree->Branch("badMuonFilter", &badMuonFilter, "badMuonFilter/F");
      Float_t badChargedHadronFilter; bdttree->Branch("badChargedHadronFilter", &badChargedHadronFilter, "badChargedHadronFilter/F");

      std::cout << "\t  Finished creating branches in output tree" << std::endl;


      Float_t Ncut0;  //bdttree->Branch("Ncut0",&Ncut0,"Ncut0/F");
      Float_t Ncut1;
      Float_t Ncut2;
      Float_t Ncut3;
      Float_t Ncut4;
      Float_t Ncut5;
      Float_t Ncut6;


      std::cout << "\t  Getting filter efficiency" << std::endl;
      TH2F* filterEfficiencyH = nullptr;
      if(sample.filterEfficiencyFile() != "")
      {
        TDirectory* cwd = gDirectory;
        TFile filterEfficiencyFile(sample.filterEfficiencyFile().c_str(), "READ");
        cwd->cd();
        filterEfficiencyH = static_cast<TH2F*>(filterEfficiencyFile.Get("filterEfficiency")->Clone("filterEfficiencyH"));
      }

      // Get total number of entries and other important values
      Nevt = 0;
      Ncut0 = 0;
      Ncut1 = 0;
      Ncut2 = 0;
      Ncut3 = 0;
      Ncut4 = 0;
      Ncut5 = 0;
      Ncut6 = 0;
      double ISRCParam = 1;
      double EWKISRCParam = 1;
      if(!process.isdata()) // Only perform the below for MC
      {
        std::cout << "\t  Getting Initial number of events, nvtx distribution and sum of gen weights: " << std::flush;

        TVectorD* sampleNevt = static_cast<TVectorD*>(puWeightFile.Get(("sample_"+sample.tag()+"_Nevt").c_str())->Clone("sampleNevt"));
        Nevt = (*sampleNevt)[0];
        TVectorD* sampleSumGenWeight = static_cast<TVectorD*>(puWeightFile.Get(("sample_"+sample.tag()+"_sumGenWeight").c_str())->Clone("sampleSumGenWeight"));
        sumGenWeight = (*sampleSumGenWeight)[0];
        TVectorD* processISRWeights = static_cast<TVectorD*>(puWeightFile.Get(("process_"+process.tag()+"_ISRCParams").c_str())->Clone("processISRWeights"));
        ISRCParam =    (*processISRWeights)[0];
        EWKISRCParam = (*processISRWeights)[1];

        delete sampleNevt;
        delete sampleSumGenWeight;
        delete processISRWeights;

        std::cout << Nevt << "; " << sumGenWeight << std::endl;
        std::cout << "\t  C value for ISR reweighting: " << ISRCParam << std::endl;
        std::cout << "\t  C value for EWK ISR reweighting: " << EWKISRCParam << std::endl;
      }

      for(auto &file : sample)
      {
        if(doSync && max_sync_count > 0 && sync_count >= max_sync_count)
          break;
        std::cout << "\t  Processing file: " << file << std::endl;
        TFile finput(file.c_str(), "READ");
        foutput.cd();
        TTree *inputtree;
        if(process.selection() != "")
          inputtree = static_cast<TTree*>(finput.Get("tree"))->CopyTree(process.selection().c_str());
        else
          inputtree = static_cast<TTree*>(finput.Get("tree"));

        // Read Branches you are interested in from the input tree
        Float_t mtw1;        inputtree->SetBranchAddress("event_mtw1"       , &mtw1);
        Float_t mtw2;        inputtree->SetBranchAddress("event_mtw2"       , &mtw2);
        Float_t mtw;         inputtree->SetBranchAddress("event_mtw"       , &mtw);
        Float_t met_pt;      inputtree->SetBranchAddress("met_pt"    , &met_pt);
        Float_t met_phi;     inputtree->SetBranchAddress("met_phi",   &met_phi);
        Int_t nLepGood;      inputtree->SetBranchAddress("nLepGood"   , &nLepGood);
        Int_t LepGood_pdgId[LEPCOLL_LIMIT];  inputtree->SetBranchAddress("LepGood_pdgId", &LepGood_pdgId);
        Int_t LepGood_mediumMuonId[LEPCOLL_LIMIT]; inputtree->SetBranchAddress("LepGood_mediumMuonId",&LepGood_mediumMuonId);
        Float_t LepGood_pt[LEPCOLL_LIMIT];  inputtree->SetBranchAddress("LepGood_pt", &LepGood_pt);
        Float_t LepGood_eta[LEPCOLL_LIMIT];  inputtree->SetBranchAddress("LepGood_eta", &LepGood_eta);
        Float_t LepGood_phi[LEPCOLL_LIMIT];  inputtree->SetBranchAddress("LepGood_phi", &LepGood_phi);
        Float_t LepGood_relIso03[LEPCOLL_LIMIT]; inputtree->SetBranchAddress("LepGood_relIso03",&LepGood_relIso03);
        Float_t LepGood_absIso03[LEPCOLL_LIMIT]; inputtree->SetBranchAddress("LepGood_absIso03",&LepGood_absIso03);
        Float_t LepGood_dxy[LEPCOLL_LIMIT]; inputtree->SetBranchAddress("LepGood_dxy",&LepGood_dxy);
        Float_t LepGood_dz[LEPCOLL_LIMIT]; inputtree->SetBranchAddress("LepGood_dz",&LepGood_dz);
        Float_t LepGood_mass[LEPCOLL_LIMIT]; inputtree->SetBranchAddress("LepGood_mass",&LepGood_mass);
        Float_t LepGood_mt[LEPCOLL_LIMIT]; inputtree->SetBranchAddress("LepGood_mt", &LepGood_mt);
        Float_t LepGood_Q80[LEPCOLL_LIMIT]; inputtree->SetBranchAddress("LepGood_Q80", &LepGood_Q80);
        Float_t LepGood_cosPhiLepMet[LEPCOLL_LIMIT]; inputtree->SetBranchAddress("LepGood_cosPhiLepMet", &LepGood_cosPhiLepMet);
        Int_t LepGood_charge[LEPCOLL_LIMIT]; inputtree->SetBranchAddress("LepGood_charge",&LepGood_charge);
        Int_t nJetIn;  inputtree->SetBranchAddress("nJet", &nJetIn);
        Float_t Jet_pt[JETCOLL_LIMIT];  inputtree->SetBranchAddress("Jet_pt", &Jet_pt);
        Int_t Jet_id[JETCOLL_LIMIT];  inputtree->SetBranchAddress("Jet_id", &Jet_id);
        Float_t Jet_eta[JETCOLL_LIMIT];  inputtree->SetBranchAddress("Jet_eta", &Jet_eta);
        Float_t Jet_phi[JETCOLL_LIMIT];  inputtree->SetBranchAddress("Jet_phi", &Jet_phi);
        Float_t Jet_btagCSV[JETCOLL_LIMIT];  inputtree->SetBranchAddress("Jet_btagCSV", &Jet_btagCSV);
        Float_t Jet_mass[JETCOLL_LIMIT];  inputtree->SetBranchAddress("Jet_mass", &Jet_mass);
        Float_t Jet_rawPt[JETCOLL_LIMIT]; inputtree->SetBranchAddress("Jet_rawPt", &Jet_rawPt);
        UInt_t run;  inputtree->SetBranchAddress("run", &run);
        ULong64_t evt;  inputtree->SetBranchAddress("evt", &evt);
        UInt_t lumi;  inputtree->SetBranchAddress("lumi", &lumi);
        Int_t nVert_i; inputtree->SetBranchAddress("nVert", &nVert_i);

        int nLHEweight;
        int LHEweight_id[LHEWEIGHT_LIMIT];
        float LHEweight_wgt[LHEWEIGHT_LIMIT];
        Int_t Jet_hadronFlavour[JETCOLL_LIMIT];

        if(!process.isdata())
        {
          inputtree->SetBranchAddress("nLHEweight"   , &nLHEweight);
          inputtree->SetBranchAddress("LHEweight_id" , &LHEweight_id);
          inputtree->SetBranchAddress("LHEweight_wgt", &LHEweight_wgt);
          inputtree->SetBranchAddress("Jet_hadronFlavour", &Jet_hadronFlavour);
        }

        Float_t nIsr; inputtree->SetBranchAddress("nIsr", &nIsr);

        Float_t met_JetEnUp_Pt;      inputtree->SetBranchAddress("met_JetEnUp_Pt"    , &met_JetEnUp_Pt);
        Float_t met_JetEnUp_Phi;     inputtree->SetBranchAddress("met_JetEnUp_Phi",   &met_JetEnUp_Phi);
        Float_t met_JetEnDown_Pt;      inputtree->SetBranchAddress("met_JetEnDown_Pt"    , &met_JetEnDown_Pt);
        Float_t met_JetEnDown_Phi;     inputtree->SetBranchAddress("met_JetEnDown_Phi",   &met_JetEnDown_Phi);
        Float_t met_JetResUp_Pt;
        Float_t met_JetResUp_Phi;
        Float_t met_JetResDown_Pt;
        Float_t met_JetResDown_Phi;
        Float_t Jet_corr[JETCOLL_LIMIT]; inputtree->SetBranchAddress("Jet_corr", &Jet_corr);
        Float_t Jet_corr_JECUp[JETCOLL_LIMIT]; inputtree->SetBranchAddress("Jet_corr_JECUp", &Jet_corr_JECUp);
        Float_t Jet_corr_JECDown[JETCOLL_LIMIT]; inputtree->SetBranchAddress("Jet_corr_JECDown", &Jet_corr_JECDown);
        Float_t Jet_corr_JER[JETCOLL_LIMIT];
        Float_t Jet_corr_JERUp[JETCOLL_LIMIT];
        Float_t Jet_corr_JERDown[JETCOLL_LIMIT];

        if(!process.isdata())
        {
          inputtree->SetBranchAddress("met_JetResUp_Pt"    , &met_JetResUp_Pt);
          inputtree->SetBranchAddress("met_JetResUp_Phi",   &met_JetResUp_Phi);
          inputtree->SetBranchAddress("met_JetResDown_Pt"    , &met_JetResDown_Pt);
          inputtree->SetBranchAddress("met_JetResDown_Phi",   &met_JetResDown_Phi);
          inputtree->SetBranchAddress("Jet_corr_JER", &Jet_corr_JER);
          inputtree->SetBranchAddress("Jet_corr_JERUp", &Jet_corr_JERUp);
          inputtree->SetBranchAddress("Jet_corr_JERDown", &Jet_corr_JERDown);
        }

        Int_t   LepGood_mcMatchId[LEPCOLL_LIMIT];
        Int_t nGenPart = 0;
        Int_t GenPart_motherId[GENPART_LIMIT];
        Int_t GenPart_grandmotherId[GENPART_LIMIT];
        Int_t GenPart_sourceId[GENPART_LIMIT];
        Int_t GenPart_status[GENPART_LIMIT];
        Int_t GenPart_pdgId[GENPART_LIMIT];
        Float_t GenPart_pt[GENPART_LIMIT];
        Float_t GenPart_eta[GENPART_LIMIT];
        Float_t GenPart_phi[GENPART_LIMIT];
        Float_t xsec = 1;
        Float_t nTrueInt = 1;
        if(!process.isdata())
        {
          inputtree->SetBranchAddress("genWeight", &genWeight);
          inputtree->SetBranchAddress("xsec", &xsec);
          inputtree->SetBranchAddress("nTrueInt", &nTrueInt);
          inputtree->SetBranchAddress("LepGood_mcMatchId", &LepGood_mcMatchId);

          inputtree->SetBranchAddress("nGenPart", &nGenPart);
          inputtree->SetBranchAddress("GenPart_motherId", &GenPart_motherId);
          inputtree->SetBranchAddress("GenPart_grandmotherId", &GenPart_grandmotherId);
          inputtree->SetBranchAddress("GenPart_sourceId", &GenPart_sourceId);
          inputtree->SetBranchAddress("GenPart_status", &GenPart_status);
          inputtree->SetBranchAddress("GenPart_pdgId", &GenPart_pdgId);
          inputtree->SetBranchAddress("GenPart_pt", &GenPart_pt);
          inputtree->SetBranchAddress("GenPart_eta", &GenPart_eta);
          inputtree->SetBranchAddress("GenPart_phi", &GenPart_phi);
        }

        // 2015 HLT
        /*Int_t HLT_PFMET170_JetIdCleaned;   inputtree->SetBranchAddress("HLT_PFMET170_JetIdCleaned", &HLT_PFMET170_JetIdCleaned);
        Int_t HLT_PFMET90_PFMHT90;   inputtree->SetBranchAddress("HLT_PFMET90_PFMHT90", &HLT_PFMET90_PFMHT90);
        Int_t HLT_PFMETNoMu90_PFMHTNoMu90;   inputtree->SetBranchAddress("HLT_PFMETNoMu90_PFMHTNoMu90", &HLT_PFMETNoMu90_PFMHTNoMu90);// */

        // 2016 HLT
        Int_t   HLT_PFMET90_PFMHT90_IDTight; inputtree->SetBranchAddress("HLT_PFMET90_PFMHT90_IDTight"  , &HLT_PFMET90_PFMHT90_IDTight  );
        Int_t HLT_PFMET100_PFMHT100_IDTight; inputtree->SetBranchAddress("HLT_PFMET100_PFMHT100_IDTight", &HLT_PFMET100_PFMHT100_IDTight);
        Int_t HLT_PFMET110_PFMHT110_IDTight; inputtree->SetBranchAddress("HLT_PFMET110_PFMHT110_IDTight", &HLT_PFMET110_PFMHT110_IDTight);
        Int_t HLT_PFMET120_PFMHT120_IDTight; inputtree->SetBranchAddress("HLT_PFMET120_PFMHT120_IDTight", &HLT_PFMET120_PFMHT120_IDTight);
        Int_t HLT_Ele24_eta2p1_WPLoose_Gsf;  inputtree->SetBranchAddress("HLT_Ele24_eta2p1_WPLoose_Gsf" , &HLT_Ele24_eta2p1_WPLoose_Gsf );
        Int_t HLT_IsoMu24;                   inputtree->SetBranchAddress("HLT_IsoMu24"                  , &HLT_IsoMu24                  );
        Int_t HLT_Ele25_eta2p1_WPLoose_Gsf;  inputtree->SetBranchAddress("HLT_Ele25_eta2p1_WPLoose_Gsf" , &HLT_Ele25_eta2p1_WPLoose_Gsf );
        Int_t HLT_IsoMu27;                   inputtree->SetBranchAddress("HLT_IsoMu27"                  , &HLT_IsoMu27                  );

        Int_t Flag_METFilters; inputtree->SetBranchAddress("Flag_METFilters", &Flag_METFilters);
        Int_t Flag_HBHENoiseFilter; inputtree->SetBranchAddress("Flag_HBHENoiseFilter", &Flag_HBHENoiseFilter);
        Int_t Flag_HBHENoiseIsoFilter; inputtree->SetBranchAddress("Flag_HBHENoiseIsoFilter", &Flag_HBHENoiseIsoFilter);
        Int_t Flag_eeBadScFilter; inputtree->SetBranchAddress("Flag_eeBadScFilter", &Flag_eeBadScFilter);
        Int_t Flag_EcalDeadCellTriggerPrimitiveFilter; inputtree->SetBranchAddress("Flag_EcalDeadCellTriggerPrimitiveFilter", &Flag_EcalDeadCellTriggerPrimitiveFilter);
        Int_t Flag_goodVertices; inputtree->SetBranchAddress("Flag_goodVertices", &Flag_goodVertices);
        Int_t Flag_globalTightHalo2016Filter; inputtree->SetBranchAddress("Flag_globalTightHalo2016Filter", &Flag_globalTightHalo2016Filter);
        Int_t Flag_badMuonMoriond2017; inputtree->SetBranchAddress("Flag_badMuonMoriond2017", &Flag_badMuonMoriond2017);
        Int_t Flag_badCloneMuonMoriond2017; inputtree->SetBranchAddress("Flag_badCloneMuonMoriond2017", &Flag_badCloneMuonMoriond2017);
        Float_t Flag_badMuonFilter; inputtree->SetBranchAddress("Flag_badMuonFilter", &Flag_badMuonFilter);
        Float_t Flag_badChargedHadronFilter; inputtree->SetBranchAddress("Flag_badChargedHadronFilter", &Flag_badChargedHadronFilter);

        Int_t GenSusyMGravitino=0;
        Int_t GenSusyMStop=0;
        Int_t GenSusyMSbottom=0;
        Int_t GenSusyMNeutralino=0;
        if(!process.isdata())
        {
          inputtree->SetBranchAddress("GenSusyMGravitino", &GenSusyMGravitino);
          inputtree->SetBranchAddress("GenSusyMStop", &GenSusyMStop);
          inputtree->SetBranchAddress("GenSusyMSbottom", &GenSusyMSbottom);
          inputtree->SetBranchAddress("GenSusyMNeutralino", &GenSusyMNeutralino);
        }

        // Read the number of entries in the inputtree
        size_t nentries = static_cast<size_t>(inputtree->GetEntries());
        std::cout << "\t    The file has " << nentries << " events." << std::endl;
        std::cout << "\t    Progress Bar: " << std::flush;
        int statusPrint = 0;
        if(nentries < 20)
          statusPrint = 1;
        else
          statusPrint = nentries/20;
        for(size_t i = 0; i < nentries; i++)
        {
          // Uncomment this if you suspect the loop is not going through the events
          /*if(i%2 == 0)
            std::cout << "$\b" << std::flush;
          else
            std::cout << " \b" << std::flush;// */
          if((statusPrint == 1) || (i%statusPrint == 0 && i != 0))
            std::cout << "*" << std::flush;

          if(doSync && max_sync_count > 0 && sync_count >= max_sync_count)
            break;

          inputtree->GetEntry(i);
          nVert = nVert_i;
          nIsr_out = nIsr;
          if(!process.isdata())
          {
            puWeight = static_cast<float>(puWeightDistrib->GetBinContent(puWeightDistrib->FindBin(nTrueInt)));
            if(puWeightDistribUp != nullptr)
            {
              puWeight.Systematic("PU_Up")   = puWeightDistribUp->GetBinContent(puWeightDistribUp->FindBin(nTrueInt));
              puWeight.Systematic("PU_Down") = puWeightDistribDown->GetBinContent(puWeightDistribDown->FindBin(nTrueInt));
            }
          }
          else
            puWeight = 1.0f;

          // Object ID
          std::vector<int> validLeptons;
          std::vector<int> looseLeptons;
          ValueWithSystematics<std::vector<double>> jetPt;
          ValueWithSystematics<std::vector<int>> validJets;
          ValueWithSystematics<std::vector<int>> bjetList; // Same as validJets, but sorted by CSV value

          validLeptons.clear();
          looseLeptons.clear();

          for(Int_t i = 0; i < nJetIn; ++i)
          {
            jetPt.Value().push_back(Jet_pt[i]);
            if(!process.isdata())
            {
              jetPt.Systematic("JES_Up").push_back(Jet_pt[i]/Jet_corr[i]*Jet_corr_JECUp[i]);
              jetPt.Systematic("JES_Down").push_back(Jet_pt[i]/Jet_corr[i]*Jet_corr_JECDown[i]);

              if(Jet_corr_JER[i] < 0)
              {
                jetPt.Systematic("JER_Up").push_back(Jet_pt[i]);
                jetPt.Systematic("JER_Down").push_back(Jet_pt[i]);
              }
              else
              {
                jetPt.Systematic("JER_Up").push_back(Jet_pt[i]/Jet_corr_JER[i]*Jet_corr_JERUp[i]);
                jetPt.Systematic("JER_Down").push_back(Jet_pt[i]/Jet_corr_JER[i]*Jet_corr_JERDown[i]);
              }
            }
          }
          std::vector<std::string> list;
          list.push_back("Value");
          loadSystematics(list, jetPt);

          for(auto & syst : list)
          {
            if(syst != "Value")
            {
              std::vector<int> empty;
              validJets.Systematic(syst) = empty;
              bjetList.Systematic(syst) = empty;
            }

            for(Int_t i = 0; i < nJetIn; ++i)
            {
              if(std::abs(Jet_eta[i]) < 2.4 && (jetPt.GetSystematicOrValue(syst))[i] > jetPtThreshold)
              {
                validJets.GetSystematicOrValue(syst).push_back(i);
                bjetList.GetSystematicOrValue(syst).push_back(i);
              }
            }

            std::sort(validJets.GetSystematicOrValue(syst).begin(), validJets.GetSystematicOrValue(syst).end(), [&jetPt, &syst] (const int &left, const int &right) {
              return (jetPt.GetSystematicOrValue(syst))[left] > (jetPt.GetSystematicOrValue(syst))[right];
            });
            std::sort(bjetList.GetSystematicOrValue(syst).begin(), bjetList.GetSystematicOrValue(syst).end(), [Jet_btagCSV] (const int &left, const int &right) {
              return Jet_btagCSV[left] > Jet_btagCSV[right];
            });
          }

          auto jetThreshold = [&jetPt, &validJets] (const std::string& syst = "Value") -> bool
          {
            if(validJets.GetSystematicOrValue(syst).size() == 0)
              return false;
            return (jetPt.GetSystematicOrValue(syst))[validJets.GetSystematicOrValue(syst)[0]] > ISR_JET_PT;
          };

          if(preemptiveDropEvents)
          {
            if(static_cast<bool>(validJets.size() > 0))
            {
              ValueWithSystematics<bool> jetPass = jetThreshold();
              for(auto& syst: validJets.Systematics())
              {
                jetPass.Systematic(syst) = jetThreshold(syst);
              }

              if(!static_cast<bool>(jetPass))
                continue;
            }
            else
              continue;
          }

          nGoodMu = 0;
          nGoodEl = 0;
          nGoodMu_loose = 0;
          nGoodEl_loose = 0;
          for(int i = 0; i < nLepGood; ++i)
          {
            bool lPTETA = false;
            if(std::abs(LepGood_pdgId[i]) == 13) // If a muon
            {
              lPTETA = LepGood_pt[i] > 3.5;
              lPTETA = lPTETA && (std::abs(LepGood_eta[i]) < 2.4);
            }
            else // If an electron
            {
              lPTETA = LepGood_pt[i] > 5.0;
              lPTETA = lPTETA && (std::abs(LepGood_eta[i]) < 2.5);
              // Also veto the gap in the ECAL
              lPTETA = lPTETA && ( (std::abs(LepGood_eta[i]) > ECALGap_MaxEta)
                                || (std::abs(LepGood_eta[i]) < ECALGap_MinEta) );
            }

            bool lID       = (std::abs(LepGood_dxy[i]) < 0.02)
                          && (std::abs(LepGood_dz[i]) < 0.1);
            bool lID_loose = (std::abs(LepGood_dxy[i]) < 0.1)
                          && (std::abs(LepGood_dz[i]) < 0.5);

            bool lIS       = LepGood_relIso03[i] < 0.2 || LepGood_absIso03[i] < 5.0;
            bool lIS_loose = LepGood_relIso03[i] < 0.8 || LepGood_absIso03[i] < 20.0;

            if(lPTETA && lID && lIS)
            {
              validLeptons.push_back(i);
              if(std::abs(LepGood_pdgId[i]) == 13)
                nGoodMu++;
              else
                nGoodEl++;
            }
            if(lPTETA && lID_loose && lIS_loose)
            {
              looseLeptons.push_back(i);
              if(std::abs(LepGood_pdgId[i]) == 13)
                nGoodMu_loose++;
              else
                nGoodEl_loose++;
            }
          }
          std::sort(validLeptons.begin(), validLeptons.end(), [&LepGood_pt] (const int &left, const int &right) {
            return LepGood_pt[left] > LepGood_pt[right];
            });
          std::sort(looseLeptons.begin(), looseLeptons.end(), [&LepGood_pt] (const int &left, const int &right) {
            return LepGood_pt[left] > LepGood_pt[right];
            });

          auto lepSel = [&LepGood_pt] (std::vector<int> &leptons) -> bool
          {
            if(leptons.size() > 0)
            {
              if(leptons.size() > 1)
              {
                if(LepGood_pt[leptons[1]] > SECOND_LEPTON_PT)
                  return false;
              }
              return true;
            }
            return false;
          };

          isTight = lepSel(validLeptons);

          isLoose = lepSel(looseLeptons);

          isLooseNotTight = isLoose && !isTight;
          if(isLooseNotTight)
          {
            validLeptons = looseLeptons;
            nGoodMu = nGoodMu_loose;
            nGoodEl = nGoodEl_loose;
          }

          if(preemptiveDropEvents && looseNotTight && !isLooseNotTight)
            continue;

          if(preemptiveDropEvents && !doLooseLeptons && isLooseNotTight)
            continue;

          if(preemptiveDropEvents && !(isLoose || isTight))
            continue;

          // Setting the values to be saved in the output tree
          ValueWithSystematics<double> MetDou = met_pt;
          ValueWithSystematics<double> MetPhi = met_phi;
          if(!process.isdata())
          {
            MetDou.Systematic("JES_Up") = met_JetEnUp_Pt;
            MetDou.Systematic("JES_Down") = met_JetEnDown_Pt;
            MetDou.Systematic("JER_Up") = met_JetResUp_Pt;
            MetDou.Systematic("JER_Down") = met_JetResDown_Pt;
            MetPhi.Systematic("JES_Up") = met_JetEnUp_Phi;
            MetPhi.Systematic("JES_Down") = met_JetEnDown_Phi;
            MetPhi.Systematic("JER_Up") = met_JetResUp_Phi;
            MetPhi.Systematic("JER_Down") = met_JetResDown_Phi;
          }
          Met = MetDou;

          if(preemptiveDropEvents)
          {
            if(!swap && !static_cast<bool>(Met > MIN_MET))
              continue;
          }


          if(!process.isdata())
          {
            std::map<int, std::string>& lheNames = getLHEMap(nLHEweight);

            int refLHEKey = -1;
            int refQ2 = -1;
            for(int i = 0; i < nLHEweight; ++i)
            {
              std::string name = lheNames[LHEweight_id[i]];
              if(name.substr(0, 2) == "Q2")
              {
                if(name.substr(3,1) == "0") // If it is the reference Q^2 variation
                {
                  refQ2 = i;
                  refLHEKey = LHEweight_id[i];
                  break;
                }
              }
            }

            Q2Var = 1;
            if(refQ2 >= 0)
            {
              for(int i = 0; i < nLHEweight; ++i)
              {
                std::string name = lheNames[LHEweight_id[i]];
                if(name.substr(0, 2) == "Q2")
                {
                  if(name.substr(3,1) == "0") // If it is the reference Q^2 variation
                    continue;

                  double norm = lheScaleMap->at(refLHEKey)/lheScaleMap->at(LHEweight_id[i]);
                  Q2Var.Systematic(name) = norm * LHEweight_wgt[i]/LHEweight_wgt[refQ2];
                }
              }
            }
          }

          float lep_phi, lep_eta;
          if(validLeptons.size() > 0)
          {
            auto leptonIndex = validLeptons[0];
            lep_phi     = LepGood_phi[leptonIndex];
            lep_eta     = LepGood_eta[leptonIndex];
          }
          else
          {
            lep_phi     = -9999;
            lep_eta     = -9999;
          }

          auto loadQuantity = [] (Float_t* vector, ValueWithSystematics<std::vector<int>> indexer, size_t index, double defaultValue = -9999) -> ValueWithSystematics<double>
          {
            ValueWithSystematics<double> retVal = defaultValue;

            std::vector<std::string> list;
            loadSystematics(list, indexer);
            for(auto& syst: list)
              retVal.Systematic(syst);
            list.push_back("Value");

            for(auto& syst: list)
            {
              if(indexer.GetSystematicOrValue(syst).size() > index)
              {
                retVal.GetSystematicOrValue(syst) = vector[indexer.GetSystematicOrValue(syst)[index]];
              }
            }

            return retVal;
          };
          auto loadSysQuantity = [] (ValueWithSystematics<std::vector<double>> vector, ValueWithSystematics<std::vector<int>> indexer, size_t index, double defaultValue = -9999) -> ValueWithSystematics<double>
          {
            ValueWithSystematics<double> retVal = defaultValue;

            std::vector<std::string> list;
            loadSystematics(list, indexer);
            loadSystematics(list, vector);
            for(auto& syst: list)
              retVal.Systematic(syst);
            list.push_back("Value");

            for(auto& syst: list)
            {
              if(indexer.GetSystematicOrValue(syst).size() > index)
              {
                retVal.GetSystematicOrValue(syst) = vector.GetSystematicOrValue(syst)[indexer.GetSystematicOrValue(syst)[index]];
              }
            }

            return retVal;
          };

          ValueWithSystematics<double> Jet2EtaDou, Jet1EtaDou;
          Jet2Pt     = loadSysQuantity(jetPt,    validJets, 1);
          JetB2pt    = loadSysQuantity(jetPt,    bjetList,  1);
          Jet2EtaDou = loadQuantity(Jet_eta,     validJets, 1);
          Jet2CSV    = loadQuantity(Jet_btagCSV, validJets, 1);
          JetB2eta   = loadQuantity(Jet_eta,     bjetList,  1);
          JetB2CSV   = loadQuantity(Jet_btagCSV, bjetList,  1);
          JetB2index = loadQuantity(identity,    bjetList,  1);
          Jet1EtaDou = loadQuantity(Jet_eta,     validJets, 0);

          Jet2Eta = Jet2EtaDou;
          Jet1Eta = Jet1EtaDou;

          ValueWithSystematics<double> Jet1Phi = loadQuantity(Jet_phi, validJets, 0);
          ValueWithSystematics<double> Jet2Phi = loadQuantity(Jet_phi, validJets, 1);

          auto DeltaPhiSys = [] (const ValueWithSystematics<double>& phi1, const ValueWithSystematics<double>& phi2, double defaultValue = -9999) -> ValueWithSystematics<double>
          {
            ValueWithSystematics<double> retVal = phi1 - phi2;

            std::vector<std::string> list;
            list.push_back("Value");
            loadSystematics(list, retVal);

            for(auto& syst: list)
            {
              if(phi1.GetSystematicOrValue(syst) == defaultValue || phi2.GetSystematicOrValue(syst) == defaultValue)
                retVal.GetSystematicOrValue(syst) = defaultValue;
              else
              {
                while(retVal.GetSystematicOrValue(syst) >= M_PIl)
                  retVal.GetSystematicOrValue(syst) -= (2*M_PIl);
                while(retVal.GetSystematicOrValue(syst) < -M_PIl)
                  retVal.GetSystematicOrValue(syst) += (2*M_PIl);

                if(retVal.GetSystematicOrValue(syst) < 0)
                  retVal.GetSystematicOrValue(syst) = -retVal.GetSystematicOrValue(syst);
              }
            }

            return retVal;
          };
          auto CosDeltaPhiSys = [] (const ValueWithSystematics<double>& phi1, const ValueWithSystematics<double>& phi2, double defaultValue = -9999) -> ValueWithSystematics<double>
          {
            ValueWithSystematics<double> retVal = phi1 - phi2;
            retVal = retVal.Cos();

            std::vector<std::string> list;
            list.push_back("Value");
            loadSystematics(list, retVal);

            for(auto& syst: list)
            {
              if(phi1.GetSystematicOrValue(syst) == defaultValue || phi2.GetSystematicOrValue(syst) == defaultValue)
                retVal.GetSystematicOrValue(syst) = defaultValue;
            }

            return retVal;
          };
          auto DeltaEtaSys = [] (const ValueWithSystematics<double>& eta1, const ValueWithSystematics<double>& eta2, double defaultValue = -9999) -> ValueWithSystematics<double>
          {
            ValueWithSystematics<double> retVal = eta1 - eta2;

            std::vector<std::string> list;
            list.push_back("Value");
            loadSystematics(list, retVal);

            for(auto& syst: list)
            {
              if(eta1.GetSystematicOrValue(syst) == defaultValue || eta2.GetSystematicOrValue(syst) == defaultValue)
                retVal.GetSystematicOrValue(syst) = defaultValue;
            }

            return retVal;
          };
          auto QuadSumSys = []  (const ValueWithSystematics<double>& par1, const ValueWithSystematics<double>& par2, double defaultValue = -9999) -> ValueWithSystematics<double>
          {
            ValueWithSystematics<double> retVal = ValueWithSystematics<double>(par1.pow(2) + par2.pow(2)).Sqrt();

            std::vector<std::string> list;
            list.push_back("Value");
            loadSystematics(list, retVal);

            for(auto& syst: list)
            {
              if(par1.GetSystematicOrValue(syst) == defaultValue || par2.GetSystematicOrValue(syst) == defaultValue)
                retVal.GetSystematicOrValue(syst) = defaultValue;
            }

            return retVal;
          };

          ValueWithSystematics<double> dphijj = DeltaPhiSys(Jet1Phi, Jet2Phi);
          ValueWithSystematics<double> detajj = DeltaEtaSys(Jet1EtaDou, Jet2EtaDou);
          DPhiJet1Jet2 = dphijj;
          DrJet1Jet2 = QuadSumSys(dphijj, detajj);

          ValueWithSystematics<double> dphi = DeltaPhiSys(Jet2Phi, ValueWithSystematics<double>(lep_phi));
          ValueWithSystematics<double> deta = DeltaEtaSys(Jet2EtaDou, ValueWithSystematics<double>(lep_eta));
          DrJet2Lep = QuadSumSys(dphi, deta);

          if(preemptiveDropEvents && !static_cast<bool>(DPhiJet1Jet2 < 2.5 || Jet2Pt < 60))
            continue;

          TLorentzVector VLep;
          if(validLeptons.size() > 0)
          {
            auto leptonIndex = validLeptons[0];
            LepChg      = LepGood_charge[leptonIndex];
            LepID       = LepGood_pdgId[leptonIndex];
            LepPt       = LepGood_pt[leptonIndex];
            LepEta      = LepGood_eta[leptonIndex];
            LepDxy      = LepGood_dxy[leptonIndex];
            LepDz       = LepGood_dz[leptonIndex];
            LepIso03    = LepGood_relIso03[leptonIndex];
            Float_t minPt = 25.;
            LepHybIso03 = LepIso03*std::min(LepPt, minPt);
            VLep.SetPtEtaPhiM(LepPt, LepEta, lep_phi, LepGood_mass[leptonIndex]);

            ValueWithSystematics<double> CosDeltaPhiDou = CosDeltaPhiSys(MetPhi, ValueWithSystematics<double>(lep_phi));
            CosDeltaPhi = CosDeltaPhiDou;
            mt = ValueWithSystematics<double>(2.0 * LepPt * MetDou * (1 - CosDeltaPhiDou)).Sqrt();
            Q80 = 1.0 - 80.*80./(2.0 * LepPt * Met);

            if(!process.isdata())
            {
              leptonIDSF = getLeptonIDSFSys(LepID, LepPt, LepEta);
              leptonISOSF = getLeptonISOSFSys(LepID, LepPt, LepEta);
              if(sample.isFastsim() || process.isfastsim()) // Maybe need to add some more conditions here
                leptonFullFastSF = getFullFastSFSys(LepID, LepPt, LepEta);
              else
                leptonFullFastSF = 1.0;
            }

            if(!process.isdata() && doPromptTagging)
            {
              auto mcMatchId = LepGood_mcMatchId[leptonIndex];
              bool isPromptFlag = !(mcMatchId == 0 || mcMatchId == 99 || mcMatchId == 100);
              if(!isPromptFlag)
              {
                for(int genPartIndex = 0; genPartIndex < nGenPart; ++genPartIndex)
                {
                  if(std::abs(GenPart_pdgId[genPartIndex]) == 15)
                  {
                    if(std::abs(GenPart_motherId[genPartIndex]) == 24 || std::abs(GenPart_motherId[genPartIndex]) == 23 || (GenPart_motherId[genPartIndex] == -9999 && genPartIndex < 3 ))
                    {
                      double dphi = DeltaPhi(GenPart_phi[genPartIndex], lep_phi);
                      double deta = GenPart_eta[genPartIndex] - lep_eta;
                      double dr = std::sqrt(std::pow(dphi,2) + std::pow(deta,2));

                      if(dr < 0.15)
                      {
                        isPromptFlag = true;
                        break;
                      }
                    }
                  }
                }
              }

              isPrompt    = isPromptFlag?1:0;
            }
            else
            {
              isPrompt = 1;
            }
          }
          else
          {
            mt          = -9999;
            Q80         =  9999; // Q80 has a range of [-Inf, 1], so it does not make sense to give it a negative value when not defined
            CosDeltaPhi = -9999;
            LepChg      = -9999;
            LepID       = -9999;
            LepPt       = -9999;
            LepEta      = -9999;
            LepDxy      = -9999;
            LepDz       = -9999;
            LepIso03    = -9999;
            LepHybIso03 = -9999;
            isPrompt    = -9999;
          }
          if(validLeptons.size() > 1)
          {
            auto leptonIndex = validLeptons[1];
            Lep2Chg      = LepGood_charge[leptonIndex];
            Lep2ID       = LepGood_pdgId[leptonIndex];
            Lep2Pt       = LepGood_pt[leptonIndex];
            Lep2Eta      = LepGood_eta[leptonIndex];
            Lep2Dxy      = LepGood_dxy[leptonIndex];
            Lep2Dz       = LepGood_dz[leptonIndex];
            Lep2Iso03    = LepGood_relIso03[leptonIndex];
          }
          else
          {
            Lep2Chg      = -9999;
            Lep2ID       = -9999;
            Lep2Pt       = -9999;
            Lep2Eta      = -9999;
            Lep2Dxy      = -9999;
            Lep2Dz       = -9999;
            Lep2Iso03    = -9999;
          }

          if(!process.isdata())
          {
            bTagSF = getBTagSFSys(bReader, validJets, jetPt, Jet_eta, Jet_btagCSV, Jet_hadronFlavour);
            triggerEfficiency = triggerEfficiencyFromMETSys(MetDou);
            // For EWK ISR, assume syst 100%
            if(process.tag() == "WJets")
              EWKISRweight = EWKISRCParam * EWKISRweightFromISRpTSys(LepPt, lep_phi, MetDou, MetPhi);
            // For ISR, assume syst 50%
            if(process.tag() == "ttbar" || process.tag() == "ttbar_lep" || process.tag() == "ttbar_lo" || process.issignal())
              ISRweight = ISRCParam * ISRweightFromNISRJetSys(nIsr);
            // TODO: missing tt_pow reweighting https://twiki.cern.ch/twiki/bin/viewauth/CMS/SUSRecommendationsMoriond17
          }

          ValueWithSystematics<double> JetB1EtaDou, JetB1Phi;
          Jet1Pt     = loadSysQuantity(jetPt,    validJets, 0);
          JetHBpt    = loadSysQuantity(jetPt,    bjetList,  0);
          Jet1CSV    = loadQuantity(Jet_btagCSV, validJets, 0);
          JetB1EtaDou= loadQuantity(Jet_eta,     bjetList,  0);
          JetHBCSV   = loadQuantity(Jet_btagCSV, bjetList,  0);
          JetHBindex = loadQuantity(identity,    bjetList,  0);
          JetB1Phi   = loadQuantity(Jet_phi,     bjetList,  0);

          JetHBeta = JetB1EtaDou;

          dphi = DeltaPhiSys(Jet1Phi, ValueWithSystematics<double>(lep_phi));
          deta = DeltaEtaSys(Jet1EtaDou, ValueWithSystematics<double>(lep_eta));
          DrJet1Lep = QuadSumSys(dphi, deta);

          dphi = DeltaPhiSys(JetB1Phi, ValueWithSystematics<double>(lep_phi));
          deta = DeltaEtaSys(JetB1EtaDou, ValueWithSystematics<double>(lep_eta));
          DrJetHBLep = QuadSumSys(dphi, deta);

          list.clear();
          list.push_back("Value");
          loadSystematics(list, jetPt);
          loadSystematics(list, validJets);

          if(validLeptons.size() > 0)
          {
            for(auto& syst: list)
            {
              auto& jetList = validJets.GetSystematicOrValue(syst);
              if(jetList.size() > 0)
              {
                double smallestDeltaR = 999999999.;
                int closestJet = -1;

                for(auto& jet: jetList)
                {
                  double dphi = DeltaPhi(Jet_phi[jet], lep_phi);
                  double deta = Jet_eta[jet] - lep_eta;
                  double dr = std::pow(dphi,2) + std::pow(deta,2); // The square of the dr is used since it is injective
                  if(dr < smallestDeltaR)
                  {
                    smallestDeltaR = dr;
                    closestJet = jet;
                  }
                }

                TLorentzVector VJ, JLep;
                VJ.SetPtEtaPhiM(Jet_pt[closestJet], Jet_eta[closestJet], Jet_phi[closestJet], Jet_mass[closestJet]);
                JLep = VJ + VLep;
                JetLepMass.GetSystematicOrValue(syst) = JLep.M();

                TLorentzVector VJ3;
                for(auto& jet: jetList)
                {
                  if(jet == closestJet)
                    continue;
                  TLorentzVector VJi;
                  VJi.SetPtEtaPhiM(Jet_pt[jet], Jet_eta[jet], Jet_phi[jet], Jet_mass[jet]);
                  VJ3 += VJi;
                }
                J3Mass.GetSystematicOrValue(syst) = VJ3.M();
              }
              else
              {
                JetLepMass.GetSystematicOrValue(syst) = -9999;
                J3Mass.GetSystematicOrValue(syst) = -9999;
              }
            }
          }
          else
          {
            JetLepMass = -9999;
            J3Mass = -9999;
          }

          Jet3Pt     = loadSysQuantity(jetPt,    validJets, 2);
          Jet3Eta    = loadQuantity(Jet_eta,     validJets, 2);
          Jet3CSV    = loadQuantity(Jet_btagCSV, validJets, 2);

          list.clear();
          list.push_back("Value");
          loadSystematics(list, bjetList);

          NbLoose = 0;
          NbTight = 0;
          NbLooseTo50 = 0;
          NbLoose50 = 0;
          NbMediumTo50 = 0;
          NbMedium50 = 0;
          NbTightTo50 = 0;
          NbTight50 = 0;
          for(auto& syst: list)
          {
            for(auto& bjet: bjetList.GetSystematicOrValue(syst))
            {
              const auto &csv = Jet_btagCSV[bjet];
              const auto &pt = jetPt.GetSystematicOrValue(syst)[bjet];

              if(csv > CSV_Loose)
                ++(NbLoose.GetSystematicOrValue(syst));
              if(csv > CSV_Tight)
                ++(NbTight.GetSystematicOrValue(syst));

              if(pt > 50)
              {
                if(csv > CSV_Loose)
                  ++(NbLoose50.GetSystematicOrValue(syst));
                if(csv > CSV_Medium)
                  ++(NbMedium50.GetSystematicOrValue(syst));
                if(csv > CSV_Tight)
                  ++(NbTight50.GetSystematicOrValue(syst));
              }
              else
              {
                if(csv > CSV_Loose)
                  ++(NbLooseTo50.GetSystematicOrValue(syst));
                if(csv > CSV_Medium)
                  ++(NbMediumTo50.GetSystematicOrValue(syst));
                if(csv > CSV_Tight)
                  ++(NbTightTo50.GetSystematicOrValue(syst));
              }

              // Since the bjetList is sorted by CSV, as soon as it goes below the loose definition, there are no further bjets
              if(csv < CSV_Loose)
                break;
            }
          }

          list.clear();
          list.push_back("Value");
          loadSystematics(list, validJets);

          HT = 0;
          Njet = validJets.size();
          Njet30 = 0;
          Njet40 = 0;
          Njet50 = 0;
          Njet60 = 0;
          Njet70 = 0;
          Njet80 = 0;
          Njet90 = 0;
          Njet100 = 0;
          for(auto& syst: list)
          {
            for(auto &jet : validJets.GetSystematicOrValue(syst))
            {
              const auto &pt = jetPt.GetSystematicOrValue(syst)[jet];

              HT.GetSystematicOrValue(syst) += pt;

              if(pt > 30)
                ++(Njet30.GetSystematicOrValue(syst));
              if(pt > 40)
                ++(Njet40.GetSystematicOrValue(syst));
              if(pt > 50)
                ++(Njet50.GetSystematicOrValue(syst));
              if(pt > 60)
                ++(Njet60.GetSystematicOrValue(syst));
              if(pt > 70)
                ++(Njet70.GetSystematicOrValue(syst));
              if(pt > 80)
                ++(Njet80.GetSystematicOrValue(syst));
              if(pt > 90)
                ++(Njet90.GetSystematicOrValue(syst));
              if(pt > 100)
                ++(Njet100.GetSystematicOrValue(syst));
            }
          }

          genGravitinoM  = GenSusyMGravitino;
          genStopM       = GenSusyMStop;
          genSbottomM    = GenSusyMSbottom;
          genNeutralinoM = GenSusyMNeutralino;

          Run = run;
          Event = evt;
          LumiSec = lumi;
          if(overrideXSec)
            XS = sample.crossSection();
          else
            XS = xsec;
          if(process.issignal())
            XS = stopCrossSection(genStopM, genNeutralinoM).value();

          HLT_PFMET90_PFMHT90                 = HLT_PFMET90_PFMHT90_IDTight;
          HLT_PFMET100_PFMHT100               = HLT_PFMET100_PFMHT100_IDTight;
          HLT_PFMET110_PFMHT110               = HLT_PFMET110_PFMHT110_IDTight;
          HLT_PFMET120_PFMHT120               = HLT_PFMET120_PFMHT120_IDTight;
          //HLT_Ele                             = HLT_Ele24_eta2p1_WPLoose_Gsf;
          //HLT_Mu                              = HLT_IsoMu24;
          HLT_Ele                             = HLT_Ele25_eta2p1_WPLoose_Gsf;
          HLT_Mu                              = HLT_IsoMu27;
          METFilters                          = Flag_METFilters;
          HBHENoiseFilter                     = Flag_HBHENoiseFilter;
          HBHENoiseIsoFilter                  = Flag_HBHENoiseIsoFilter;
          eeBadScFilter                       = Flag_eeBadScFilter;
          EcalDeadCellTriggerPrimitiveFilter  = Flag_EcalDeadCellTriggerPrimitiveFilter;
          goodVertices                        = Flag_goodVertices;
          globalTightHalo2016Filter           = Flag_globalTightHalo2016Filter;
          badMuonMoriond2017                  = Flag_badMuonMoriond2017;
          badCloneMuonMoriond2017             = Flag_badCloneMuonMoriond2017;
          badMuonFilter                       = Flag_badMuonFilter;
          badChargedHadronFilter              = Flag_badChargedHadronFilter;

          // Filter Efficiency
          if(filterEfficiencyH != nullptr)
          {
            auto theBin = filterEfficiencyH->FindBin(genStopM, genNeutralinoM);
            filterEfficiency = filterEfficiencyH->GetBinContent(theBin);
          }
          else
            filterEfficiency = 1.0;

          if(swap)
          {
            auto tmp = Met.Value();
            Met = LepPt;
            LepPt = tmp;
            triggerEfficiency = 1.0f;
          }

          if(!process.isdata())
            weight = puWeight*XS*filterEfficiency*(genWeight/sumGenWeight)*triggerEfficiency*EWKISRweight*ISRweight*leptonIDSF*leptonISOSF*leptonFullFastSF*Q2Var*bTagSF;
          else
            weight = 1.0f;

          if(isLooseNotTight)
          {
            auto efficiency = getLeptonTightLooseRatioSys(LepID, LepPt, LepEta);
            looseNotTightWeight = efficiency/(1-efficiency);
            weight *= looseNotTightWeight;
          }
          else
          {
            looseNotTightWeight = 1;
          }

          //if(doSync)
          if(doSync && HT.Value() > 200 && Jet1Pt.Value() > 100 && Met.Value() > 200)
          {
            if(sync_count < max_sync_count)
            {
              SyFile << "Run:LS:Ev " << run << ":" << lumi << ":" << evt << std::endl;
              SyFile << "   Mstop: " << genStopM << "; Mlsp: " << genNeutralinoM << std::endl;
              SyFile << "   HT: " << HT.Value() << "; MET: " << Met.Value() << std::endl;
              SyFile << "   Njet: " << Njet.Value() << std::endl;
              for(size_t i = 0; i < validJets.Value().size(); ++i)
              {
                auto jet = validJets.Value()[i];
                SyFile << "   jet " << i+1 << ":  pT: " << Jet_pt[jet] << "; eta: " << Jet_eta[jet] << "; raw pT: " << Jet_rawPt[jet] << "; ID: " << Jet_id[jet] << "; abs(eta): " << std::abs(Jet_eta[jet]) << std::endl;
              }
              SyFile << "   Nlep: " << validLeptons.size() << " ( e - " << nGoodEl << "; mu - " << nGoodMu << ")" << std::endl;
              SyFile << "   leading lepton:  pT: " << LepPt << "; eta: " << LepEta << "; phi: " << lep_phi << "; PDG ID: " << LepID << "; RelIso: " << LepIso03 << "; dxy: " << LepDxy << "; dz: " << LepDz << std::endl;
              SyFile << "   Delta Phi Jet1 Jet2: " << DPhiJet1Jet2.Value() << std::endl;
              SyFile << "   weight for 10 fb-1: " << weight.Value()*10000 << " (without SFs: " << 10000*XS*filterEfficiency*(genWeight/sumGenWeight) << ")" << std::endl;
              SyFile << "   passed: ";
              if(HT.Value() > 200 && Met.Value() > 200 && Jet1Pt.Value() > 90) // TODO: Probably change these cuts to reflect newest selection
              {
                SyFile << "Cut0";
                bool passCut1 = false;
                if(validLeptons.size() == 1)
                  passCut1 = true;
                if(validLeptons.size() == 2)
                {
                  if(LepGood_pt[validLeptons[1]] < SECOND_LEPTON_PT)
                    passCut1 = true;
                }

                if(passCut1 && LepPt < 30)
                {
                  SyFile << ";Cut1";

                  if(Jet1Pt.Value() > ISR_JET_PT)
                  {
                    SyFile << ";Cut2";

                    if(DPhiJet1Jet2.Value() < 2.5 || Jet2Pt.Value() < 60)
                    {
                      SyFile << ";Cut3";

                      if(Met.Value() > 280)
                        SyFile << ";Cut4";
                    }
                  }
                }
              }
              else
                SyFile << "None";

              //Printout detailed info (for a specific event for instance)
              //if(lumi == 91151 && evt == 195485531)
              if(true)
              {
                SyFile << std::endl << "nLepGood: " << nLepGood << std::endl;
                SyFile << "LepGood:" << std::endl;
                for(int i = 0; i < nLepGood; ++i)
                  SyFile << "   lep " << i+1 << ": ID: " << LepGood_pdgId[i] << "; pt: " << LepGood_pt[i] << "; eta:" << LepGood_eta[i] << "; phi: " << LepGood_phi[i] << "; relIso03: " << LepGood_relIso03[i] << "; pdgID: " << LepGood_pdgId[i] << "; dxy: " << LepGood_dxy[i] << "; dz: " << LepGood_dz[i] << std::endl;
                SyFile << "nJetGood: " << nJetIn << std::endl;
                for(int i = 0; i < nJetIn; ++i)
                  SyFile << "   jet " << i+1 << ": pt: " << Jet_pt[i] << "; eta: " << Jet_eta[i] << "; phi: " << Jet_phi[i] << "; mass: " << Jet_mass[i] << "; ID: " << Jet_id[i] << std::endl;
              }

              SyFile << std::endl << std::endl;

              ++sync_count;
            }
          }

          // Compute selection cuts
          bool metRequirement = Met.Value() > 300;
          bool htRequirement = HT.Value() > 200;
          bool jetRequirement = Jet1Pt.Value() > ISR_JET_PT;
          bool antiQCDRequirement = DPhiJet1Jet2.Value() < 2.5 || Jet2Pt.Value() < 60;
          bool leptonRequirement = isTight;
          bool deltaMRequirement = LepPt < 30;

          // Fill counters for parallel selection computation
          ++Ncut0;
          if(metRequirement)
          {
            ++Ncut1;
            if(htRequirement)
            {
              ++Ncut2;
              if(jetRequirement)
              {
                ++Ncut3;
                if(antiQCDRequirement)
                {
                  ++Ncut4;
                  if(leptonRequirement)
                  {
                    ++Ncut5;
                    if(deltaMRequirement)
                    {
                      ++Ncut6;
                    }
                  }
                }
              }
            }
          }

          if(!noSkim)
          {
            // If we are doing the loose not tight category, reject events that do not fit the category
            if(looseNotTight && !isLooseNotTight)
              continue;

            // Special check in case we are swapping MET and LepPt
            if(swap && LepPt < 5)
              continue;

            // Only keep events whose leptons pass our requirements
            if(looseNotTight && !isLooseNotTight)
              continue;

            if(!doLooseLeptons && isLooseNotTight)
              continue;

            if(!(isLoose || isTight))
              continue;

            // No need to keep events without jets
            if(!static_cast<bool>(validJets.size() != 0))
              continue;

            // So-called ISR jet requirement (even though it's on the pT of the leading jet)
            if(!static_cast<bool>(Jet1Pt >= ISR_JET_PT))
              continue;

            // Cut to help reduce the QCD background
            if(!static_cast<bool>(DPhiJet1Jet2 < 2.5 || Jet2Pt < 60))
              continue;

            // Minimal requirement on MET
            if(!static_cast<bool>(Met >= MIN_MET))
              continue;

            // MET filters
            //if ( METFilters                         != 1 ) continue;
            if ( HBHENoiseFilter                    != 1 ) continue;
            if ( HBHENoiseIsoFilter                 != 1 ) continue;
            if ( EcalDeadCellTriggerPrimitiveFilter != 1 ) continue;
            if ( goodVertices                       != 1 ) continue;
            if (process.isdata())
              if ( eeBadScFilter                      != 1 ) continue;
            if (!process.issignal())
              if ( globalTightHalo2016Filter          != 1 ) continue;
            if ( badMuonFilter                      != 1 ) continue; // Should probably only use 1 of these two
            //if ( badMuonMoriond2017                 != 1 ) continue;
            if ( badCloneMuonMoriond2017            != 1 ) continue; // This one removes some of the spikes in QCD for some reason
            if ( badChargedHadronFilter             != 1 ) continue;

            bool passHLT = false;
            if(!swap) // Normally use the MET triggers, but only for data
            {
              if(process.isdata())
              {
                if(HLT_PFMET90_PFMHT90_IDTight != 0)
                  passHLT = true;
                if(HLT_PFMET100_PFMHT100_IDTight != 0)
                  passHLT = true;
                if(HLT_PFMET110_PFMHT110_IDTight != 0)
                  passHLT = true;
                if(HLT_PFMET120_PFMHT120_IDTight != 0)
                  passHLT = true;
              }
              else
                passHLT = true;
            }
            else // If swapping MET and LepPt, apply single lepton triggers to data and MC, for data avoid double counting
            {
              if(HLT_IsoMu27 != 0)
                passHLT = true;
              if(HLT_Ele25_eta2p1_WPLoose_Gsf != 0)
                passHLT = true;

              // Remove double counting by removing from the electron PD the events with the muon HLT
              if(sample.tag().find("SingleEl") != std::string::npos && process.isdata())
              {
                if(HLT_IsoMu27 != 0)
                  passHLT = false;
              }
            }

            if(!passHLT)
              continue;
          }

          bdttree->Fill();
        }
        std::cout << std::endl;
        if(process.selection() != "")
          delete inputtree;
      }
      foutput.cd();
      bdttree->Write("",TObject::kOverwrite);

      TVectorD v(7);
      v[0] = Ncut0;
      v[1] = Ncut1;
      v[2] = Ncut2;
      v[3] = Ncut3;
      v[4] = Ncut4;
      v[5] = Ncut5;
      v[6] = Ncut6;
      v.Write("ParellelSelection");
      //puDistrib.Write();

      if(filterEfficiencyH != nullptr)
        delete filterEfficiencyH;
    }

    delete puWeightDistrib;
    if(puWeightDistribUp != nullptr)
    {
      delete puWeightDistribUp;
      delete puWeightDistribDown;
    }
  }

  std::cout << "Done!" << std::endl << std::endl;

  return 0;
}

void printHelp()
{
  std::cout << "The program has the following parameters:" << std::endl;
  std::cout << "\t--help\t- Print this message" << std::endl;
  std::cout << "\t--json\t- Used to specify the json file which describes the sample, this option is obligatory. The paraeter immediately after this one should be the desired json file" << std::endl;
  std::cout << "\t--outDir\t- Used to specify the output directory. The parameter immediately after this one should be the desired directory. By default the directory './OUT/' is used" << std::endl;
  std::cout << "\t--doSync\t- Whether a txt file with the event output should be created for synchronization purposes" << std::endl;
  std::cout << "\t--suffix\t- Suffix to add at the end of the name of the output files." << std::endl;

  return;
}
