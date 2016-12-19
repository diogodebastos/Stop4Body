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
#include "UserCode/Stop4Body/interface/doubleWithUncertainty.h"

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
float DeltaPhi(float, float);

const double ECALGap_MinEta =  1.4442; // ECAL gap parameters
const double ECALGap_MaxEta =  1.5660;
const double CSV_Loose = 0.460;
const double CSV_Medium = 0.800;
const double CSV_Tight = 0.935;

doubleUnc stopCrossSection(double stopM, double lspM);

int main(int argc, char** argv)
{
  std::string jsonFileName = "";
  std::string outputDirectory = "./OUT/";
  bool doSync = false;
  bool noSkim = false;
  bool oldVeto = false;
  bool threshold30 = false;
  std::string suffix = "";
  size_t max_sync_count = 0;

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

    if(argument == "--oldVeto")
      oldVeto = true;

    if(argument == "--jetThreshold30ForPreselection")
      threshold30 = true;
  }

  if(jsonFileName == "")
  {
    std::cout << "You must define a json file" << std::endl;
    return 1;
  }

  if(!fileExists(outputDirectory+"/puWeights.root"))
  {
    std::cout << "You must first obtain the PU weights before trying to process the samples" << std::endl;
    return 1;
  }

  TDirectory* cwd = gDirectory;
  TFile puWeightFile((outputDirectory + "/puWeights.root").c_str(), "READ");
  cwd->cd();

  std::cout << "Reading JSON file" << std::endl;
  SampleReader samples(jsonFileName);

  for(auto &process : samples)
  {
    std::cout << "Processing process: " << process.tag() << std::endl;
    TH1D* puWeightDistrib = static_cast<TH1D*>(puWeightFile.Get(("process_"+process.tag()+"_puWeight").c_str()));
    for(auto &sample : process)
    {
      std::cout << "\tProcessing sample: " << sample.tag() << std::endl;
      std::string outputFile = outputDirectory + "/" + sample.tag();
      if(suffix != "")
        outputFile += "_" + suffix;
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

      TFile foutput(outputFile.c_str(), "RECREATE");
      TTree *bdttree= new TTree("bdttree","bdttree");

      // New branch in bdt tree
      Float_t Run;  bdttree->Branch("Run",&Run,"Run/F");
      Float_t Event;  bdttree->Branch("Event",&Event,"Event/F");
      Float_t LumiSec;  bdttree->Branch("LumiSec",&LumiSec,"LumiSec/F");
      Float_t Nevt;  bdttree->Branch("Nevt",&Nevt,"Nevt/F");
      Float_t XS; bdttree->Branch("XS",&XS,"XS/F");
      Float_t nVert; bdttree->Branch("nVert", &nVert, "nVert/F");
      Float_t weight; bdttree->Branch("weight", &weight, "weight/F");
      Float_t puWeight; bdttree->Branch("puWeight", &puWeight, "puWeight/F");
      Float_t genWeight; bdttree->Branch("genWeight", &genWeight, "genWeight/F");
      Float_t sumGenWeight; bdttree->Branch("sumGenWeight", &sumGenWeight, "sumGenWeight/F");
      Float_t LepID;  bdttree->Branch("LepID",&LepID,"LepID/F");
      Float_t LepChg;  bdttree->Branch("LepChg",&LepChg,"LepChg/F");
      Float_t LepPt;  bdttree->Branch("LepPt",&LepPt,"LepPt/F");
      Float_t LepEta;  bdttree->Branch("LepEta",&LepEta,"LepEta/F");
      Float_t LepDxy;  bdttree->Branch("LepDxy",&LepDxy,"LepDxy/F");
      Float_t LepDz;  bdttree->Branch("LepDz",&LepDz,"LepDz/F");
      Float_t LepSip3;  bdttree->Branch("LepSip3",&LepSip3,"LepSip3/F");
      Float_t LepIso03;  bdttree->Branch("LepIso03",&LepIso03,"LepIso03/F");
      Float_t LepIso04;  bdttree->Branch("LepIso04",&LepIso04,"LepIso04/F");
      Float_t nGoodMu;  bdttree->Branch("nGoodMu",&nGoodMu,"nGoodMu/F");
      Float_t nGoodEl;  bdttree->Branch("nGoodEl",&nGoodEl,"nGoodEl/F");
      Float_t nGoodTrack;  bdttree->Branch("nGoodTrack",&nGoodTrack,"nGoodTrack/F");
      Float_t Met; bdttree->Branch("Met",&Met,"Met/F");
      Float_t mt; bdttree->Branch("mt",&mt,"mt/F");
      Float_t Q80; bdttree-> Branch("Q80",&Q80,"Q80/F");
      Float_t CosDeltaPhi; bdttree->Branch("CosDeltaPhi",&CosDeltaPhi,"CosDeltaPhi/F");
      Float_t NbLoose30; bdttree->Branch("NbLoose30",&NbLoose30,"NbLoose30/F");
      Float_t NbTight30;  bdttree->Branch("NbTight30",&NbTight30,"NbTight30/F");
      Float_t NbLoose20; bdttree->Branch("NbLoose20",&NbLoose20,"NbLoose20/F");
      Float_t NbTight20;  bdttree->Branch("NbTight20",&NbTight20,"NbTight20/F");
      Float_t NbLoose20to50; bdttree->Branch("NbLoose20to50",&NbLoose20to50,"NbLoose20to50/F");
      Float_t NbLoose50;     bdttree->Branch("NbLoose50",&NbLoose50,"NbLoose50/F");
      Float_t NbMedium20to50; bdttree->Branch("NbMedium20to50",&NbMedium20to50,"NbMedium20to50/F");
      Float_t NbMedium50;     bdttree->Branch("NbMedium50",&NbMedium50,"NbMedium50/F");
      Float_t NbTight20to50; bdttree->Branch("NbTight20to50",&NbTight20to50,"NbTight20to50/F");
      Float_t NbTight50;     bdttree->Branch("NbTight50",&NbTight50,"NbTight50/F");
      Float_t Njet;  bdttree->Branch("Njet",&Njet,"Njet/F");
      Float_t Njet30;  bdttree->Branch("Njet30",&Njet30,"Njet30/F");
      Float_t Njet60;  bdttree->Branch("Njet60",&Njet60,"Njet60/F");
      Float_t Jet1Pt;  bdttree->Branch("Jet1Pt",&Jet1Pt,"Jet1Pt/F");
      Float_t Jet1Eta;  bdttree->Branch("Jet1Eta",&Jet1Eta,"Jet1Eta/F");
      Float_t Jet1CSV;  bdttree->Branch("Jet1CSV",&Jet1CSV,"Jet1CSV/F"); // *
      Float_t Jet2Pt;  bdttree->Branch("Jet2Pt",&Jet2Pt,"Jet2Pt/F");
      Float_t Jet2Eta;  bdttree->Branch("Jet2Eta",&Jet2Eta,"Jet2Eta/F");
      Float_t Jet2CSV;  bdttree->Branch("Jet2CSV",&Jet2CSV,"Jet2CSV/F"); // *
      Float_t Jet3Pt;  bdttree->Branch("Jet3Pt",&Jet3Pt,"Jet3Pt/F"); // *
      Float_t Jet3Eta;  bdttree->Branch("Jet3Eta",&Jet3Eta,"Jet3Eta/F"); // *
      Float_t Jet3CSV;  bdttree->Branch("Jet3CSV",&Jet3CSV,"Jet3CSV/F"); // *
      Float_t DPhiJet1Jet2;  bdttree->Branch("DPhiJet1Jet2",&DPhiJet1Jet2,"DPhiJet1Jet2/F");
      Float_t DPhiJet1Jet2_30;  bdttree->Branch("DPhiJet1Jet2_30",&DPhiJet1Jet2_30,"DPhiJet1Jet2_30/F");
      Float_t JetHBpt;  bdttree->Branch("JetHBpt",&JetHBpt,"JetHBpt/F");
      Float_t JetHBeta;  bdttree->Branch("JetHBeta",&JetHBeta,"JetHBeta/F"); // *
      Float_t JetHBindex; bdttree->Branch("JetHBindex", &JetHBindex, "JetHBindex/F"); // *
      Float_t JetHBCSV; bdttree->Branch("JetHBCSV", &JetHBCSV, "JetHBCSV/F"); // *
      Float_t JetB2pt;  bdttree->Branch("JetB2pt",&JetB2pt,"JetB2pt/F");
      Float_t JetB2eta;  bdttree->Branch("JetB2eta",&JetB2eta,"JetB2eta/F"); // *
      Float_t JetB2index; bdttree->Branch("JetB2index", &JetB2index, "JetB2index/F"); // *
      Float_t JetB2CSV; bdttree->Branch("JetB2CSV", &JetB2CSV, "JetB2CSV/F");
      Float_t DrJet1Lep;  bdttree->Branch("DrJet1Lep",&DrJet1Lep,"DrJet1Lep/F");
      Float_t DrJet2Lep;  bdttree->Branch("DrJet2Lep",&DrJet2Lep,"DrJet2Lep/F");
      Float_t DrJetHBLep;  bdttree->Branch("DrJetHBLep",&DrJetHBLep,"DrJetHBLep/F");
      Float_t DrJet1Jet2;  bdttree->Branch("DrJet1Jet2",&DrJet1Jet2,"DrJet1Jet2/F");
      Float_t JetLepMass;  bdttree->Branch("JetLepMass",&JetLepMass,"JetLepMass/F");
      Float_t J3Mass;  bdttree->Branch("J3Mass",&J3Mass,"J3Mass/F");
      Float_t HT20;  bdttree->Branch("HT20",&HT20,"HT20/F");
      Float_t HT25;  bdttree->Branch("HT25",&HT25,"HT25/F");
      Float_t HT30;  bdttree->Branch("HT30",&HT30,"HT30/F");
      Float_t Ncut0;  //bdttree->Branch("Ncut0",&Ncut0,"Ncut0/F");
      Float_t Ncut1;
      Float_t Ncut2;
      Float_t Ncut3;
      Float_t Ncut4;
      Float_t PFMET170JetIdCleaned; bdttree->Branch("PFMET170JetIdCleaned", &PFMET170JetIdCleaned,"PFMET170JetIdCleaned/F");
      Float_t PFMET90_PFMHT90; bdttree->Branch("PFMET90_PFMHT90", &PFMET90_PFMHT90,"PFMET90_PFMHT90/F");
      Float_t PFMETNoMu90_PFMHTNoMu90; bdttree->Branch("PFMETNoMu90_PFMHTNoMu90", &PFMETNoMu90_PFMHTNoMu90,"PFMETNoMu90_PFMHTNoMu90/F");
      Float_t METFilters; bdttree->Branch("METFilters", &METFilters, "METFilters/F");
      Float_t HBHENoiseFilter; bdttree->Branch("HBHENoiseFilter", &HBHENoiseFilter,"HBHENoiseFilter/F");
      Float_t HBHENoiseIsoFilter; bdttree->Branch("HBHENoiseIsoFilter", &HBHENoiseIsoFilter,"HBHENoiseIsoFilter/F");
      Float_t eeBadScFilter; bdttree->Branch("eeBadScFilter", &eeBadScFilter,"eeBadScFilter/F");
      Float_t EcalDeadCellTriggerPrimitiveFilter; bdttree->Branch("EcalDeadCellTriggerPrimitiveFilter", &EcalDeadCellTriggerPrimitiveFilter,"EcalDeadCellTriggerPrimitiveFilter/F");
      Float_t goodVertices; bdttree->Branch("goodVertices", &goodVertices,"goodVertices/F");
      Float_t globalTightHalo2016Filter; bdttree->Branch("globalTightHalo2016Filter", &globalTightHalo2016Filter, "globalTightHalo2016Filter/F");
      Float_t genGravitinoM; bdttree->Branch("genGravitinoM", &genGravitinoM, "genGravitinoM/F");
      Float_t genStopM; bdttree->Branch("genStopM", &genStopM, "genStopM/F");
      Float_t genSbottomM; bdttree->Branch("genSbottomM", &genSbottomM, "genSbottomM/F");
      Float_t genNeutralinoM; bdttree->Branch("genNeutralinoM", &genNeutralinoM, "genNeutralinoM/F");
      Float_t filterEfficiency=1; bdttree->Branch("filterEfficiency", &filterEfficiency, "filterEfficiency/F");


      TH1* filterEfficiencyH = nullptr;
      if(sample.filterEfficiencyFile() != "")
      {
        TDirectory* cwd = gDirectory;
        TFile filterEfficiencyFile(sample.filterEfficiencyFile().c_str(), "READ");
        TCanvas* canvas = static_cast<TCanvas*>(filterEfficiencyFile.Get("c1"));
        cwd->cd();
        filterEfficiencyH = static_cast<TH1*>(canvas->GetPrimitive("filterEfficiencies")->Clone("filterEfficiency"));
      }

      // Get total number of entries
      Nevt = 0;
      sumGenWeight = 0;
      Ncut0 = 0;
      Ncut1 = 0;
      Ncut2 = 0;
      Ncut3 = 0;
      Ncut4 = 0;
      TH1D puDistrib((sample.tag()+"nvtx").c_str(), "nvtx;Evt.", 100, 0, 100);
      std::cout << "\t  Getting Initial number of events, nvtx distribution and sum of gen weights: " << std::flush;
      for(auto &file : sample)
      {
        TFile finput(file.c_str(), "READ");
        foutput.cd();
        TTree *inputtree;
        if(process.selection() != "")
          inputtree = static_cast<TTree*>(finput.Get("tree"))->CopyTree(process.selection().c_str());
        else
          inputtree = static_cast<TTree*>(finput.Get("tree"));

        Int_t thisNevt = static_cast<Int_t>(inputtree->GetEntries());
        Nevt += thisNevt;

        Float_t thisGenWeight = 0;
        Int_t nvtx = 0;
        inputtree->SetBranchAddress("genWeight", &thisGenWeight);
        inputtree->SetBranchAddress("nVert", &nvtx);
        for(Int_t i = 0; i < thisNevt; ++i)
        {
          inputtree->GetEntry(i);
          sumGenWeight += thisGenWeight;
          puDistrib.Fill(nvtx, thisGenWeight);
        }

        if(process.selection() != "")
          delete inputtree;
      }
      std::cout << Nevt << "; " << sumGenWeight << std::endl;

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
        Float_t mtw1;        inputtree->SetBranchAddress("mtw1"       , &mtw1);
        Float_t mtw2;        inputtree->SetBranchAddress("mtw2"       , &mtw2);
        Float_t mtw;         inputtree->SetBranchAddress("mtw"       , &mtw);
        Float_t met_pt;      inputtree->SetBranchAddress("met_pt"    , &met_pt);
        Float_t met_phi;     inputtree->SetBranchAddress("met_phi",   &met_phi);
        Int_t nBJetLoose30;  inputtree->SetBranchAddress("nBJetLoose30"       , &nBJetLoose30);
        Int_t nBJetTight30;  inputtree->SetBranchAddress("nBJetTight30"       , &nBJetTight30);
        Int_t nBJetLoose20;  inputtree->SetBranchAddress("nBJetLoose20"       , &nBJetLoose20);
        Int_t nBJetTight20;  inputtree->SetBranchAddress("nBJetTight20"       , &nBJetTight20);
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
        Int_t LepGood_eleVeto[40];
        if(oldVeto)
          inputtree->SetBranchAddress("LepGood_eleCutIdSpring15_25ns_v1", &LepGood_eleVeto);
        else
          inputtree->SetBranchAddress("LepGood_SPRING15_25ns_v1", &LepGood_eleVeto);
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
        Int_t LepOther_eleVeto[40];
        if(oldVeto)
          inputtree->SetBranchAddress("LepOther_eleCutIdSpring15_25ns_v1", &LepOther_eleVeto);
        else
          inputtree->SetBranchAddress("LepOther_SPRING15_25ns_v1", &LepOther_eleVeto);
        Int_t LepOther_charge[40]; inputtree->SetBranchAddress("LepOther_charge",&LepOther_charge);
        Float_t Jet_chEMEF[40];  inputtree->SetBranchAddress("Jet_chEMEF", &Jet_chEMEF);
        Float_t Jet_neEMEF[40];  inputtree->SetBranchAddress("Jet_neEMEF", &Jet_neEMEF);
        Float_t Jet_chHEF[40];  inputtree->SetBranchAddress("Jet_chHEF", &Jet_chHEF);
        Float_t Jet_neHEF[40];  inputtree->SetBranchAddress("Jet_neHEF", &Jet_neHEF);
        Float_t Jet_pt[40];  inputtree->SetBranchAddress("Jet_pt", &Jet_pt);
        Int_t Jet_id[40];  inputtree->SetBranchAddress("Jet_id", &Jet_id);
        Float_t Jet_eta[40];  inputtree->SetBranchAddress("Jet_eta", &Jet_eta);
        Float_t Jet_phi[40];  inputtree->SetBranchAddress("Jet_phi", &Jet_phi);
        Float_t Jet_btagCSV[40];  inputtree->SetBranchAddress("Jet_btagCSV", &Jet_btagCSV);
        Float_t Jet_mass[40];  inputtree->SetBranchAddress("Jet_mass", &Jet_mass);
        Float_t Jet_rawPt[40]; inputtree->SetBranchAddress("Jet_rawPt", &Jet_rawPt);
        //Int_t nJet20;  inputtree->SetBranchAddress("nJet20", &nJet20);
        Int_t nJet20;  inputtree->SetBranchAddress("nJet20a", &nJet20);
        Int_t nJet30;  inputtree->SetBranchAddress("nJet30", &nJet30);
        UInt_t run;  inputtree->SetBranchAddress("run", &run);
        ULong64_t evt;  inputtree->SetBranchAddress("evt", &evt);
        UInt_t lumi;  inputtree->SetBranchAddress("lumi", &lumi);
        Float_t Tracks_pt[200];  inputtree->SetBranchAddress("Tracks_pt", &Tracks_pt);
        Float_t Tracks_eta[200];  inputtree->SetBranchAddress("Tracks_eta", &Tracks_eta);
        Float_t Tracks_dz[200];  inputtree->SetBranchAddress("Tracks_dz", &Tracks_dz);
        Float_t Tracks_dxy[200];  inputtree->SetBranchAddress("Tracks_dxy", &Tracks_dxy);
        Float_t Tracks_phi[200];  inputtree->SetBranchAddress("Tracks_phi", &Tracks_phi);
        Float_t Tracks_CosPhiJet12[200];  inputtree->SetBranchAddress("Tracks_CosPhiJet12", &Tracks_CosPhiJet12);
        Float_t Tracks_matchedJetDr[200];  inputtree->SetBranchAddress("Tracks_matchedJetDr", &Tracks_matchedJetDr);
        Float_t Tracks_matchedJetIndex[200];  inputtree->SetBranchAddress("Tracks_matchedJetIndex", &Tracks_matchedJetIndex);
        Int_t nTracks;  inputtree->SetBranchAddress("nTracks", &nTracks);
        Int_t nVert_i; inputtree->SetBranchAddress("nVert", &nVert_i);
        inputtree->SetBranchAddress("puWeight", &puWeight);
        inputtree->SetBranchAddress("genWeight", &genWeight);

        Float_t xsec = 1;
        Float_t nTrueInt = 1;
        if(!process.isdata())
        {
          inputtree->SetBranchAddress("xsec", &xsec);
          inputtree->SetBranchAddress("nTrueInt", &nTrueInt);
        }

        // 2015 HLT
        /*Int_t HLT_PFMET170_JetIdCleaned;   inputtree->SetBranchAddress("HLT_PFMET170_JetIdCleaned", &HLT_PFMET170_JetIdCleaned);
        Int_t HLT_PFMET90_PFMHT90;   inputtree->SetBranchAddress("HLT_PFMET90_PFMHT90", &HLT_PFMET90_PFMHT90);
        Int_t HLT_PFMETNoMu90_PFMHTNoMu90;   inputtree->SetBranchAddress("HLT_PFMETNoMu90_PFMHTNoMu90", &HLT_PFMETNoMu90_PFMHTNoMu90);// */

        // 2016 HLT
        Int_t HLT_PFMET170_JetIdCleaned = 0;//   inputtree->SetBranchAddress("HLT_PFMET170_JetIdCleaned", &HLT_PFMET170_JetIdCleaned);
        Int_t HLT_PFMET90_PFMHT90;   inputtree->SetBranchAddress("HLT_PFMET90_PFMHT90", &HLT_PFMET90_PFMHT90);
        Int_t HLT_PFMETNoMu90_PFMHTNoMu90;   inputtree->SetBranchAddress("HLT_PFMETNoMu90_PFMHTNoMu90", &HLT_PFMETNoMu90_PFMHTNoMu90);

        Int_t Flag_METFilters; inputtree->SetBranchAddress("Flag_METFilters", &Flag_METFilters);
        Int_t Flag_HBHENoiseFilter; inputtree->SetBranchAddress("Flag_HBHENoiseFilter", &Flag_HBHENoiseFilter);
        Int_t Flag_HBHENoiseIsoFilter; inputtree->SetBranchAddress("Flag_HBHENoiseIsoFilter", &Flag_HBHENoiseIsoFilter);
        Int_t Flag_eeBadScFilter; inputtree->SetBranchAddress("Flag_eeBadScFilter", &Flag_eeBadScFilter);
        Int_t Flag_EcalDeadCellTriggerPrimitiveFilter; inputtree->SetBranchAddress("Flag_EcalDeadCellTriggerPrimitiveFilter", &Flag_EcalDeadCellTriggerPrimitiveFilter);
        Int_t Flag_goodVertices; inputtree->SetBranchAddress("Flag_goodVertices", &Flag_goodVertices);
        Int_t Flag_globalTightHalo2016Filter; inputtree->SetBranchAddress("Flag_globalTightHalo2016Filter", &Flag_globalTightHalo2016Filter);

        // Variables that are copied directly to output tree (unfortunately there is a type mismatch)
        /*inputtree->SetBranchAddress("GenSusyMGravitino", &genGravitinoM);
        inputtree->SetBranchAddress("GenSusyMStop", &genStopM);
        inputtree->SetBranchAddress("GenSusyMSbottom", &genSbottomM);
        inputtree->SetBranchAddress("GenSusyMNeutralino", &genNeutralinoM);// */
        Int_t GenSusyMGravitino; inputtree->SetBranchAddress("GenSusyMGravitino", &GenSusyMGravitino);
        Int_t GenSusyMStop; inputtree->SetBranchAddress("GenSusyMStop", &GenSusyMStop);
        Int_t GenSusyMSbottom; inputtree->SetBranchAddress("GenSusyMSbottom", &GenSusyMSbottom);
        Int_t GenSusyMNeutralino; inputtree->SetBranchAddress("GenSusyMNeutralino", &GenSusyMNeutralino);

        // Read the number of entries in the inputtree
        Int_t nentries = (Int_t)inputtree->GetEntries();
        std::cout << "\t    The file has " << nentries << " events." << std::endl;
        std::cout << "\t    Progress Bar: " << std::flush;
        int statusPrint = nentries/20;
        for(Int_t i = 0; i < nentries; i++)
        {
          // Uncomment this if you suspect the loop is not going through the events
          /*if(i%2 == 0)
            std::cout << "$\b" << std::flush;
          else
            std::cout << " \b" << std::flush;// */
          if(i%statusPrint == 0 && i != 0)
            std::cout << "*" << std::flush;

          if(doSync && max_sync_count > 0 && sync_count >= max_sync_count)
            break;

          inputtree->GetEntry(i);
          nVert = nVert_i;
          puWeight = puWeightDistrib->GetBinContent(puWeightDistrib->FindBin(nTrueInt));

          // Object ID
          std::vector<int> validJets;
          std::vector<int> validTracks;
          std::vector<std::pair<int, int>> validLeptons; // First is the type, second the index for that type
          std::vector<std::pair<int,float>> bjetList; // First is the jet index, second is the jet CSV value

          validJets.clear();
          validTracks.clear();
          validLeptons.clear();
          bjetList.clear();

          for(Int_t i = 0; i < nJet20; ++i)
          {
            if(std::abs(Jet_eta[i]) < 2.4 && Jet_pt[i] > 20)
            {
              validJets.push_back(i);
            }
          }

          for(auto &j : validJets)
          {
            if(Jet_btagCSV[j] != -10)
            {
              bjetList.push_back(std::make_pair(j,Jet_btagCSV[j]));
            }
          }
          std::sort(bjetList.begin(), bjetList.end(), [](const std::pair<int,float> &left, const std::pair<int,float> &right) {
              return left.second > right.second;
              });

          for(Int_t l = 0; l < nTracks; ++l)
          {
            int index=Tracks_matchedJetIndex[l];

            if( Tracks_pt[l] > 2.5
              && std::abs(Tracks_eta[l]) < 2.5
              && std::abs(Tracks_dz[l]) < 0.1
              && std::abs(Tracks_dxy[l]) < 0.1
              && Tracks_CosPhiJet12[l]  < 0.7
              && ( Tracks_matchedJetDr[l] > 0.4 || (index >=0 && Jet_pt[ index ]  < 60 )))
            {
              validTracks.push_back(l);
            }
          }

          nGoodMu = 0;
          nGoodEl = 0;
          for(Int_t type = 0; type < 2; ++type)
          {
            Int_t   leptonNumber                     = nLepGood;
            Int_t   *lepton_pdgId                    = LepGood_pdgId;
            //Int_t   *lepton_mediumMuonId             = LepGood_mediumMuonId;
            Int_t   *lepton_eleVeto = LepGood_eleVeto;
            //Int_t   *lepton_charge                   = LepGood_charge;
            Float_t *lepton_pt                       = LepGood_pt;
            Float_t *lepton_eta                      = LepGood_eta;
            //Float_t *lepton_phi                      = LepGood_phi;
            Float_t *lepton_relIso03                 = LepGood_relIso03;
            //Float_t *lepton_relIso04                 = LepGood_relIso04;
            Float_t *lepton_dxy                      = LepGood_dxy;
            Float_t *lepton_dz                       = LepGood_dz;
            //Float_t *lepton_sip3d                    = LepGood_sip3d;
            //Float_t *lepton_mass                     = LepGood_mass;
            if(type == 1)
            {
              leptonNumber                    = nLepOther;
              lepton_pdgId                    = LepOther_pdgId;
              //lepton_mediumMuonId             = LepOther_mediumMuonId;
              lepton_eleVeto = LepOther_eleVeto;
              //lepton_charge                   = LepGood_charge;
              lepton_pt                       = LepOther_pt;
              lepton_eta                      = LepOther_eta;
              //lepton_phi                      = LepOther_phi;
              lepton_relIso03                 = LepOther_relIso03;
              //lepton_relIso04                 = LepOther_relIso04;
              lepton_dxy                      = LepOther_dxy;
              lepton_dz                       = LepOther_dz;
              //lepton_sip3d                    = LepOther_sip3d;
              //lepton_mass                     = LepOther_mass;
            }

            for(Int_t i = 0; i < leptonNumber; ++i)
            {
              bool lPTETA = lepton_pt[i] > 5.0;
                         //&& lepton_pt[i] < 30.0;
              if(std::abs(lepton_pdgId[i]) == 13)
              {
                lPTETA = lPTETA && (std::abs(lepton_eta[i]) < 2.4);
                if(type == 1) // Only add LepOther for electrons, not for muons
                  continue;
              }
              else
              {
                lPTETA = lPTETA && (std::abs(lepton_eta[i]) < 2.5);
                // ECAL Gap
                lPTETA = lPTETA && (  (std::abs(lepton_eta[i]) > ECALGap_MaxEta)
                                   || (std::abs(lepton_eta[i]) < ECALGap_MinEta)   );
              }

              bool lID = (std::abs(lepton_dxy[i]) < 0.02)
                      && (std::abs(lepton_dz[i]) < 0.5);
              if(std::abs(lepton_pdgId[i]) == 11)
                lID = lID && (lepton_eleVeto[i] >= 1);

              bool lIS = ((lepton_pt[i] >= 25.0) && (lepton_relIso03[i] < 0.2))
                      || ((lepton_pt[i] <  25.0) && ((lepton_pt[i] * lepton_relIso03[i]) < 5.0));

              if(lPTETA && lID && lIS)
              {
                validLeptons.push_back(std::make_pair(type, i));
                if(std::abs(lepton_pdgId[i]) == 13)
                {
                  nGoodMu += 1;
                }
                if(std::abs(lepton_pdgId[i]) == 11 )
                {
                  nGoodEl += 1;
                }
              }
            }
          }
          std::sort(validLeptons.begin(), validLeptons.end(), [LepGood_pt, LepOther_pt](const std::pair<int,int> &left, const std::pair<int,int> &right) {
              Float_t left_pt, right_pt;
              if(left.first == 1)
              {
                left_pt = LepOther_pt[left.second];
              }
              else
              {
                left_pt = LepGood_pt[left.second];
              }
              if(right.first == 1)
              {
                right_pt = LepOther_pt[right.second];
              }
              else
              {
                right_pt = LepGood_pt[right.second];
              }
              return left_pt > right_pt;
              });

          //if(validLeptons.size() == 0)
          //  continue;



          // Set the value of the branches in the bdttree
          //Nevt = nentries;
          mt = mtw;
          Q80 = mtw1;
          CosDeltaPhi = mtw2;
          Met = met_pt;
          NbLoose30 = nBJetLoose30;
          NbTight30 = nBJetTight30;
          NbLoose20 = nBJetLoose20;
          NbTight20 = nBJetTight20;

          nGoodTrack = validTracks.size();

          TLorentzVector VLep;
          float lep_phi, lep_eta;
          if(validLeptons.size() != 0)
          {
          Int_t lep_ind = validLeptons[0].second;
          if(validLeptons[0].first == 1)
          {
            lep_phi  = LepOther_phi[lep_ind];
            lep_eta  = LepOther_eta[lep_ind];
            LepChg   = LepOther_charge[lep_ind];
            LepID    = LepOther_pdgId[lep_ind];
            LepPt    = LepOther_pt[lep_ind];
            LepEta   = LepOther_eta[lep_ind];
            LepDxy   = LepOther_dxy[lep_ind];
            LepDz    = LepOther_dz[lep_ind];
            LepSip3  = LepOther_sip3d[lep_ind];
            LepIso03 = LepOther_relIso03[lep_ind];
            LepIso04 = LepOther_relIso04[lep_ind];
            VLep.SetPtEtaPhiM(LepPt, LepEta, lep_phi, LepOther_mass[lep_ind]);
          }
          else
          {
            lep_phi  = LepGood_phi[lep_ind];
            lep_eta  = LepGood_eta[lep_ind];
            LepChg   = LepGood_charge[lep_ind];
            LepID    = LepGood_pdgId[lep_ind];
            LepPt    = LepGood_pt[lep_ind];
            LepEta   = LepGood_eta[lep_ind];
            LepDxy   = LepGood_dxy[lep_ind];
            LepDz    = LepGood_dz[lep_ind];
            LepSip3  = LepGood_sip3d[lep_ind];
            LepIso03 = LepGood_relIso03[lep_ind];
            LepIso04 = LepGood_relIso04[lep_ind];
            VLep.SetPtEtaPhiM(LepPt, LepEta, lep_phi, LepGood_mass[lep_ind]);
          }
          }
          else
          {
            lep_eta = -999;
            lep_phi = -999;
            LepChg = -999;
            LepID = -999;
            LepPt = -999;
            LepEta = -999;
            LepDxy = -999;
            LepDz = -999;
            LepSip3 = -999;
            LepIso03 = -999;
            LepIso04 = -999;
          }

          float DrJetLepMax = 999999.;
          Int_t ij = 0;
          for(auto &j : validJets)
          {
            float dpi = DeltaPhi(Jet_phi[j],lep_phi);
            float dei = Jet_eta[j]-lep_eta;
            //float dri = sqrt( pow(dpi,2) + pow(dei,2) );
            float dri = pow(dpi,2) + pow(dei,2);
            if(dri < DrJetLepMax)
            {
              DrJetLepMax = dri;
              ij = j;
            }
          }

          TLorentzVector VJ, JLep;
          VJ.SetPtEtaPhiM(Jet_pt[ij], Jet_eta[ij], Jet_phi[ij], Jet_mass[ij]);
          JLep = VJ + VLep;
          JetLepMass = JLep.M();

          TLorentzVector VJ3i, VJ3;
          for(auto &j : validJets)
          {
            if (j == ij)
              continue;
            VJ3i.SetPtEtaPhiM(Jet_pt[j], Jet_eta[j], Jet_phi[j], Jet_mass[j]);
            VJ3 += VJ3i;
          }
          J3Mass = VJ3.M();

          if(validJets.size() >= 1)
          {
            Jet1Pt = Jet_pt[validJets[0]];
            Jet1Eta = Jet_eta[validJets[0]];
            Jet1CSV = Jet_btagCSV[validJets[0]];
            float dphi = DeltaPhi(Jet_phi[validJets[0]], lep_phi);
            float deta = Jet_eta[validJets[0]] - lep_eta;
            DrJet1Lep = sqrt( pow(dphi,2) + pow(deta,2) );
          }
          else
          {
            Jet1Pt = -999.;
            Jet1Eta = -999.;
            Jet1CSV = -999.;
            DrJet1Lep = -999.;
            Jet2Pt = -999.;
            Jet2Eta = -999.;
            Jet2CSV = -999.;
            Jet3Pt = -999.;
            Jet3Eta = -999.;
            Jet3CSV = -999.;
          }

          if(validJets.size() >= 2)
          {
            float dphijj, detajj;
            Jet2Pt = Jet_pt[validJets[1]];
            Jet2Eta = Jet_eta[validJets[1]];
            Jet2CSV = Jet_btagCSV[validJets[1]];
            dphijj = DeltaPhi(Jet_phi[validJets[0]], Jet_phi[validJets[1]]);
            detajj = Jet_eta[validJets[0]] - Jet_eta[validJets[1]];
            DPhiJet1Jet2 = dphijj;
            if(Jet_pt[validJets[1]] > 30)
              DPhiJet1Jet2_30 = dphijj;
            else
              DPhiJet1Jet2_30 = -999.;
            DrJet1Jet2 = sqrt( pow(dphijj,2) + pow(detajj,2) );

            float dphi = DeltaPhi(Jet_phi[validJets[1]], lep_phi);
            float deta = Jet_eta[validJets[1]] - lep_eta;
            DrJet2Lep = sqrt( pow(dphi,2) + pow(deta,2) );
          }
          else
          {
            Jet2Pt = -999.;
            Jet2Eta = -999.;
            Jet2CSV = -999.;
            DPhiJet1Jet2 = -999.;
            DPhiJet1Jet2_30 = -999.;
            DrJet1Jet2 = -999.;
            DrJet2Lep = -999.;
          }

          if(validJets.size() >= 3)
          {
            Jet3Pt = Jet_pt[validJets[2]];
            Jet3Eta = Jet_eta[validJets[2]];
            Jet3CSV = Jet_btagCSV[validJets[2]];
          }
          else
          {
            Jet3Pt = -999.;
            Jet3Eta = -999.;
            Jet3CSV = -999.;
          }

          JetHBpt = -999.;
          JetHBeta = -999.;
          JetHBindex = -1;
          JetHBCSV = -999.;
          JetB2pt = -999.;
          JetB2eta = -999.;
          JetB2index = -1;
          JetB2CSV = -999.;
          DrJetHBLep = -999.;

          if(bjetList.size() >= 1)
          {
            Int_t iBtag = bjetList[0].first;
            JetHBpt = Jet_pt[iBtag];
            JetHBeta = Jet_eta[iBtag];
            JetHBindex = iBtag;
            JetHBCSV = Jet_btagCSV[iBtag];
            float dphib, detab;
            dphib = DeltaPhi(Jet_phi[iBtag], lep_phi);
            detab = Jet_eta[iBtag] - lep_eta;
            DrJetHBLep = sqrt( pow(dphib,2) + pow(detab,2) );
          }

          if(bjetList.size() > 1)
          {
            Int_t iBtag = bjetList[1].first;
            JetB2pt = Jet_pt[iBtag];
            JetB2eta = Jet_eta[iBtag];
            JetB2index = iBtag;
            JetB2CSV = Jet_btagCSV[iBtag];
          }

          NbLoose20to50 = 0;
          NbLoose50 = 0;
          NbMedium20to50 = 0;
          NbMedium50 = 0;
          NbTight20to50 = 0;
          NbTight50 = 0;
          for(auto& bjet : bjetList)
          {
            auto& index = bjet.first;
            auto& csv = bjet.second;
            auto& pt = Jet_pt[index];

            if(pt > 50)
            {
              if(csv > CSV_Loose)
                ++NbLoose50;
              if(csv > CSV_Medium)
                ++NbMedium50;
              if(csv > CSV_Tight)
                ++NbTight50;
            }
            else
            {
              if(csv > CSV_Loose)
                ++NbLoose20to50;
              if(csv > CSV_Medium)
                ++NbMedium20to50;
              if(csv > CSV_Tight)
                ++NbTight20to50;
            }
          }

          HT20 = 0.;
          HT25 = 0.;
          HT30 = 0.;
          Njet = 0;
          Njet30 = 0;
          Njet60 = 0;
          for(auto &j : validJets)
          {
            if(Jet_pt[j] > 20.)
            {
              HT20 += Jet_pt[j];
              Njet += 1;
            }
            if(Jet_pt[j] > 25.)
              HT25 += Jet_pt[j];
            if(Jet_pt[j] > 30.)
            {
              HT30 += Jet_pt[j];
              Njet30 += 1;
            }
            if(Jet_pt[j] > 60.)
            {
              Njet60 += 1;
            }
          }

          genGravitinoM  = GenSusyMGravitino;
          genStopM       = GenSusyMStop;
          genSbottomM    = GenSusyMSbottom;
          genNeutralinoM = GenSusyMNeutralino;

          Run = run;
          Event = evt;
          LumiSec = lumi;
          XS = xsec;
          //XS = file.crossSection;
          if(process.issignal())
            XS = stopCrossSection(genStopM, genNeutralinoM).value();

          PFMET170JetIdCleaned                = HLT_PFMET170_JetIdCleaned;
          PFMET90_PFMHT90                     = HLT_PFMET90_PFMHT90;
          PFMETNoMu90_PFMHTNoMu90             = HLT_PFMETNoMu90_PFMHTNoMu90;
          METFilters                          = Flag_METFilters;
          HBHENoiseFilter                     = Flag_HBHENoiseFilter;
          HBHENoiseIsoFilter                  = Flag_HBHENoiseIsoFilter;
          eeBadScFilter                       = Flag_eeBadScFilter;
          EcalDeadCellTriggerPrimitiveFilter  = Flag_EcalDeadCellTriggerPrimitiveFilter;
          goodVertices                        = Flag_goodVertices;
          globalTightHalo2016Filter           = Flag_globalTightHalo2016Filter;

          // Filter Efficiency
          if(filterEfficiencyH != nullptr)
          {
            auto theBin = filterEfficiencyH->FindBin(genStopM, genNeutralinoM);
            filterEfficiency = filterEfficiencyH->GetBinContent(theBin);
          }
          else
            filterEfficiency = 1.0;

          if(!process.isdata())
            weight = puWeight*XS*filterEfficiency*(genWeight/sumGenWeight);
          else
            weight = 1;


          if(doSync)
          {
            if(sync_count < max_sync_count)
            {
              SyFile << "Run:LS:Ev " << run << ":" << lumi << ":" << evt << std::endl;
              SyFile << "   Mstop: " << genStopM << "; Mlsp: " << genNeutralinoM << std::endl;
              SyFile << "   HT: " << HT30 << "; MET: " << Met << std::endl;
              SyFile << "   Njet(pT>30): " << Njet30 << std::endl;
              //SyFile << "   leading jet:  pT: " << Jet1Pt << "; eta: " << Jet1Eta << "; raw pT: " << ((validJets.size() > 0)?(Jet_rawPt[validJets[0]]):(-999)) << std::endl;
              //SyFile << "   subleading jet:  pT: " << Jet2Pt << "; eta: " << Jet2Eta << "; raw pT: " << ((validJets.size() > 1)?(Jet_rawPt[validJets[1]]):(-999)) << std::endl;
              for(int i = 0; i < Njet30; ++i)
                SyFile << "   jet " << i+1 << ":  pT: " << Jet_pt[validJets[i]] << "; eta: " << Jet_eta[validJets[i]] << "; raw pT: " << Jet_rawPt[validJets[i]] << "; ID: " << Jet_id[validJets[i]] << "; abs(eta): " << std::abs(Jet_eta[validJets[i]]) << std::endl;
              SyFile << "   Nlep: " << nGoodEl+nGoodMu << std::endl;
              SyFile << "   leading lepton:  pT: " << LepPt << "; eta: " << LepEta << "; PDG ID: " << LepID << std::endl;
              SyFile << "   Delta Phi Jet1 Jet2: " << DPhiJet1Jet2 << std::endl;
              SyFile << "   weight: " << 10000*XS*filterEfficiency/Nevt << std::endl;
              SyFile << "   passed: ";
              if(HT30 > 200 && Met > 200 && Jet1Pt > 90)
              {
                SyFile << "Cut0";
                bool passCut1 = false;
                if(validLeptons.size() == 1)
                  passCut1 = true;
                if(validLeptons.size() > 1)
                {
                  float lep_pt;
                  if(validLeptons[1].first == 1)
                    lep_pt = LepOther_pt[validLeptons[1].second];
                  else
                    lep_pt = LepGood_pt[validLeptons[1].second];

                  if(lep_pt < 20)
                    passCut1 = true;
                }

                if(passCut1 && LepPt < 30)
                {
                  SyFile << ";Cut1";

                  if(Jet1Pt > 100)
                  {
                    SyFile << ";Cut2";
                    if( ((threshold30 && DPhiJet1Jet2_30 < 2.5) || (!threshold30 && DPhiJet1Jet2 < 2.5))
                     || (Jet2Pt < 60) )
                    {
                      SyFile << ";Cut3";
                      if(Met > 280)
                        SyFile << ";Cut4";
                    }
                  }
                }
              }
              else
              {
                SyFile << "None";
              }
              //if(lumi == 91151 && evt == 195485531)
              if(true)
              {
                //SyFile << std::endl << "Y no lepton?";
                SyFile << std::endl << "nLepGood: " << nLepGood << "    nLepOther: " << nLepOther << std::endl;
                SyFile << "LepGood:" << std::endl;
                for(int i = 0; i < nLepGood; ++i)
                  SyFile << "   lep " << i+1 << ": ID: " << LepGood_pdgId[i] << "; pt: " << LepGood_pt[i] << "; eta:" << LepGood_eta[i] << "; relIso03: " << LepGood_relIso03[i] << "; dxy: " << LepGood_dxy[i] << "; dz: " << LepGood_dz[i] << "; eleVeto: " << LepGood_eleVeto[i] << std::endl;
                SyFile << "LepOther:" << std::endl;
                for(int i = 0; i < nLepOther; ++i)
                  SyFile << "   lep " << i+1 << ": ID: " << LepOther_pdgId[i] << "; pt: " << LepOther_pt[i] << "; eta:" << LepOther_eta[i] << "; relIso03: " << LepOther_relIso03[i] << "; dxy: " << LepOther_dxy[i] << "; dz: " << LepOther_dz[i] << "; eleVeto: " << LepOther_eleVeto[i] << std::endl;
                SyFile << "nJetGood: " << nJet20 << std::endl;
                for(int i = 0; i < nJet20; ++i)
                  SyFile << "   jet " << i+1 << ": pt: " << Jet_pt[i] << "; eta: " << Jet_eta[i] << "; phi: " << Jet_phi[i] << "; mass: " << Jet_mass[i] << "; ID: " << Jet_id[i] << std::endl;
              }
              SyFile << std::endl << std::endl;

              ++sync_count;
            }
          }

          if(validJets.size() > 0)
          {
            if(HT30 > 200 && Met > 200 && Jet_pt[validJets[0]] > 90)
            {
              ++Ncut0;
              bool passCut1 = false;
              if(validLeptons.size() == 1)
                passCut1 = true;
              if(validLeptons.size() > 1)
              {
                float lep_pt;
                if(validLeptons[1].first == 1)
                  lep_pt = LepOther_pt[validLeptons[1].second];
                else
                  lep_pt = LepGood_pt[validLeptons[1].second];

                if(lep_pt < 20)
                  passCut1 = true;
              }

              if(passCut1 && LepPt < 30)
              {
                ++Ncut1;
                if(Jet1Pt > 100)
                {
                  ++Ncut2;
                  if( ((threshold30 && DPhiJet1Jet2_30 < 2.5) || (!threshold30 && DPhiJet1Jet2 < 2.5))
                   || (Jet2Pt < 60) )
                  {
                    ++Ncut3;
                    if(Met > 280)
                      ++Ncut4;
                  }
                }
              }
            }
          }

          // Skim
          if(validLeptons.size() == 0)  // Commented out above
            continue;
          /*if(validLeptons.size() >= 1)
          {
            float lep_pt;
            if(validLeptons[0].first == 1)
              lep_pt = LepOther_pt[validLeptons[0].second];
            else
              lep_pt = LepGood_pt[validLeptons[0].second];

            if(lep_pt > 30)
              continue;
          }// */
          if(validLeptons.size() >= 2)
          {
            float lep_pt;
            if(validLeptons[1].first == 1)
              lep_pt = LepOther_pt[validLeptons[1].second];
            else
              lep_pt = LepGood_pt[validLeptons[1].second];

            if(lep_pt > 20)
              continue;
          }
          if(Njet == 0)
            continue;
          if(!noSkim)
          {
            bool isISR = ((Jet_pt[validJets[0]] > 90.)  &&  (Njet > 0));
            bool dPhi = (DPhiJet1Jet2 < 2.5 || Jet2Pt < 60);
            if(threshold30)
              dPhi = (DPhiJet1Jet2_30 < 2.5 || Jet2Pt < 60);
            bool met = (Met > 100.);
            if(!isISR)   continue;
            if(!dPhi)    continue;
            if(!met)     continue;
          }


          // MET filters
          //if ( METFilters                         != 1 ) continue;
          if ( HBHENoiseFilter                    != 1 ) continue;
          if ( HBHENoiseIsoFilter                 != 1 ) continue;
          if ( EcalDeadCellTriggerPrimitiveFilter != 1 ) continue;
          if ( goodVertices                       != 1 ) continue;
          if ( eeBadScFilter                      != 1 ) continue;
          if ( globalTightHalo2016Filter          != 1 ) continue;
          //if ( badMuonFilter                      != 1 ) continue;
          //if ( badChargedHadronFilter             != 1 ) continue;

          bdttree->Fill();
        }
        std::cout << std::endl;
        if(process.selection() != "")
          delete inputtree;
      }
      foutput.cd();
      bdttree->Write("",TObject::kOverwrite);

      TVectorD v(5);
      v[0] = Ncut0;
      v[1] = Ncut1;
      v[2] = Ncut2;
      v[3] = Ncut3;
      v[4] = Ncut4;
      v.Write("Ncut");
      puDistrib.Write();

      if(filterEfficiencyH != nullptr)
        delete filterEfficiencyH;
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

bool fileExists(std::string fileName)
{
  std::ifstream infile(fileName);
  return infile.good();
}

float DeltaPhi(float p1, float p2)
{
  float x = p1 - p2;

  while(x >= TMath::Pi())
    x -= (2.*TMath::Pi());

  while(x < -TMath::Pi())
    x += (2.*TMath::Pi());

  return std::abs(x);
}

doubleUnc stopCrossSection(double stopM, double lspM)
{
  double val = 0, unc = 0.1;

  int massIndex = (stopM - 100.0)/5.0;

  switch(massIndex)
  {
  case 0:
    val = 1521.11;
    unc = 15.4038;
    break;
  case 1:
    val = 1233.18;
    unc = 15.4059;
    break;
  case 2:
    val = 1013.76;
    unc = 15.4088;
    break;
  case 3:
    val = 832.656;
    unc = 15.1503;
    break;
  case 4:
    val = 689.799;
    unc = 15.044;
    break;
  case 5:
    val = 574.981;
    unc = 14.9895;
    break;
  case 6:
    val = 481.397;
    unc = 14.8906;
    break;
  case 7:
    val = 405.159;
    unc = 14.8952;
    break;
  case 8:
    val = 342.865;
    unc = 14.9119;
    break;
  case 9:
    val = 291.752;
    unc = 14.8022;
    break;
  case 10:
    val = 249.409;
    unc = 14.7477;
    break;
  case 11:
    val = 214.221;
    unc = 14.5928;
    break;
  case 12:
    val = 184.623;
    unc = 14.5821;
    break;
  case 13:
    val = 159.614;
    unc = 14.7859;
    break;
  case 14:
    val = 139.252;
    unc = 14.547;
    break;
  case 15:
    val = 121.416;
    unc = 14.6341;
    break;
  case 16:
    val = 106.194;
    unc = 14.2033;
    break;
  case 17:
    val = 93.3347;
    unc = 14.4893;
    break;
  case 18:
    val = 82.2541;
    unc = 14.4677;
    break;
  case 19:
    val = 72.7397;
    unc = 14.4452;
    break;
  case 20:
    val = 64.5085;
    unc = 14.4098;
    break;
  case 21:
    val = 57.2279;
    unc = 14.4191;
    break;
  case 22:
    val = 50.9226;
    unc = 14.2457;
    break;
  case 23:
    val = 45.3761;
    unc = 14.344;
    break;
  case 24:
    val = 40.5941;
    unc = 14.2634;
    break;
  case 25:
    val = 36.3818;
    unc = 14.2189;
    break;
  case 26:
    val = 32.6679;
    unc = 14.1592;
    break;
  case 27:
    val = 29.3155;
    unc = 14.2233;
    break;
  case 28:
    val = 26.4761;
    unc = 14.1723;
    break;
  case 29:
    val = 23.8853;
    unc = 13.9482;
    break;
  case 30:
    val = 21.5949;
    unc = 14.0595;
    break;
  case 31:
    val = 19.5614;
    unc = 13.8755;
    break;
  case 32:
    val = 17.6836;
    unc = 13.9505;
    break;
  case 33:
    val = 16.112;
    unc = 13.9531;
    break;
  case 34:
    val = 14.6459;
    unc = 13.9278;
    break;
  case 35:
    val = 13.3231;
    unc = 14.2549;
    break;
  case 36:
    val = 12.1575;
    unc = 14.1584;
    break;
  case 37:
    val = 11.0925;
    unc = 14.0904;
    break;
  case 38:
    val = 10.1363;
    unc = 13.8967;
    break;
  case 39:
    val = 9.29002;
    unc = 13.9107;
    break;
  case 40:
    val = 8.51615;
    unc = 13.9223;
    break;
  case 41:
    val = 7.81428;
    unc = 13.8996;
    break;
  case 42:
    val = 7.17876;
    unc = 13.9357;
    break;
  case 43:
    val = 6.60266;
    unc = 13.9256;
    break;
  case 44:
    val = 6.08444;
    unc = 13.7957;
    break;
  case 45:
    val = 5.60471;
    unc = 13.8144;
    break;
  case 46:
    val = 5.17188;
    unc = 13.6954;
    break;
  case 47:
    val = 4.77871;
    unc = 13.7554;
    break;
  case 48:
    val = 4.41629;
    unc = 13.7945;
    break;
  case 49:
    val = 4.08881;
    unc = 13.7075;
    break;
  case 50:
    val = 3.78661;
    unc = 13.6877;
    break;
  case 51:
    val = 3.50911;
    unc = 13.8089;
    break;
  case 52:
    val = 3.25619;
    unc = 13.8002;
    break;
  case 53:
    val = 3.02472;
    unc = 13.7093;
    break;
  case 54:
    val = 2.8077;
    unc = 13.8064;
    break;
  case 55:
    val = 2.61162;
    unc = 13.8477;
    break;
  case 56:
    val = 2.43031;
    unc = 13.6999;
    break;
  case 57:
    val = 2.26365;
    unc = 13.728;
    break;
  case 58:
    val = 2.10786;
    unc = 13.732;
    break;
  case 59:
    val = 1.9665;
    unc = 13.4737;
    break;
  case 60:
    val = 1.83537;
    unc = 13.6985;
    break;
  case 61:
    val = 1.70927;
    unc = 13.7114;
    break;
  case 62:
    val = 1.60378;
    unc = 13.5468;
    break;
  case 63:
    val = 1.49798;
    unc = 13.4453;
    break;
  case 64:
    val = 1.39688;
    unc = 13.6719;
    break;
  case 65:
    val = 1.31169;
    unc = 13.5013;
    break;
  case 66:
    val = 1.22589;
    unc = 13.3237;
    break;
  case 67:
    val = 1.14553;
    unc = 13.5478;
    break;
  case 68:
    val = 1.07484;
    unc = 13.7238;
    break;
  case 69:
    val = 1.01019;
    unc = 13.4187;
    break;
  case 70:
    val = 0.948333;
    unc = 13.4559;
    break;
  case 71:
    val = 0.890847;
    unc = 13.4587;
    break;
  case 72:
    val = 0.836762;
    unc = 13.4468;
    break;
  case 73:
    val = 0.787221;
    unc = 13.4149;
    break;
  case 74:
    val = 0.740549;
    unc = 13.4127;
    break;
  case 75:
    val = 0.697075;
    unc = 13.3926;
    break;
  case 76:
    val = 0.655954;
    unc = 13.4392;
    break;
  case 77:
    val = 0.618562;
    unc = 13.3705;
    break;
  case 78:
    val = 0.582467;
    unc = 13.3914;
    break;
  case 79:
    val = 0.549524;
    unc = 13.3691;
    break;
  case 80:
    val = 0.51848;
    unc = 13.3797;
    break;
  case 81:
    val = 0.489324;
    unc = 13.3608;
    break;
  case 82:
    val = 0.462439;
    unc = 13.3046;
    break;
  case 83:
    val = 0.436832;
    unc = 13.3703;
    break;
  case 84:
    val = 0.412828;
    unc = 13.272;
    break;
  case 85:
    val = 0.390303;
    unc = 13.3443;
    break;
  case 86:
    val = 0.368755;
    unc = 13.3769;
    break;
  case 87:
    val = 0.348705;
    unc = 13.2706;
    break;
  case 88:
    val = 0.330157;
    unc = 13.2981;
    break;
  case 89:
    val = 0.312672;
    unc = 13.277;
    break;
  case 90:
    val = 0.296128;
    unc = 13.2687;
    break;
  case 91:
    val = 0.280734;
    unc = 13.2363;
    break;
  case 92:
    val = 0.266138;
    unc = 13.193;
    break;
  case 93:
    val = 0.251557;
    unc = 13.1731;
    break;
  case 94:
    val = 0.238537;
    unc = 13.3409;
    break;
  case 95:
    val = 0.226118;
    unc = 13.2741;
    break;
  case 96:
    val = 0.214557;
    unc = 13.1697;
    break;
  case 97:
    val = 0.203566;
    unc = 13.3257;
    break;
  case 98:
    val = 0.193079;
    unc = 13.2037;
    break;
  case 99:
    val = 0.183604;
    unc = 13.0973;
    break;
  case 100:
    val = 0.174599;
    unc = 13.2074;
    break;
  case 101:
    val = 0.166131;
    unc = 13.0154;
    break;
  case 102:
    val = 0.158242;
    unc = 13.142;
    break;
  case 103:
    val = 0.150275;
    unc = 13.285;
    break;
  case 104:
    val = 0.142787;
    unc = 13.0642;
    break;
  case 105:
    val = 0.136372;
    unc = 12.7962;
    break;
  case 106:
    val = 0.129886;
    unc = 13.2957;
    break;
  case 107:
    val = 0.123402;
    unc = 13.016;
    break;
  case 108:
    val = 0.11795;
    unc = 12.7132;
    break;
  case 109:
    val = 0.112008;
    unc = 12.808;
    break;
  case 110:
    val = 0.107045;
    unc = 12.9232;
    break;
  case 111:
    val = 0.102081;
    unc = 13.0012;
    break;
  case 112:
    val = 0.09725;
    unc = 12.9038;
    break;
  case 113:
    val = 0.0927515;
    unc = 12.9548;
    break;
  case 114:
    val = 0.0885084;
    unc = 13.0218;
    break;
  case 115:
    val = 0.0844877;
    unc = 13.0703;
    break;
  case 116:
    val = 0.0806192;
    unc = 13.1131;
    break;
  case 117:
    val = 0.0769099;
    unc = 13.1517;
    break;
  case 118:
    val = 0.0734901;
    unc = 13.2344;
    break;
  case 119:
    val = 0.0701805;
    unc = 13.2716;
    break;
  case 120:
    val = 0.0670476;
    unc = 13.3429;
    break;
  case 121:
    val = 0.0641426;
    unc = 13.363;
    break;
  case 122:
    val = 0.0612942;
    unc = 13.3941;
    break;
  case 123:
    val = 0.0585678;
    unc = 13.4663;
    break;
  case 124:
    val = 0.0560753;
    unc = 13.4984;
    break;
  case 125:
    val = 0.0536438;
    unc = 13.5804;
    break;
  case 126:
    val = 0.0513219;
    unc = 13.5682;
    break;
  case 127:
    val = 0.0491001;
    unc = 13.6268;
    break;
  case 128:
    val = 0.0470801;
    unc = 13.6895;
    break;
  case 129:
    val = 0.045061;
    unc = 13.6816;
    break;
  case 130:
    val = 0.0431418;
    unc = 13.7455;
    break;
  case 131:
    val = 0.0413447;
    unc = 13.7833;
    break;
  case 132:
    val = 0.0396264;
    unc = 13.8518;
    break;
  case 133:
    val = 0.0379036;
    unc = 13.8537;
    break;
  case 134:
    val = 0.0363856;
    unc = 13.9334;
    break;
  case 135:
    val = 0.0348796;
    unc = 13.9597;
    break;
  case 136:
    val = 0.0334669;
    unc = 14.0267;
    break;
  case 137:
    val = 0.0320548;
    unc = 14.0406;
    break;
  case 138:
    val = 0.0307373;
    unc = 14.115;
    break;
  case 139:
    val = 0.0295348;
    unc = 14.1397;
    break;
  case 140:
    val = 0.0283338;
    unc = 14.171;
    break;
  case 141:
    val = 0.0272206;
    unc = 14.241;
    break;
  case 142:
    val = 0.0261233;
    unc = 14.2891;
    break;
  case 143:
    val = 0.0251107;
    unc = 14.3632;
    break;
  case 144:
    val = 0.0241099;
    unc = 14.3805;
    break;
  case 145:
    val = 0.0230866;
    unc = 14.4428;
    break;
  case 146:
    val = 0.0221834;
    unc = 14.4791;
    break;
  case 147:
    val = 0.0213766;
    unc = 14.5511;
    break;
  case 148:
    val = 0.0204715;
    unc = 14.6131;
    break;
  case 149:
    val = 0.0197653;
    unc = 14.6602;
    break;
  case 150:
    val = 0.0189612;
    unc = 14.702;
    break;
  case 151:
    val = 0.0182516;
    unc = 14.7648;
    break;
  case 152:
    val = 0.0175509;
    unc = 14.7944;
    break;
  case 153:
    val = 0.0168336;
    unc = 14.8528;
    break;
  case 154:
    val = 0.0162314;
    unc = 14.8772;
    break;
  case 155:
    val = 0.015625;
    unc = 14.9567;
    break;
  case 156:
    val = 0.0150143;
    unc = 15.0389;
    break;
  case 157:
    val = 0.0144112;
    unc = 15.0614;
    break;
  case 158:
    val = 0.0138979;
    unc = 15.1;
    break;
  case 159:
    val = 0.0133962;
    unc = 15.1325;
    break;
  case 160:
    val = 0.0128895;
    unc = 15.2026;
    break;
  case 161:
    val = 0.0123843;
    unc = 15.2968;
    break;
  case 162:
    val = 0.0119837;
    unc = 15.3089;
    break;
  case 163:
    val = 0.0114713;
    unc = 15.3678;
    break;
  case 164:
    val = 0.0110688;
    unc = 15.4082;
    break;
  case 165:
    val = 0.0106631;
    unc = 15.4806;
    break;
  case 166:
    val = 0.0102629;
    unc = 15.5313;
    break;
  case 167:
    val = 0.0098874;
    unc = 15.6066;
    break;
  case 168:
    val = 0.00952142;
    unc = 15.6055;
    break;
  case 169:
    val = 0.00916636;
    unc = 15.6849;
    break;
  case 170:
    val = 0.00883465;
    unc = 15.7177;
    break;
  case 171:
    val = 0.00851073;
    unc = 15.8094;
    break;
  case 172:
    val = 0.00820884;
    unc = 15.844;
    break;
  case 173:
    val = 0.00791403;
    unc = 15.9216;
    break;
  case 174:
    val = 0.00763112;
    unc = 15.9742;
    break;
  case 175:
    val = 0.00735655;
    unc = 16.0548;
    break;
  case 176:
    val = 0.00710317;
    unc = 16.0626;
    break;
  case 177:
    val = 0.00684867;
    unc = 16.144;
    break;
  case 178:
    val = 0.00660695;
    unc = 16.1813;
    break;
  case 179:
    val = 0.00637546;
    unc = 16.2158;
    break;
  case 180:
    val = 0.00615134;
    unc = 16.2953;
    break;
  case 181:
    val = 0.00593765;
    unc = 16.3716;
    break;
  case 182:
    val = 0.00572452;
    unc = 16.3857;
    break;
  case 183:
    val = 0.00553094;
    unc = 16.4628;
    break;
  case 184:
    val = 0.00533968;
    unc = 16.4963;
    break;
  case 185:
    val = 0.00514619;
    unc = 16.5762;
    break;
  case 186:
    val = 0.00497235;
    unc = 16.5838;
    break;
  case 187:
    val = 0.00479906;
    unc = 16.6646;
    break;
  case 188:
    val = 0.00463806;
    unc = 16.6947;
    break;
  case 189:
    val = 0.00447537;
    unc = 16.7071;
    break;
  case 190:
    val = 0.00432261;
    unc = 16.7859;
    break;
  case 191:
    val = 0.00417983;
    unc = 16.8637;
    break;
  case 192:
    val = 0.00403886;
    unc = 16.8981;
    break;
  case 193:
    val = 0.0038962;
    unc = 16.9794;
    break;
  case 194:
    val = 0.00376343;
    unc = 16.9764;
    break;
  case 195:
    val = 0.00364174;
    unc = 17.0634;
    break;
  case 196:
    val = 0.00352093;
    unc = 17.0908;
    break;
  case 197:
    val = 0.00339813;
    unc = 17.1929;
    break;
  case 198:
    val = 0.00328695;
    unc = 17.2274;
    break;
  case 199:
    val = 0.00317628;
    unc = 17.2617;
    break;
  case 200:
    val = 0.00307413;
    unc = 17.3377;
    break;
  case 201:
    val = 0.00297377;
    unc = 17.3822;
    break;
  case 202:
    val = 0.00287148;
    unc = 17.4725;
    break;
  case 203:
    val = 0.00278078;
    unc = 17.5091;
    break;
  case 204:
    val = 0.00268873;
    unc = 17.5883;
    break;
  case 205:
    val = 0.00260821;
    unc = 17.6126;
    break;
  case 206:
    val = 0.00251529;
    unc = 17.6836;
    break;
  case 207:
    val = 0.00243484;
    unc = 17.7128;
    break;
  case 208:
    val = 0.00236295;
    unc = 17.7977;
    break;
  case 209:
    val = 0.00228192;
    unc = 17.8507;
    break;
  case 210:
    val = 0.00221047;
    unc = 17.9259;
    break;
  case 211:
    val = 0.00213907;
    unc = 18.0255;
    break;
  case 212:
    val = 0.00206845;
    unc = 18.0518;
    break;
  case 213:
    val = 0.0020063;
    unc = 18.0954;
    break;
  case 214:
    val = 0.00194569;
    unc = 18.1194;
    break;
  case 215:
    val = 0.0018741;
    unc = 18.2145;
    break;
  case 216:
    val = 0.00182266;
    unc = 18.3074;
    break;
  case 217:
    val = 0.00176211;
    unc = 18.3375;
    break;
  case 218:
    val = 0.00170006;
    unc = 18.4075;
    break;
  case 219:
    val = 0.00164968;
    unc = 18.4438;
    break;
  case 220:
    val = 0.00159844;
    unc = 18.5209;
    break;
  case 221:
    val = 0.0015472;
    unc = 18.5977;
    break;
  case 222:
    val = 0.00149657;
    unc = 18.6485;
    break;
  case 223:
    val = 0.00145544;
    unc = 18.7347;
    break;
  case 224:
    val = 0.00140288;
    unc = 18.8774;
    break;
  case 225:
    val = 0.00136155;
    unc = 18.989;
    break;
  case 226:
    val = 0.00131271;
    unc = 18.8763;
    break;
  case 227:
    val = 0.0012717;
    unc = 18.9588;
    break;
  case 228:
    val = 0.00123066;
    unc = 19.049;
    break;
  case 229:
    val = 0.00119994;
    unc = 19.1442;
    break;
  case 230:
    val = 0.0011583;
    unc = 19.3006;
    break;
  case 231:
    val = 0.00112694;
    unc = 19.4441;
    break;
  case 232:
    val = 0.00108716;
    unc = 19.4141;
    break;
  case 233:
    val = 0.00105517;
    unc = 19.6361;
    break;
  case 234:
    val = 0.00102241;
    unc = 19.6297;
    break;
  case 235:
    val = 0.000991293;
    unc = 19.762;
    break;
  case 236:
    val = 0.000961012;
    unc = 19.7926;
    break;
  case 237:
    val = 0.000932394;
    unc = 19.8682;
    break;
  case 238:
    val = 0.000903404;
    unc = 19.9924;
    break;
  case 239:
    val = 0.000876957;
    unc = 20.0777;
    break;
  case 240:
    val = 0.000850345;
    unc = 20.1604;
    break;
  case 241:
    val = 0.00082443;
    unc = 20.2883;
    break;
  case 242:
    val = 0.00079983;
    unc = 20.373;
    break;
  case 243:
    val = 0.000775222;
    unc = 20.4622;
    break;
  case 244:
    val = 0.000751372;
    unc = 20.5919;
    break;
  case 245:
    val = 0.000728912;
    unc = 20.6884;
    break;
  case 246:
    val = 0.000706867;
    unc = 20.7763;
    break;
  case 247:
    val = 0.000685372;
    unc = 20.8587;
    break;
  case 248:
    val = 0.000664649;
    unc = 20.9879;
    break;
  case 249:
    val = 0.000644804;
    unc = 21.1487;
    break;
  case 250:
    val = 0.000625155;
    unc = 21.2761;
    break;
  case 251:
    val = 0.000606802;
    unc = 21.3529;
    break;
  case 252:
    val = 0.000588512;
    unc = 21.4428;
    break;
  case 253:
    val = 0.000570506;
    unc = 21.6584;
    break;
  case 254:
    val = 0.000553379;
    unc = 21.6036;
    break;
  case 255:
    val = 0.000536646;
    unc = 21.775;
    break;
  case 256:
    val = 0.000521404;
    unc = 21.8383;
    break;
  case 257:
    val = 0.000505008;
    unc = 21.9675;
    break;
  case 258:
    val = 0.000490353;
    unc = 22.1444;
    break;
  case 259:
    val = 0.000476164;
    unc = 22.2016;
    break;
  case 260:
    val = 0.000461944;
    unc = 22.2704;
    break;
  case 261:
    val = 0.000448172;
    unc = 22.4911;
    break;
  case 262:
    val = 0.000435082;
    unc = 22.5606;
    break;
  case 263:
    val = 0.000422967;
    unc = 22.6095;
    break;
  case 264:
    val = 0.000410381;
    unc = 22.797;
    break;
  case 265:
    val = 0.000398106;
    unc = 22.8949;
    break;
  case 266:
    val = 0.000386792;
    unc = 23.1319;
    break;
  case 267:
    val = 0.000375724;
    unc = 23.1724;
    break;
  case 268:
    val = 0.000364616;
    unc = 23.2234;
    break;
  case 269:
    val = 0.000353965;
    unc = 23.4637;
    break;
  case 270:
    val = 0.000343923;
    unc = 23.4948;
    break;
  case 271:
    val = 0.000333885;
    unc = 23.5468;
    break;
  case 272:
    val = 0.000324344;
    unc = 23.771;
    break;
  case 273:
    val = 0.0003153;
    unc = 23.8004;
    break;
  case 274:
    val = 0.00030583;
    unc = 24.0064;
    break;
  case 275:
    val = 0.000296811;
    unc = 24.0314;
    break;
  case 276:
    val = 0.000288149;
    unc = 23.9248;
    break;
  case 277:
    val = 0.000279711;
    unc = 24.1257;
    break;
  case 278:
    val = 0.000271724;
    unc = 24.1274;
    break;
  case 279:
    val = 0.000264275;
    unc = 24.3545;
    break;
  case 280:
    val = 0.000256248;
    unc = 24.372;
    break;
  case 281:
    val = 0.000248853;
    unc = 24.5827;
    break;
  case 282:
    val = 0.000241844;
    unc = 24.6187;
    break;
  case 283:
    val = 0.000234438;
    unc = 24.8442;
    break;
  case 284:
    val = 0.000227374;
    unc = 24.8909;
    break;
  case 285:
    val = 0.000221045;
    unc = 25.0895;
    break;
  case 286:
    val = 0.000214431;
    unc = 24.8728;
    break;
  case 287:
    val = 0.000208092;
    unc = 25.1043;
    break;
  case 288:
    val = 0.000201748;
    unc = 25.3207;
    break;
  case 289:
    val = 0.000196399;
    unc = 25.5641;
    break;
  case 290:
    val = 0.000190474;
    unc = 25.5213;
    break;
  case 291:
    val = 0.000185188;
    unc = 25.7329;
    break;
  case 292:
    val = 0.000179263;
    unc = 25.6931;
    break;
  case 293:
    val = 0.000174021;
    unc = 25.9111;
    break;
  case 294:
    val = 0.000169176;
    unc = 25.8106;
    break;
  case 295:
    val = 0.000163861;
    unc = 26.0597;
    break;
  case 296:
    val = 0.000159583;
    unc = 26.2958;
    break;
  case 297:
    val = 0.000154719;
    unc = 26.195;
    break;
  case 298:
    val = 0.000150506;
    unc = 26.4111;
    break;
  case 299:
    val = 0.000145626;
    unc = 26.3077;
    break;
  case 300:
    val = 0.000141382;
    unc = 26.5291;
    break;
  case 301:
    val = 0.000137131;
    unc = 26.7424;
    break;
  case 302:
    val = 0.000132187;
    unc = 26.668;
    break;
  case 303:
    val = 0.000127929;
    unc = 26.9117;
    break;
  case 304:
    val = 0.000124086;
    unc = 26.7738;
    break;
  case 305:
    val = 0.00011982;
    unc = 27.0483;
    break;
  case 306:
    val = 0.000116042;
    unc = 26.8071;
    break;
  case 307:
    val = 0.000112767;
    unc = 27.127;
    break;
  case 308:
    val = 0.000108936;
    unc = 26.9351;
    break;
  case 309:
    val = 0.000105746;
    unc = 27.1783;
    break;
  case 310:
    val = 0.000102693;
    unc = 27.292;
    break;
  case 311:
    val = 0.000100112;
    unc = 27.4445;
    break;
  case 312:
    val = 9.75763e-05;
    unc = 27.5431;
    break;
  case 313:
    val = 9.52062e-05;
    unc = 27.6946;
    break;
  case 314:
    val = 9.29857e-05;
    unc = 27.7869;
    break;
  case 315:
    val = 9.08285e-05;
    unc = 27.9347;
    break;
  case 316:
    val = 8.87433e-05;
    unc = 28.1539;
    break;
  case 317:
    val = 8.66618e-05;
    unc = 28.3509;
    break;
  case 318:
    val = 8.46535e-05;
    unc = 28.4432;
    break;
  case 319:
    val = 8.27102e-05;
    unc = 28.591;
    break;
  case 320:
    val = 8.07774e-05;
    unc = 28.7497;
    break;
  case 321:
    val = 7.8666e-05;
    unc = 28.8194;
    break;
  case 322:
    val = 7.6572e-05;
    unc = 29.0265;
    break;
  case 323:
    val = 7.45994e-05;
    unc = 29.1193;
    break;
  case 324:
    val = 7.25199e-05;
    unc = 29.3013;
    break;
  case 325:
    val = 7.05189e-05;
    unc = 29.3697;
    break;
  case 326:
    val = 6.85712e-05;
    unc = 29.4972;
    break;
  case 327:
    val = 6.67296e-05;
    unc = 29.6167;
    break;
  case 328:
    val = 6.49184e-05;
    unc = 29.7686;
    break;
  case 329:
    val = 6.30949e-05;
    unc = 29.8524;
    break;
  case 330:
    val = 6.13637e-05;
    unc = 29.9789;
    break;
  case 331:
    val = 5.97301e-05;
    unc = 30.0928;
    break;
  case 332:
    val = 5.80751e-05;
    unc = 30.2585;
    break;
  case 333:
    val = 5.65479e-05;
    unc = 30.366;
    break;
  case 334:
    val = 5.49998e-05;
    unc = 30.5241;
    break;
  case 335:
    val = 5.35686e-05;
    unc = 30.6718;
    break;
  case 336:
    val = 5.20828e-05;
    unc = 30.6799;
    break;
  case 337:
    val = 5.07079e-05;
    unc = 30.9201;
    break;
  case 338:
    val = 4.93948e-05;
    unc = 31.0043;
    break;
  case 339:
    val = 4.80635e-05;
    unc = 31.138;
    break;
  case 340:
    val = 4.67492e-05;
    unc = 31.2291;
    break;
  case 341:
    val = 4.55055e-05;
    unc = 31.4321;
    break;
  case 342:
    val = 4.42835e-05;
    unc = 31.5499;
    break;
  case 343:
    val = 4.30744e-05;
    unc = 31.6302;
    break;
  case 344:
    val = 4.19954e-05;
    unc = 31.7151;
    break;
  case 345:
    val = 4.08527e-05;
    unc = 31.9048;
    break;
  case 346:
    val = 3.97561e-05;
    unc = 31.9718;
    break;
  case 347:
    val = 3.87041e-05;
    unc = 32.2028;
    break;
  case 348:
    val = 3.76008e-05;
    unc = 32.268;
    break;
  case 349:
    val = 3.66914e-05;
    unc = 32.4529;
    break;
  case 350:
    val = 3.56995e-05;
    unc = 32.5039;
    break;
  case 351:
    val = 3.47689e-05;
    unc = 32.6767;
    break;
  case 352:
    val = 3.38528e-05;
    unc = 32.8878;
    break;
  case 353:
    val = 3.29644e-05;
    unc = 32.8975;
    break;
  case 354:
    val = 3.20679e-05;
    unc = 32.9608;
    break;
  case 355:
    val = 3.12583e-05;
    unc = 33.1541;
    break;
  case 356:
    val = 3.04342e-05;
    unc = 33.3117;
    break;
  case 357:
    val = 2.96516e-05;
    unc = 33.2866;
    break;
  case 358:
    val = 2.88952e-05;
    unc = 33.6279;
    break;
  case 359:
    val = 2.81145e-05;
    unc = 33.6845;
    break;
  case 360:
    val = 2.73974e-05;
    unc = 33.8247;
    break;
  case 361:
    val = 2.66796e-05;
    unc = 33.9708;
    break;
  case 362:
    val = 2.59941e-05;
    unc = 33.9526;
    break;
  case 363:
    val = 2.52784e-05;
    unc = 34.1137;
    break;
  case 364:
    val = 2.46598e-05;
    unc = 34.2714;
    break;
  case 365:
    val = 2.39932e-05;
    unc = 34.2328;
    break;
  case 366:
    val = 2.33737e-05;
    unc = 34.394;
    break;
  case 367:
    val = 2.27623e-05;
    unc = 34.5138;
    break;
  case 368:
    val = 2.21454e-05;
    unc = 34.6933;
    break;
  case 369:
    val = 2.15924e-05;
    unc = 35.0815;
    break;
  case 370:
    val = 2.10232e-05;
    unc = 34.9444;
    break;
  case 371:
    val = 2.05211e-05;
    unc = 35.0155;
    break;
  case 372:
    val = 1.98996e-05;
    unc = 35.2135;
    break;
  case 373:
    val = 1.9408e-05;
    unc = 35.3328;
    break;
  case 374:
    val = 1.88974e-05;
    unc = 35.4643;
    break;
  case 375:
    val = 1.84612e-05;
    unc = 35.7904;
    break;
  case 376:
    val = 1.79562e-05;
    unc = 35.8898;
    break;
  case 377:
    val = 1.75673e-05;
    unc = 35.989;
    break;
  case 378:
    val = 1.70612e-05;
    unc = 36.0953;
    break;
  case 379:
    val = 1.66228e-05;
    unc = 36.4709;
    break;
  case 380:
    val = 1.62355e-05;
    unc = 36.5277;
    break;
  default:
    val = 0;
    unc = 20;
  }
  unc = unc/100;

  doubleUnc retVal(val, val*unc);
  return retVal;
}
