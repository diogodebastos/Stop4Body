#ifndef MY_COMMON_FUNCTION
#define MY_COMMON_FUNCTION

#include "UserCode/Stop4Body/interface/doubleWithUncertainty.h"

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

bool fileExists(std::string);
float DeltaPhi(double, double);

doubleUnc triggerEfficiencyFromMET(double);
doubleUnc WISRScaleFactorFromLepMet(double lep_pt, double lep_phi, double met_pt, double met_phi);
doubleUnc ISRweightFromNISRJet(int nISRJet);
doubleUnc EWKISRweightFromISRpT(double ISRpT);
doubleUnc EWKISRweightFromISRpT(double lep_pt, double lep_phi, double met_pt, double met_phi);
doubleUnc getLeptonIDSF(double LepID, double LepPt, double LepEta);
doubleUnc getLeptonISOSF(double LepID, double LepPt, double LepEta);

doubleUnc stopCrossSection(double stopM, double lspM);

#endif
