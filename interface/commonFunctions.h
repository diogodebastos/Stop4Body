#ifndef MY_COMMON_FUNCTION
#define MY_COMMON_FUNCTION

#include "UserCode/Stop4Body-nanoAOD/interface/doubleWithUncertainty.h"
#include "UserCode/Stop4Body-nanoAOD/interface/ValueWithSystematics.h"

#include "CondFormats/BTauObjects/interface/BTagCalibration.h"
#include "CondTools/BTau/interface/BTagCalibrationReader.h"

#include "TH1.h"
#include "TH2.h"
#include "TStyle.h"
#include <string>

extern TH2D* centralElectronRecoSFHist2018;
extern TH2D* centralElectronSFHist2018;
extern TH2D* TnPlowPtIDSFHist2018;
extern TH2D* elFullFastIDSFHist2018;
//extern TH2D* elFullFastHIIPSFHist2018;
extern TH2D* centralMuonSFHist2018;
extern TH2D* lowMuonSFHist2018;
extern TH2D* muFullFastIDSFHist2018;
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

extern TH2D* centralElectronRecoSFHist2017;
extern TH2D* centralElectronRecoSFHist2017_lowEt;
extern TH2D* centralElectronSFHist2017;
extern TH2D* TnPlowPtIDSFHist2017;
extern TH2D* elFullFastIDSFHist2017;
// extern TH2D* elFullFastHIIPSFHist2017;
extern TH2D* centralMuonSFHist2017;
extern TH2D* lowMuonSFHist2017;
extern TH2D* muFullFastIDSFHist2017;
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
extern TH2D* electronFullFastSFIDHist;
extern TH2D* electronFullFastSFHIIPHist;
extern TH2D* muonFullFastSFIDHist;
extern TH2D* muonFullFastSFHIIPHist;

extern TH1D* weightsSt_2017;
extern TH1D* weightsDeepCSV_2017;

bool fileExists(std::string);
std::string getBaseName(const std::string&);
float DeltaPhi(double, double);
float DeltaR(double, double ,double ,double );

bool replace(std::string&, const std::string&, const std::string&);

doubleUnc triggerEfficiencyFromMET(double, int);
doubleUnc WISRScaleFactorFromLepMet(double lep_pt, double lep_phi, double met_pt, double met_phi);
doubleUnc ISRweightFromNISRJet(int nISRJet);
doubleUnc EWKISRweightFromISRpT(double ISRpT);
doubleUnc EWKISRweightFromISRpT(double lep_pt, double lep_phi, double met_pt, double met_phi);
doubleUnc getLeptonRecoSF(double LepID, double LepPt, double LepEta, int year);
doubleUnc getLeptonIDSF(double LepID, double LepPt, double LepEta, int year);
doubleUnc getLeptonISOSF(double LepID, double LepPt, double LepEta);
doubleUnc getLeptonIDSF2016(double LepID, double LepPt, double LepEta);
doubleUnc getLeptonISOSF2016(double LepID, double LepPt, double LepEta);
doubleUnc getLeptonTightLooseRatio2017(double LepID, double LepPt, double LepEta);
doubleUnc getLeptonTightLooseRatio(double LepID, double LepPt, double LepEta);
doubleUnc stopCrossSectionNNLO(int stopM);
doubleUnc stopCrossSectionNLO(double stopM, double lspM);

ValueWithSystematics<double> triggerEfficiencyFromMETSys(double);
ValueWithSystematics<double> triggerEfficiencyFromMETSys(ValueWithSystematics<double>);
ValueWithSystematics<double> WISRScaleFactorFromLepMetSys(double lep_pt, double lep_phi, double met_pt, double met_phi);
ValueWithSystematics<double> getGenWptWeight(double genW_pt, std::string sample, int year);
ValueWithSystematics<double> ISRweightFromNISRJetSys(int nISRJet);
ValueWithSystematics<double> EWKISRweightFromISRpTSys(double ISRpT);
ValueWithSystematics<double> EWKISRweightFromISRpTSys(double lep_pt, double lep_phi, double met_pt, double met_phi);
ValueWithSystematics<double> EWKISRweightFromISRpTSys(ValueWithSystematics<double> ISRpT);
ValueWithSystematics<double> EWKISRweightFromISRpTSys(double lep_pt, double lep_phi, ValueWithSystematics<double> met_pt, ValueWithSystematics<double> met_phi);
ValueWithSystematics<double> getLeptonRecoSFSys(double LepID, double LepPt, double LepEta, int year);
ValueWithSystematics<double> getLeptonIDSFSys(double LepID, double LepPt, double LepEta, int year);
ValueWithSystematics<double> getLeptonISOSFSys(double LepID, double LepPt, double LepEta);
ValueWithSystematics<double> getLeptonIDSFSys2016(double LepID, double LepPt, double LepEta);
ValueWithSystematics<double> getLeptonISOSF2016Sys(double LepID, double LepPt, double LepEta)
;
ValueWithSystematics<double> getLeptonTightLooseRatioNanoAODSys(double LepID, double LepPt, double LepEta, int year);
ValueWithSystematics<double> getLeptonTightLooseRatio2017MCClosureSys(double LepID, double LepPt, double LepEta);
ValueWithSystematics<double> getL1preFiringMapsSys(ValueWithSystematics<std::vector<int>>& validJets, ValueWithSystematics<std::vector<double>>& jetPt, Float_t* Jet_eta);
ValueWithSystematics<double> getLeptonIDSFSys(double LepID, double LepPt, double LepEta);
ValueWithSystematics<double> getLeptonISOSFSys(double LepID, double LepPt, double LepEta);
ValueWithSystematics<double> getFullFastSFSys(double LepID, double LepPt, double LepEta, int year);
ValueWithSystematics<double> getLeptonTightLooseRatioSys(double LepID, double LepPt, double LepEta);

ValueWithSystematics<double> getBTagSFSys(BTagCalibrationReader& bReader, ValueWithSystematics<std::vector<int>>& validJets, ValueWithSystematics<std::vector<double>>& jetPt, Float_t* Jet_eta, Float_t* Jet_btagCSV, Int_t* Jet_hadronFlavour);

ValueWithSystematics<double> normStweightSys(Float_t St, int year);
ValueWithSystematics<double> normSTandHTweightSys(Float_t St, double HT,int year);
ValueWithSystematics<double> normSTandJet1PtweightSys(Float_t St, double Jet1Pt,int year);


TStyle* getTDRStyle();

//ValueWithSystematics<double> stopCrossSectionSys(double stopM, double lspM);

#endif
