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

#include "UserCode/Stop4Body-nanoAOD/interface/json.hpp"
#include "UserCode/Stop4Body-nanoAOD/interface/SampleReader.h"
#include "UserCode/Stop4Body-nanoAOD/interface/commonFunctions.h"
#include "UserCode/Stop4Body-nanoAOD/interface/doubleWithUncertainty.h"
#include "UserCode/Stop4Body-nanoAOD/interface/ValueWithSystematics.h"
#include "UserCode/Stop4Body-nanoAOD/interface/LHEweightMAPs.h"

#include "CondFormats/BTauObjects/interface/BTagCalibration.h"
#include "CondTools/BTau/interface/BTagCalibrationReader.h"

#define GENPART_LIMIT    400
#define JETCOLL_LIMIT    200
#define LEPCOLL_LIMIT    40
#define TAUCOLL_LIMIT    40
#define TRACOLL_LIMIT    200
#define LHEWEIGHT_LIMIT  9

#define ISR_JET_PT       70
#define SECOND_LEPTON_PT 20
//#define MIN_MET          100
//#define MIN_MET          0

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
const double DR_CutOff = 0.4;

// 2018 histograms

extern TH2D* centralElectronRecoSFHist2018;
extern TH2D* centralElectronSFHist2018;
extern TH2D* TnPlowPtIDSFHist2018;
extern TH2D* TnPElectronISOIPSFHist2018;
extern TH2D* elFullFastIDSFHist2018;
//extern TH2D* elFullFastHIIPSFHist2018;
extern TH2D* centralMuonSFHist2018;
extern TH2D* lowMuonSFHist2018;
extern TH2D* TnPMuonISOIPSFHist2018;

extern TH2D* muFullFastIDSFHist2018;
extern TH1D* genFilterHist2018;
//extern TH2D* muFullFastHIIPSFHist2018;

extern TH1D* weightsSt_2018;
extern TH2D* weightsSTandHT_2018;
extern TH2D* weightsSTandJet1Pt_2018;

extern TH1D* WJetsToLNu_HT100to200_gen_WptHist2018;
extern TH1D* WJetsToLNu_HT200to400_gen_WptHist2018;
extern TH1D* WJetsToLNu_HT400to600_gen_WptHist2018;
extern TH1D* WJetsToLNu_HT600to800_gen_WptHist2018;
extern TH1D* WJetsToLNu_HT800to1200_gen_WptHist2018;
extern TH1D* WJetsToLNu_HT1200to2500_gen_WptHist2018;
extern TH1D* WJetsToLNu_HT2500toInf_gen_WptHist2018;

extern TH1D* electronTightToLoose_2018_LowEta;
extern TH1D* electronTightToLoose_2018_HighEta;
extern TH1D* muonTightToLoose_2018_LowEta;
extern TH1D* muonTightToLoose_2018_HighEta;

// 2017 histograms

extern TH2D* centralElectronRecoSFHist2017;
extern TH2D* centralElectronRecoSFHist2017_lowEt;
extern TH2D* centralElectronSFHist2017;
extern TH2D* TnPlowPtIDSFHist2017;
extern TH2D* TnPElectronISOIPSFHist2017;
extern TH2D* elFullFastIDSFHist2017;
// extern TH2D* elFullFastHIIPSFHist2017;
extern TH2D* centralMuonSFHist2017;
extern TH2D* lowMuonSFHist2017;
extern TH2D* TnPMuonISOIPSFHist2017;

extern TH2D* muFullFastIDSFHist2017;
extern TH1D* genFilterHist2017;
// extern TH2D* muFullFastHIIPSFHist2017;

extern TH1D* WJetsToLNu_HT100to200_gen_WptHist2017;
extern TH1D* WJetsToLNu_HT200to400_gen_WptHist2017;
extern TH1D* WJetsToLNu_HT400to600_gen_WptHist2017;
extern TH1D* WJetsToLNu_HT600to800_gen_WptHist2017;
extern TH1D* WJetsToLNu_HT800to1200_gen_WptHist2017;
extern TH1D* WJetsToLNu_HT1200to2500_gen_WptHist2017;
extern TH1D* WJetsToLNu_HT2500toInf_gen_WptHist2017;

extern TH2D* L1prefiring_jetpt_2017BtoFHist;

extern TH1D* electronTightToLoose_2017_LowEta;
extern TH1D* electronTightToLoose_2017_HighEta;
extern TH1D* muonTightToLoose_2017_LowEta;
extern TH1D* muonTightToLoose_2017_HighEta;

extern TH1D* mcClosure_electronTightToLoose_2017_LowEta;
extern TH1D* mcClosure_electronTightToLoose_2017_HighEta;
extern TH1D* mcClosure_muonTightToLoose_2017_LowEta;
extern TH1D* mcClosure_muonTightToLoose_2017_HighEta;

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

extern TH1D* weightsSt_2017;
extern TH1D* weightsDeepCSV_2017;

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
  double MIN_MET = 100;
  double genJetPtThreshold = 20;
  bool overrideXSec = false;
  bool swap = false;
  bool looseNotTight = false;
  bool doFakeClosure = false;
  bool preemptiveDropEvents = false;
  bool doLooseLeptons = false;
  bool doPromptTagging = false;
  bool doJetHT = false;
  bool doSingleEl = false;
  bool noTrim = false;
  int part = -1;
  int year = 0;
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

    if(argument == "--doFakeClosure")
      doFakeClosure = true;

    if(argument == "--preemptiveDropEvents")
      preemptiveDropEvents = true;

    if(argument == "--doLooseLeptons")
      doLooseLeptons = true;

    if(argument == "--doPromptTagging")
      doPromptTagging = true;

    if(argument == "--doJetHT")
      doJetHT = true;

    if(argument == "--doSingleEl")
      doSingleEl = true;

    if(argument == "--bTagCalibrationFile")
      bTagCalibrationFile = argv[++i];

    if(argument == "--part")
    {
      std::stringstream converter;
      converter << argv[++i];
      converter >> part;
    }
    if(argument == "--year")
    {
      std::stringstream converter;
      converter << argv[++i];
      converter >> year;
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

  if(doJetHT || doSingleEl)
  {
    MIN_MET = 0;
  }

  // CSVv2 for 2016
  double CSV_Loose = 0.5426;
  double CSV_Medium = 0.800;
  double CSV_Tight = 0.935;

  if(bTagCalibrationFile == ""){
    if (year==2017)
    {
      // CSVv2 for 2017
      /*
      bTagCalibrationFile = "../data/CSVv2_94XSF_V2_B_F.csv";
      CSV_Loose = 0.5803;
      CSV_Medium = 0.8838;
      CSV_Tight = 0.9693;
      */
      // DeepCSV for 2017
      //
      bTagCalibrationFile = "../data/DeepCSV_94XSF_V5_B_F.csv";
      CSV_Loose = 0.1522;
      CSV_Medium = 0.4941;
      CSV_Tight = 0.8001;
      // 
      // DeepFlavour for 2017
      /*
      bTagCalibrationFile = "../data/DeepFlavour_94XSF_V4_B_F.csv";
      CSV_Loose = 0.0521;
      CSV_Medium = 0.3033;
      CSV_Tight = 0.7489;
      */
    }
    else if (year==2018)
    {
      // DeepCSV for 2018
      //
      bTagCalibrationFile = "../data/DeepCSV_102XSF_V2.csv";
      CSV_Loose = 0.1241;
      CSV_Medium = 0.4184;
      CSV_Tight = 0.7527;
      //
      // DeepFlavour for 2018
      /*
      bTagCalibrationFile = "../data/DeepJet_102XSF_V2.csv";
      CSV_Loose = 0.0494;
      CSV_Medium = 0.2770;
      CSV_Tight = 0.7264;
      */
    }
    else{
      bTagCalibrationFile = "../data/CSVv2_Moriond17_B_H.csv";
    }
  }
  // For quicker DEBUGGING or to run FakeRate because we don't use b-tag SFs
  //bTagCalibrationFile = "../data/CSVv2_Moriond17_B_H.csv";
  if(doJetHT || doSingleEl){
    bTagCalibrationFile = "../data/CSVv2_Moriond17_B_H.csv";
  }

  std::cout << "Year: " << year << std::endl;
  std::cout << "bTagCalib file: " << bTagCalibrationFile << std::endl;

  if(!fileExists(outputDirectory+"/preProcessSamples.root"))
  {
    std::cout << "You must first obtain the preProcessSamples file before trying to process the samples" << std::endl;
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
  TFile preProcessSamplesFile((outputDirectory + "/preProcessSamples.root").c_str(), "READ");
  TFile lheScaleFile((lheScaleDir + "/lheWeights.root").c_str(), "READ");

  // HISTS FOR 2018

  // - RECO SFs Et 10-500
  TFile centralElectronRecoSFFile2018("../data/egammaEffi.txt_EGM2D_updatedAll.root", "READ");
  centralElectronRecoSFHist2018 = static_cast<TH2D*>(centralElectronRecoSFFile2018.Get("EGamma_SF2D"));

  // - Centraly produced SFs for Ele ID 10-500 GeV
  TFile centralElectronSFFile2018("../data/ElectronScaleFactors_Run2018.root", "READ");
  centralElectronSFHist2018 = static_cast<TH2D*>(centralElectronSFFile2018.Get("Run2018_CutBasedLooseNoIso94XV2"));

  // - Analysis specific SFs for Ele ID low-pt 5-20 GeV
  TFile TnPlowPtIDSFFile2018("../data/egammaEffi.txt_EGM2D_2018_TnP_lowPt_ID.root", "READ");
   TnPlowPtIDSFHist2018 = static_cast<TH2D*>(TnPlowPtIDSFFile2018.Get("EGamma_SF2D"));

  // - Analysis specific SFs for Ele ISO+IP
  TFile TnPElectronISOIPSFFile2018("../data/egammaEffi.txt_IPISO_2018_EGM2D.root", "READ");
  TnPElectronISOIPSFHist2018 = static_cast<TH2D*>(TnPElectronISOIPSFFile2018.Get("EGamma_SF2D"));

  // El Full/Fast Sim SFs
  TFile elFullFastSFFile2018("../data/detailed_ele_full_fast_sf_18.root", "READ");
  elFullFastIDSFHist2018 = static_cast<TH2D*>(elFullFastSFFile2018.Get("CutBasedLooseNoIso94XV2_sf"));
  //elFullFastHIIPSFHist2018 = static_cast<TH2D*>(elFullFastSFFile2018.Get(""));

  // Signal Gen Filter Efficiency
  TFile genFilterFile2018("../data/filterEffs_SMS_T2tt_dM_10to80_genHT_160_genMET_80_mWMin_0p1_2018.root", "READ");
  genFilterHist2018 = static_cast<TH1D*>(genFilterFile2018.Get("hFilterEff"));

  // - Centraly produced SFs for Mu ID 20-120 GeV
  TFile centralMuonSFFile2018("../data/MuonScaleFactors_ID_Run2018.root", "READ");
  centralMuonSFHist2018 = static_cast<TH2D*>(centralMuonSFFile2018.Get("NUM_MediumID_DEN_TrackerMuons_pt_abseta"));

  // - Centraly produced SFs for Mu ID low-pt 2-50 GeV
  TFile lowMuonSFFile2018("../data/RunABCD_SF_ID_JPsi_syst.root", "READ");
  lowMuonSFHist2018 = static_cast<TH2D*>(lowMuonSFFile2018.Get("NUM_MediumID_DEN_genTracks_pt_abseta"));

  // - Analysis specific SFs for Mu ISO/IP 3.5-500 GeV TODO
  TFile TnPMuonISOIPSFFile2018("../data/2018_NUM_S4BISO_DEN_MediumID_abseta_pt.root", "READ");
  TCanvas* tnpMuIso18c = static_cast<TCanvas*>(TnPMuonISOIPSFFile2018.Get("cNUM_S4BISO_DEN_MediumID_abseta_pt"));
  TnPMuonISOIPSFHist2018 = static_cast<TH2D*>(tnpMuIso18c->GetPrimitive("NUM_S4BISO_DEN_MediumID_abseta_pt"));  

  // Mu Full/Fast Sim SFs
  TFile muFullFastSFFile2018("../data/detailed_mu_full_fast_sf_18.root", "READ");
  muFullFastIDSFHist2018 = static_cast<TH2D*>(muFullFastSFFile2018.Get("miniIso02_MediumId_sf"));
  //muFullFastHIIPSFHist2018 = static_cast<TH2D*>(muFullFastSFFile2018.Get(""));

  TFile wSt2018("../data/2018_weights_St.root", "READ");
  weightsSt_2018 = static_cast<TH1D*>(wSt2018.Get("wrat"));

  TFile wSTandHT2018("../data/2018_weightsSTandHT.root", "READ");
  weightsSTandHT_2018 = static_cast<TH2D*>(wSTandHT2018.Get("wrat"));

  TFile wSTandJet1Pt2018("../data/2018_weightsSTandJet1Pt.root", "READ");
  weightsSTandJet1Pt_2018 = static_cast<TH2D*>(wSTandJet1Pt2018.Get("wrat"));

  TFile tightToLooseRatios2018("../data/tightToLooseRatios_2018.root", "READ");
  electronTightToLoose_2018_LowEta = static_cast<TH1D*>(tightToLooseRatios2018.Get("tightToLooseRatios_electron_LepPt_LowEta"));
  electronTightToLoose_2018_HighEta = static_cast<TH1D*>(tightToLooseRatios2018.Get("tightToLooseRatios_electron_LepPt_HighEta"));
  muonTightToLoose_2018_LowEta = static_cast<TH1D*>(tightToLooseRatios2018.Get("tightToLooseRatios_muon_LepPt_LowEta"));
  muonTightToLoose_2018_HighEta = static_cast<TH1D*>(tightToLooseRatios2018.Get("tightToLooseRatios_muon_LepPt_HighEta"));
  /*
  mcClosure_electronTightToLoose_2017_LowEta = static_cast<TH1D*>(tightToLooseRatios2017.Get("mcClosure_tightToLooseRatios_2017_electron_LepPt_LowEta"));
  mcClosure_electronTightToLoose_2017_HighEta = static_cast<TH1D*>(tightToLooseRatios2017.Get("mcClosure_tightToLooseRatios_2017_electron_LepPt_HighEta"));
  mcClosure_muonTightToLoose_2017_LowEta = static_cast<TH1D*>(tightToLooseRatios2017.Get("mcClosure_tightToLooseRatios_2017_muon_LepPt_LowEta"));
  mcClosure_muonTightToLoose_2017_HighEta = static_cast<TH1D*>(tightToLooseRatios2017.Get("mcClosure_tightToLooseRatios_2017_muon_LepPt_HighEta"));
  */
  
  TFile WJetsToLNu_HT100to200_gen_WptFile2018("../data/2018_sample_WJetsToLNu_HT100to200_gen_Wpt.root","READ");
  WJetsToLNu_HT100to200_gen_WptHist2018 = static_cast<TH1D*>(WJetsToLNu_HT100to200_gen_WptFile2018.Get("wrat"));  
  TFile WJetsToLNu_HT200to400_gen_WptFile2018("../data/2018_sample_WJetsToLNu_HT200to400_gen_Wpt.root","READ");
  WJetsToLNu_HT200to400_gen_WptHist2018 = static_cast<TH1D*>(WJetsToLNu_HT200to400_gen_WptFile2018.Get("wrat"));  
  TFile WJetsToLNu_HT400to600_gen_WptFile2018("../data/2018_sample_WJetsToLNu_HT400to600_gen_Wpt.root","READ");
  WJetsToLNu_HT400to600_gen_WptHist2018 = static_cast<TH1D*>(WJetsToLNu_HT400to600_gen_WptFile2018.Get("wrat"));  
  TFile WJetsToLNu_HT600to800_gen_WptFile2018("../data/2018_sample_WJetsToLNu_HT600to800_gen_Wpt.root","READ");
  WJetsToLNu_HT600to800_gen_WptHist2018 = static_cast<TH1D*>(WJetsToLNu_HT600to800_gen_WptFile2018.Get("wrat"));  
  TFile WJetsToLNu_HT800to1200_gen_WptFile2018("../data/2018_sample_WJetsToLNu_HT800to1200_gen_Wpt.root","READ");
  WJetsToLNu_HT800to1200_gen_WptHist2018 = static_cast<TH1D*>(WJetsToLNu_HT800to1200_gen_WptFile2018.Get("wrat"));  
  TFile WJetsToLNu_HT1200to2500_gen_WptFile2018("../data/2018_sample_WJetsToLNu_HT1200to2500_gen_Wpt.root","READ");
  WJetsToLNu_HT1200to2500_gen_WptHist2018 = static_cast<TH1D*>(WJetsToLNu_HT1200to2500_gen_WptFile2018.Get("wrat"));  
  TFile WJetsToLNu_HT2500toInf_gen_WptFile2018("../data/2018_sample_WJetsToLNu_HT2500toInf_gen_Wpt.root","READ");
  WJetsToLNu_HT2500toInf_gen_WptHist2018 = static_cast<TH1D*>(WJetsToLNu_HT2500toInf_gen_WptFile2018.Get("wrat"));

  // HISTS FOR 2017

  // - El RECO SFs Et 20-500
  TFile centralElectronRecoSFFile2017("../data/egammaEffi.txt_EGM2D_runBCDEF_passingRECO.root", "READ");
  centralElectronRecoSFHist2017 = static_cast<TH2D*>(centralElectronRecoSFFile2017.Get("EGamma_SF2D"));

  // - El RECO SFs Et 10-20
  TFile centralElectronRecoSFFile2017_lowEt("../data/egammaEffi.txt_EGM2D_runBCDEF_passingRECO_lowEt.root", "READ");
  centralElectronRecoSFHist2017_lowEt = static_cast<TH2D*>(centralElectronRecoSFFile2017_lowEt.Get("EGamma_SF2D"));

  // - Centraly produced SFs for Ele ID 10-500 GeV
  TFile centralElectronSFFile2017("../data/ElectronScaleFactors_Run2017.root", "READ");
  centralElectronSFHist2017 = static_cast<TH2D*>(centralElectronSFFile2017.Get("Run2017_CutBasedLooseNoIso94XV2"));

  // - Analysis specific SFs for Ele ID low-pt 5-20 GeV
  TFile TnPlowPtIDSFFile2017("../data/egammaEffi.txt_EGM2D_2017_TnP_lowPt_ID.root", "READ");
  TnPlowPtIDSFHist2017 = static_cast<TH2D*>(TnPlowPtIDSFFile2017.Get("EGamma_SF2D"));

  // - Analysis specific SFs for Ele ISO/IP 5-500 GeV
  TFile TnPElectronISOIPSFFile2017("../data/egammaEffi.txt_IPISO_2017_EGM2D.root", "READ");
  TnPElectronISOIPSFHist2017 = static_cast<TH2D*>(TnPElectronISOIPSFFile2017.Get("EGamma_SF2D"));

  // El Full/Fast Sim SFs
  TFile elFullFastSFFile2017("../data/detailed_ele_full_fast_sf_17.root", "READ");
  elFullFastIDSFHist2017 = static_cast<TH2D*>(elFullFastSFFile2017.Get("CutBasedLooseNoIso94XV2_sf"));
  //elFullFastHIIPSFHist2017 = static_cast<TH2D*>(elFullFastSFFile2017.Get(""));

  // Signal Gen Filter Efficiency
  TFile genFilterFile2017("../data/filterEffs_SMS_T2tt_dM_10to80_genHT_160_genMET_80_mWMin_0p1_2017.root", "READ");
  genFilterHist2017 = static_cast<TH1D*>(genFilterFile2017.Get("hFilterEff"));

  // - Centraly produced SFs for Mu ID 20-120 GeV
  TFile centralMuonSFFile2017("../data/MuonScaleFactors_ID_Run2017.root", "READ");
  centralMuonSFHist2017 = static_cast<TH2D*>(centralMuonSFFile2017.Get("NUM_MediumID_DEN_genTracks_pt_abseta"));

  // - Centraly produced SFs for Mu ID low-pt 2-50 GeV
  TFile lowMuonSFFile2017("../data/RunBCDEF_SF_ID_JPsi_syst.root", "READ");
  lowMuonSFHist2017 = static_cast<TH2D*>(lowMuonSFFile2017.Get("NUM_MediumID_DEN_genTracks_pt_abseta"));

  // - Analysis specific SFs for Mu ISO/IP 3.5-500 GeV
  TFile TnPMuonISOIPSFFile2017("../data/2017_NUM_S4BISO_DEN_MediumID_abseta_pt.root", "READ");
  TCanvas* tnpMuIso17c = static_cast<TCanvas*>(TnPMuonISOIPSFFile2017.Get("cNUM_S4BISO_DEN_MediumID_abseta_pt"));
  TnPMuonISOIPSFHist2017 = static_cast<TH2D*>(tnpMuIso17c->GetPrimitive("NUM_S4BISO_DEN_MediumID_abseta_pt"));

  // Mu Full/Fast Sim SFs
  TFile muFullFastSFFile2017("../data/detailed_mu_full_fast_sf_17.root", "READ");
  muFullFastIDSFHist2017 = static_cast<TH2D*>(muFullFastSFFile2017.Get("miniIso02_MediumId_sf"));
  //muFullFastHIIPSFHist2017 = static_cast<TH2D*>(muFullFastSFFile2017.Get(""));

  // L1prefire issue on 2017 Data
  TFile L1prefiring_jetpt_2017BtoFFile("../data/L1prefiring_jetpt_2017BtoF.root");
  L1prefiring_jetpt_2017BtoFHist = static_cast<TH2D*>(L1prefiring_jetpt_2017BtoFFile.Get("L1prefiring_jetpt_2017BtoF"));

  TFile wSt2017("../data/2017_weights_St.root", "READ");
  weightsSt_2017 = static_cast<TH1D*>(wSt2017.Get("wrat"));

  TFile wDeepCSV2017("../data/weights_JetHBDeepCSV.root", "READ");
  weightsDeepCSV_2017 = static_cast<TH1D*>(wDeepCSV2017.Get("wrat"));

  // Tight to Loose ratio weight for the fake lepton method prediction
  TFile tightToLooseRatios2017("../data/tightToLooseRatios_2017.root", "READ");
  electronTightToLoose_2017_LowEta = static_cast<TH1D*>(tightToLooseRatios2017.Get("tightToLooseRatios_electron_LepPt_LowEta"));
  electronTightToLoose_2017_HighEta = static_cast<TH1D*>(tightToLooseRatios2017.Get("tightToLooseRatios_electron_LepPt_HighEta"));
  muonTightToLoose_2017_LowEta = static_cast<TH1D*>(tightToLooseRatios2017.Get("tightToLooseRatios_muon_LepPt_LowEta"));
  muonTightToLoose_2017_HighEta = static_cast<TH1D*>(tightToLooseRatios2017.Get("tightToLooseRatios_muon_LepPt_HighEta"));

  mcClosure_electronTightToLoose_2017_LowEta = static_cast<TH1D*>(tightToLooseRatios2017.Get("mcClosure_tightToLooseRatios_2017_electron_LepPt_LowEta"));
  mcClosure_electronTightToLoose_2017_HighEta = static_cast<TH1D*>(tightToLooseRatios2017.Get("mcClosure_tightToLooseRatios_2017_electron_LepPt_HighEta"));
  mcClosure_muonTightToLoose_2017_LowEta = static_cast<TH1D*>(tightToLooseRatios2017.Get("mcClosure_tightToLooseRatios_2017_muon_LepPt_LowEta"));
  mcClosure_muonTightToLoose_2017_HighEta = static_cast<TH1D*>(tightToLooseRatios2017.Get("mcClosure_tightToLooseRatios_2017_muon_LepPt_HighEta"));

  TFile WJetsToLNu_HT100to200_gen_WptFile2017("../data/2017_sample_WJetsToLNu_HT100to200_gen_Wpt.root","READ");
  WJetsToLNu_HT100to200_gen_WptHist2017 = static_cast<TH1D*>(WJetsToLNu_HT100to200_gen_WptFile2017.Get("wrat"));  
  TFile WJetsToLNu_HT200to400_gen_WptFile2017("../data/2017_sample_WJetsToLNu_HT200to400_gen_Wpt.root","READ");
  WJetsToLNu_HT200to400_gen_WptHist2017 = static_cast<TH1D*>(WJetsToLNu_HT200to400_gen_WptFile2017.Get("wrat"));  
  TFile WJetsToLNu_HT400to600_gen_WptFile2017("../data/2017_sample_WJetsToLNu_HT400to600_gen_Wpt.root","READ");
  WJetsToLNu_HT400to600_gen_WptHist2017 = static_cast<TH1D*>(WJetsToLNu_HT400to600_gen_WptFile2017.Get("wrat"));  
  TFile WJetsToLNu_HT600to800_gen_WptFile2017("../data/2017_sample_WJetsToLNu_HT600to800_gen_Wpt.root","READ");
  WJetsToLNu_HT600to800_gen_WptHist2017 = static_cast<TH1D*>(WJetsToLNu_HT600to800_gen_WptFile2017.Get("wrat"));  
  TFile WJetsToLNu_HT800to1200_gen_WptFile2017("../data/2017_sample_WJetsToLNu_HT800to1200_gen_Wpt.root","READ");
  WJetsToLNu_HT800to1200_gen_WptHist2017 = static_cast<TH1D*>(WJetsToLNu_HT800to1200_gen_WptFile2017.Get("wrat"));  
  TFile WJetsToLNu_HT1200to2500_gen_WptFile2017("../data/2017_sample_WJetsToLNu_HT1200to2500_gen_Wpt.root","READ");
  WJetsToLNu_HT1200to2500_gen_WptHist2017 = static_cast<TH1D*>(WJetsToLNu_HT1200to2500_gen_WptFile2017.Get("wrat"));  
  TFile WJetsToLNu_HT2500toInf_gen_WptFile2017("../data/2017_sample_WJetsToLNu_HT2500toInf_gen_Wpt.root","READ");
  WJetsToLNu_HT2500toInf_gen_WptHist2017 = static_cast<TH1D*>(WJetsToLNu_HT2500toInf_gen_WptFile2017.Get("wrat"));

  // HISTS FOR 2016
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

  // B-tagging calibration

  //BTagCalibration bCalib("csvv2", bTagCalibrationFile);
  //BTagCalibration bCalib("deepcsv", bTagCalibrationFile);
  BTagCalibration bCalib("deepjet", bTagCalibrationFile);
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

  std::cout << "b-tag file loaded!" << std::endl;

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
    //Commented until it's fixed for nanoAOD properly

    TH1D* puWeightDistrib = nullptr;
    TH1D* puWeightDistribUp   = nullptr;
    TH1D* puWeightDistribDown = nullptr;

    if(!process.isdata()){
      puWeightDistrib = static_cast<TH1D*>(preProcessSamplesFile.Get( ("process_"+process.tag()+"_puWeight").c_str())->Clone("puWeightDistrib"));
      {
        auto tmp = preProcessSamplesFile.Get( ("process_"+process.tag()+"_puWeight_Up").c_str());
        if(tmp != nullptr)
        {
          puWeightDistribUp   = static_cast<TH1D*>(tmp->Clone("puWeightDistribUp"));
          tmp = preProcessSamplesFile.Get( ("process_"+process.tag()+"_puWeight_Down").c_str());
          puWeightDistribDown = static_cast<TH1D*>(tmp->Clone("puWeightDistribDown"));
        }
      }
    }

    if(!(process.isdata() || process.issignal() || process.tag()=="VV"))
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
      Float_t genStopM; bdttree->Branch("genStopM", &genStopM, "genStopM/F");
      Float_t genNeutralinoM; bdttree->Branch("genNeutralinoM", &genNeutralinoM, "genNeutralinoM/F");
      Float_t gen_Wpt;    bdttree->Branch("gen_Wpt",&gen_Wpt,"gen_Wpt/F");
      Float_t genSt_pt;   bdttree->Branch("genSt_pt",&genSt_pt,"genSt_pt/F");
      Float_t genSt_eta;  bdttree->Branch("genSt_eta",&genSt_eta,"genSt_eta/F");
      Float_t genSt_phi;  bdttree->Branch("genSt_phi",&genSt_phi,"genSt_phi/F");
      Float_t genSt_mass; bdttree->Branch("genSt_mass",&genSt_mass,"genSt_mass/F");

      std::cout << "\t    Creating branches for systematic variations:" << std::endl;
      std::cout << "\t      Creating systematic variation variables" << std::endl;
      ValueWithSystematics<float> triggerEfficiency;
      ValueWithSystematics<float> EWKISRweight;
      ValueWithSystematics<float> GENWweight;
      ValueWithSystematics<float> ISRweight;
      ValueWithSystematics<float> puWeight;
      ValueWithSystematics<float> leptonRecoSF;
      ValueWithSystematics<float> leptonIDSF;
      ValueWithSystematics<float> leptonISOSF;
      ValueWithSystematics<float> leptonFullFastSF;
      ValueWithSystematics<float> looseNotTightWeight;
      ValueWithSystematics<float> looseNotTightWeight2017MCClosure;
      ValueWithSystematics<float> l1prefireWeight;
      ValueWithSystematics<float> Q2Var;
      ValueWithSystematics<float> bTagSF;
      ValueWithSystematics<float> normStweight;
      ValueWithSystematics<float> normSTandHTweight;
      ValueWithSystematics<float> normSTandJet1Ptweight;
      ValueWithSystematics<float> weight;

      std::cout << "\t      Creating the variations if needed" << std::endl;
      if(!process.isdata())
      {
        // Performing dummy computations just so that the placeholders for all the uncertainties are created
        std::cout << "\t        trigger" << std::endl;
        triggerEfficiency = triggerEfficiencyFromMETSys(200.0,2017);
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

        //normStweight = normStweightSys(20.0);
        normStweight = 1;
        normStweight.Systematic("normStWeight_Up");
        normStweight.Systematic("normStWeight_Down");

        //normSTandHTweight = normSTandHTweightSys(0.5);
        normSTandHTweight = 1;
        normSTandHTweight.Systematic("normSTandHTweight_Up");
        normSTandHTweight.Systematic("normSTandHTweight_Down");

        //normSTandJet1Ptweight = normSTandJet1PtweightSys(0.5);
        normSTandJet1Ptweight = 1;
        normSTandJet1Ptweight.Systematic("normSTandJet1Ptweight_Up");
        normSTandJet1Ptweight.Systematic("normSTandJet1Ptweight_Down");

        std::cout << "\t        ISR" << std::endl;
        ISRweight = ISRweightFromNISRJetSys(2);
        GENWweight = getGenWptWeight(20, "WJetsToLNu_HT100to200", 2017);
        std::cout << "\t        pu" << std::endl;
        puWeight = 1.0f;
        puWeight.Systematic("PU_Up");
        puWeight.Systematic("PU_Down");
        std::cout << "\t        lRECO" << std::endl;
        leptonRecoSF = getLeptonRecoSFSys(13, 7.5, 1.1, 2017);
        std::cout << "\t        lID" << std::endl;
        leptonIDSF = getLeptonIDSFSys(13, 7.5, 1.1, 2017);
        std::cout << "\t        lISO" << std::endl;
        leptonISOSF = getLeptonISOSFSys(13, 20, 1.1, 2017);
        // Using 2016 ISO SFs
        //leptonISOSF = getLeptonISOSFSys(13, 7.5, 1.1, 2017);
        std::cout << "\t        FullFastSim" << std::endl;
        leptonFullFastSF = getFullFastSFSys(11, 20, 1.1, 2017);
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
        std::cout << "\t        l1preFire" << std::endl;
        l1prefireWeight = 1;
        l1prefireWeight.Systematic("L1prefireWeight_Up");
        l1prefireWeight.Systematic("L1prefireWeight_Down");
      }
      
      std::cout << "\t        looseNotTight" << std::endl;
      //looseNotTightWeight = getLeptonTightLooseRatioSys(11, 20, 1.1);
      looseNotTightWeight = getLeptonTightLooseRatioNanoAODSys(11, 20, 1.1, 2017);
      looseNotTightWeight2017MCClosure = getLeptonTightLooseRatio2017MCClosureSys(11, 20, 1.1);

      std::cout << "\t        weight" << std::endl;
      weight = puWeight * triggerEfficiency * normStweight * ISRweight * leptonRecoSF * leptonIDSF * leptonISOSF * leptonFullFastSF * looseNotTightWeight * Q2Var * bTagSF * l1prefireWeight;

      std::cout << "\t        locking" << std::endl;
      if(!process.isdata())
      {
        // Then lock the variables so that the placeholders are not removed or new ones are created
        triggerEfficiency.Lock();
        EWKISRweight.Lock();
        GENWweight.Lock();
        ISRweight.Lock();
        puWeight.Lock();
        leptonRecoSF.Lock();
        leptonIDSF.Lock();
        leptonISOSF.Lock();
        leptonFullFastSF.Lock();
        bTagSF.Lock();
        Q2Var.Lock();
        l1prefireWeight.Lock();
        normStweight.Lock();
        normSTandHTweight.Lock();
        normSTandJet1Ptweight.Lock();
      }
      looseNotTightWeight.Lock();
      looseNotTightWeight2017MCClosure.Lock();
      weight.Lock();


      triggerEfficiency = 1.0;
      EWKISRweight = 1.0;
      GENWweight = 1.0;
      ISRweight = 1.0;
      puWeight = 1.0;
      leptonRecoSF = 1.0;
      leptonIDSF = 1.0;
      leptonISOSF = 1.0;
      leptonFullFastSF = 1.0;
      looseNotTightWeight = 1.0;
      looseNotTightWeight2017MCClosure = 1.0;
      l1prefireWeight = 1.0;
      bTagSF = 1.0;
      Q2Var = 1.0;
      normStweight = 1.0;
      normSTandHTweight = 1.0;
      normSTandJet1Ptweight = 1.0;
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
      bdttree->Branch("GENWweight", &(GENWweight.Value()), "GENWweight/F");
      bdttree->Branch("ISRweight", &(ISRweight.Value()), "ISRweight/F");
      bdttree->Branch("puWeight", &(puWeight.Value()), "puWeight/F");
      bdttree->Branch("leptonRecoSF", &(leptonRecoSF.Value()), "leptonRecoSF/F");
      bdttree->Branch("leptonIDSF", &(leptonIDSF.Value()), "leptonIDSF/F");
      bdttree->Branch("leptonISOSF", &(leptonISOSF.Value()), "leptonISOSF/F");
      bdttree->Branch("leptonFullFastSF", &(leptonFullFastSF.Value()), "leptonFullFastSF/F");
      bdttree->Branch("bTagSF", &(bTagSF.Value()), "bTagSF/F");
      bdttree->Branch("weight", &(weight.Value()), "weight/F");
      bdttree->Branch("isLooseNotTight", &isLooseNotTight);
      bdttree->Branch("looseNotTightWeight", &looseNotTightWeight.Value(), "looseNotTightWeight/F");
      bdttree->Branch("looseNotTightWeight2017MCClosure", &looseNotTightWeight2017MCClosure.Value(), "looseNotTightWeight2017MCClosure/F");
      bdttree->Branch("l1prefireWeight", &l1prefireWeight.Value(), "l1prefireWeight/F");
      bdttree->Branch("normStweight", &(normStweight.Value()), "normStweight/F");
      bdttree->Branch("normSTandHTweight", &(normSTandHTweight.Value()), "normSTandHTweight/F");
      bdttree->Branch("normSTandJet1Ptweight", &(normSTandJet1Ptweight.Value()), "normSTandJet1Ptweight/F");


      if(!process.isdata() && noTrim)
      {
        for(auto& systematic: triggerEfficiency.Systematics())
          bdttree->Branch(("triggerEfficiency_"+systematic).c_str(), &(triggerEfficiency.Systematic(systematic)));
        for(auto& systematic: EWKISRweight.Systematics())
          bdttree->Branch(("EWKISRweight_"+systematic).c_str(), &(EWKISRweight.Systematic(systematic)));
        for(auto& systematic: GENWweight.Systematics())
          bdttree->Branch(("GENWweight_"+systematic).c_str(), &(GENWweight.Systematic(systematic)));
        for(auto& systematic: ISRweight.Systematics())
          bdttree->Branch(("ISRweight_"+systematic).c_str(), &(ISRweight.Systematic(systematic)));
        for(auto& systematic: puWeight.Systematics())
          bdttree->Branch(("puWeight_"+systematic).c_str(), &(puWeight.Systematic(systematic)));
        for(auto& systematic: leptonRecoSF.Systematics())
          bdttree->Branch(("leptonRecoSF_"+systematic).c_str(), &(leptonRecoSF.Systematic(systematic)));
        for(auto& systematic: leptonIDSF.Systematics())
          bdttree->Branch(("leptonIDSF_"+systematic).c_str(), &(leptonIDSF.Systematic(systematic)));
        for(auto& systematic: leptonISOSF.Systematics())
          bdttree->Branch(("leptonISOSF_"+systematic).c_str(), &(leptonISOSF.Systematic(systematic)));
        for(auto& systematic: leptonFullFastSF.Systematics())
          bdttree->Branch(("leptonFullFastSF_"+systematic).c_str(), &(leptonFullFastSF.Systematic(systematic)));
        for(auto& systematic: bTagSF.Systematics())
          bdttree->Branch(("bTagSF_"+systematic).c_str(), &(bTagSF.Systematic(systematic)));
        for(auto& systematic: normStweight.Systematics())
          bdttree->Branch(("normStweight_"+systematic).c_str(), &(normStweight.Systematic(systematic)));
        for(auto& systematic: normSTandHTweight.Systematics())
          bdttree->Branch(("normSTandHTweight_"+systematic).c_str(), &(normSTandHTweight.Systematic(systematic)));
        for(auto& systematic: normSTandJet1Ptweight.Systematics())
          bdttree->Branch(("normSTandJet1Ptweight_"+systematic).c_str(), &(normSTandJet1Ptweight.Systematic(systematic)));
      }
      if(noTrim)
        for(auto& systematic: looseNotTightWeight.Systematics())
        bdttree->Branch(("looseNotTightWeight_"+systematic).c_str(), &(looseNotTightWeight.Systematic(systematic)));
        for(auto& systematic: looseNotTightWeight2017MCClosure.Systematics())
        bdttree->Branch(("looseNotTightWeight2017MCClosure_"+systematic).c_str(), &(looseNotTightWeight2017MCClosure.Systematic(systematic)));
      for(auto& systematic: weight.Systematics())
        bdttree->Branch(("weight_"+systematic).c_str(), &(weight.Systematic(systematic)));
      std::cout << "\t      Finished." << std::endl;

      bool isTight;      bdttree->Branch("isTight",   &isTight);
      bool isLoose;      bdttree->Branch("isLoose",   &isLoose);
      Float_t LepID;     bdttree->Branch("LepID",     &LepID,     "LepID/F");
      Float_t LepChg;    bdttree->Branch("LepChg",    &LepChg,    "LepChg/F");
      Float_t LepPt;     bdttree->Branch("LepPt",     &LepPt,     "LepPt/F");
      Float_t LepEta;    bdttree->Branch("LepEta",    &LepEta,    "LepEta/F");
      Float_t LepPhi;    bdttree->Branch("LepPhi",    &LepPhi,    "LepPhi/F");
      Float_t LepDxy;    bdttree->Branch("LepDxy",    &LepDxy,    "LepDxy/F");
      Float_t LepDz;     bdttree->Branch("LepDz",     &LepDz,     "LepDz/F");
      Float_t LepMT;     bdttree->Branch("LepMT",     &LepMT,     "LepMT/F");
      Float_t LepAbsIso03;  bdttree->Branch("LepAbsIso03",  &LepAbsIso03,  "LepAbsIso03/F");
      Float_t LepRelIso03;  bdttree->Branch("LepRelIso03",  &LepRelIso03,  "LepRelIso03/F");
      Float_t LepHybIso03; bdttree->Branch("LepHybIso03",  &LepHybIso03,  "LepHybIso03/F");
      Float_t isPrompt;  bdttree->Branch("isPrompt",  &isPrompt,  "isPrompt/F");
      Float_t Lep2ID;    bdttree->Branch("Lep2ID",    &Lep2ID,    "Lep2ID/F");
      Float_t Lep2Chg;   bdttree->Branch("Lep2Chg",   &Lep2Chg,   "Lep2Chg/F");
      Float_t Lep2Pt;    bdttree->Branch("Lep2Pt",    &Lep2Pt,    "Lep2Pt/F");
      Float_t Lep2Eta;   bdttree->Branch("Lep2Eta",   &Lep2Eta,   "Lep2Eta/F");
      Float_t Lep2Dxy;   bdttree->Branch("Lep2Dxy",   &Lep2Dxy,   "Lep2Dxy/F");
      Float_t Lep2Dz;    bdttree->Branch("Lep2Dz",    &Lep2Dz,    "Lep2Dz/F");
      Float_t Lep2Iso03; bdttree->Branch("Lep2Iso03", &Lep2Iso03, "Lep2Iso03/F");
      Float_t nGoodEl;   bdttree->Branch("nGoodEl",&nGoodEl,"nGoodEl/F");
      Float_t nGoodEl_cutId_veto;   bdttree->Branch("nGoodEl_cutId_veto",&nGoodEl_cutId_veto,"nGoodEl_cutId_veto/F");
      Float_t nGoodEl_cutId_loose;   bdttree->Branch("nGoodEl_cutId_loose",&nGoodEl_cutId_loose,"nGoodEl_cutId_loose/F");
      Float_t nGoodEl_cutId_medium;   bdttree->Branch("nGoodEl_cutId_medium",&nGoodEl_cutId_medium,"nGoodEl_cutId_medium/F");
      Float_t nGoodEl_cutId_tight;   bdttree->Branch("nGoodEl_cutId_tight",&nGoodEl_cutId_tight,"nGoodEl_cutId_tight/F");
      Float_t nGoodMu;   bdttree->Branch("nGoodMu",&nGoodMu,"nGoodMu/F");
      Float_t nGoodMu_cutId_soft;   bdttree->Branch("nGoodMu_cutId_soft",&nGoodMu_cutId_soft,"nGoodMu_cutId_soft/F");
      Float_t nGoodMu_cutId_loose;   bdttree->Branch("nGoodMu_cutId_loose",&nGoodMu_cutId_loose,"nGoodMu_cutId_loose/F");
      Float_t nGoodMu_cutId_medium;   bdttree->Branch("nGoodMu_cutId_medium",&nGoodMu_cutId_medium,"nGoodMu_cutId_medium/F");
      Float_t nGoodMu_cutId_tight;   bdttree->Branch("nGoodMu_cutId_tight",&nGoodMu_cutId_tight,"nGoodMu_cutId_tight/F");

      Float_t nGoodEl_loose;
      Float_t nGoodMu_loose;


      ValueWithSystematics<float> Met;
      ValueWithSystematics<double> Met_phi;
      ValueWithSystematics<float> CaloMet;
      ValueWithSystematics<float> mt;
      ValueWithSystematics<float> Q80;
      ValueWithSystematics<float> CosDeltaPhi;
      ValueWithSystematics<float> DPhiLepMet;
      ValueWithSystematics<float> DPhiLepHTmiss;
      ValueWithSystematics<float> nIsr_out;
      ValueWithSystematics<float> Njet;
      /*
      ValueWithSystematics<float> Njet30;
      ValueWithSystematics<float> Njet40;
      ValueWithSystematics<float> Njet50;
      ValueWithSystematics<float> Njet60;
      ValueWithSystematics<float> Njet70;
      ValueWithSystematics<float> Njet80;
      ValueWithSystematics<float> Njet90;
      ValueWithSystematics<float> Njet100;
      */
      ValueWithSystematics<float> Jet1Pt;
      ValueWithSystematics<float> Jet1Eta;
      ValueWithSystematics<double> Jet1Phi;
      ValueWithSystematics<double> Jet1neEmEF;
      //ValueWithSystematics<float> Jet1CSV;
      ValueWithSystematics<float> Jet1DeepCSV;
      ValueWithSystematics<float> Jet1DeepJet;
      ValueWithSystematics<float> Jet2Pt;
      ValueWithSystematics<float> Jet2Eta;
      ValueWithSystematics<double> Jet2Phi;
//      ValueWithSystematics<float> Jet2CSV;
      ValueWithSystematics<float> Jet2DeepCSV;
      ValueWithSystematics<float> Jet2DeepJet;
      ValueWithSystematics<float> Jet3Pt;
      ValueWithSystematics<float> Jet3Eta;
      ValueWithSystematics<double> Jet3Phi;
      //ValueWithSystematics<float> Jet3CSV;
      ValueWithSystematics<float> Jet3DeepCSV;
      ValueWithSystematics<float> Jet3DeepJet;
      ValueWithSystematics<float> DPhiJet1Jet2;
      ValueWithSystematics<float> eta1p5Jet1Pt;
      ValueWithSystematics<float> eta1p5Jet1Eta;
      ValueWithSystematics<float> eta5Jet1Pt;
      ValueWithSystematics<float> eta5Jet1Eta;
      ValueWithSystematics<float> HT;
      ValueWithSystematics<float> HTpt;
      ValueWithSystematics<float> HTeta;
      ValueWithSystematics<float> HTphi;
      ValueWithSystematics<float> HTmiss;
      ValueWithSystematics<float> HTmissPt;
      ValueWithSystematics<float> HTmissEta;
      ValueWithSystematics<double> HTmissPhi;
      ValueWithSystematics<float> eta1p5HT;
      ValueWithSystematics<float> eta5HT;
      ValueWithSystematics<float> HT_raw;
      ValueWithSystematics<float> DPhiJet1HTmiss;
      ValueWithSystematics<float> DPhiJet1Met;
      ValueWithSystematics<float> DPhiJet2HTmiss;
      ValueWithSystematics<float> DPhiJet2Met;
      ValueWithSystematics<float> DPhiJet3HTmiss;
      ValueWithSystematics<float> DPhiJet3Met;

      ValueWithSystematics<float> Ngenjet;
      ValueWithSystematics<float> genJet1Pt;
      ValueWithSystematics<float> genJet2Pt;
      ValueWithSystematics<float> genHT;

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
      //ValueWithSystematics<float> JetHBCSV;
      ValueWithSystematics<float> JetHBDeepCSV;
      ValueWithSystematics<float> JetHBDeepJet;
      ValueWithSystematics<float> JetB2pt;
      ValueWithSystematics<float> JetB2eta;
      ValueWithSystematics<float> JetB2index;
      //ValueWithSystematics<float> JetB2CSV;
      ValueWithSystematics<float> JetB2DeepCSV;
      ValueWithSystematics<float> JetB2DeepJet;

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
        DPhiLepMet.Systematic("JES_Up");
        DPhiLepMet.Systematic("JES_Down");
        DPhiLepMet.Systematic("JER_Up");
        DPhiLepMet.Systematic("JER_Down");
        DPhiLepHTmiss.Systematic("JES_Up");
        DPhiLepHTmiss.Systematic("JES_Down");
        DPhiLepHTmiss.Systematic("JER_Up");
        DPhiLepHTmiss.Systematic("JER_Down");
        Njet.Systematic("JES_Up");
        Njet.Systematic("JES_Down");
        Njet.Systematic("JER_Up");
        Njet.Systematic("JER_Down");
        /*
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
        */
        Jet1Pt.Systematic("JES_Up");
        Jet1Pt.Systematic("JES_Down");
        Jet1Pt.Systematic("JER_Up");
        Jet1Pt.Systematic("JER_Down");
        Jet1Eta.Systematic("JES_Up");
        Jet1Eta.Systematic("JES_Down");
        Jet1Eta.Systematic("JER_Up");
        Jet1Eta.Systematic("JER_Down");
        Jet1Phi.Systematic("JES_Up");
        Jet1Phi.Systematic("JES_Down");
        Jet1Phi.Systematic("JER_Up");
        Jet1Phi.Systematic("JER_Down");
        Jet1neEmEF.Systematic("JES_Up");
        Jet1neEmEF.Systematic("JES_Down");
        Jet1neEmEF.Systematic("JER_Up");
        Jet1neEmEF.Systematic("JER_Down");
        /*
        Jet1CSV.Systematic("JES_Up");
        Jet1CSV.Systematic("JES_Down");
        Jet1CSV.Systematic("JER_Up");
        Jet1CSV.Systematic("JER_Down");
        */
        Jet1DeepCSV.Systematic("JES_Up");
        Jet1DeepCSV.Systematic("JES_Down");
        Jet1DeepCSV.Systematic("JER_Up");
        Jet1DeepCSV.Systematic("JER_Down");
        Jet1DeepJet.Systematic("JES_Up");
        Jet1DeepJet.Systematic("JES_Down");
        Jet1DeepJet.Systematic("JER_Up");
        Jet1DeepJet.Systematic("JER_Down");
        Jet2Pt.Systematic("JES_Up");
        Jet2Pt.Systematic("JES_Down");
        Jet2Pt.Systematic("JER_Up");
        Jet2Pt.Systematic("JER_Down");
        Jet2Eta.Systematic("JES_Up");
        Jet2Eta.Systematic("JES_Down");
        Jet2Eta.Systematic("JER_Up");
        Jet2Eta.Systematic("JER_Down");
        Jet2Phi.Systematic("JES_Up");
        Jet2Phi.Systematic("JES_Down");
        Jet2Phi.Systematic("JER_Up");
        Jet2Phi.Systematic("JER_Down");
        /*
        Jet2CSV.Systematic("JES_Up");
        Jet2CSV.Systematic("JES_Down");
        Jet2CSV.Systematic("JER_Up");
        Jet2CSV.Systematic("JER_Down");
        */
        Jet2DeepCSV.Systematic("JES_Up");
        Jet2DeepCSV.Systematic("JES_Down");
        Jet2DeepCSV.Systematic("JER_Up");
        Jet2DeepCSV.Systematic("JER_Down");
        Jet2DeepJet.Systematic("JES_Up");
        Jet2DeepJet.Systematic("JES_Down");
        Jet2DeepJet.Systematic("JER_Up");
        Jet2DeepJet.Systematic("JER_Down");
        Jet3Pt.Systematic("JES_Up");
        Jet3Pt.Systematic("JES_Down");
        Jet3Pt.Systematic("JER_Up");
        Jet3Pt.Systematic("JER_Down");
        Jet3Eta.Systematic("JES_Up");
        Jet3Eta.Systematic("JES_Down");
        Jet3Eta.Systematic("JER_Up");
        Jet3Eta.Systematic("JER_Down");
        Jet3Phi.Systematic("JES_Up");
        Jet3Phi.Systematic("JES_Down");
        Jet3Phi.Systematic("JER_Up");
        Jet3Phi.Systematic("JER_Down");
        /*
        Jet3CSV.Systematic("JES_Up");
        Jet3CSV.Systematic("JES_Down");
        Jet3CSV.Systematic("JER_Up");
        Jet3CSV.Systematic("JER_Down");
        */
        Jet3DeepCSV.Systematic("JES_Up");
        Jet3DeepCSV.Systematic("JES_Down");
        Jet3DeepCSV.Systematic("JER_Up");
        Jet3DeepCSV.Systematic("JER_Down");
        Jet3DeepJet.Systematic("JES_Up");
        Jet3DeepJet.Systematic("JES_Down");
        Jet3DeepJet.Systematic("JER_Up");
        Jet3DeepJet.Systematic("JER_Down");
        DPhiJet1Jet2.Systematic("JES_Up");
        DPhiJet1Jet2.Systematic("JES_Down");
        DPhiJet1Jet2.Systematic("JER_Up");
        DPhiJet1Jet2.Systematic("JER_Down");
        eta1p5Jet1Pt.Systematic("JES_Up");
        eta1p5Jet1Pt.Systematic("JES_Down");
        eta1p5Jet1Pt.Systematic("JER_Up");
        eta1p5Jet1Pt.Systematic("JER_Down");
        eta1p5Jet1Eta.Systematic("JES_Up");
        eta1p5Jet1Eta.Systematic("JES_Down");
        eta1p5Jet1Eta.Systematic("JER_Up");
        eta1p5Jet1Eta.Systematic("JER_Down");
        eta5Jet1Pt.Systematic("JES_Up");
        eta5Jet1Pt.Systematic("JES_Down");
        eta5Jet1Pt.Systematic("JER_Up");
        eta5Jet1Pt.Systematic("JER_Down");
        eta5Jet1Eta.Systematic("JES_Up");
        eta5Jet1Eta.Systematic("JES_Down");
        eta5Jet1Eta.Systematic("JER_Up");
        eta5Jet1Eta.Systematic("JER_Down");
        HT.Systematic("JES_Up");
        HT.Systematic("JES_Down");
        HT.Systematic("JER_Up");
        HT.Systematic("JER_Down");
        HTpt.Systematic("JES_Up");
        HTpt.Systematic("JES_Down");
        HTpt.Systematic("JER_Up");
        HTpt.Systematic("JER_Down");
        HTpt.Systematic("JES_Up");
        HTpt.Systematic("JES_Down");
        HTpt.Systematic("JER_Up");
        HTpt.Systematic("JER_Down");
        HTphi.Systematic("JES_Up");
        HTphi.Systematic("JES_Down");
        HTphi.Systematic("JER_Up");
        HTphi.Systematic("JER_Down");
        HTmiss.Systematic("JES_Up");
        HTmiss.Systematic("JES_Down");
        HTmiss.Systematic("JER_Up");
        HTmiss.Systematic("JER_Down");
        HTmissPt.Systematic("JES_Up");
        HTmissPt.Systematic("JES_Down");
        HTmissPt.Systematic("JER_Up");
        HTmissPt.Systematic("JER_Down");
        HTmissEta.Systematic("JES_Up");
        HTmissEta.Systematic("JES_Down");
        HTmissEta.Systematic("JER_Up");
        HTmissEta.Systematic("JER_Down");
        HTmissPhi.Systematic("JES_Up");
        HTmissPhi.Systematic("JES_Down");
        HTmissPhi.Systematic("JER_Up");
        HTmissPhi.Systematic("JER_Down");
        eta1p5HT.Systematic("JES_Up");
        eta1p5HT.Systematic("JES_Down");
        eta1p5HT.Systematic("JER_Up");
        eta1p5HT.Systematic("JER_Down");
        eta5HT.Systematic("JES_Up");
        eta5HT.Systematic("JES_Down");
        eta5HT.Systematic("JER_Up");
        eta5HT.Systematic("JER_Down");
        HT_raw.Systematic("JES_Up");
        HT_raw.Systematic("JES_Down");
        HT_raw.Systematic("JER_Up");
        HT_raw.Systematic("JER_Down");
        DPhiJet1HTmiss.Systematic("JES_Up");
        DPhiJet1HTmiss.Systematic("JES_Down");
        DPhiJet1HTmiss.Systematic("JER_Up");
        DPhiJet1HTmiss.Systematic("JER_Down");
        DPhiJet1Met.Systematic("JES_Up");
        DPhiJet1Met.Systematic("JES_Down");
        DPhiJet1Met.Systematic("JER_Up");
        DPhiJet1Met.Systematic("JER_Down");
        DPhiJet2HTmiss.Systematic("JES_Up");
        DPhiJet2HTmiss.Systematic("JES_Down");
        DPhiJet2HTmiss.Systematic("JER_Up");
        DPhiJet2HTmiss.Systematic("JER_Down");
        DPhiJet2Met.Systematic("JES_Up");
        DPhiJet2Met.Systematic("JES_Down");
        DPhiJet2Met.Systematic("JER_Up");
        DPhiJet2Met.Systematic("JER_Down");
        DPhiJet3HTmiss.Systematic("JES_Up");
        DPhiJet3HTmiss.Systematic("JES_Down");
        DPhiJet3HTmiss.Systematic("JER_Up");
        DPhiJet3HTmiss.Systematic("JER_Down");
        DPhiJet3Met.Systematic("JES_Up");
        DPhiJet3Met.Systematic("JES_Down");
        DPhiJet3Met.Systematic("JER_Up");
        DPhiJet3Met.Systematic("JER_Down");
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
        /*
        JetHBCSV.Systematic("JES_Up");
        JetHBCSV.Systematic("JES_Down");
        JetHBCSV.Systematic("JER_Up");
        JetHBCSV.Systematic("JER_Down");
        */
        JetHBDeepCSV.Systematic("JES_Up");
        JetHBDeepCSV.Systematic("JES_Down");
        JetHBDeepCSV.Systematic("JER_Up");
        JetHBDeepCSV.Systematic("JER_Down");
        JetHBDeepJet.Systematic("JES_Up");
        JetHBDeepJet.Systematic("JES_Down");
        JetHBDeepJet.Systematic("JER_Up");
        JetHBDeepJet.Systematic("JER_Down");
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
        /*
        JetB2CSV.Systematic("JES_Up");
        JetB2CSV.Systematic("JES_Down");
        JetB2CSV.Systematic("JER_Up");
        JetB2CSV.Systematic("JER_Down");
        */
        JetB2DeepCSV.Systematic("JES_Up");
        JetB2DeepCSV.Systematic("JES_Down");
        JetB2DeepCSV.Systematic("JER_Up");
        JetB2DeepCSV.Systematic("JER_Down");
        JetB2DeepJet.Systematic("JES_Up");
        JetB2DeepJet.Systematic("JES_Down");
        JetB2DeepJet.Systematic("JER_Up");
        JetB2DeepJet.Systematic("JER_Down");
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
        Met_phi.Lock();
        CaloMet.Lock();
        mt.Lock();
        Q80.Lock();
        CosDeltaPhi.Lock();
        DPhiLepMet.Lock();
        DPhiLepHTmiss.Lock();
        nIsr_out.Lock();
        Njet.Lock();
        /*
        Njet30.Lock();
        Njet40.Lock();
        Njet50.Lock();
        Njet60.Lock();
        Njet70.Lock();
        Njet80.Lock();
        Njet90.Lock();
        Njet100.Lock();
        */
        Jet1Pt.Lock();
        Jet1Eta.Lock();
        Jet1Phi.Lock();
        Jet1neEmEF.Lock();
        //Jet1CSV.Lock();
        Jet1DeepCSV.Lock();
        Jet1DeepJet.Lock();
        Jet2Pt.Lock();
        Jet2Eta.Lock();
        Jet2Phi.Lock();
        //Jet2CSV.Lock();
        Jet2DeepCSV.Lock();
        Jet2DeepJet.Lock();
        Jet3Pt.Lock();
        Jet3Eta.Lock();
        Jet3Phi.Lock();
        //Jet3CSV.Lock();
        Jet3DeepCSV.Lock();
        Jet3DeepJet.Lock();
        DPhiJet1Jet2.Lock();
        eta1p5Jet1Pt.Lock();
        eta1p5Jet1Eta.Lock();
        eta5Jet1Pt.Lock();
        eta5Jet1Eta.Lock();
        HT.Lock();
        HTpt.Lock();
        HTeta.Lock();
        HTphi.Lock();
        HTmiss.Lock();
        HTmissPt.Lock();
        HTmissEta.Lock();
        HTmissPhi.Lock();
        eta1p5HT.Lock();
        eta5HT.Lock();
        HT_raw.Lock();
        DPhiJet1HTmiss.Lock();
        DPhiJet1Met.Lock();
        DPhiJet2HTmiss.Lock();
        DPhiJet2Met.Lock();
        DPhiJet3HTmiss.Lock();
        DPhiJet3Met.Lock();
        Ngenjet.Lock();
        genJet1Pt.Lock();
        genJet2Pt.Lock();
        genHT.Lock();
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
        //JetHBCSV.Lock();
        JetHBDeepCSV.Lock();
        JetHBDeepJet.Lock();
        JetB2pt.Lock();
        JetB2eta.Lock();
        JetB2index.Lock();
        //JetB2CSV.Lock();
        JetB2DeepCSV.Lock();
        JetB2DeepJet.Lock();
        DrJet1Lep.Lock();
        DrJet2Lep.Lock();
        DrJetHBLep.Lock();
        DrJet1Jet2.Lock();
        JetLepMass.Lock();
        J3Mass.Lock();
      }

      Float_t St; bdttree->Branch("St",&St,"St/F");
      double St_phi; bdttree->Branch("St_phi",&St_phi,"St_phi/D");

      bdttree->Branch("Met",&Met.Value(),"Met/F");
      bdttree->Branch("Met_phi",&Met_phi.Value(),"Met_phi/D");
      bdttree->Branch("CaloMet",&CaloMet.Value(),"CaloMet/F");
      bdttree->Branch("mt",&mt.Value(),"mt/F");
      bdttree-> Branch("Q80",&Q80.Value(),"Q80/F");
      bdttree->Branch("CosDeltaPhi",&CosDeltaPhi.Value(),"CosDeltaPhi/F");
      bdttree->Branch("DPhiLepMet",&DPhiLepMet.Value(),"DPhiLepMet/F");
      bdttree->Branch("DPhiLepHTmiss",&DPhiLepHTmiss.Value(),"DPhiLepHTmiss/F");
      bdttree->Branch("nIsr", &nIsr_out.Value(), "nIsr/F");
      bdttree->Branch("Njet",&Njet.Value(),"Njet/F");
      /*
      bdttree->Branch("Njet30",&Njet30.Value(),"Njet30/F");
      bdttree->Branch("Njet40",&Njet40.Value(),"Njet40/F");
      bdttree->Branch("Njet50",&Njet50.Value(),"Njet50/F");
      bdttree->Branch("Njet60",&Njet60.Value(),"Njet60/F");
      bdttree->Branch("Njet70",&Njet70.Value(),"Njet70/F");
      bdttree->Branch("Njet80",&Njet80.Value(),"Njet80/F");
      bdttree->Branch("Njet90",&Njet90.Value(),"Njet90/F");
      bdttree->Branch("Njet100",&Njet100.Value(),"Njet100/F");
      */
      //bdttree->Branch("JetValidPt",&JetValidPt.Value(),"JetValidPt/F");
      //bdttree->Branch("JetValidEta",&JetValidEta.Value(),"JetValidEta/F");
      bdttree->Branch("Jet1Pt",&Jet1Pt.Value(),"Jet1Pt/F");
      bdttree->Branch("Jet1Eta",&Jet1Eta.Value(),"Jet1Eta/F");
      bdttree->Branch("Jet1Phi",&Jet1Phi.Value(),"Jet1Phi/D");
      bdttree->Branch("Jet1neEmEF",&Jet1neEmEF.Value(),"Jet1neEmEF/D");
      //bdttree->Branch("Jet1CSV",&Jet1CSV.Value(),"Jet1CSV/F");
      bdttree->Branch("Jet1DeepCSV",&Jet1DeepCSV.Value(),"Jet1DeepCSV/F");
      bdttree->Branch("Jet1DeepJet",&Jet1DeepJet.Value(),"Jet1DeepJet/F");
      bdttree->Branch("Jet2Pt",&Jet2Pt.Value(),"Jet2Pt/F");
      bdttree->Branch("Jet2Eta",&Jet2Eta.Value(),"Jet2Eta/F");
      bdttree->Branch("Jet2Phi",&Jet2Phi.Value(),"Jet2Phi/D");
      //bdttree->Branch("Jet2CSV",&Jet2CSV.Value(),"Jet2CSV/F");
      bdttree->Branch("Jet2DeepCSV",&Jet2DeepCSV.Value(),"Jet2DeepCSV/F");
      bdttree->Branch("Jet2DeepJet",&Jet2DeepJet.Value(),"Jet2DeepJet/F");
      bdttree->Branch("Jet3Pt",&Jet3Pt.Value(),"Jet3Pt/F");
      bdttree->Branch("Jet3Eta",&Jet3Eta.Value(),"Jet3Eta/F");
      bdttree->Branch("Jet3Phi",&Jet3Phi.Value(),"Jet3Phi/D");
      //bdttree->Branch("Jet3CSV",&Jet3CSV.Value(),"Jet3CSV/F");
      bdttree->Branch("Jet3DeepCSV",&Jet3DeepCSV.Value(),"Jet3DeepCSV/F");
      bdttree->Branch("Jet3DeepJet",&Jet3DeepJet.Value(),"Jet3DeepJet/F");
      bdttree->Branch("DPhiJet1Jet2",&DPhiJet1Jet2.Value(),"DPhiJet1Jet2/F");
      bdttree->Branch("eta1p5Jet1Pt",&eta1p5Jet1Pt.Value(),"eta1p5Jet1Pt/F");
      bdttree->Branch("eta1p5Jet1Eta",&eta1p5Jet1Eta.Value(),"eta1p5Jet1Eta/F");
      bdttree->Branch("eta5Jet1Pt",&eta5Jet1Pt.Value(),"eta5Jet1Pt/F");
      bdttree->Branch("eta5Jet1Eta",&eta5Jet1Eta.Value(),"eta5Jet1Eta/F");
      bdttree->Branch("HT",&HT.Value(),"HT/F");
      bdttree->Branch("HTpt",&HTpt.Value(),"HTpt/F");
      bdttree->Branch("HTeta",&HTeta.Value(),"HTeta/F");
      bdttree->Branch("HTphi",&HTphi.Value(),"HTphi/F");
      bdttree->Branch("HTmiss",&HTmiss.Value(),"HTmiss/F");
      bdttree->Branch("HTmissPt",&HTmissPt.Value(),"HTmissPt/F");
      bdttree->Branch("HTmissEta",&HTmissEta.Value(),"HTmissEta/F");
      bdttree->Branch("HTmissPhi",&HTmissPhi.Value(),"HTmissPhi/D");
      bdttree->Branch("eta1p5HT",&eta1p5HT.Value(),"eta1p5HT/F");
      bdttree->Branch("eta5HT",&eta5HT.Value(),"eta5HT/F");
      bdttree->Branch("HT_raw",&HT_raw.Value(),"HT_raw/F");
      bdttree->Branch("DPhiJet1HTmiss",&DPhiJet1HTmiss.Value(),"DPhiJet1HTmiss/F");
      bdttree->Branch("DPhiJet1Met",&DPhiJet1Met.Value(),"DPhiJet1Met/F");
      bdttree->Branch("DPhiJet2HTmiss",&DPhiJet2HTmiss.Value(),"DPhiJet2HTmiss/F");
      bdttree->Branch("DPhiJet2Met",&DPhiJet2Met.Value(),"DPhiJet2Met/F");
      bdttree->Branch("DPhiJet3HTmiss",&DPhiJet3HTmiss.Value(),"DPhiJet3HTmiss/F");
      bdttree->Branch("DPhiJet3Met",&DPhiJet3Met.Value(),"DPhiJet3Met/F");
      bdttree->Branch("Ngenjet",&Ngenjet.Value(),"Ngenjet/F");
      bdttree->Branch("genJet1Pt",&genJet1Pt.Value(),"genJet1Pt/F");
      bdttree->Branch("genJet2Pt",&genJet2Pt.Value(),"genJet2Pt/F");
      bdttree->Branch("genHT",&genHT.Value(),"genHT/F");

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
      //bdttree->Branch("JetHBCSV", &JetHBCSV.Value(), "JetHBCSV/F");
      bdttree->Branch("JetHBDeepCSV", &JetHBDeepCSV.Value(), "JetHBDeepCSV/F");
      bdttree->Branch("JetHBDeepJet", &JetHBDeepJet.Value(), "JetHBDeepJet/F");

      bdttree->Branch("JetB2pt",&JetB2pt.Value(),"JetB2pt/F");
      bdttree->Branch("JetB2eta",&JetB2eta.Value(),"JetB2eta/F");
      bdttree->Branch("JetB2index", &JetB2index.Value(), "JetB2index/F");
      //bdttree->Branch("JetB2CSV", &JetB2CSV.Value(), "JetB2CSV/F");
      bdttree->Branch("JetB2DeepCSV", &JetB2DeepCSV.Value(), "JetB2DeepCSV/F");
      bdttree->Branch("JetB2DeepJet", &JetB2DeepJet.Value(), "JetB2DeepJet/F");

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
        /*
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
        */
        //for(auto& systematic: JetValidPt.Systematics())
          //bdttree->Branch(("JetValidPt_"+systematic).c_str(), &(JetValidPt.Systematic(systematic)));
        //for(auto& systematic: JetValidEta.Systematics())
          //bdttree->Branch(("JetValidEta_"+systematic).c_str(), &(JetValidEta.Systematic(systematic)));
        for(auto& systematic: Jet1Pt.Systematics())
          bdttree->Branch(("Jet1Pt_"+systematic).c_str(), &(Jet1Pt.Systematic(systematic)));
        for(auto& systematic: Jet1Eta.Systematics())
          bdttree->Branch(("Jet1Eta_"+systematic).c_str(), &(Jet1Eta.Systematic(systematic)));
        for(auto& systematic: Jet1Phi.Systematics())
          bdttree->Branch(("Jet1Phi_"+systematic).c_str(), &(Jet1Phi.Systematic(systematic)));
        //for(auto& systematic: Jet1CSV.Systematics())
          //bdttree->Branch(("Jet1CSV_"+systematic).c_str(), &(Jet1CSV.Systematic(systematic)));
        for(auto& systematic: Jet1DeepCSV.Systematics())
          bdttree->Branch(("Jet1DeepCSV_"+systematic).c_str(), &(Jet1DeepCSV.Systematic(systematic)));
        for(auto& systematic: Jet1DeepJet.Systematics())
          bdttree->Branch(("Jet1DeepJet_"+systematic).c_str(), &(Jet1DeepJet.Systematic(systematic)));
        for(auto& systematic: Jet2Pt.Systematics())
          bdttree->Branch(("Jet2Pt_"+systematic).c_str(), &(Jet2Pt.Systematic(systematic)));
        for(auto& systematic: Jet2Eta.Systematics())
          bdttree->Branch(("Jet2Eta_"+systematic).c_str(), &(Jet2Eta.Systematic(systematic)));
        for(auto& systematic: Jet2Phi.Systematics())
          bdttree->Branch(("Jet2Phi_"+systematic).c_str(), &(Jet2Phi.Systematic(systematic)));
        //for(auto& systematic: Jet2CSV.Systematics())
        //  bdttree->Branch(("Jet2CSV_"+systematic).c_str(), &(Jet2CSV.Systematic(systematic)));
        for(auto& systematic: Jet2DeepCSV.Systematics())
          bdttree->Branch(("Jet2DeepCSV_"+systematic).c_str(), &(Jet2DeepCSV.Systematic(systematic)));
        for(auto& systematic: Jet2DeepJet.Systematics())
          bdttree->Branch(("Jet2DeepJet_"+systematic).c_str(), &(Jet2DeepJet.Systematic(systematic)));
        for(auto& systematic: Jet3Pt.Systematics())
          bdttree->Branch(("Jet3Pt_"+systematic).c_str(), &(Jet3Pt.Systematic(systematic)));
        for(auto& systematic: Jet3Eta.Systematics())
          bdttree->Branch(("Jet3Eta_"+systematic).c_str(), &(Jet3Eta.Systematic(systematic)));
        for(auto& systematic: Jet3Phi.Systematics())
          bdttree->Branch(("Jet3Phi_"+systematic).c_str(), &(Jet3Phi.Systematic(systematic)));
        //for(auto& systematic: Jet3CSV.Systematics())
          //bdttree->Branch(("Jet3CSV_"+systematic).c_str(), &(Jet3CSV.Systematic(systematic)));
        for(auto& systematic: Jet3DeepCSV.Systematics())
          bdttree->Branch(("Jet3DeepCSV_"+systematic).c_str(), &(Jet3DeepCSV.Systematic(systematic)));
        for(auto& systematic: Jet3DeepJet.Systematics())
          bdttree->Branch(("Jet3DeepJet_"+systematic).c_str(), &(Jet3DeepJet.Systematic(systematic)));
        for(auto& systematic: DPhiJet1Jet2.Systematics())
          bdttree->Branch(("DPhiJet1Jet2_"+systematic).c_str(), &(DPhiJet1Jet2.Systematic(systematic)));
        for(auto& systematic: HT.Systematics())
          bdttree->Branch(("HT_"+systematic).c_str(), &(HT.Systematic(systematic)));
        for(auto& systematic: HT_raw.Systematics())
          bdttree->Branch(("HT_raw_"+systematic).c_str(), &(HT_raw.Systematic(systematic)));
        for(auto& systematic: genHT.Systematics())
          bdttree->Branch(("genHT_"+systematic).c_str(), &(genHT.Systematic(systematic)));
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
        //for(auto& systematic: JetHBCSV.Systematics())
          //bdttree->Branch(("JetHBCSV_"+systematic).c_str(), &(JetHBCSV.Systematic(systematic)));
        for(auto& systematic: JetHBDeepCSV.Systematics())
          bdttree->Branch(("JetHBDeepCSV_"+systematic).c_str(), &(JetHBDeepCSV.Systematic(systematic)));
        for(auto& systematic: JetHBDeepJet.Systematics())
          bdttree->Branch(("JetHBDeepJet_"+systematic).c_str(), &(JetHBDeepJet.Systematic(systematic)));
        for(auto& systematic: JetB2pt.Systematics())
          bdttree->Branch(("JetB2pt_"+systematic).c_str(), &(JetB2pt.Systematic(systematic)));
        for(auto& systematic: JetB2eta.Systematics())
          bdttree->Branch(("JetB2eta_"+systematic).c_str(), &(JetB2eta.Systematic(systematic)));
        for(auto& systematic: JetB2index.Systematics())
          bdttree->Branch(("JetB2index_"+systematic).c_str(), &(JetB2index.Systematic(systematic)));
        //for(auto& systematic: JetB2CSV.Systematics())
          //bdttree->Branch(("JetB2CSV_"+systematic).c_str(), &(JetB2CSV.Systematic(systematic)));
        for(auto& systematic: JetB2DeepCSV.Systematics())
          bdttree->Branch(("JetB2DeepCSV_"+systematic).c_str(), &(JetB2DeepCSV.Systematic(systematic)));
        for(auto& systematic: JetB2DeepJet.Systematics())
          bdttree->Branch(("JetB2DeepJet_"+systematic).c_str(), &(JetB2DeepJet.Systematic(systematic)));
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

      Float_t HLT_PFMET90_PFMHT90;           bdttree->Branch("HLT_PFMET90_PFMHT90",   &HLT_PFMET90_PFMHT90,   "HLT_PFMET90_PFMHT90/F"  );
      Float_t HLT_PFMET100_PFMHT100;         bdttree->Branch("HLT_PFMET100_PFMHT100", &HLT_PFMET100_PFMHT100, "HLT_PFMET100_PFMHT100/F");
      Float_t HLT_PFMET100_PFMHT100_PFHT60;         bdttree->Branch("HLT_PFMET100_PFMHT100_PFHT60", &HLT_PFMET100_PFMHT100_PFHT60, "HLT_PFMET100_PFMHT100_PFHT60/F");
      Float_t HLT_PFMET110_PFMHT110;         bdttree->Branch("HLT_PFMET110_PFMHT110", &HLT_PFMET110_PFMHT110, "HLT_PFMET110_PFMHT110/F");
      Float_t HLT_PFMET120_PFMHT120;         bdttree->Branch("HLT_PFMET120_PFMHT120", &HLT_PFMET120_PFMHT120, "HLT_PFMET120_PFMHT120/F");
      Float_t HLT_PFMET120_PFMHT120_PFHT60;  bdttree->Branch("HLT_PFMET120_PFMHT120_PFHT60", &HLT_PFMET120_PFMHT120_PFHT60, "HLT_PFMET120_PFMHT120_PFHT60/F");
      Float_t HLT_PFMETNoMu120_PFMHTNoMu120; bdttree->Branch("HLT_PFMETNoMu120_PFMHTNoMu120", &HLT_PFMETNoMu120_PFMHTNoMu120, "HLT_PFMETNoMu120_PFMHTNoMu120/F");
      Float_t HLT_PFMETNoMu120_PFMHTNoMu120_PFHT60; bdttree->Branch("HLT_PFMETNoMu120_PFMHTNoMu120_PFHT60", &HLT_PFMETNoMu120_PFMHTNoMu120_PFHT60, "HLT_PFMETNoMu120_PFMHTNoMu120_PFHT60/F");
      //TODO: This should be updated for all samples after the next heppy run
      Float_t HLT_PFHT780;
      Float_t HLT_PFHT1050;
      if(process.isdata() || process.issignal()) {
        bdttree->Branch("HLT_PFHT780", &HLT_PFHT780, "HLT_PFHT780/F");
        bdttree->Branch("HLT_PFHT1050", &HLT_PFHT1050, "HLT_PFHT1050/F");
      }
      // 2017
      Float_t HLT_Mu27; bdttree->Branch("HLT_Mu27",&HLT_Mu27,"HLT_Mu27/F");
      Float_t HLT_el35; bdttree->Branch("HLT_el35",&HLT_el35,"HLT_el35/F");
      // 2018
      Float_t HLT_Mu24; bdttree->Branch("HLT_Mu24",&HLT_Mu24,"HLT_Mu24/F");
      Float_t HLT_el32; bdttree->Branch("HLT_el32",&HLT_el32,"HLT_el32/F");

      Float_t METFilters; bdttree->Branch("METFilters", &METFilters, "METFilters/F");
      Float_t HBHENoiseFilter; bdttree->Branch("HBHENoiseFilter", &HBHENoiseFilter,"HBHENoiseFilter/F");
      Float_t HBHENoiseIsoFilter; bdttree->Branch("HBHENoiseIsoFilter", &HBHENoiseIsoFilter,"HBHENoiseIsoFilter/F");
      Float_t eeBadScFilter; bdttree->Branch("eeBadScFilter", &eeBadScFilter,"eeBadScFilter/F");
      Float_t ecalBadCalibFilterV2; bdttree->Branch("ecalBadCalibFilterV2", &ecalBadCalibFilterV2,"ecalBadCalibFilterV2/F");
      Float_t EcalDeadCellTriggerPrimitiveFilter; bdttree->Branch("EcalDeadCellTriggerPrimitiveFilter", &EcalDeadCellTriggerPrimitiveFilter,"EcalDeadCellTriggerPrimitiveFilter/F");
      Float_t goodVertices; bdttree->Branch("goodVertices", &goodVertices,"goodVertices/F");
      Float_t globalSuperTightHalo2016Filter; bdttree->Branch("globalSuperTightHalo2016Filter", &globalSuperTightHalo2016Filter, "globalSuperTightHalo2016Filter/F");
//      Float_t badMuonMoriond2017; bdttree->Branch("badMuonMoriond2017", &badMuonMoriond2017, "badMuonMoriond2017/F");
//      Float_t badCloneMuonMoriond2017; bdttree->Branch("badCloneMuonMoriond2017", &badCloneMuonMoriond2017, "badCloneMuonMoriond2017/F");
      Float_t BadPFMuonFilter; bdttree->Branch("BadPFMuonFilter", &BadPFMuonFilter, "BadPFMuonFilter/F");
      Float_t BadChargedCandidateFilter; bdttree->Branch("BadChargedCandidateFilter", &BadChargedCandidateFilter, "BadChargedCandidateFilter/F");

      std::cout << "\t  Finished creating branches in output tree" << std::endl;


      Float_t Ncut0;  //bdttree->Branch("Ncut0",&Ncut0,"Ncut0/F");
      Float_t Ncut1;
      Float_t Ncut2;
      Float_t Ncut3;
      Float_t Ncut4;
      Float_t Ncut5;
      Float_t Ncut6;


      std::cout << "\t  Getting filter efficiency" << std::endl;
      /*
      TH2F* filterEfficiencyH = nullptr;
      if(sample.filterEfficiencyFile() != "")
      {
        TDirectory* cwd = gDirectory;
        TFile filterEfficiencyFile(sample.filterEfficiencyFile().c_str(), "READ");
        cwd->cd();
        filterEfficiencyH = static_cast<TH2F*>(filterEfficiencyFile.Get("filterEfficiency")->Clone("filterEfficiencyH"));
      }
      */

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

        TVectorD* sampleNevt = static_cast<TVectorD*>(preProcessSamplesFile.Get(("sample_"+sample.tag()+"_Nevt").c_str())->Clone("sampleNevt"));
        Nevt = (*sampleNevt)[0];

        TVectorD* sampleSumGenWeight = static_cast<TVectorD*>(preProcessSamplesFile.Get(("sample_"+sample.tag()+"_sumGenWeight").c_str())->Clone("sampleSumGenWeight"));
        sumGenWeight = (*sampleSumGenWeight)[0];

        TVectorD* processISRWeights = static_cast<TVectorD*>(preProcessSamplesFile.Get(("process_"+process.tag()+"_ISRCParams").c_str())->Clone("processISRWeights"));
        ISRCParam =    (*processISRWeights)[0];
        EWKISRCParam = (*processISRWeights)[1];

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
          inputtree = static_cast<TTree*>(finput.Get("Events"))->CopyTree(process.selection().c_str());
        else
          inputtree = static_cast<TTree*>(finput.Get("Events"));
        // PU weight from nanoAOD
        /*
        Float_t autoPUweight;
        Float_t autoPUweightUp;
        Float_t autoPUweightDown;
        if(!process.isdata())
        {
          inputtree->SetBranchAddress("puWeight",&autoPUweight);
          inputtree->SetBranchAddress("puWeightUp",&autoPUweightUp);
          inputtree->SetBranchAddress("puWeightDown",&autoPUweightDown);
        }
        */
        // Read Branches you are interested in from the input tree
//        Float_t mtw1;        inputtree->SetBranchAddress("event_mtw1"       , &mtw1);
//        Float_t mtw2;        inputtree->SetBranchAddress("event_mtw2"       , &mtw2);
//        Float_t mtw;         inputtree->SetBranchAddress("event_mtw"       , &mtw);
        Float_t MET_pt;      
        Float_t MET_phi;
        if(year==2017){
          inputtree->SetBranchAddress("METFixEE2017_pt", &MET_pt);
          inputtree->SetBranchAddress("METFixEE2017_phi", &MET_phi);
        }
        else{
          inputtree->SetBranchAddress("MET_pt"    , &MET_pt);
          inputtree->SetBranchAddress("MET_phi",   &MET_phi);   
        }

        Float_t CaloMET_pt; inputtree->SetBranchAddress("CaloMET_pt", &CaloMET_pt);
        Float_t CaloMET_phi; inputtree->SetBranchAddress("CaloMET_phi", &CaloMET_phi);

        UInt_t nLepGood;      inputtree->SetBranchAddress("nLepGood"   , &nLepGood);
        Int_t LepGood_jetIdx[LEPCOLL_LIMIT];  inputtree->SetBranchAddress("LepGood_jetIdx", &LepGood_jetIdx);
        Int_t LepGood_pdgId[LEPCOLL_LIMIT];  inputtree->SetBranchAddress("LepGood_pdgId", &LepGood_pdgId);
        Int_t LepGood_cutBased[LEPCOLL_LIMIT];  inputtree->SetBranchAddress("LepGood_cutBased", &LepGood_cutBased); //only for Electron: 0=fail, 1=veto, 2=loose, 3=medium, 4=tight
        Float_t LepGood_mvaFall17V1Iso[LEPCOLL_LIMIT];  inputtree->SetBranchAddress("LepGood_mvaFall17V1Iso", &LepGood_mvaFall17V1Iso); //MVA Iso ID V1 score
        Bool_t LepGood_isGlobal[LEPCOLL_LIMIT];  inputtree->SetBranchAddress("LepGood_isGlobal", &LepGood_isGlobal);
        Bool_t LepGood_isPFcand[LEPCOLL_LIMIT];  inputtree->SetBranchAddress("LepGood_isPFcand", &LepGood_isPFcand);
        Bool_t LepGood_isTracker[LEPCOLL_LIMIT];  inputtree->SetBranchAddress("LepGood_isTracker", &LepGood_isTracker);
        //Bool_t LepGood_looseId[LEPCOLL_LIMIT];  inputtree->SetBranchAddress("LepGood_looseId", &LepGood_looseId);
        Bool_t LepGood_mediumId[LEPCOLL_LIMIT];  inputtree->SetBranchAddress("LepGood_mediumId", &LepGood_mediumId);
        Bool_t LepGood_tightId[LEPCOLL_LIMIT];  inputtree->SetBranchAddress("LepGood_tightId", &LepGood_tightId);
        Bool_t LepGood_softId[LEPCOLL_LIMIT];  inputtree->SetBranchAddress("LepGood_softId", &LepGood_softId);
        Float_t LepGood_pt[LEPCOLL_LIMIT];  inputtree->SetBranchAddress("LepGood_pt", &LepGood_pt);
        Float_t LepGood_eta[LEPCOLL_LIMIT];  inputtree->SetBranchAddress("LepGood_eta", &LepGood_eta);
        Float_t LepGood_phi[LEPCOLL_LIMIT];  inputtree->SetBranchAddress("LepGood_phi", &LepGood_phi);
        Float_t LepGood_pfRelIso03_all[LEPCOLL_LIMIT]; inputtree->SetBranchAddress("LepGood_pfRelIso03_all",&LepGood_pfRelIso03_all);
        Float_t LepGood_pfAbsIso03_all[LEPCOLL_LIMIT];
        //inputtree->SetBranchAddress("LepGood_pfAbsIso03_all",&LepGood_pfAbsIso03_all); // TODO: Fix this at getting samples level (lxplus)
        Float_t LepGood_dxy[LEPCOLL_LIMIT]; inputtree->SetBranchAddress("LepGood_dxy",&LepGood_dxy);
        Float_t LepGood_dz[LEPCOLL_LIMIT]; inputtree->SetBranchAddress("LepGood_dz",&LepGood_dz);
        Float_t LepGood_mass[LEPCOLL_LIMIT]; inputtree->SetBranchAddress("LepGood_mass",&LepGood_mass);
// UNUSED   Float_t LepGood_mt[LEPCOLL_LIMIT]; inputtree->SetBranchAddress("LepGood_mt", &LepGood_mt);
// UNUSED           Float_t LepGood_Q80[LEPCOLL_LIMIT]; inputtree->SetBranchAddress("LepGood_Q80", &LepGood_Q80);
// UNUSED           Float_t LepGood_cosPhiLepMet[LEPCOLL_LIMIT]; inputtree->SetBranchAddress("LepGood_cosPhiLepMet", &LepGood_cosPhiLepMet);
        Int_t LepGood_charge[LEPCOLL_LIMIT]; inputtree->SetBranchAddress("LepGood_charge",&LepGood_charge);
        UInt_t nJetIn;  inputtree->SetBranchAddress("nJet", &nJetIn);
        Float_t Jet_pt[JETCOLL_LIMIT];  inputtree->SetBranchAddress("Jet_pt", &Jet_pt);
        Float_t Jet_neEmEF[JETCOLL_LIMIT];  inputtree->SetBranchAddress("Jet_neEmEF", &Jet_neEmEF);
        Int_t Jet_jetId[JETCOLL_LIMIT];  inputtree->SetBranchAddress("Jet_jetId", &Jet_jetId);  // Jet ID flags bit1 is loose (always false in 2017 since it does not exist), bit2 is tight, bit3 is tightLepVeto --> This flag represents: passlooseID*1+passtightID*2+passtightLepVetoID*4
        Float_t Jet_eta[JETCOLL_LIMIT];  inputtree->SetBranchAddress("Jet_eta", &Jet_eta);
        Float_t Jet_phi[JETCOLL_LIMIT];  inputtree->SetBranchAddress("Jet_phi", &Jet_phi);
        Float_t Jet_btagCSVV2[JETCOLL_LIMIT];  inputtree->SetBranchAddress("Jet_btagCSVV2", &Jet_btagCSVV2);
        Float_t Jet_btagDeepB[JETCOLL_LIMIT];  inputtree->SetBranchAddress("Jet_btagDeepB", &Jet_btagDeepB);
        Float_t Jet_btagDeepFlavB[JETCOLL_LIMIT];  inputtree->SetBranchAddress("Jet_btagDeepFlavB", &Jet_btagDeepFlavB);
        Float_t Jet_mass[JETCOLL_LIMIT];  inputtree->SetBranchAddress("Jet_mass", &Jet_mass);
        Float_t Jet_rawFactor[JETCOLL_LIMIT]; inputtree->SetBranchAddress("Jet_rawFactor", &Jet_rawFactor);
        UInt_t run;  inputtree->SetBranchAddress("run", &run);
        ULong64_t event;  inputtree->SetBranchAddress("event", &event);
        UInt_t luminosityBlock;  inputtree->SetBranchAddress("luminosityBlock", &luminosityBlock);

        UInt_t nGenJet;
        Float_t GenJet_pt[JETCOLL_LIMIT];
        if(!process.isdata())
        {
          inputtree->SetBranchAddress("nGenJet", &nGenJet);
          inputtree->SetBranchAddress("GenJet_pt", &GenJet_pt);
        }

        //Int_t Pileup_nPU; inputtree->SetBranchAddress("PV_npvs", &Pileup_nPU);
        Int_t Pileup_nPU; inputtree->SetBranchAddress("PV_npvsGood", &Pileup_nPU);

        UInt_t nLHEScaleWeight;
        float LHEScaleWeight[LHEWEIGHT_LIMIT];
        Int_t Jet_hadronFlavour[JETCOLL_LIMIT];

        if(!(process.isdata() || process.issignal() || process.tag()=="VV"))
        {
          inputtree->SetBranchAddress("nLHEScaleWeight"   , &nLHEScaleWeight);
          inputtree->SetBranchAddress("LHEScaleWeight", &LHEScaleWeight);
          inputtree->SetBranchAddress("Jet_hadronFlavour", &Jet_hadronFlavour);
        }

        Float_t nIsr;
        Float_t MET_pt_jesTotalUp;
        Float_t MET_phi_jesTotalUp;
        Float_t MET_pt_jesTotalDown;
        Float_t MET_phi_jesTotalDown;

        Float_t MET_pt_jerUp;
        Float_t MET_phi_jerUp;
        Float_t MET_pt_jerDown;
        Float_t MET_phi_jerDown;
        Float_t Jet_pt_nom[JETCOLL_LIMIT];

        Float_t Jet_corr_JEC[JETCOLL_LIMIT];
        //Float_t Jet_corr_JECUp[JETCOLL_LIMIT];
        Float_t Jet_pt_jesTotalUp[JETCOLL_LIMIT];

        //Float_t Jet_corr_JECDown[JETCOLL_LIMIT];
        Float_t Jet_pt_jesTotalDown[JETCOLL_LIMIT];

        Float_t Jet_corr_JER[JETCOLL_LIMIT];
        //Float_t Jet_corr_JERUp[JETCOLL_LIMIT];
        Float_t Jet_pt_jerUp[JETCOLL_LIMIT];
        //Float_t Jet_corr_JERDown[JETCOLL_LIMIT];
        Float_t Jet_pt_jerDown[JETCOLL_LIMIT];

        if(!process.isdata())
        {
          inputtree->SetBranchAddress("nIsr", &nIsr);
          inputtree->SetBranchAddress("MET_pt_jesTotalUp", &MET_pt_jesTotalUp);
          inputtree->SetBranchAddress("MET_phi_jesTotalUp", &MET_phi_jesTotalUp);
          inputtree->SetBranchAddress("MET_pt_jesTotalDown", &MET_pt_jesTotalDown);
          inputtree->SetBranchAddress("MET_phi_jesTotalDown", &MET_phi_jesTotalDown);
          inputtree->SetBranchAddress("MET_pt_jerUp"    , &MET_pt_jerUp);
          inputtree->SetBranchAddress("MET_phi_jerUp",   &MET_phi_jerUp);
          inputtree->SetBranchAddress("MET_pt_jerDown"    , &MET_pt_jerDown);
          inputtree->SetBranchAddress("MET_phi_jerDown",   &MET_phi_jerDown);
          inputtree->SetBranchAddress("Jet_pt_nom", &Jet_pt_nom);
          inputtree->SetBranchAddress("Jet_corr_JEC", &Jet_corr_JEC);
          inputtree->SetBranchAddress("Jet_pt_jesTotalUp", &Jet_pt_jesTotalUp);
          inputtree->SetBranchAddress("Jet_pt_jesTotalDown", &Jet_pt_jesTotalDown);
          inputtree->SetBranchAddress("Jet_corr_JER", &Jet_corr_JER);
          inputtree->SetBranchAddress("Jet_pt_jerUp", &Jet_pt_jerUp);
          inputtree->SetBranchAddress("Jet_pt_jerDown", &Jet_pt_jerDown);
        }

        //Int_t LepGood_genPartFlav[LEPCOLL_LIMIT];
        UChar_t LepGood_genPartFlav[LEPCOLL_LIMIT];
        UInt_t nGenPart = 0;
        Int_t GenPart_motherId[GENPART_LIMIT];
        Int_t GenPart_status[GENPART_LIMIT];
        Int_t GenPart_pdgId[GENPART_LIMIT];
        Float_t GenPart_pt[GENPART_LIMIT];
        Float_t GenPart_eta[GENPART_LIMIT];
        Float_t GenPart_phi[GENPART_LIMIT];
        Float_t xsec = 1;
        Float_t Pileup_nTrueInt = 1;
        if(!process.isdata())
        {
          inputtree->SetBranchAddress("genWeight", &genWeight);
          inputtree->SetBranchAddress("xsec", &xsec);
          inputtree->SetBranchAddress("Pileup_nTrueInt", &Pileup_nTrueInt);
          inputtree->SetBranchAddress("LepGood_genPartFlav", &LepGood_genPartFlav);

          inputtree->SetBranchAddress("nGenPart", &nGenPart);
          inputtree->SetBranchAddress("GenPart_genPartIdxMother", &GenPart_motherId);
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

        //TODO: This should be updated for all samples after the next heppy run
        // Remove the "if's" for both data bool
        // 2016 HLT
        Bool_t HLT_PFMET90_PFMHT90_IDTight;
        Bool_t HLT_PFMET100_PFMHT100_IDTight;
        Bool_t HLT_PFMET110_PFMHT110_IDTight;
        Bool_t HLT_PFMET120_PFMHT120_IDTight;
        Bool_t HLT_IsoMu24;
        Bool_t HLT_IsoMu27;
        Bool_t HLT_Ele32_WPTight_Gsf;
        Bool_t HLT_Ele35_WPTight_Gsf;
        //TODO: This should be updated for all samples after the next heppy run
        Bool_t HLT_PFHT780_input;
        Bool_t HLT_PFHT1050_input;
//        Bool_t HLT_PFMET100_PFMHT100_IDTight_PFHT60;
        Bool_t HLT_PFMET120_PFMHT120_IDTight_PFHT60;
        Bool_t HLT_PFMETNoMu120_PFMHTNoMu120_IDTight;
        Bool_t HLT_PFMETNoMu120_PFMHTNoMu120_IDTight_PFHT60;

        Bool_t Flag_METFilters = 1;
        Bool_t Flag_HBHENoiseFilter = 1;
        Bool_t Flag_HBHENoiseIsoFilter = 1;
        Bool_t Flag_eeBadScFilter = 1;
        Bool_t Flag_ecalBadCalibFilterV2 = 1;
        Bool_t Flag_EcalDeadCellTriggerPrimitiveFilter = 1;
        Bool_t Flag_goodVertices = 1;
        Bool_t Flag_globalSuperTightHalo2016Filter = 1;

        bool skip = true;

        //if(!skip || process.issignal()){
        if (!process.issignal()) {
          if(!skip){
            inputtree->SetBranchAddress("HLT_PFMET90_PFMHT90_IDTight", &HLT_PFMET90_PFMHT90_IDTight);
            inputtree->SetBranchAddress("HLT_PFMET100_PFMHT100_IDTight", &HLT_PFMET100_PFMHT100_IDTight);
            Bool_t HLT_Ele24_eta2p1_WPLoose_Gsf; inputtree->SetBranchAddress("HLT_Ele24_eta2p1_WPLoose_Gsf", &HLT_Ele24_eta2p1_WPLoose_Gsf);
            Bool_t HLT_Ele25_eta2p1_WPLoose_Gsf; inputtree->SetBranchAddress("HLT_Ele25_eta2p1_WPLoose_Gsf", &HLT_Ele25_eta2p1_WPLoose_Gsf );
          }
          else { // 2017 && 2018 HLT
            inputtree->SetBranchAddress("HLT_IsoMu24", &HLT_IsoMu24);
            inputtree->SetBranchAddress("HLT_IsoMu27", &HLT_IsoMu27);
            inputtree->SetBranchAddress("HLT_Ele32_WPTight_Gsf", &HLT_Ele32_WPTight_Gsf);
            inputtree->SetBranchAddress("HLT_Ele35_WPTight_Gsf", &HLT_Ele35_WPTight_Gsf);
            //inputtree->SetBranchAddress("HLT_PFMET100_PFMHT100_IDTight_PFHT60", &HLT_PFMET100_PFMHT100_IDTight_PFHT60);
            inputtree->SetBranchAddress("HLT_PFMET110_PFMHT110_IDTight", &HLT_PFMET110_PFMHT110_IDTight);
            inputtree->SetBranchAddress("HLT_PFMET120_PFMHT120_IDTight", &HLT_PFMET120_PFMHT120_IDTight);
            inputtree->SetBranchAddress("HLT_PFMET120_PFMHT120_IDTight_PFHT60", &HLT_PFMET120_PFMHT120_IDTight_PFHT60);

            if (year == 2018){
              //inputtree->SetBranchAddress("HLT_PFMET120_PFMHT120_IDTight_PFHT60", &HLT_PFMET120_PFMHT120_IDTight_PFHT60);
              inputtree->SetBranchAddress("HLT_PFMETNoMu120_PFMHTNoMu120_IDTight_PFHT60", &HLT_PFMETNoMu120_PFMHTNoMu120_IDTight_PFHT60);
            }
            inputtree->SetBranchAddress("HLT_PFMETNoMu120_PFMHTNoMu120_IDTight", &HLT_PFMETNoMu120_PFMHTNoMu120_IDTight);
            inputtree->SetBranchAddress("HLT_PFHT780", &HLT_PFHT780_input);
            inputtree->SetBranchAddress("HLT_PFHT1050", &HLT_PFHT1050_input);
          }
        }

//      Bool_t Flag_METFilters; inputtree->SetBranchAddress("Flag_METFilters", &Flag_METFilters);
        if (skip && !process.issignal()) {
          inputtree->SetBranchAddress("Flag_METFilters", &Flag_METFilters);
          inputtree->SetBranchAddress("Flag_HBHENoiseFilter", &Flag_HBHENoiseFilter);
          inputtree->SetBranchAddress("Flag_HBHENoiseIsoFilter", &Flag_HBHENoiseIsoFilter);
          inputtree->SetBranchAddress("Flag_eeBadScFilter", &Flag_eeBadScFilter);
          inputtree->SetBranchAddress("Flag_ecalBadCalibFilterV2", &Flag_ecalBadCalibFilterV2);
          inputtree->SetBranchAddress("Flag_EcalDeadCellTriggerPrimitiveFilter", &Flag_EcalDeadCellTriggerPrimitiveFilter);
          inputtree->SetBranchAddress("Flag_goodVertices", &Flag_goodVertices);
          inputtree->SetBranchAddress("Flag_globalSuperTightHalo2016Filter", &Flag_globalSuperTightHalo2016Filter);
        }
//        Int_t Flag_badMuonMoriond2017; inputtree->SetBranchAddress("Flag_badMuonMoriond2017", &Flag_badMuonMoriond2017);
//        Int_t Flag_badCloneMuonMoriond2017; inputtree->SetBranchAddress("Flag_badCloneMuonMoriond2017", &Flag_badCloneMuonMoriond2017);
        Bool_t Flag_BadPFMuonFilter; inputtree->SetBranchAddress("Flag_BadPFMuonFilter", &Flag_BadPFMuonFilter);
        Bool_t Flag_BadChargedCandidateFilter; inputtree->SetBranchAddress("Flag_BadChargedCandidateFilter", &Flag_BadChargedCandidateFilter);

        Float_t GenSusyMStop=0;
        Float_t GenSusyMNeutralino=0;
        if(process.issignal())
        {
          inputtree->SetBranchAddress("GenSusyMStop", &GenSusyMStop);
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
          nVert = Pileup_nPU;
          nIsr_out = nIsr;
          if(!process.isdata())
          {
            // PU from nanoAOD
            /*
            puWeight = autoPUweight;
            puWeight.Systematic("PU_Up") = autoPUweightUp;
            puWeight.Systematic("PU_Down") = autoPUweightDown;
            */
            puWeight = static_cast<float>(puWeightDistrib->GetBinContent(puWeightDistrib->FindBin(Pileup_nTrueInt)));
            if(puWeightDistribUp != nullptr)
            {
              puWeight.Systematic("PU_Up")   = puWeightDistribUp->GetBinContent(puWeightDistribUp->FindBin(Pileup_nTrueInt));
              puWeight.Systematic("PU_Down") = puWeightDistribDown->GetBinContent(puWeightDistribDown->FindBin(Pileup_nTrueInt));
            }
          }
          else
            puWeight = 1.0f;

          // Object ID
          std::vector<int> validLeptons;
          std::vector<int> looseLeptons;
          //std::vector<int> l1PreFiringJets;
          ValueWithSystematics<std::vector<double>> jetPt;
          ValueWithSystematics<std::vector<double>> genJetPt;
          ValueWithSystematics<std::vector<int>> allJets;
          ValueWithSystematics<std::vector<int>> validJets;
          ValueWithSystematics<std::vector<int>> eta1p5Jets;
          ValueWithSystematics<std::vector<int>> eta5Jets;
          ValueWithSystematics<std::vector<int>> genJets;
          ValueWithSystematics<std::vector<int>> bjetList; // Same as validJets, but sorted by CSV value
          //ValueWithSystematics<std::vector<int>> l1PreFiringList;

          validLeptons.clear();
          looseLeptons.clear();

          //TODO: Check if the correct thing to do is actually divide by Jet_pt. If so, when getting Sys the nominal correction value for the other correction factor (JER when doing JEC and vice versa) is included. Shouldn't it be remove?

// ***** MET ******

          // Setting the values to be saved in the output tree
          ValueWithSystematics<double> MetDou = MET_pt;
          ValueWithSystematics<double> MetPhi = MET_phi;
          ValueWithSystematics<double> CaloMetDou = CaloMET_pt;
          ValueWithSystematics<double> CaloMetPhi = CaloMetPhi;
          if(!process.isdata())
          {
            MetDou.Systematic("JES_Up") = MET_pt_jesTotalUp;
            MetDou.Systematic("JES_Down") = MET_pt_jesTotalDown;
            MetDou.Systematic("JER_Up") = MET_pt_jerUp;
            MetDou.Systematic("JER_Down") = MET_pt_jerDown;
            MetPhi.Systematic("JES_Up") = MET_phi_jesTotalUp;
            MetPhi.Systematic("JES_Down") = MET_phi_jesTotalDown;
            MetPhi.Systematic("JER_Up") = MET_phi_jerUp;
            MetPhi.Systematic("JER_Down") = MET_phi_jerDown;
          }
          Met = MetDou;
          Met_phi = MET_phi;
          CaloMet = CaloMetDou;

          if(preemptiveDropEvents)
          {
            if(!swap && !static_cast<bool>(Met > MIN_MET))
              continue;
          }

// ***** LEPTONS ******

          nGoodEl = 0;
          nGoodEl_loose = 0;
          nGoodEl_cutId_veto = 0;
          nGoodEl_cutId_loose = 0;
          nGoodEl_cutId_medium = 0;
          nGoodEl_cutId_tight = 0;

          nGoodMu = 0;
          nGoodMu_loose = 0;
          nGoodMu_cutId_soft = 0;
          nGoodMu_cutId_loose = 0;
          nGoodMu_cutId_medium = 0;
          nGoodMu_cutId_tight = 0;

          for(UInt_t i = 0; i < nLepGood; ++i)
          {
            LepGood_pfAbsIso03_all[i] = LepGood_pfRelIso03_all[i] * LepGood_pt[i];

            if ((year==2018) && ( (std::abs(LepGood_pdgId[i]) == 11) && (LepGood_pt[i] > 30) && (LepGood_eta[i] > -3.0) && (LepGood_eta[i] < -1.4) && (LepGood_phi[i] > -1.57) && (LepGood_phi[i] < -0.87) ) )
              continue;  // Veto events in HEM 15/16

            bool lPTETA = false;
            if(std::abs(LepGood_pdgId[i]) == 13) // If a muon
            {
              lPTETA = LepGood_pt[i] > 3.5;
              lPTETA = lPTETA && (std::abs(LepGood_eta[i]) < 2.4);
            }
            else if(std::abs(LepGood_pdgId[i]) == 11) // If an electron
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
            //bool lID_loose = (std::abs(LepGood_dxy[i]) < 0.03)
            //              && (std::abs(LepGood_dz[i]) < 0.15);
            bool lIS       = LepGood_pfRelIso03_all[i] < 0.2 || LepGood_pfAbsIso03_all[i] < 5.0;
            bool lIS_loose = LepGood_pfRelIso03_all[i] < 0.8 || LepGood_pfAbsIso03_all[i] < 20.0;
            //bool lIS_loose = LepGood_pfRelIso03_all[i] < 0.6 || LepGood_pfAbsIso03_all[i] < 15.0;
            //bool lIS_loose = LepGood_pfRelIso03_all[i] < 0.3 || LepGood_pfAbsIso03_all[i] < 7.5;

            //if(lPTETA && lID && lIS && LepGood_isPFcand[i]==1)
            bool vallep = false;
            if(lPTETA && lID && lIS)
            {
              if(std::abs(LepGood_pdgId[i]) == 13){
                if(LepGood_isPFcand[i]==1){
                  nGoodMu++;
                }
                if(LepGood_tightId[i]){
                  nGoodMu_cutId_tight++;
                  nGoodMu_cutId_medium++;
                  nGoodMu_cutId_loose++;
                  vallep = true;
                }
                else if(LepGood_mediumId[i]){
                  nGoodMu_cutId_medium++;
                  nGoodMu_cutId_loose++;
                  vallep = true;
                }
                //else if(LepGood_looseId[i]){
                else if(LepGood_isPFcand[i]==1 && (LepGood_isGlobal[i] || LepGood_isTracker[i]) ){
                  nGoodMu_cutId_loose++;
                }
                nGoodMu=nGoodMu_cutId_loose;
              }
              else if(std::abs(LepGood_pdgId[i]) == 11){
                if(LepGood_isPFcand[i]==1){
                  nGoodEl++;
                }
                if(LepGood_cutBased[i] == 4){
                  nGoodEl_cutId_tight++;
                  nGoodEl_cutId_medium++;
                  nGoodEl_cutId_loose++;
                  nGoodEl_cutId_veto++;
                  vallep = true;
                }
                else if(LepGood_cutBased[i] == 3){
                  nGoodEl_cutId_medium++;
                  nGoodEl_cutId_loose++;
                  nGoodEl_cutId_veto++;
                  vallep = true;
                }
                else if(LepGood_cutBased[i] == 2){
                  nGoodEl_cutId_loose++;
                  nGoodEl_cutId_veto++;
                  vallep = true;
                }
                else if(LepGood_cutBased[i] == 1){
                  nGoodEl_cutId_veto++;
                }
                nGoodEl=nGoodEl_cutId_veto;
                //else if(LepGood_cutBased[i] == 0 && LepGood_isPFcand[i]==1){
                //  nGoodEl++;
                //}
              }
              if(vallep){
                validLeptons.push_back(i);
              }
            }
            bool ll = false;
            if(lPTETA && lID_loose && lIS_loose)
            {
              //if(std::abs(LepGood_pdgId[i]) == 13 && LepGood_isPFcand[i]==1){
              if(std::abs(LepGood_pdgId[i]) == 13 && LepGood_isPFcand[i]==1 && (LepGood_isGlobal[i] || LepGood_isTracker[i])){
                nGoodMu_loose++;
                ll = true;
              }
              //else if(std::abs(LepGood_pdgId[i]) == 11 && LepGood_isPFcand[i]==1){
              else if(std::abs(LepGood_pdgId[i]) == 11 && LepGood_cutBased[i]>0){
                nGoodEl_loose++;
                ll = true;
              }
              if(ll){
                looseLeptons.push_back(i);
              }
            }
          }

          std::sort(validLeptons.begin(), validLeptons.end(), [&LepGood_pt] (const int &left, const int &right){
            return LepGood_pt[left] > LepGood_pt[right];
          });

          std::sort(looseLeptons.begin(), looseLeptons.end(), [&LepGood_pt] (const int &left, const int &right){
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

/* FOR DEBUGGING
          if(nLepGood>3){
            std::cout << "nLepGood: " << nLepGood << std::endl;
            std::cout << "nJet: " << nJetIn << std::endl;
            std::cout << "validLeptons: ";
            for(size_t lep = 0; lep < validLeptons.size(); lep++){
              std::cout << validLeptons.at(lep) << " ";
            }
            std::cout << std::endl;
            std::cout << "looseLeptons: ";
            for(size_t lep = 0; lep < looseLeptons.size(); lep++){
              std::cout << looseLeptons.at(lep) << " ";
            }
            std::cout << std::endl;
            std::cout << "jetIdx: ";
            for(size_t lep = 0; lep < looseLeptons.size(); lep++){
              std::cout << LepGood_jetIdx[looseLeptons.at(lep)] << " ";
            }
            std::cout << std::endl;
            std::cout << std::endl;
          }
*/

// ***** JETS ******

          for(UInt_t i = 0; i < nJetIn; ++i)
          {
            jetPt.Value().push_back(Jet_pt[i]);
            if(!process.isdata())
            {
              /* This is the old way but it's an unecessary complication
              Jet_corr_JECUp[i] = Jet_pt_jesTotalUp[i] / Jet_pt_raw[i];
              Jet_corr_JECDown[i] = Jet_pt_jesTotalDown[i] / Jet_pt_raw[i];
              jetPt.Systematic("JES_Up").push_back(Jet_pt[i]/Jet_corr_JEC[i]*Jet_corr_JECUp[i]);
              jetPt.Systematic("JES_Down").push_back(Jet_pt[i]/Jet_corr_JEC[i]*Jet_corr_JECDown[i]);
              */
              jetPt.Systematic("JES_Up").push_back(Jet_pt_jesTotalUp[i]);
              jetPt.Systematic("JES_Down").push_back(Jet_pt_jesTotalDown[i]);

              jetPt.Systematic("JER_Up").push_back(Jet_pt_jerUp[i]);
              jetPt.Systematic("JER_Down").push_back(Jet_pt_jerDown[i]);


//              if(Jet_corr_JER[i] < 0)
//              {
//                jetPt.Systematic("JER_Up").push_back(Jet_pt[i]);
//                jetPt.Systematic("JER_Down").push_back(Jet_pt[i]);
//              }
//              else
//              {
                // Almost sure this is wrong
                //Jet_corr_JERUp[i] = Jet_pt_jerUp[i]/Jet_pt[i];
                //Jet_corr_JERDown[i] = Jet_pt_jerDown[i]/Jet_pt[i];
                //jetPt.Systematic("JER_Up").push_back(Jet_pt[i]/Jet_corr_JER[i]*Jet_corr_JERUp[i]);
                //jetPt.Systematic("JER_Down").push_back(Jet_pt[i]/Jet_corr_JER[i]*Jet_corr_JERDown[i]);

                /* This is the old way but it's an unecessary complication
                Jet_corr_JERUp[i] = Jet_pt_jerUp[i] / Jet_pt_nom[i];
                Jet_corr_JERDown[i] = Jet_pt_jerDown[i] / Jet_pt_nom[i];
                jetPt.Systematic("JER_Up").push_back(Jet_pt[i]/Jet_corr_JER[i]*Jet_corr_JERUp[i]);
                jetPt.Systematic("JER_Down").push_back(Jet_pt[i]/Jet_corr_JER[i]*Jet_corr_JERDown[i]);
                */
//                jetPt.Systematic("JER_Up").push_back(Jet_pt_jerUp[i]);
//                jetPt.Systematic("JER_Down").push_back(Jet_pt_jerDown[i]);
//              }

            }
          }
          //genHT = 0;
          for(UInt_t i = 0; i < nGenJet; ++i)
          {
            genJetPt.Value().push_back(GenJet_pt[i]);
          }

          std::vector<std::string> list;
          list.push_back("Value");
          loadSystematics(list, jetPt);

          std::vector<std::string> genList;
          genList.push_back("Value");
          loadSystematics(genList, genJetPt);

          for(auto & syst : list)
          {
            if(syst != "Value")
            {
              std::vector<int> empty;
              allJets.Systematic(syst) = empty;
              validJets.Systematic(syst) = empty;
              eta1p5Jets.Systematic(syst) = empty;
              eta5Jets.Systematic(syst) = empty;
              bjetList.Systematic(syst) = empty;
              //l1PreFiringList.Systematic(syst) = empty;
            }

            float deltaR;
            Int_t looseLep;
            UInt_t jetMask[nJetIn];
            // Clean on Tight Leptons
            //for (size_t lep = 0; lep < validLeptons.size(); lep++) {
              //looseLep = validLeptons.at(lep);
            // Clean on Loose Leptons
            for (size_t lep = 0; lep < looseLeptons.size(); lep++) {
              looseLep = looseLeptons.at(lep);
            // Clean on Any Lepton
            //for (size_t lep = 0; lep < nLepGood; lep++) {
              //looseLep = lep;
              float bestDR = DR_CutOff;
              UInt_t closestJet = 999;
              for(UInt_t jet = 0; jet < nJetIn; ++jet)
              {
                const auto &clJetPt = jetPt.GetSystematicOrValue(syst)[jet]; 
                float lep_jet_ratio = LepGood_pt[looseLep]/clJetPt;
                //if(Jet_pt[jet] > jetPtThreshold){
                //if(Jet_jetId[jet] >= 2 && std::abs(Jet_eta[jet]) < 2.4 && Jet_pt[jet] > jetPtThreshold){
                //if((Jet_jetId[jet] >= 2) && (std::abs(Jet_eta[jet]) < 2.4) && ((jetPt.GetSystematicOrValue(syst))[jet] > jetPtThreshold)){
                if((Jet_jetId[jet] >= 2) && ((jetPt.GetSystematicOrValue(syst))[jet] > jetPtThreshold)){
                  deltaR = DeltaR(LepGood_eta[looseLep], LepGood_phi[looseLep], Jet_eta[jet],Jet_phi[jet]);
                  if ((deltaR < bestDR) && (lep_jet_ratio > 0.5)) {
                    closestJet = jet;
                    bestDR = deltaR;
                  }
                }
              }
              if(closestJet < 999){
                jetMask[closestJet] = 1;
              }
            }

            for(UInt_t jet = 0; jet < nJetIn; ++jet)
            {
              //if ((year==2018) && ((Jet_eta[jet] > -3.2) && (Jet_eta[jet] < -1.2) && (Jet_phi[jet] > -1.77) && (Jet_phi[jet] < -0.67)))
              //  continue;  // Veto events in HEM 15/16

              allJets.GetSystematicOrValue(syst).push_back(jet);

              // Don't consider Jets with DPhiJetMet < 1.5

              double dphiJetMet = DeltaPhi(Jet_phi[jet], MET_phi);

              if((Jet_jetId[jet] >= 2) && ((jetPt.GetSystematicOrValue(syst))[jet] > jetPtThreshold) && (jetMask[jet] != 1) && (dphiJetMet > -1))
              {
                if(std::abs(Jet_eta[jet]) < 2.4){
                  validJets.GetSystematicOrValue(syst).push_back(jet);
                  bjetList.GetSystematicOrValue(syst).push_back(jet);
                }
                if(std::abs(Jet_eta[jet]) < 1.5){
                  eta1p5Jets.GetSystematicOrValue(syst).push_back(jet);
                }
                if(std::abs(Jet_eta[jet]) < 5){
                  eta5Jets.GetSystematicOrValue(syst).push_back(jet);
                }
              }
            }
// DEBUG
/*
	    //if (syst == "JER_Down"){
	    if (syst == "Value" && nJetIn > 10){
	      std::cout << "syst: " << syst << std::endl;
              std::cout << "Njet: " << Njet.Value() << std::endl;
              //std::cout << "VJ size: " << validJets.size() << std::endl;
              std::cout << "nJetIn: " << nJetIn << std::endl;
              std::cout << "allJets.size: " << allJets.GetSystematicOrValue(syst).size() << std::endl;
              std::cout << "allJets: ";
              for(size_t jet = 0; jet < allJets.GetSystematicOrValue(syst).size(); jet++){
                std::cout << allJets.GetSystematicOrValue(syst).at(jet) << " ";
              }
              std::cout << std::endl;
              std::cout << "validJets.size: " << validJets.GetSystematicOrValue(syst).size() << std::endl;
              std::cout << "validJets: ";
              for(size_t jet = 0; jet < validJets.GetSystematicOrValue(syst).size(); jet++){
                std::cout << validJets.GetSystematicOrValue(syst).at(jet) << " ";
		if(validJets.GetSystematicOrValue(syst).size() > 10){
                  std::cout << std::endl;
                  std::cout << " idx: " << jet << " - " << jetPt.GetSystematicOrValue(syst).at(jet) ;
                }
              }
              std::cout << std::endl;
              std::cout << std::endl;
	    }
*/
            std::sort(allJets.GetSystematicOrValue(syst).begin(), allJets.GetSystematicOrValue(syst).end(), [&jetPt, &syst] (const int &left, const int &right) {
              return (jetPt.GetSystematicOrValue(syst))[left] > (jetPt.GetSystematicOrValue(syst))[right];
            });

            std::sort(validJets.GetSystematicOrValue(syst).begin(), validJets.GetSystematicOrValue(syst).end(), [&jetPt, &syst] (const int &left, const int &right) {
              return (jetPt.GetSystematicOrValue(syst))[left] > (jetPt.GetSystematicOrValue(syst))[right];
            });

            std::sort(eta1p5Jets.GetSystematicOrValue(syst).begin(), eta1p5Jets.GetSystematicOrValue(syst).end(), [&jetPt, &syst] (const int &left, const int &right) {
              return (jetPt.GetSystematicOrValue(syst))[left] > (jetPt.GetSystematicOrValue(syst))[right];
            });

            std::sort(eta5Jets.GetSystematicOrValue(syst).begin(), eta5Jets.GetSystematicOrValue(syst).end(), [&jetPt, &syst] (const int &left, const int &right) {
              return (jetPt.GetSystematicOrValue(syst))[left] > (jetPt.GetSystematicOrValue(syst))[right];
            });
            /*
            std::sort(bjetList.GetSystematicOrValue(syst).begin(), bjetList.GetSystematicOrValue(syst).end(), [Jet_btagCSVV2] (const int &left, const int &right) {
              return Jet_btagCSVV2[left] > Jet_btagCSVV2[right];
            });
            */
            std::sort(bjetList.GetSystematicOrValue(syst).begin(), bjetList.GetSystematicOrValue(syst).end(), [Jet_btagDeepB] (const int &left, const int &right) {
              return Jet_btagDeepB[left] > Jet_btagDeepB[right];
            });
            /*
            std::sort(bjetList.GetSystematicOrValue(syst).begin(), bjetList.GetSystematicOrValue(syst).end(), [Jet_btagDeepFlavB] (const int &left, const int &right) {
              return Jet_btagDeepFlavB[left] > Jet_btagDeepFlavB[right];
            });
            */
          }

          for(auto & syst : genList)
          {
            if(syst != "Value")
            {
              std::vector<int> empty;
              genJets.Systematic(syst) = empty;
            }

            for(UInt_t i = 0; i < nGenJet; ++i)
            {
              if(genJetPt.GetSystematicOrValue(syst)[i] > genJetPtThreshold){
                genJets.GetSystematicOrValue(syst).push_back(i);
              }
            }
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

          if(!(process.isdata() || process.issignal() || process.tag()=="VV"))
          {
            std::map<int, std::string>& lheNames = getNanoAODQ2ScaleMap();

            int refLHEKey = -1;
            int refQ2 = -1;
            for(UInt_t i = 0; i < nLHEScaleWeight; ++i)
            {
              std::string name = lheNames[i];
              if(name.substr(0, 2) == "Q2")
              {
                if(name.substr(3,1) == "0") // If it is the reference Q^2 variation
                {
                  refQ2 = i;
                  refLHEKey = i;
                  break;
                }
              }
            }

            Q2Var = 1;
            if(refQ2 >= 0)
            {
              for(UInt_t i = 0; i < nLHEScaleWeight; ++i)
              {
                std::string name = lheNames[i];
                if(name.substr(0, 2) == "Q2")
                {
                  if(name.substr(3,1) == "0") // If it is the reference Q^2 variation
                    continue;

                  double norm = lheScaleMap->at(refLHEKey)/lheScaleMap->at(i);

                  Q2Var.Systematic(name) = norm * LHEScaleWeight[i]/LHEScaleWeight[refQ2];
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

          ValueWithSystematics<double> Jet2EtaDou, Jet2PhiDou, Jet1EtaDou, Jet1PhiDou, eta1p5Jet1EtaDou, eta5Jet1EtaDou;
          Jet2Pt     = loadSysQuantity(jetPt,    validJets, 1);
          JetB2pt    = loadSysQuantity(jetPt,    bjetList,  1);
          Jet2EtaDou = loadQuantity(Jet_eta,     validJets, 1);
          Jet2PhiDou = loadQuantity(Jet_phi,     validJets, 1);
          //Jet2CSV    = loadQuantity(Jet_btagCSVV2, validJets, 1);
          Jet2DeepCSV = loadQuantity(Jet_btagDeepB, validJets, 1);
          //Jet2DeepJet = loadQuantity(Jet_btagDeepFlavB, validJets, 1);
          JetB2eta   = loadQuantity(Jet_eta,     bjetList,  1);
          //JetB2CSV   = loadQuantity(Jet_btagCSVV2, bjetList,  1);
          JetB2DeepCSV   = loadQuantity(Jet_btagDeepB, bjetList,  1);
          //JetB2DeepJet   = loadQuantity(Jet_btagDeepFlavB, bjetList,  1);
          JetB2index = loadQuantity(identity,    bjetList,  1);
          Jet1EtaDou = loadQuantity(Jet_eta,     validJets, 0);
          Jet1PhiDou = loadQuantity(Jet_phi,     validJets, 0);
          eta1p5Jet1EtaDou = loadQuantity(Jet_eta, eta1p5Jets, 0);
          eta5Jet1EtaDou   = loadQuantity(Jet_eta, eta5Jets, 0);
          genJet2Pt  = loadSysQuantity(genJetPt, genJets, 1);

          Jet2Eta = Jet2EtaDou;
          Jet2Phi = Jet2PhiDou;
          Jet1Eta = Jet1EtaDou;
          Jet1Phi = Jet1PhiDou;
          eta1p5Jet1Eta = eta1p5Jet1EtaDou;
          eta5Jet1Eta = eta5Jet1EtaDou;

          //ValueWithSystematics<double> Jet1Phi = loadQuantity(Jet_phi, validJets, 0);
          //ValueWithSystematics<double> Jet2Phi = loadQuantity(Jet_phi, validJets, 1);

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
            LepPhi      = LepGood_phi[leptonIndex];
            LepDxy      = LepGood_dxy[leptonIndex];
            LepDz       = LepGood_dz[leptonIndex];
            LepAbsIso03 = LepGood_pfAbsIso03_all[leptonIndex];
            LepRelIso03 = LepGood_pfRelIso03_all[leptonIndex];
// UNUSED   LepMT       = LepGood_mt[leptonIndex];
            Float_t minPt = 25.;
            LepHybIso03 = LepRelIso03*std::min(LepPt, minPt);
            VLep.SetPtEtaPhiM(LepPt, LepEta, lep_phi, LepGood_mass[leptonIndex]);

            ValueWithSystematics<double> CosDeltaPhiDou = CosDeltaPhiSys(MetPhi, ValueWithSystematics<double>(lep_phi));
            CosDeltaPhi = CosDeltaPhiDou;
            mt = ValueWithSystematics<double>(2.0 * LepPt * MetDou * (1 - CosDeltaPhiDou)).Sqrt();
            Q80 = 1.0 - 80.*80./(2.0 * LepPt * Met);

            if(!process.isdata())
            {
              leptonRecoSF = getLeptonRecoSFSys(LepID, LepPt, LepEta, year);
              leptonIDSF   = getLeptonIDSFSys(LepID, LepPt, LepEta, year);
              leptonISOSF  = getLeptonISOSFSys(LepID, LepPt, LepEta, year);
              //leptonISOSF = getLeptonISOSF2017Sys(LepID, LepPt, LepEta);
              if(sample.isFastsim() || process.isfastsim()) // Maybe need to add some more conditions here
                leptonFullFastSF = getFullFastSFSys(LepID, LepPt, LepEta, year);
              else
                leptonFullFastSF = 1.0;
            }

            if(!process.isdata() && doPromptTagging)
            {
              auto genPartFlav = LepGood_genPartFlav[leptonIndex];
//              bool isPromptFlag = !(genPartFlav == 0 || genPartFlav == 99 || genPartFlav == 100);
              // 1 = prompt electron (including gamma*->mu mu), 15 = electron from prompt tau, 22 = prompt photon (likely conversion), 5 = electron from b, 4 = electron from c, 3 = electron from light or unknown, 0 = unmatched
              // 1 = prompt muon (including gamma*->mu mu), 15 = muon from prompt tau, 5 = muon from b, 4 = muon from c, 3 = muon from light or unknown, 0 = unmatched
              //bool isPromptFlag = !(genPartFlav == 0 || genPartFlav == 3 || genPartFlav == 4 || genPartFlav == 5 || genPartFlav == 15 || genPartFlav == 22);
              //
              bool isPromptFlag;
              if(genPartFlav == 1){
                isPromptFlag = true; 
              }
              else{
                isPromptFlag = false;
              }
              //
              if(!isPromptFlag)
              {
                for(UInt_t genPartIndex = 0; genPartIndex < nGenPart; ++genPartIndex)
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
            LepMT       = -9999;
            LepAbsIso03 = -9999;
            LepRelIso03 = -9999;
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
            Lep2Iso03    = LepGood_pfRelIso03_all[leptonIndex];
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
            //bTagSF = getBTagSFSys(bReader, validJets, jetPt, Jet_eta, Jet_btagCSVV2, Jet_hadronFlavour);
            bTagSF = getBTagSFSys(bReader, validJets, jetPt, Jet_eta, Jet_btagDeepB, Jet_hadronFlavour);
            //bTagSF = getBTagSFSys(bReader, validJets, jetPt, Jet_eta, Jet_btagDeepFlavB, Jet_hadronFlavour);
            triggerEfficiency = triggerEfficiencyFromMETSys(MetDou, year);
            // For EWK ISR, assume syst 100%
            if(process.tag() == "WJets" || process.tag() == "WNJets")
            //if(process.tag() == "WJets" || process.tag() == "WNJets" || process.tag() == "WJetsNLO")
              EWKISRweight = EWKISRCParam * EWKISRweightFromISRpTSys(LepPt, lep_phi, MetDou, MetPhi);
            // For ISR, assume syst 50%
            /*
            if(process.tag() == "ttbar" || process.tag() == "ttbar_lep" || process.tag() == "ttbar_lo" || process.issignal())
              ISRweight = ISRCParam * ISRweightFromNISRJetSys(nIsr);
              */
            if(process.issignal())
              ISRweight = ISRCParam * ISRweightFromNISRJetSys(nIsr);
            // TODO: missing tt_pow reweighting https://twiki.cern.ch/twiki/bin/viewauth/CMS/SUSRecommendationsMoriond17
            if (year==2017)
            {
              l1prefireWeight = getL1preFiringMapsSys(validJets, jetPt, Jet_eta);
            }
            else{
              l1prefireWeight = 1.0;
            }
          }

          if(!process.isdata())
          {
            if(process.tag() == "WJets")
            {
              for(UInt_t genPartIndex = 0; genPartIndex < nGenPart; ++genPartIndex)
              {
                if(std::abs(GenPart_pdgId[genPartIndex])==24 && GenPart_status[genPartIndex] == 22) // W in the intermidiate process
                  {
                    gen_Wpt = GenPart_pt[genPartIndex];
                    //GENWweight = getGenWptWeight(gen_Wpt, sample.tag() ,year);
                  }
              }
            }
            else{
              GENWweight = 1 ;
            }
          }

          ValueWithSystematics<double> JetB1EtaDou, JetB1Phi, Jet1neEmEFDou;
          Jet1Pt     = loadSysQuantity(jetPt,    validJets, 0);
          eta1p5Jet1Pt = loadSysQuantity(jetPt,  eta1p5Jets,0);
          eta5Jet1Pt   = loadSysQuantity(jetPt,  eta5Jets,  0);
          JetHBpt    = loadSysQuantity(jetPt,    bjetList,  0);
          Jet1neEmEFDou = loadQuantity(Jet_neEmEF, validJets, 0);
          //Jet1CSV    = loadQuantity(Jet_btagCSVV2, validJets, 0);
          Jet1DeepCSV    = loadQuantity(Jet_btagDeepB, validJets, 0);
          //Jet1DeepJet    = loadQuantity(Jet_btagDeepFlavB, validJets, 0);
          JetB1EtaDou= loadQuantity(Jet_eta,     bjetList,  0);
          //JetHBCSV   = loadQuantity(Jet_btagCSVV2, bjetList,  0);
          JetHBDeepCSV   = loadQuantity(Jet_btagDeepB, bjetList,  0);
          //JetHBDeepJet   = loadQuantity(Jet_btagDeepFlavB, bjetList,  0);
          JetHBindex = loadQuantity(identity,    bjetList,  0);
          JetB1Phi   = loadQuantity(Jet_phi,     bjetList,  0);
          genJet1Pt  = loadSysQuantity(genJetPt, genJets, 0);

          JetHBeta = JetB1EtaDou;
          Jet1neEmEF = Jet1neEmEFDou;

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
          Jet3Phi    = loadQuantity(Jet_phi,     validJets, 2);
          //Jet3CSV    = loadQuantity(Jet_btagCSVV2, validJets, 2);
          Jet3DeepCSV    = loadQuantity(Jet_btagDeepB, validJets, 2);
          //Jet3DeepJet    = loadQuantity(Jet_btagDeepFlavB, validJets, 2);

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
              //const auto &csv = Jet_btagCSVV2[bjet];
              const auto &csv = Jet_btagDeepB[bjet];
              //const auto &csv = Jet_btagDeepFlavB[bjet];
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

          TLorentzVector vJet;
          HT = 0;
          HTpt = 0;
          HTeta = 0;
          HTphi = 0;
          Njet = validJets.size();
          /*
          Njet30 = 0;
          Njet40 = 0;
          Njet50 = 0;
          Njet60 = 0;
          Njet70 = 0;
          Njet80 = 0;
          Njet90 = 0;
          Njet100 = 0;
          */

          for(auto& syst: list)
          {
            //std::cout << "syst: " << syst << std::endl;
            TLorentzVector vHT;
            TLorentzVector vHTmiss;
            for(auto &jet : validJets.GetSystematicOrValue(syst))
            {
              const auto &pt = jetPt.GetSystematicOrValue(syst)[jet];
              //const auto &eta = Jet_eta[jet];
              //const auto &phi = Jet_phi[jet];

              vJet.SetPtEtaPhiM(pt, Jet_eta[jet], Jet_phi[jet], Jet_mass[jet]);

              vHT += vJet;

              HT.GetSystematicOrValue(syst) += pt;
	      //std::cout << "  eta: " << eta << std::endl;
              /*
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
              */
            }
            HTpt.GetSystematicOrValue(syst) = vHT.Pt();
            HTeta.GetSystematicOrValue(syst) = vHT.Eta();
            HTphi.GetSystematicOrValue(syst) = vHT.Phi();
            vHTmiss = - vHT;
	    //std::cout << " Sum eta: " << HTeta.GetSystematicOrValue(syst) << std::endl;
            HTmiss.GetSystematicOrValue(syst) = HT.GetSystematicOrValue(syst);
            HTmissPt.GetSystematicOrValue(syst) = vHTmiss.Pt();
            HTmissEta.GetSystematicOrValue(syst) = vHTmiss.Eta();
            HTmissPhi.GetSystematicOrValue(syst) = vHTmiss.Phi();
          }

          ValueWithSystematics<double> dphilepmet = DeltaPhiSys(ValueWithSystematics<double>(lep_phi), Met_phi);
          DPhiLepMet = dphilepmet;         
          ValueWithSystematics<double> dphilephtmiss = DeltaPhiSys(ValueWithSystematics<double>(lep_phi), HTmissPhi);
          DPhiLepHTmiss = dphilephtmiss;

          ValueWithSystematics<double> dphijhtmiss = DeltaPhiSys(Jet1Phi, HTmissPhi);
          DPhiJet1HTmiss = dphijhtmiss;
          ValueWithSystematics<double> dphijmet = DeltaPhiSys(Jet1Phi, Met_phi);
          DPhiJet1Met = dphijmet;

          ValueWithSystematics<double> dphij2htmiss = DeltaPhiSys(Jet2Phi, HTmissPhi);
          DPhiJet2HTmiss = dphij2htmiss;
          ValueWithSystematics<double> dphij2met = DeltaPhiSys(Jet2Phi, Met_phi);
          DPhiJet2Met = dphij2met;

          ValueWithSystematics<double> dphij3htmiss = DeltaPhiSys(Jet3Phi, HTmissPhi);
          DPhiJet3HTmiss = dphij3htmiss;
          ValueWithSystematics<double> dphij3met = DeltaPhiSys(Jet3Phi, Met_phi);
          DPhiJet3Met = dphij3met;


          eta1p5HT = 0;

          list.clear();
          list.push_back("Value");
          loadSystematics(list, eta1p5Jets);
          for(auto& syst: list)
          {
            for(auto &jet : eta1p5Jets.GetSystematicOrValue(syst))
            {
              const auto &pt = jetPt.GetSystematicOrValue(syst)[jet];

              eta1p5HT.GetSystematicOrValue(syst) += pt;
            }
          }

          eta5HT = 0;

          list.clear();
          list.push_back("Value");
          loadSystematics(list, eta5Jets);
          for(auto& syst: list)
          {
            for(auto &jet : eta5Jets.GetSystematicOrValue(syst))
            {
              const auto &pt = jetPt.GetSystematicOrValue(syst)[jet];

              eta5HT.GetSystematicOrValue(syst) += pt;
            }
          }

          HT_raw = 0;

          list.clear();
          list.push_back("Value");
          loadSystematics(list, allJets);
          for(auto& syst: list)
          {
            for(auto &jet : allJets.GetSystematicOrValue(syst))
            {
              const auto &pt = jetPt.GetSystematicOrValue(syst)[jet];

              HT_raw.GetSystematicOrValue(syst) += pt;
            }
          }

          genHT = 0;
          Ngenjet = genJets.size();

          genList.clear();
          genList.push_back("Value");

          loadSystematics(genList, genJets);
          for(auto& syst: genList)
          {
            for(auto &jet : genJets.GetSystematicOrValue(syst))
            {
              const auto &pt = genJetPt.GetSystematicOrValue(syst)[jet];
              genHT.GetSystematicOrValue(syst) += pt;
          //    if(pt  > genJetPtThreshold) {
           //     genHT.GetSystematicOrValue(syst) += pt;
            //  }
            }
          }

          //Loop Leptons and done at generator level
          // https://github.com/cms-sw/genproductions/blob/677cd2503f287fa52c21bb9440a2c9e6adcc4ab8/bin/MadGraph5_aMCatNLO/cards/production/2017/13TeV/WJets_updated_ptbin_NLO/Wpt_50To100/WJetsToLNu_012j_Wpt-50To100_5f_NLO_FXFX_cuts.f#L429-L443

          TLorentzVector vGenPart;
          TLorentzVector vGenSt;

          if(!process.isdata())
          {
            if(process.tag() == "WJetsNLO")
            {
              for(UInt_t genPart = 0; genPart < nGenPart; ++genPart)
              {
                bool isLepton = ((std::abs(GenPart_pdgId[genPart])==11) || (std::abs(GenPart_pdgId[genPart])==13) || (std::abs(GenPart_pdgId[genPart])==15));
                bool isNu = ((std::abs(GenPart_pdgId[genPart])==12) || (std::abs(GenPart_pdgId[genPart])==14) || (std::abs(GenPart_pdgId[genPart])==16));
                if((GenPart_status[genPart]==1) && (isLepton || isNu))
                {
                  vGenPart.SetPtEtaPhiM(GenPart_pt[genPart], GenPart_eta[genPart], GenPart_phi[genPart],0); // mass comes from pdgId but for this only pt and phi are needed
                  vGenSt += vGenPart;
                }
              }
              genSt_pt = vGenSt.Pt();
              genSt_eta = vGenSt.Eta();
              genSt_phi = vGenSt.Phi();
              genSt_mass = vGenSt.M();
            }
          }
          
          TLorentzVector vLep;
          TLorentzVector vMet;
          TLorentzVector vSt;
          for (size_t lep = 0; lep < validLeptons.size(); lep++) {
            vLep.SetPtEtaPhiM(LepGood_pt[lep], LepGood_eta[lep], LepGood_phi[lep], LepGood_mass[lep]);
            vSt += vLep;
          }
          vMet.SetPtEtaPhiM(MET_pt, 0, MET_phi, 0);
          vSt += vMet;

          St = vSt.Pt();
          St_phi = vSt.Phi();

          if(process.tag() == "WJetsNLO"){
          //if(process.tag() == "WJetsNLO" && HT.Value() > 260){
            normStweight = normStweightSys(St,year);
            normSTandHTweight = normSTandHTweightSys(St,HT.Value(),year);
            normSTandJet1Ptweight = normSTandJet1PtweightSys(St,Jet1Pt.Value(),year);
            //normSTandHTweight = normStweightSys(Jet_btagDeepFlavB);
          }
          else{
            normStweight = 1;
            normSTandHTweight = 1;
            normSTandJet1Ptweight = 1;
          }

          genStopM       = GenSusyMStop;
          genNeutralinoM = GenSusyMNeutralino;
          int fixStopM = std::round(genStopM / 25.) * 25;
          int fixNeutralinoM = std::round(genNeutralinoM / 5.) * 5;

          Run = run;
          Event = event;
          LumiSec = luminosityBlock;
          if(overrideXSec)
            XS = sample.crossSection();
          else
            XS = xsec;
          if(process.issignal())
            XS = stopCrossSectionNNLO(fixStopM).value();

          //TODO: This should be updated for all samples after the next heppy run
          //2016 HLT
          //if(!skip || process.issignal()){
          if(!skip){
           HLT_PFMET90_PFMHT90                 = HLT_PFMET90_PFMHT90_IDTight;
           HLT_PFMET100_PFMHT100               = HLT_PFMET100_PFMHT100_IDTight;
//           HLT_Ele                             = HLT_Ele25_eta2p1_WPLoose_Gsf;
           //HLT_Mu                              = HLT_IsoMu27;
          }
          else //2017 HLT
          {
//           HLT_PFMET100_PFMHT100_PFHT60        = HLT_PFMET100_PFMHT100_IDTight_PFHT60;
           HLT_PFMET110_PFMHT110               = HLT_PFMET110_PFMHT110_IDTight;
           HLT_PFMET120_PFMHT120               = HLT_PFMET120_PFMHT120_IDTight;
           HLT_PFMET120_PFMHT120_PFHT60        = HLT_PFMET120_PFMHT120_IDTight_PFHT60;
           HLT_PFMETNoMu120_PFMHTNoMu120       = HLT_PFMETNoMu120_PFMHTNoMu120_IDTight;
           HLT_PFMETNoMu120_PFMHTNoMu120_PFHT60= HLT_PFMETNoMu120_PFMHTNoMu120_IDTight_PFHT60;
           HLT_PFHT780                         = HLT_PFHT780_input;
           HLT_PFHT1050                        = HLT_PFHT1050_input;
          }

          HLT_Mu24                            = HLT_IsoMu24;
          HLT_Mu27                            = HLT_IsoMu27;
          HLT_el32                            = HLT_Ele32_WPTight_Gsf;
          HLT_el35                            = HLT_Ele35_WPTight_Gsf;
          METFilters                          = Flag_METFilters;
          HBHENoiseFilter                     = Flag_HBHENoiseFilter;
          HBHENoiseIsoFilter                  = Flag_HBHENoiseIsoFilter;
          eeBadScFilter                       = Flag_eeBadScFilter;
          ecalBadCalibFilterV2                = Flag_ecalBadCalibFilterV2;
          EcalDeadCellTriggerPrimitiveFilter  = Flag_EcalDeadCellTriggerPrimitiveFilter;
          goodVertices                        = Flag_goodVertices;
          globalSuperTightHalo2016Filter      = Flag_globalSuperTightHalo2016Filter;
//          badMuonMoriond2017                  = Flag_badMuonMoriond2017;
//          badCloneMuonMoriond2017             = Flag_badCloneMuonMoriond2017;
          BadPFMuonFilter                     = Flag_BadPFMuonFilter;
          BadChargedCandidateFilter           = Flag_BadChargedCandidateFilter;

          // Filter Efficiency
          if (process.issignal())
          {
            filterEfficiency = getGenFilterEff(fixStopM,fixNeutralinoM,year);
          }
          else
            filterEfficiency = 1.0;
          /*
          if(filterEfficiencyH != nullptr)
          {
            auto theBin = filterEfficiencyH->FindBin(fixStopM, fixNeutralinoM);
            filterEfficiency = filterEfficiencyH->GetBinContent(theBin);
          }
          else
            filterEfficiency = 1.0;
          */

          if(swap)
          {
            auto tmp = Met.Value();
            Met = LepPt;
            LepPt = tmp;
            triggerEfficiency = 1.0f;
          }

          if(!process.isdata())
            //weight = puWeight*XS*filterEfficiency*(genWeight/sumGenWeight)*triggerEfficiency*EWKISRweight*ISRweight*leptonRecoSF*leptonIDSF*leptonISOSF*leptonFullFastSF*Q2Var*bTagSF;
            weight = puWeight*XS*filterEfficiency*(genWeight/sumGenWeight)*triggerEfficiency*normStweight*ISRweight*leptonRecoSF*leptonIDSF*leptonISOSF*leptonFullFastSF*Q2Var*bTagSF*l1prefireWeight;
          else
            weight = 1.0f;

          if(isLooseNotTight)
          {
            //auto efficiency = getLeptonTightLooseRatioSys(LepID, LepPt, LepEta);
            if(doFakeClosure)
            {
              auto efficiency2017MCClosure = getLeptonTightLooseRatio2017MCClosureSys(LepID, LepPt, LepEta);
              looseNotTightWeight2017MCClosure = efficiency2017MCClosure/(1-efficiency2017MCClosure);
              weight *= looseNotTightWeight2017MCClosure;
            }
            else
            {
              auto efficiency = getLeptonTightLooseRatioNanoAODSys(LepID, LepPt, LepEta, year);
              looseNotTightWeight = efficiency/(1-efficiency);
              weight *= looseNotTightWeight;
            }
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
              SyFile << "Run:LS:Ev " << run << ":" << luminosityBlock << ":" << event << std::endl;
              SyFile << "   Mstop: " << genStopM << "; Mlsp: " << genNeutralinoM << std::endl;
              SyFile << "   HT: " << HT.Value() << "; MET: " << Met.Value() << std::endl;
              SyFile << "   Njet: " << Njet.Value() << std::endl;
              for(size_t i = 0; i < validJets.Value().size(); ++i)
              {
//                Float_t Jet_rawPt[JETCOLL_LIMIT];
//                Jet_rawPt[jet] = (1-Jet_rawFactor[jet])*Jet_pt[jet]
                auto jet = validJets.Value()[i];
  //              SyFile << "   jet " << i+1 << ":  pT: " << Jet_pt[jet] << "; eta: " << Jet_eta[jet] << "; raw pT: " << Jet_rawPt[jet] << "; ID: " << Jet_jetId[jet] << "; abs(eta): " << std::abs(Jet_eta[jet]) << std::endl;
                SyFile << "   jet " << i+1 << ":  pT: " << Jet_pt[jet] << "; eta: " << Jet_eta[jet] << "; ID: " << Jet_jetId[jet] << "; abs(eta): " << std::abs(Jet_eta[jet]) << std::endl;
              }
              SyFile << "   Nlep: " << validLeptons.size() << " ( e - " << nGoodEl << "; mu - " << nGoodMu << ")" << std::endl;
              SyFile << "   leading lepton:  pT: " << LepPt << "; eta: " << LepEta << "; phi: " << lep_phi << "; PDG ID: " << LepID << "; RelIso: " << LepRelIso03 << "; dxy: " << LepDxy << "; dz: " << LepDz << std::endl;
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
              //if(luminosityBlock == 91151 && event == 195485531)
              if(true)
              {
                SyFile << std::endl << "nLepGood: " << nLepGood << std::endl;
                SyFile << "LepGood:" << std::endl;
                for(UInt_t i = 0; i < nLepGood; ++i)
                  SyFile << "   lep " << i+1 << ": ID: " << LepGood_pdgId[i] << "; pt: " << LepGood_pt[i] << "; eta:" << LepGood_eta[i] << "; phi: " << LepGood_phi[i] << "; relIso03: " << LepGood_pfRelIso03_all[i] << "; pdgID: " << LepGood_pdgId[i] << "; dxy: " << LepGood_dxy[i] << "; dz: " << LepGood_dz[i] << std::endl;
                SyFile << "nJetGood: " << nJetIn << std::endl;
                for(UInt_t i = 0; i < nJetIn; ++i)
                  SyFile << "   jet " << i+1 << ": pt: " << Jet_pt[i] << "; eta: " << Jet_eta[i] << "; phi: " << Jet_phi[i] << "; mass: " << Jet_mass[i] << "; ID: " << Jet_jetId[i] << std::endl;
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
          //
          if(doJetHT)
          {
            if(!static_cast<bool>(MET_pt < 60))
              continue;
          }
          if(doSingleEl)
          {
            if(!static_cast<bool>(Jet1Pt >= ISR_JET_PT))
              continue;
          }
          //
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
            if ( goodVertices                       != 1 ) continue;
            if (!process.issignal())
              if ( globalSuperTightHalo2016Filter   != 1 ) continue;
            if ( HBHENoiseFilter                    != 1 ) continue;
            if ( HBHENoiseIsoFilter                 != 1 ) continue;
            if ( EcalDeadCellTriggerPrimitiveFilter != 1 ) continue;
            if ( BadPFMuonFilter                    != 1 ) continue;
            if ( ecalBadCalibFilterV2               != 1 ) continue;
            if (process.isdata())
              if ( eeBadScFilter                      != 1 ) continue;
            // Should probably only use 1 of these two
            //if ( badMuonMoriond2017                 != 1 ) continue;
//            if ( badCloneMuonMoriond2017            != 1 ) continue; // This one removes some of the spikes in QCD for some reason
            //if ( BadChargedCandidateFilter             != 1 ) continue;

            bool passHLT = false;
            if(!swap) // Normally use the MET triggers, but only for data
            {
             //TODO: This should be updated for all samples after the next heppy run
              if(process.isdata() && !doJetHT && !doSingleEl)
              {
                /* Using onlu Unprescaled triggers for 2017 Data
                if(HLT_PFMET90_PFMHT90_IDTight != 0)
                  passHLT = true;
                if(HLT_PFMET100_PFMHT100_IDTight_PFHT60 != 0)
                  passHLT = true;
                if(HLT_PFMET110_PFMHT110_IDTight != 0)
                  passHLT = true;
                */
                if(HLT_PFMET120_PFMHT120_IDTight != 0)
                  passHLT = true;
                if(HLT_PFMET120_PFMHT120_IDTight_PFHT60 != 0)
                  passHLT = true;
              }
              else if (process.isdata() && doJetHT) {
                if(HLT_PFHT1050 != 0)
                  passHLT = true;
              }
              else if (process.isdata() && doSingleEl) {
                if(year==2017 && HLT_el35 != 0){
                  passHLT = true;
                }
                if(year==2018 && HLT_el32 != 0){
                  passHLT = true;
                }
              }
              else
                passHLT = true;
            }
            /*
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
            */
            if(!passHLT)
              continue;
          }

          bool vetoJetHEM = false;
          list.clear();
          list.push_back("Value");
          loadSystematics(list, jetPt);

          if(year==2018){ // Veto events in HEM 15/16
            for(auto & syst : list)
            {
              for(int jet : validJets.GetSystematicOrValue(syst))
              {
                //double pt = jetPt.GetSystematicOrValue("Value")[jet];
                double eta = Jet_eta[jet];
                double phi = Jet_phi[jet];

                ValueWithSystematics<double> dphijethtmiss = DeltaPhiSys(ValueWithSystematics<double>(phi), HTmissPhi);
                if(dphijethtmiss <  0.5){
                  if((eta > -3.2) && (eta < -1.2) && (phi > -1.77) && (phi < -0.67)){
                    vetoJetHEM = true;
                  }
                }
              }
            }
          }
         if(vetoJetHEM)
           continue;

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

      /*
      if(filterEfficiencyH != nullptr)
        delete filterEfficiencyH;
      */
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
