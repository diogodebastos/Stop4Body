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

#include "UserCode/Stop4Body/interface/json.hpp"

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

int main(int argc, char** argv)
{
  std::string jsonFileName = "";
  std::string outputDirectory = "./OUT/";
  bool doSync = false;

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

    if(argument == "doSync")
      doSync = true;
  }

  if(jsonFileName == "")
  {
    std::cout << "You must define a json file" << std::endl;
  }

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
        std::string path = file["path"];
        std::string identifier = process["tag"];
        identifier += ":";
        identifier += file["tag"];
        if(path == "")
          emptyPath.push_back(identifier);
        else
        {
          if(fileExists(path))
          {
            FileInfo tmpFile;

            tmpFile.path = path;
            tmpFile.crossSection = file["xsec"];
            tmpFile.branchingRatio = file["br"];
            tmpFile.tag = file["tag"];

            filesToProcess.push_back(tmpFile);
          }
          else
            invalidPath.push_back(identifier + "  ->  " + path);
        }
      }
    }
    else
      emptyLines.push_back(process["tag"]);
  }


  for(auto &file : filesToProcess)
  {
    std::cout << "Processing file " << file.path << std::endl;
    std::cout << "Putting output in: " << outputDirectory + "/" + file.tag + "_bdt.root" << std::endl;

    TFile finput(file.path.c_str(), "READ");
    TFile foutput((outputDirectory + "/" + file.tag + "_bdt.root").c_str(), "RECREATE");

    std::ofstream SyFile;
    if(doSync)
    {
      SyFile.open(outputDirectory + "/" + file.tag + "_synch.txt", std::ofstream::out | std::ofstream::trunc);
      SyFile.setf(std::ios::fixed);
      SyFile.precision(3);
    }

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
    Float_t Tracks_pt[200];  inputtree->SetBranchAddress("Tracks_pt", &Tracks_pt);
    Float_t Tracks_eta[200];  inputtree->SetBranchAddress("Tracks_eta", &Tracks_eta);
    Float_t Tracks_dz[200];  inputtree->SetBranchAddress("Tracks_dz", &Tracks_dz);
    Float_t Tracks_dxy[200];  inputtree->SetBranchAddress("Tracks_dxy", &Tracks_dxy);
    Float_t Tracks_phi[200];  inputtree->SetBranchAddress("Tracks_phi", &Tracks_phi);
    Float_t Tracks_CosPhiJet12[200];  inputtree->SetBranchAddress("Tracks_CosPhiJet12", &Tracks_CosPhiJet12);
    Float_t Tracks_matchedJetDr[200];  inputtree->SetBranchAddress("Tracks_matchedJetDr", &Tracks_matchedJetDr);
    Float_t Tracks_matchedJetIndex[200];  inputtree->SetBranchAddress("Tracks_matchedJetIndex", &Tracks_matchedJetIndex);
    Int_t nTracks;  inputtree->SetBranchAddress("nTracks", &nTracks);

    Int_t HLT_PFMET170_JetIdCleaned;   inputtree->SetBranchAddress("HLT_PFMET170_JetIdCleaned", &HLT_PFMET170_JetIdCleaned);
    Int_t HLT_PFMET90_PFMHT90_IDTight;   inputtree->SetBranchAddress("HLT_PFMET90_PFMHT90_IDTight", &HLT_PFMET90_PFMHT90_IDTight);
    Int_t HLT_PFMETNoMu90_PFMHTNoMu90_IDTight ;   inputtree->SetBranchAddress("HLT_PFMETNoMu90_PFMHTNoMu90_IDTight", &HLT_PFMETNoMu90_PFMHTNoMu90_IDTight);
    Int_t Flag_HBHENoiseFilter; inputtree->SetBranchAddress("Flag_HBHENoiseFilter", &Flag_HBHENoiseFilter);
    Int_t Flag_HBHENoiseIsoFilter; inputtree->SetBranchAddress("Flag_HBHENoiseIsoFilter", &Flag_HBHENoiseIsoFilter);
    Int_t Flag_eeBadScFilter; inputtree->SetBranchAddress("Flag_eeBadScFilter", &Flag_eeBadScFilter);
    Int_t Flag_EcalDeadCellTriggerPrimitiveFilter; inputtree->SetBranchAddress("Flag_EcalDeadCellTriggerPrimitiveFilter", &Flag_EcalDeadCellTriggerPrimitiveFilter);
    Int_t Flag_goodVertices; inputtree->SetBranchAddress("Flag_goodVertices", &Flag_goodVertices);

    // New branch in bdt tree
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
    Float_t Njet;  bdttree->Branch("Njet",&Njet,"Njet/F");
    Float_t Jet1Pt;  bdttree->Branch("Jet1Pt",&Jet1Pt,"Jet1Pt/F");
    Float_t Jet1Eta;  bdttree->Branch("Jet1Eta",&Jet1Eta,"Jet1Eta/F");
    Float_t Jet2Pt;  bdttree->Branch("Jet2Pt",&Jet2Pt,"Jet2Pt/F");
    Float_t Jet2Eta;  bdttree->Branch("Jet2Eta",&Jet2Eta,"Jet2Eta/F");
    Float_t DPhiJet1Jet2;  bdttree->Branch("DPhiJet1Jet2",&DPhiJet1Jet2,"DPhiJet1Jet2/F");
    Float_t JetHBpt;  bdttree->Branch("JetHBpt",&JetHBpt,"JetHBpt/F");
    Float_t DrJet1Lep;  bdttree->Branch("DrJet1Lep",&DrJet1Lep,"DrJet1Lep/F");
    Float_t DrJet2Lep;  bdttree->Branch("DrJet2Lep",&DrJet2Lep,"DrJet2Lep/F");
    Float_t DrJetHBLep;  bdttree->Branch("DrJetHBLep",&DrJetHBLep,"DrJetHBLep/F");
    Float_t DrJet1Jet2;  bdttree->Branch("DrJet1Jet2",&DrJet1Jet2,"DrJet1Jet2/F");
    Float_t JetLepMass;  bdttree->Branch("JetLepMass",&JetLepMass,"JetLepMass/F");
    Float_t J3Mass;  bdttree->Branch("J3Mass",&J3Mass,"J3Mass/F");
    Float_t HT20;  bdttree->Branch("HT20",&HT20,"HT20/F");
    Float_t HT25;  bdttree->Branch("HT25",&HT25,"HT25/F");
    Float_t HT30;  bdttree->Branch("HT30",&HT30,"HT30/F");
    Float_t XS; bdttree->Branch("XS",&XS,"XS/F");
    Float_t Run;  bdttree->Branch("Run",&Run,"Run/F");
    Float_t Event;  bdttree->Branch("Event",&Event,"Event/F");
    Float_t LumiSec;  bdttree->Branch("LumiSec",&LumiSec,"LumiSec/F");
    Float_t Nevt;  bdttree->Branch("Nevt",&Nevt,"Nevt/F");
    Float_t PFMET170JetIdCleaned; bdttree->Branch("PFMET170JetIdCleaned", &PFMET170JetIdCleaned,"PFMET170JetIdCleaned/F");
    Float_t PFMET90_PFMHT90_IDTight; bdttree->Branch("PFMET90_PFMHT90_IDTight", &PFMET90_PFMHT90_IDTight,"PFMET90_PFMHT90_IDTight/F");
    Float_t PFMETNoMu90_PFMHTNoMu90_IDTight; bdttree->Branch("PFMETNoMu90_PFMHTNoMu90_IDTight", &PFMETNoMu90_PFMHTNoMu90_IDTight,"PFMETNoMu90_PFMHTNoMu90_IDTight/F");
    Float_t HBHENoiseFilter; bdttree->Branch("HBHENoiseFilter", &HBHENoiseFilter,"HBHENoiseFilter/F");
    Float_t HBHENoiseIsoFilter; bdttree->Branch("HBHENoiseIsoFilter", &HBHENoiseIsoFilter,"HBHENoiseIsoFilter/F");
    Float_t eeBadScFilter; bdttree->Branch("eeBadScFilter", &eeBadScFilter,"eeBadScFilter/F");
    Float_t EcalDeadCellTriggerPrimitiveFilter; bdttree->Branch("EcalDeadCellTriggerPrimitiveFilter", &EcalDeadCellTriggerPrimitiveFilter,"EcalDeadCellTriggerPrimitiveFilter/F");
    Float_t goodVertices; bdttree->Branch("goodVertices", &goodVertices,"goodVertices/F");

    // Read the number of entries in the inputtree
    Int_t nentries = (Int_t)inputtree->GetEntries();
    std::cout << "\tThe file has " << nentries << " events." << std::endl;
    std::cout << "\tProgress Bar: " << std::flush;
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
      inputtree->GetEntry(i);

      // Preselection
      nGoodMu = 0;
      nGoodEl = 0;
      Int_t ilep = nLepGood;
      for(Int_t l = 0; l < nLepGood; l++)
      {
        bool lPTETA = (LepGood_pt[l] > 5.0)
                   && (LepGood_pt[l] < 30.0)
                   && (abs(LepGood_eta[l]) < 2.4);
        bool lID = (abs(LepGood_dxy[l]) < 0.05)
                && (abs(LepGood_dz[l]) < 0.2)
                && (LepGood_sip3d[l] < 4.0);
        bool lIS = ((LepGood_pt[l] >= 25.0) && (LepGood_relIso04[l] < 0.2))
                || ((LepGood_pt[l] <  25.0) && ((LepGood_pt[l] * LepGood_relIso04[l]) < 5.0));

        if( lPTETA && lID && lIS )
        {
          if( (abs(LepGood_pdgId[l]) == 13)  &&  (LepGood_mediumMuonId[l] == 1) )
            nGoodMu += 1;
          if( abs(LepGood_pdgId[l]) == 11 )
            nGoodEl += 1;
          if(l < ilep)
            ilep = l;
        }
      }

      nGoodTrack=0;
      for (Int_t l = 0; l < nTracks; l++)
      {
        int index=Tracks_matchedJetIndex[l];

        if( Tracks_pt[l] > 2.5
          && fabs(Tracks_eta[l]) < 2.5
          && fabs(Tracks_dz[l]) < 0.1
          && fabs(Tracks_dxy[l]) < 0.1
          && Tracks_CosPhiJet12[l]  < 0.7
          && ( Tracks_matchedJetDr[l] > 0.4 || (index >=0 && Jet_pt[ index ]  < 60 )))
        {
          nGoodTrack++;
        }
      }

      // Set the value of the branches in the bdttree
      Nevt = nentries;
      mt = mtw;
      Q80 = mtw1;
      CosDeltaPhi = mtw2;
      Met = met_pt;
      NbLoose30 = nBJetLoose30;
      NbTight30 = nBJetTight30;

      float DrJetLepMax = 999.;
      Int_t ij = 0;
      for(Int_t j = 0; j < nJet20; j++)
      {
        float dpi = DeltaPhi(Jet_phi[j],LepGood_phi[ilep]);
        float dei = Jet_eta[j]-LepGood_eta[ilep];
        float dri = sqrt( pow(dpi,2) + pow(dei,2) );
        if(dri < DrJetLepMax)
        {
          DrJetLepMax = dri;
          ij = j;
        }
      }

      TLorentzVector VJ, VLep, JLep;
      VLep.SetPtEtaPhiM(LepGood_pt[ilep], LepGood_eta[ilep], LepGood_phi[ilep], LepGood_mass[ilep]);
      VJ.SetPtEtaPhiM(Jet_pt[ij], Jet_eta[ij], Jet_phi[ij], Jet_mass[ij]);
      JLep = VJ + VLep;
      JetLepMass = JLep.M();

      TLorentzVector VJ3i, VJ3;
      for(Int_t j = 0; j < nJet20; j++)
      {
        if (j == ij)
          continue;
        VJ3i.SetPtEtaPhiM(Jet_pt[j], Jet_eta[j], Jet_phi[j], Jet_mass[j]);
        VJ3 += VJ3i;
      }
      J3Mass = VJ3.M();

      if(nJet20 >= 1)
      {
        Jet1Pt = Jet_pt[0];
        Jet1Eta = Jet_eta[0];
        float dphi = DeltaPhi(Jet_phi[0], LepGood_phi[ilep]);
        float deta = Jet_eta[0] - LepGood_eta[ilep];
        DrJet1Lep = sqrt( pow(dphi,2) + pow(deta,2) );
      }
      else
      {
        Jet1Pt = -999.;
        Jet1Eta = -999.;
        DrJet1Lep = -999.;
        Jet2Pt = -999.;
        Jet2Eta = -999.;
      }

      if(nJet20>=2)
      {
        float dphijj, detajj;
        Jet2Pt = Jet_pt[1];
        Jet2Eta = Jet_eta[1];
        dphijj = DeltaPhi(Jet_phi[0], Jet_phi[1]);
        detajj = Jet_eta[0] - Jet_eta[1];
        DPhiJet1Jet2 = dphijj;
        DrJet1Jet2 = sqrt( pow(dphijj,2) + pow(detajj,2) );

        float dphi = DeltaPhi(Jet_phi[1], LepGood_phi[ilep]);
        float deta = Jet_eta[1] - LepGood_eta[ilep];
        DrJet2Lep = sqrt( pow(dphi,2) + pow(deta,2) );
      }
      else
      {
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
      for(Int_t j = 0; j < nJet20; j++)
      {
        if((Jet_btagCSV[j] != -10)  &&  (Jet_btagCSV[j] > BtagMax))
        {
          BtagMax = Jet_btagCSV[j];
          iBtag = j;
        }
      }
      JetHBpt = Jet_pt[iBtag];
      float dphib, detab;
      dphib = DeltaPhi(Jet_phi[iBtag], LepGood_phi[ilep]);
      detab = Jet_eta[iBtag] - LepGood_eta[ilep];
      DrJetHBLep = sqrt( pow(dphib,2) + pow(detab,2) );

      LepChg=LepGood_charge[ilep];
      LepID=LepGood_pdgId[ilep];
      LepPt=LepGood_pt[ilep];
      LepEta=LepGood_eta[ilep];
      LepDxy=LepGood_dxy[ilep];
      LepDz=LepGood_dz[ilep];
      LepSip3=LepGood_sip3d[ilep];
      LepIso03=LepGood_relIso03[ilep];
      LepIso04=LepGood_relIso04[ilep];
      HT20 = 0.;
      HT25 = 0.;
      HT30 = 0.;
      Njet = 0;
      for(Int_t j = 0; j < nJet20; j++)
      {
        if(Jet_pt[j] > 20.)
        {
          HT20 += Jet_pt[j];
          Njet += 1;
        }
        if(Jet_pt[j] > 25.)
          HT25 += Jet_pt[j];
        if(Jet_pt[j] > 30.)
          HT30 += Jet_pt[j];
      }

      Run = run;
      Event = evt;
      LumiSec = lumi;
      XS = xsec;
      //XS = file.crossSection;

      PFMET170JetIdCleaned                = HLT_PFMET170_JetIdCleaned;
      PFMET90_PFMHT90_IDTight             = HLT_PFMET90_PFMHT90_IDTight;
      PFMETNoMu90_PFMHTNoMu90_IDTight     = HLT_PFMETNoMu90_PFMHTNoMu90_IDTight;
      HBHENoiseFilter                     = Flag_HBHENoiseFilter;
      HBHENoiseIsoFilter                  = Flag_HBHENoiseIsoFilter;
      eeBadScFilter                       = Flag_eeBadScFilter;
      EcalDeadCellTriggerPrimitiveFilter  = Flag_EcalDeadCellTriggerPrimitiveFilter;
      goodVertices                        = Flag_goodVertices;

      // Skim
      bool emu = (nGoodMu == 1)  ||  (nGoodEl == 1);
      bool isISR = (Jet_pt[0] > 110.)  &&  (Njet > 0);
      bool dPhi = DPhiJet1Jet2 < 2.5;
      bool met = Met > 100.;
      if(!emu)     continue;
      if(!isISR)   continue;
      if(!dPhi)    continue;
      if(!met)     continue;

      if(abs(LepGood_pdgId[ilep]) == 13  &&  doSync)
      {
        SyFile << "Run:LS:Ev " << run << ":" << lumi << ":" << evt << std::endl;
        SyFile << " pT(l): " << LepPt << " eta(l): " << LepEta << " pdgID: " << LepID << std::endl;
        SyFile << " Met: " << Met << " Q80: " << Q80 << " CosDeltaPhi: " << CosDeltaPhi << std::endl;
        SyFile << " N(j): " << Njet << " pT(j1): " << Jet1Pt << " pT(j2): " << Jet2Pt << " pT(j3): " << Jet_pt[2] << " HT: " << HT25 << std::endl;
        //SyFile << " eta(j1): " << Jet1Eta << " eta(j2): " << Jet2Eta;
        //SyFile << " dPhi(j1,j2): " << DPhiJet1Jet2;
        SyFile << " N(b): " << NbLoose30 << " pT(b): " << JetHBpt << std::endl;

        for(Int_t j = 0; j < Njet; j++)
        {
          SyFile << " pT(j): " << Jet_pt[j] << " btag(j): " << Jet_btagCSV[j] << std::endl;
        }

        SyFile << " dR(j1,l): " << DrJet1Lep << " dR(b,l): " << DrJetHBLep << " dR(j1,j2): " << DrJet1Jet2 << " M(l,j): " << JetLepMass << " M(3j): " << J3Mass << std::endl;
      }

      bdttree->Fill();
    }
    std::cout << std::endl;

    foutput.cd();
    bdttree->Write("",TObject::kOverwrite);
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
  std::cout << "\t--json\t- Used to specify the json file which describes the sample, this option is obligatory. The paraeter immediately after this one should be the desired json file" << std::endl;
  std::cout << "\t--outDir\t- Used to specify the output directory. The parameter immediately after this one should be the desired directory. By default the directory './OUT/' is used" << std::endl;
  std::cout << "\t--doSync\t- Whether a txt file with the event output should be created for synchronization purposes." << std::endl;

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

  return abs(x);
}
