#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <list>
#include <iterator>
#include <algorithm>
#include <iomanip>
#include "TROOT.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TChain.h"
#include "TObject.h"
#include "TCanvas.h"
#include "TMath.h"
#include <math.h>
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

float DeltaPhi(float p1, float p2) {

  float x = p1 - p2;
  if (x >= TMath::Pi()) {x -= (2.*TMath::Pi());}
  if (x < -TMath::Pi()) {x += (2.*TMath::Pi());}
  return abs(x);

}

void addBranch(TString fileinput, TString fileoutput) {

  TFile finput(fileinput);
  TFile foutput(fileoutput,"recreate");
 
  TTree *inputtree = (TTree*)finput.Get("tree");
  TTree *bdttree= new TTree("bdttree","bdttree");

  // Read Branches you are interested in from the input tree
  Float_t mtw1;        inputtree->SetBranchAddress("mtw1"       , &mtw1);
  Float_t mtw2;        inputtree->SetBranchAddress("mtw2"       , &mtw2);
  Float_t mtw;        inputtree->SetBranchAddress("mtw"       , &mtw);
  Float_t met_pt;      inputtree->SetBranchAddress("met_pt"    , &met_pt);
  Float_t met_phi;     inputtree->SetBranchAddress("met_phi",   &met_phi);
  Int_t nBJetLoose30; inputtree->SetBranchAddress("nBJetLoose30"       , &nBJetLoose30);
  Int_t nBJetTight30; inputtree->SetBranchAddress("nBJetTight30"       , &nBJetTight30);
  Int_t nLepGood;      inputtree->SetBranchAddress("nLepGood"   , &nLepGood);
  Int_t LepGood_pdgId[40];  inputtree->SetBranchAddress("LepGood_pdgId", &LepGood_pdgId);
  Int_t LepGood_mediumMuonId[40]; inputtree->SetBranchAddress("LepGood_mediumMuonId",&LepGood_mediumMuonId);
  Float_t LepGood_pt[40];  inputtree->SetBranchAddress("LepGood_pt", &LepGood_pt);
  Float_t LepGood_eta[40];  inputtree->SetBranchAddress("LepGood_eta", &LepGood_eta);
  Float_t LepGood_phi[40];  inputtree->SetBranchAddress("LepGood_phi", &LepGood_phi);
  Float_t LepGood_relIso03[40]; inputtree->SetBranchAddress("LepGood_relIso03",&LepGood_relIso03);
  Float_t LepGood_relIso04[40]; inputtree->SetBranchAddress("LepGood_relIso04",&LepGood_relIso04);
  Float_t LepGood_dxy[40]; inputtree->SetBranchAddress("LepGood_dxy",&LepGood_dxy);
  Float_t LepGood_dz[40]; inputtree->SetBranchAddress("LepGood_dz",&LepGood_dz);
  Float_t LepGood_sip3d[40]; inputtree->SetBranchAddress("LepGood_sip3d",&LepGood_sip3d);
  Float_t LepGood_mass[40]; inputtree->SetBranchAddress("LepGood_mass",&LepGood_mass);
  Int_t LepGood_charge[40]; inputtree->SetBranchAddress("LepGood_charge",&LepGood_charge);
  Int_t nLepOther;      inputtree->SetBranchAddress("nLepOther"   , &nLepOther);
  Int_t LepOther_pdgId[40];  inputtree->SetBranchAddress("LepOther_pdgId", &LepOther_pdgId);
  Int_t LepOther_mediumMuonId[40]; inputtree->SetBranchAddress("LepOther_mediumMuonId",&LepOther_mediumMuonId);
  Float_t LepOther_pt[40];  inputtree->SetBranchAddress("LepOther_pt", &LepOther_pt);
  Float_t LepOther_eta[40];  inputtree->SetBranchAddress("LepOther_eta", &LepOther_eta);
  Float_t LepOther_phi[40];  inputtree->SetBranchAddress("LepOther_phi", &LepOther_phi);
  Float_t LepOther_relIso03[40]; inputtree->SetBranchAddress("LepOther_relIso03",&LepOther_relIso03);
  Float_t LepOther_relIso04[40]; inputtree->SetBranchAddress("LepOther_relIso04",&LepOther_relIso04);
  Float_t LepOther_dxy[40]; inputtree->SetBranchAddress("LepOther_dxy",&LepOther_dxy);
  Float_t LepOther_dz[40]; inputtree->SetBranchAddress("LepOther_dz",&LepOther_dz);
  Float_t LepOther_sip3d[40]; inputtree->SetBranchAddress("LepOther_sip3d",&LepOther_sip3d);
  Float_t LepOther_mass[40]; inputtree->SetBranchAddress("LepOther_mass",&LepOther_mass);
  Int_t LepOther_charge[40]; inputtree->SetBranchAddress("LepOther_charge",&LepOther_charge);
  Float_t Jet_chEMEF[40];  inputtree->SetBranchAddress("Jet_chEMEF", &Jet_chEMEF);
  Float_t Jet_neEMEF[40];  inputtree->SetBranchAddress("Jet_neEMEF", &Jet_neEMEF);
  Float_t Jet_chHEF[40];  inputtree->SetBranchAddress("Jet_chHEF", &Jet_chHEF);
  Float_t Jet_neHEF[40];  inputtree->SetBranchAddress("Jet_neHEF", &Jet_neHEF);
  Float_t Jet_pt[40];  inputtree->SetBranchAddress("Jet_pt", &Jet_pt);
  Float_t Jet_eta[40];  inputtree->SetBranchAddress("Jet_eta", &Jet_eta);
  Float_t Jet_phi[40];  inputtree->SetBranchAddress("Jet_phi", &Jet_phi);
  Float_t Jet_btagCSV[40];  inputtree->SetBranchAddress("Jet_btagCSV", &Jet_btagCSV);
  Float_t Jet_mass[40];  inputtree->SetBranchAddress("Jet_mass", &Jet_mass);
  Float_t xsec; inputtree->SetBranchAddress("xsec",&xsec);
  Int_t nJet20;  inputtree->SetBranchAddress("nJet20", &nJet20);
  Int_t nJet30;  inputtree->SetBranchAddress("nJet30", &nJet30);
  UInt_t run;  inputtree->SetBranchAddress("run", &run);
  ULong64_t evt;  inputtree->SetBranchAddress("evt", &evt);
  UInt_t lumi;  inputtree->SetBranchAddress("lumi", &lumi);

  // New branch in bdt tree
  Int_t LepID;  TBranch* bdtLepID = bdttree->Branch("LepID",&LepID,"LepID/I");
  Int_t LepChg;  TBranch* bdtLepChg = bdttree->Branch("LepChg",&LepChg,"LepChg/I");
  Float_t LepPt;  TBranch* bdtLepPt = bdttree->Branch("LepPt",&LepPt,"LepPt/F");
  Float_t LepEta;  TBranch* bdtLepEta = bdttree->Branch("LepEta",&LepEta,"LepEta/F");
  Float_t LepDxy;  TBranch* bdtLepDxy = bdttree->Branch("LepDxy",&LepDxy,"LepDxy/F");
  Float_t LepDz;  TBranch* bdtLepDz = bdttree->Branch("LepDz",&LepDz,"LepDz/F");
  Float_t LepSip3;  TBranch* bdtLepSip3 = bdttree->Branch("LepSip3",&LepSip3,"LepSip3/F");
  Float_t LepIso03;  TBranch* bdtLepIso03 = bdttree->Branch("LepIso03",&LepIso03,"LepIso03/F");
  Float_t LepIso04;  TBranch* bdtLepIso04 = bdttree->Branch("LepIso04",&LepIso04,"LepIso04/F");
  Int_t nGoodMu;  TBranch* bdtnGoodMu = bdttree->Branch("nGoodMu",&nGoodMu,"nGoodMu/I");
  Int_t nGoodEl;  TBranch* bdtnGoodEl = bdttree->Branch("nGoodEl",&nGoodEl,"nGoodEl/I");
  Float_t Met; TBranch* bdtMet= bdttree->Branch("Met",&Met,"Met/F");
  Float_t mt; TBranch* bdtmt = bdttree->Branch("mt",&mt,"mt/F");
  Float_t Q80; TBranch *bdtQ80 = bdttree-> Branch("Q80",&Q80,"Q80/F");
  Float_t CosDeltaPhi; TBranch *bdtCosDeltaPhi = bdttree->Branch("CosDeltaPhi",&CosDeltaPhi,"CosDeltaPhi/F");
  Int_t NbLoose30; TBranch* bdtNbLoose30 = bdttree->Branch("NbLoose30",&NbLoose30,"NbLoose30/I"); 
  Int_t NbTight30;  TBranch* bdtNbTight30 = bdttree->Branch("NbTight30",&NbTight30,"NbTight30/I");
  Int_t Njet;  TBranch* bdtNjet = bdttree->Branch("Njet",&Njet,"Njet/I");
  Float_t Jet1Pt;  TBranch* bdtJet1Pt = bdttree->Branch("Jet1Pt",&Jet1Pt,"Jet1Pt/F");
  Float_t Jet1Eta;  TBranch* bdtJet1Eta = bdttree->Branch("Jet1Eta",&Jet1Eta,"Jet1Eta/F");
  Float_t Jet2Pt;  TBranch* bdtJet2Pt = bdttree->Branch("Jet2Pt",&Jet2Pt,"Jet2Pt/F");
  Float_t Jet2Eta;  TBranch* bdtJet2Eta = bdttree->Branch("Jet2Eta",&Jet2Eta,"Jet2Eta/F");
  Float_t DPhiJet1Jet2;  TBranch* bdtDPhiJet1Jet2 = bdttree->Branch("DPhiJet1Jet2",&DPhiJet1Jet2,"DPhiJet1Jet2/F");
  Float_t JetHBpt;  TBranch* bdtJetHBpt = bdttree->Branch("JetHBpt",&JetHBpt,"JetHBpt/F");
  Float_t DrJet1Lep;  TBranch* bdtDrJet1Lep = bdttree->Branch("DrJet1Lep",&DrJet1Lep,"DrJet1Lep/F");
  Float_t DrJet2Lep;  TBranch* bdtDrJet2Lep = bdttree->Branch("DrJet2Lep",&DrJet2Lep,"DrJet2Lep/F");
  Float_t DrJetHBLep;  TBranch* bdtDrJetHBLep = bdttree->Branch("DrJetHBLep",&DrJetHBLep,"DrJetHBLep/F");
  Float_t DrJet1Jet2;  TBranch* bdtDrJet1Jet2 = bdttree->Branch("DrJet1Jet2",&DrJet1Jet2,"DrJet1Jet2/F");
  Float_t JetLepMass;  TBranch* bdtJetLepMass = bdttree->Branch("JetLepMass",&JetLepMass,"JetLepMass/F");
  Float_t J3Mass;  TBranch* bdtJ3Mass = bdttree->Branch("J3Mass",&J3Mass,"J3Mass/F");
  Float_t HT20;  TBranch* bdtHT20 = bdttree->Branch("HT20",&HT20,"HT20/F");
  Float_t HT25;  TBranch* bdtHT25 = bdttree->Branch("HT25",&HT25,"HT25/F");
  Float_t HT30;  TBranch* bdtHT30 = bdttree->Branch("HT30",&HT30,"HT30/F");
  Float_t XS; TBranch* bdtXS=bdttree->Branch("XS",&XS,"XS/F");;
  Int_t Run;  TBranch* bdtRun = bdttree->Branch("Run",&Run,"Run/I");
  Int_t Event;  TBranch* bdtEvent = bdttree->Branch("Event",&Event,"Event/I");
  Int_t LumiSec;  TBranch* bdtLumiSec = bdttree->Branch("LumiSec",&LumiSec,"LumiSec/I");
  Int_t Nevt;  TBranch* bdtNevt = bdttree->Branch("Nevt",&Nevt,"Nevt/I");

  cout.setf(ios::floatfield,ios::fixed);
  cout<<setprecision(1);

  ofstream SyFile ("Synch_mu_Cut4_S.txt");
  SyFile.setf(ios::fixed);
  SyFile.precision(3);

  // Read the number of entries in the inputtree
  Int_t nentries = (Int_t)inputtree->GetEntries();
  for (Int_t i = 0; i < nentries; i++){
    inputtree->GetEntry(i);

    // Preselection

    nGoodMu = 0;
    nGoodEl = 0;
    for (Int_t l = 0; l < nLepGood; l++){
      bool lPTETA = (5.<LepGood_pt[l])&&(LepGood_pt[l]<30.)&&(abs(LepGood_eta[l])<2.4);
      bool lID = (abs(LepGood_dxy[l])<0.05)&&(abs(LepGood_dz[l])<0.2)&&(LepGood_sip3d[l]<4.);
      bool lIS = ((LepGood_pt[l]>=25.)&&(LepGood_relIso04[l]<0.2))||((LepGood_pt[l]<25.)&&((LepGood_pt[l]*LepGood_relIso04[l])<5.));
      if (lPTETA&&lID&&lIS){
	if ((abs(LepGood_pdgId[l])==13)&&(LepGood_mediumMuonId[l]==1)) {nGoodMu += 1;}
	if (abs(LepGood_pdgId[l])==11) {nGoodEl += 1;}
      }
    }

    // Set the value of the branches in the bdttree
    Nevt = nentries;
    mt=mtw;
    Q80=mtw1;
    CosDeltaPhi=mtw2;
    Met=met_pt;
    NbLoose30=nBJetLoose30;
    NbTight30=nBJetTight30;

    float DrJetLepMax = 999.;
    Int_t ij = 0;
    for (Int_t j = 0; j < nJet20; j++){
      //    for (Int_t j = 0; j < nJet30; j++){
      float dpi = DeltaPhi(Jet_phi[j],LepGood_phi[0]);
      float dei = Jet_eta[j]-LepGood_eta[0];
      float dri = sqrt( pow(dpi,2) + pow(dei,2) );
      if (dri < DrJetLepMax){
	DrJetLepMax = dri;
	ij = j;
      }
    }
    TLorentzVector VJ, VLep, JLep;
    VLep.SetPtEtaPhiM(LepGood_pt[0], LepGood_eta[0], LepGood_phi[0], LepGood_mass[0]);
    VJ.SetPtEtaPhiM(Jet_pt[ij], Jet_eta[ij], Jet_phi[ij], Jet_mass[ij]);
    JLep = VJ + VLep;
    JetLepMass = JLep.M();

    TLorentzVector VJ3i, VJ3;
    for (Int_t j = 0; j < nJet20; j++){
    //    for (Int_t j = 0; j < nJet30; j++){
      if (j == ij) {continue;}
      VJ3i.SetPtEtaPhiM(Jet_pt[j], Jet_eta[j], Jet_phi[j], Jet_mass[j]);
      VJ3 += VJ3i;
    }
    J3Mass = VJ3.M();

    if (nJet20>=1){
    //    if (nJet30>=1){
      Jet1Pt=Jet_pt[0];
      Jet1Eta=Jet_eta[0];
      float dphi = DeltaPhi(Jet_phi[0],LepGood_phi[0]);
      float deta = Jet_eta[0]-LepGood_eta[0];
      DrJet1Lep=sqrt( pow(dphi,2) + pow(deta,2) );
    }
    else{
      Jet1Pt = -999.;
      Jet1Eta = -999.;
      DrJet1Lep = -999.;
      Jet2Pt = -999.;
      Jet2Eta = -999.;
    }

    if (nJet20>=2){
    //    if (nJet30>=2){
      float dphijj, detajj;
      Jet2Pt=Jet_pt[1];
      Jet2Eta=Jet_eta[1];
      dphijj = DeltaPhi(Jet_phi[0],Jet_phi[1]);
      detajj = Jet_eta[0]-Jet_eta[1];
      DPhiJet1Jet2 = dphijj;
      DrJet1Jet2 = sqrt( pow(dphijj,2) + pow(detajj,2) );

      float dphi = DeltaPhi(Jet_phi[1],LepGood_phi[0]);
      float deta = Jet_eta[1]-LepGood_eta[0];
      DrJet2Lep=sqrt( pow(dphi,2) + pow(deta,2) );
    }
    else{
      Jet2Pt = -999.;
      Jet2Eta = -999.;
      DPhiJet1Jet2 = -999.;
      DrJet1Jet2 = -999.;
      DrJet2Lep = -999.;
    }

    JetHBpt = -999.;
    DrJetHBLep = -999.;
    Float_t BtagMax = -999.;
    Int_t iBtag = 0;
    for (Int_t j = 0; j < nJet20; j++){
    //    for (Int_t j = 0; j < nJet30; j++){
      if ((Jet_btagCSV[j]!=-10)&&(Jet_btagCSV[j]>BtagMax)){
	BtagMax = Jet_btagCSV[j];
	iBtag = j;
      }
    }
    JetHBpt = Jet_pt[iBtag];
    float dphib, detab;
    dphib = DeltaPhi(Jet_phi[iBtag],LepGood_phi[0]);
    detab = Jet_eta[iBtag]-LepGood_eta[0];
    DrJetHBLep = sqrt( pow(dphib,2) + pow(detab,2) );

    LepChg=LepGood_charge[0];
    LepID=LepGood_pdgId[0];
    LepPt=LepGood_pt[0];
    LepEta=LepGood_eta[0];
    LepDxy=LepGood_dxy[0];
    LepDz=LepGood_dz[0];
    LepSip3=LepGood_sip3d[0];
    LepIso03=LepGood_relIso03[0];
    LepIso04=LepGood_relIso04[0];
    HT20 = 0.;
    HT25 = 0.;
    HT30 = 0.;
    Njet = 0;
    for (Int_t j = 0; j < nJet20; j++) {
      if (Jet_pt[j] > 20.) { 
	HT20 += Jet_pt[j];
	Njet += 1;
      }
      if (Jet_pt[j] > 25.) { HT25 += Jet_pt[j];}
      if (Jet_pt[j] > 30.) {
	HT30 += Jet_pt[j];
	//	Njet += 1;
      }
    }
    //    HT20 += LepPt;
    //    HT25 += LepPt;
    //    HT30 += LepPt;
    Run=run;
    Event=evt;
    LumiSec=lumi;
    XS=xsec;

    // Skim
    bool emu = (nGoodMu == 1)||(nGoodEl == 1);
    bool isISR = (Jet_pt[0]>110.)&&(Njet>0);
    bool dPhi = DPhiJet1Jet2 < 2.5;
    bool met = Met > 100.;
    if (!emu) continue;
    if (!isISR) continue;
    if (!dPhi) continue;
    if (!met) continue;

    // Synch
    if (abs(LepGood_pdgId[0])==13){
      SyFile << "Run:LS:Ev " << run << ":" << lumi << ":" << evt << endl <<
	" pT(l): " << LepPt << " eta(l): " << LepEta << " pdgID: " << LepID << endl <<
	" Met: " << Met << " Q80: " << Q80 << " CosDeltaPhi: " << CosDeltaPhi << endl <<
	" N(j): " << Njet << " pT(j1): " << Jet1Pt << " pT(j2): " << Jet2Pt << " pT(j3): " << Jet_pt[2] << " HT: " << HT25 << endl <<
	//	" eta(j1): " << Jet1Eta << " eta(j2): " << Jet2Eta << 
	//	" dPhi(j1,j2): " << DPhiJet1Jet2 << 
	" N(b): " << NbLoose30 << " pT(b): " << JetHBpt << endl;
      for (Int_t j = 0; j < Njet; j++) { SyFile << " pT(j): " << Jet_pt[j] << " btag(j): " << Jet_btagCSV[j] << endl; }
      SyFile << " dR(j1,l): " << DrJet1Lep << " dR(b,l): " << DrJetHBLep << " dR(j1,j2): " << DrJet1Jet2 << " M(l,j): " << JetLepMass << " M(3j): " << J3Mass <<
	endl;
    }

    bdttree->Fill();
  }

  SyFile.close();
  bdttree->Write("",TObject::kOverwrite);

}
