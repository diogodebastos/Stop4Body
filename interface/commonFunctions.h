#ifndef MY_COMMON_FUNCTION
#define MY_COMMON_FUNCTION

#include "UserCode/Stop4Body/interface/doubleWithUncertainty.h"
#include "UserCode/Stop4Body/interface/ValueWithSystematics.h"

#include "TH1.h"
#include "TH2.h"

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

bool fileExists(std::string);
std::string getBaseName(const std::string&);
float DeltaPhi(double, double);

doubleUnc triggerEfficiencyFromMET(double);
doubleUnc WISRScaleFactorFromLepMet(double lep_pt, double lep_phi, double met_pt, double met_phi);
doubleUnc ISRweightFromNISRJet(int nISRJet);
doubleUnc EWKISRweightFromISRpT(double ISRpT);
doubleUnc EWKISRweightFromISRpT(double lep_pt, double lep_phi, double met_pt, double met_phi);
doubleUnc getLeptonIDSF(double LepID, double LepPt, double LepEta);
doubleUnc getLeptonISOSF(double LepID, double LepPt, double LepEta);

doubleUnc getLeptonTightLooseRatio(double LepID, double LepPt, double LepEta);

doubleUnc stopCrossSection(double stopM, double lspM);



ValueWithSystematics<double> triggerEfficiencyFromMETSys(double);
ValueWithSystematics<double> triggerEfficiencyFromMETSys(ValueWithSystematics<double>);
ValueWithSystematics<double> WISRScaleFactorFromLepMetSys(double lep_pt, double lep_phi, double met_pt, double met_phi);
ValueWithSystematics<double> ISRweightFromNISRJetSys(int nISRJet);
ValueWithSystematics<double> EWKISRweightFromISRpTSys(double ISRpT);
ValueWithSystematics<double> EWKISRweightFromISRpTSys(double lep_pt, double lep_phi, double met_pt, double met_phi);
ValueWithSystematics<double> EWKISRweightFromISRpTSys(ValueWithSystematics<double> ISRpT);
ValueWithSystematics<double> EWKISRweightFromISRpTSys(double lep_pt, double lep_phi, ValueWithSystematics<double> met_pt, ValueWithSystematics<double> met_phi);
ValueWithSystematics<double> getLeptonIDSFSys(double LepID, double LepPt, double LepEta);
ValueWithSystematics<double> getLeptonISOSFSys(double LepID, double LepPt, double LepEta);

ValueWithSystematics<double> getLeptonTightLooseRatioSys(double LepID, double LepPt, double LepEta);

//ValueWithSystematics<double> stopCrossSectionSys(double stopM, double lspM);

#endif
