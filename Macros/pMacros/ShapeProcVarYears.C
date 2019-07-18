#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <sstream>

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TCut.h"
#include "TChain.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "string"
#include "vector"

void ShapeProcVarYears(int delta=80, bool doWjets = false, bool doTTbar = false, bool doSignal = false, std::string variable = "BDT"){

gStyle->SetOptStat(000000);
gStyle->SetPalette(1);

//std::string path16 = "/home/t3cms/bargassa/STORE/Stop4body/SET2102_DM";
std::string path16 = "/lstore/cms/cbeiraod/Stop4Body/nTuples_v2017-10-19_test_bdt";
//std::string path16 = "/home/t3cms/dbastos/LSTORE/Stop4Body/nTuples16_v2017-10-19_test_bdt";

/std::string path17 = "/home/t3cms/dbastos/LSTORE/Stop4Body/nTuples_v2019-06-05_test_bdt";
//std::string path17 = "/home/t3cms/dbastos/LSTORE/Stop4Body/nTuples16_v2017-10-19_test_bdt";
//std::string path17 = "/home/t3cms/dbastos/LSTORE/Stop4Body/CrisSamples-DM10-nTuples_v2017-10-19_test_bdt";

std::string dm = std::to_string(delta);
std::string signalPoint;
std::string title = "";
TCut lept;
int dmInt = std::stoi(dm);

if(dmInt >  60) {
  lept = "LepPt<10000.";
  if(dm == "80"){
    signalPoint = "/T2DegStop_500_420_bdt.root";
    //title = "(500,420)";
  }
  else if(dm == "70"){
    signalPoint = "/T2DegStop_525_455_bdt.root";
  }
}
else if(dmInt <= 60){
  lept = "LepPt<30.";
  if(dm == "10"){
    signalPoint = "/T2DegStop_500_490_bdt.root";
    //title = "(500,490)";
  }
  else if(dm == "20"){
    signalPoint = "/T2DegStop_500_480_bdt.root";
  }
  else if(dm == "30"){
    signalPoint = "/T2DegStop_550_520_bdt.root";
  }
  else if(dm == "40"){
    signalPoint = "/T2DegStop_550_510_bdt.root";
  }
  else if(dm == "50"){
    signalPoint = "/T2DegStop_525_475_bdt.root";
  }
  else if(dm == "60"){
    signalPoint = "/T2DegStop_525_465_bdt.root";
  }
}

/////////////////////////////////////////////////////////////
// Define cuts

TCut singlep = lept;
TCut ISRjet = "(Jet1Pt > 110.) && (badCloneMuonMoriond2017 == 1) && (badMuonMoriond2017 == 1)";
TCut dphij1j2 = "(DPhiJet1Jet2 < 2.5)||(Jet2Pt<60.)";
TCut met    = "Met>280.";
TCut HT = "HT > 200.";
TCut sr = "BDT>0.3";

TCut presel = singlep+ISRjet+dphij1j2+met+HT;
TCut region = presel;

/////////////////////////////////////////////////////////////

Int_t Xbin;
Double_t Xmin;
Double_t Xmax;

if(variable == "LepPt"){
  Xbin=40;
  Xmin=0.0;
  Xmax=200.0;
}
else if(variable == "Met"){
  Xbin=35;
  Xmin=250.0;
  Xmax=1000.0;
}
else if(variable == "mt"){
  Xbin=40;
  Xmin=0.0;
  Xmax=300.0;
}
else if(variable == "Jet1Pt"){
  Xbin=25;
  Xmin=100.0;
  Xmax=1000.0;
}
else if(variable == "Njet"){
  Xbin=6;
  Xmin=-0.5;
  Xmax=5.5;
}
else if(variable == "NbLoose"){
  Xbin=6;
  Xmin=-0.5;
  Xmax=5.5;
}
else{
  Xbin=40;
  Xmin=-0.6;
  Xmax=0.6;
}


TH1F* hs = new TH1F("hs",variable.c_str(),Xbin,Xmin,Xmax);
TH1F* hs2 = new TH1F("hs2",variable.c_str(),Xbin,Xmin,Xmax);
if(doWjets){
  title = title + "_WJets";
  //----2016----///////////////////////////////////////////////
  TFile W1((path16+dm+"/Wjets_100to200_bdt.root").c_str(), "READ");
  TTree* t1 = static_cast<TTree*>(W1.Get("bdttree"));
  TFile W2((path16+dm+"/Wjets_200to400_bdt.root").c_str(), "READ");
  TTree* t2 = static_cast<TTree*>(W2.Get("bdttree"));
  TFile W3((path16+dm+"/Wjets_400to600_bdt.root").c_str(), "READ");
  TTree* t3 = static_cast<TTree*>(W3.Get("bdttree"));
  TFile W4((path16+dm+"/Wjets_600to800_bdt.root").c_str(), "READ");
  TTree* t4 = static_cast<TTree*>(W4.Get("bdttree"));
  TFile W5((path16+dm+"/Wjets_800to1200_bdt.root").c_str(), "READ");
  TTree* t5 = static_cast<TTree*>(W5.Get("bdttree"));
  TFile W6((path16+dm+"/Wjets_1200to2500_bdt.root").c_str(), "READ");
  TTree* t6 = static_cast<TTree*>(W6.Get("bdttree"));
  TFile W7((path16+dm+"/Wjets_2500toInf_bdt.root").c_str(), "READ");
  TTree* t7 = static_cast<TTree*>(W7.Get("bdttree"));

  //THStack* hs = new THStack("hs","");

  TH1F *hw1 = new TH1F("hw1",variable.c_str(),Xbin,Xmin,Xmax);
  t1->Draw((variable+">>hw1").c_str(),"weight*splitFactor*36416.8"*region);
  hs->Add(hw1);
  TH1F *hw2 = new TH1F("hw2",variable.c_str(),Xbin,Xmin,Xmax);
  t2->Draw((variable+">>hw2").c_str(),"weight*splitFactor*36416.8"*region);
  hs->Add(hw2);
  TH1F *hw3 = new TH1F("hw3",variable.c_str(),Xbin,Xmin,Xmax);
  t3->Draw((variable+">>hw3").c_str(),"weight*splitFactor*36416.8"*region);
  hs->Add(hw3);
  TH1F *hw4 = new TH1F("hw4",variable.c_str(),Xbin,Xmin,Xmax);
  t4->Draw((variable+">>hw4").c_str(),"weight*splitFactor*36416.8"*region);
  hs->Add(hw4);
  TH1F *hw5 = new TH1F("hw5",variable.c_str(),Xbin,Xmin,Xmax);
  t5->Draw((variable+">>hw5").c_str(),"weight*splitFactor*36416.8"*region);
  hs->Add(hw5);
  TH1F *hw6 = new TH1F("hw6",variable.c_str(),Xbin,Xmin,Xmax);
  t6->Draw((variable+">>hw6").c_str(),"weight*splitFactor*36416.8"*region);
  hs->Add(hw6);
  TH1F *hw7 = new TH1F("hw7",variable.c_str(),Xbin,Xmin,Xmax);
  t7->Draw((variable+">>hw7").c_str(),"weight*splitFactor*36416.8"*region);
  hs->Add(hw7);

  delete t1;
  delete t2;
  delete t3;
  delete t4;
  delete t5;
  delete t6;
  delete t7;

  hs->Scale(1.0/hs->Integral());

  //----2017----///////////////////////////////////////////////
/*  
  TFile F1((path17+dm+"/Wjets_100to200_bdt.root").c_str(), "READ");
  TTree* z1 = static_cast<TTree*>(F1.Get("bdttree"));
  TFile F2((path17+dm+"/Wjets_200to400_bdt.root").c_str(), "READ");
  TTree* z2 = static_cast<TTree*>(F2.Get("bdttree"));
  TFile F3((path17+dm+"/Wjets_400to600_bdt.root").c_str(), "READ");
  TTree* z3 = static_cast<TTree*>(F3.Get("bdttree"));
  TFile F4((path17+dm+"/Wjets_600to800_bdt.root").c_str(), "READ");
  TTree* z4 = static_cast<TTree*>(F4.Get("bdttree"));
  TFile F5((path17+dm+"/Wjets_800to1200_bdt.root").c_str(), "READ");
  TTree* z5 = static_cast<TTree*>(F5.Get("bdttree"));
  TFile F6((path17+dm+"/Wjets_1200to2500_bdt.root").c_str(), "READ");
  TTree* z6 = static_cast<TTree*>(F6.Get("bdttree"));
  TFile F7((path17+dm+"/Wjets_2500toInf_bdt.root").c_str(), "READ");
  TTree* z7 = static_cast<TTree*>(F7.Get("bdttree"));
*/
  TFile F1((path17+dm+"/WJetsToLNu_HT100To200_bdt.root").c_str(), "READ");
  TTree* z1 = static_cast<TTree*>(F1.Get("bdttree"));
  TFile F2((path17+dm+"/WJetsToLNu_HT200To400_bdt.root").c_str(), "READ");
  TTree* z2 = static_cast<TTree*>(F2.Get("bdttree"));
  TFile F3((path17+dm+"/WJetsToLNu_HT400To600_bdt.root").c_str(), "READ");
  TTree* z3 = static_cast<TTree*>(F3.Get("bdttree"));
  TFile F4((path17+dm+"/WJetsToLNu_HT600To800_bdt.root").c_str(), "READ");
  TTree* z4 = static_cast<TTree*>(F4.Get("bdttree"));
  TFile F5((path17+dm+"/WJetsToLNu_HT800To1200_bdt.root").c_str(), "READ");
  TTree* z5 = static_cast<TTree*>(F5.Get("bdttree"));
  TFile F6((path17+dm+"/WJetsToLNu_HT1200To2500_bdt.root").c_str(), "READ");
  TTree* z6 = static_cast<TTree*>(F6.Get("bdttree"));
  TFile F7((path17+dm+"/WJetsToLNu_HT2500ToInf_bdt.root").c_str(), "READ");
  TTree* z7 = static_cast<TTree*>(F7.Get("bdttree"));

  //THStack* hs = new THStack("hs","");

  TH1F *hz1 = new TH1F("hz1",variable.c_str(),Xbin,Xmin,Xmax);
  z1->Draw((variable+">>hz1").c_str(),"weight*splitFactor*41167"*region);
  hs2->Add(hz1);
  TH1F *hz2 = new TH1F("hz2",variable.c_str(),Xbin,Xmin,Xmax);
  z2->Draw((variable+">>hz2").c_str(),"weight*splitFactor*41167"*region);
  hs2->Add(hz2);
  TH1F *hz3 = new TH1F("hz3",variable.c_str(),Xbin,Xmin,Xmax);
  z3->Draw((variable+">>hz3").c_str(),"weight*splitFactor*41167"*region);
  hs2->Add(hz3);
  TH1F *hz4 = new TH1F("hz4",variable.c_str(),Xbin,Xmin,Xmax);
  z4->Draw((variable+">>hz4").c_str(),"weight*splitFactor*41167"*region);
  hs2->Add(hz4);
  TH1F *hz5 = new TH1F("hz5",variable.c_str(),Xbin,Xmin,Xmax);
  z5->Draw((variable+">>hz5").c_str(),"weight*splitFactor*41167"*region);
  hs2->Add(hz5);
  TH1F *hz6 = new TH1F("hz6",variable.c_str(),Xbin,Xmin,Xmax);
  z6->Draw((variable+">>hz6").c_str(),"weight*splitFactor*41167"*region);
  hs2->Add(hz6);
  TH1F *hz7 = new TH1F("hz7",variable.c_str(),Xbin,Xmin,Xmax);
  z7->Draw((variable+">>hz7").c_str(),"weight*splitFactor*41167"*region);
  hs2->Add(hz7);

  delete z1;
  delete z2;
  delete z3;
  delete z4;
  delete z5;
  delete z6;
  delete z7;

  hs2->Scale(1.0/hs2->Integral());
}

TH1F* tbdt = new TH1F("tbdt",variable.c_str(),Xbin,Xmin,Xmax);
TH1F* tbdt2 = new TH1F("tbdt2",variable.c_str(),Xbin,Xmin,Xmax);
if(doTTbar){
  title = title + "_TTbar";
  TFile TT((path16+dm+"/TT_pow_bdt.root").c_str(), "READ");
  TTree* tt = static_cast<TTree*>(TT.Get("bdttree"));
  TH1F *htt = new TH1F("htt",variable.c_str(),Xbin,Xmin,Xmax);
  tt->Draw((variable+">>htt").c_str(),"weight*splitFactor*36416.8"*region);
  tbdt->Add(htt);
  tbdt->Scale(1.0/tbdt->Integral());
  delete htt;
/*  
  TFile TT2((path17+dm+"/TT_pow_bdt.root").c_str(), "READ");
  TTree* tt2 = static_cast<TTree*>(TT2.Get("bdttree"));
  TH1F *htt2 = new TH1F("htt2",variable.c_str(),Xbin,Xmin,Xmax);
  tt2->Draw((variable+">>htt2").c_str(),"weight*splitFactor*36416.8"*region);
  tbdt2->Add(htt2);
  tbdt2->Scale(1.0/tbdt2->Integral());
  delete htt2;
*/
  TFile TT2((path17+dm+"/TTJets_bdt.root").c_str(), "READ");
  TTree* tt2 = static_cast<TTree*>(TT2.Get("bdttree"));
  TH1F *htt2 = new TH1F("htt2",variable.c_str(),Xbin,Xmin,Xmax);
  tt2->Draw((variable+">>htt2").c_str(),"weight*splitFactor*41167"*region);
  tbdt2->Add(htt2);
  tbdt2->Scale(1.0/tbdt2->Integral());
  delete htt2;
}

TH1F* sbdt = new TH1F("sbdt",variable.c_str(),Xbin,Xmin,Xmax);
TH1F* sbdt2 = new TH1F("sbdt2",variable.c_str(),Xbin,Xmin,Xmax);
if(doSignal){
  title = title + "_Signal";
  TFile SGN((path16+dm+signalPoint).c_str(), "READ");
  TTree* tsgn = static_cast<TTree*>(SGN.Get("bdttree"));
  TH1F *hsgn = new TH1F("hsgn",variable.c_str(),Xbin,Xmin,Xmax);
  tsgn->Draw((variable+">>hsgn").c_str(),"weight*splitFactor*36416.8"*region);
  sbdt->Add(hsgn);
  sbdt->Scale(1.0/sbdt->Integral());
  delete hsgn;

  TFile SGN2((path17+dm+signalPoint).c_str(), "READ");
  TTree* tsgn2 = static_cast<TTree*>(SGN2.Get("bdttree"));
  TH1F *hsgn2 = new TH1F("hsgn2",variable.c_str(),Xbin,Xmin,Xmax);
  tsgn2->Draw((variable+">>hsgn2").c_str(),"weight*splitFactor*41167"*region);
  sbdt2->Add(hsgn2);
  sbdt2->Scale(1.0/sbdt2->Integral());
  delete hsgn2;
}

/////////////////////////////////////////////////////////////

TCanvas *c1 = new TCanvas("c1"," ",1400,1000);
c1->SetFillColor(10);
c1->SetBorderSize(2);

c1->SetGridx();
c1->SetGridy();
if(doWjets){
  hs->Draw();
  hs2->SetLineColor(kRed);
  hs2->Draw("same");
}

if(doTTbar){
  if(doWjets){
    tbdt->Draw("same");
  }
  else{
    tbdt->Draw();
  }
  tbdt2->SetLineColor(kRed);
  tbdt2->Draw("same");
}

if(doSignal){
  if(doWjets || doTTbar){
    sbdt->Draw("same");
  }
  else{
    sbdt->Draw();
  }
  sbdt2->SetLineColor(kRed);
  sbdt2->Draw("same");
}

/*
TLegend *lg = new TLegend(.65,.65,0.99,1.);
lg->SetFillColor(0);
lg->AddEntry(hw1,"Wjets 100<HT<200","l");
lg->AddEntry(hw2,"Wjets 200<HT<400","l");
lg->AddEntry(hw3,"Wjets 400<HT<600","l");
lg->AddEntry(hw4,"Wjets 600<HT<800","l");
lg->Draw();
*/
c1->SaveAs(("plots/BDTyears_dm"+dm+title+"_"+variable+".pdf").c_str());
}
