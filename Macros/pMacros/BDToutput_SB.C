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

// Compare normalized BDToutput shapes for S & B

void BDToutput_SB(int delta=10, bool do2017 = false){

  gStyle->SetOptStat(0);

  std::string path;
  std::string dm = std::to_string(delta);

  std::string year;
  double Lumi;
  TCut lept;

  TChain TopFile("bdttree");
  TChain W1File("bdttree");
  TChain W2File("bdttree");
  TChain W3File("bdttree");
  TChain W4File("bdttree");
  TChain W5File("bdttree");
  TChain W6File("bdttree");
  TChain W7File("bdttree");

  if(do2017){
    Lumi = 41167;
    year = "2017";
    path = "/home/t3cms/dbastos/LSTORE/Stop4Body/nTuples_v2019-06-05_test_bdt";

    TopFile.Add((path+dm+"/TTJets_bdt.root").c_str());
    W1File.Add((path+dm+"/WJetsToLNu_HT100To200_bdt.root").c_str());
    W2File.Add((path+dm+"/WJetsToLNu_HT200To400_bdt.root").c_str());
    W3File.Add((path+dm+"/WJetsToLNu_HT400To600_bdt.root").c_str());
    W4File.Add((path+dm+"/WJetsToLNu_HT600To800_bdt.root").c_str());
    W5File.Add((path+dm+"/WJetsToLNu_HT800To1200_bdt.root").c_str());
    W6File.Add((path+dm+"/WJetsToLNu_HT1200To2500_bdt.root").c_str());
    W7File.Add((path+dm+"/WJetsToLNu_HT2500ToInf_bdt.root").c_str());
  }
  else{
    Lumi = 36416.8;
    path = "/lstore/cms/bargassa/Stop4body/SET2102_DM";
    year = "2016";

    TopFile.Add((path+dm+"/TT_pow_bdt.root").c_str());
    W1File.Add((path+dm+"/Wjets_100to200_bdt.root").c_str());
    W2File.Add((path+dm+"/Wjets_200to400_bdt.root").c_str());
    W3File.Add((path+dm+"/Wjets_400to600_bdt.root").c_str());
    W4File.Add((path+dm+"/Wjets_600to800_bdt.root").c_str());
    W5File.Add((path+dm+"/Wjets_800to1200_bdt.root").c_str());
    W6File.Add((path+dm+"/Wjets_1200to2500_bdt.root").c_str());
    W7File.Add((path+dm+"/Wjets_2500toInf_bdt.root").c_str());
  }

  TChain Z1File("bdttree");
  Z1File.Add((path+dm+"/ZJetsToNuNu_HT100to200_bdt.root").c_str());
  TChain Z2File("bdttree");
  Z2File.Add((path+dm+"/ZJetsToNuNu_HT200to400_bdt.root").c_str());
  TChain Z3File("bdttree");
  Z3File.Add((path+dm+"/ZJetsToNuNu_HT400to600_bdt.root").c_str());
  TChain Z4File("bdttree");
  Z4File.Add((path+dm+"/ZJetsToNuNu_HT600to800_bdt.root").c_str());
  TChain Z5File("bdttree");
  Z5File.Add((path+dm+"/ZJetsToNuNu_HT800to1200_bdt.root").c_str());
  TChain Z6File("bdttree");
  Z6File.Add((path+dm+"/ZJetsToNuNu_HT1200to2500_bdt.root").c_str());
  TChain Z7File("bdttree");
  Z7File.Add((path+dm+"/ZJetsToNuNu_HT2500toInf_bdt.root").c_str());

  TChain SgnFile("bdttree");
  std::string title;
  if(dm == "80") {
    SgnFile.Add((path+dm+"/T2DegStop_500_420_bdt.root").c_str());
    lept = "LepPt<10000.";
    title = "(500,420)";
  }
  else if(dm == "10"){
    SgnFile.Add((path+dm+"/T2DegStop_500_490_bdt.root").c_str());
    lept = "LepPt<30.";
    title = "(500,490)";
  }

  TCut singlep = lept;

  TCut ISRjet = "Jet1Pt > 110.";
  TCut dphij1j2 = "(DPhiJet1Jet2 < 2.5)||(Jet2Pt<60.)";
  TCut met    = "Met>280.";
  TCut HT = "HT > 200.";
  TCut sr = "BDT>0.3";

  // Preselection
  //  TCut presel = singlep;
  //  TCut presel = singlep+ISRjet;
  //  TCut presel = singlep+ISRjet+dphij1j2;
  TCut presel = singlep+ISRjet+dphij1j2+met+HT;

  TCut Coup = presel;

  const Int_t Xbin=30;
  const Double_t Xmin=-0.6;
  const Double_t Xmax=0.6;

  TH1F *s1bdt = new TH1F("s1bdt",("Signal="+title+";BDT output").c_str(),Xbin,Xmin,Xmax);
  s1bdt->Sumw2();
  SgnFile.Draw("BDT>>s1bdt","weight*splitFactor"*Coup);
  s1bdt->Scale(Lumi);
  s1bdt->SetLineColor(4);
  TH1F *tbdt = new TH1F("tbdt","t#bar{t};BDT output",Xbin,Xmin,Xmax);
  tbdt->Sumw2();
  TopFile.Draw("BDT>>tbdt","weight*splitFactor"*Coup);
  tbdt->Scale(Lumi);
  tbdt->SetLineColor(3);
  TH1F *w1bdt = new TH1F("w1bdt","W+jets;BDT output",Xbin,Xmin,Xmax);
  w1bdt->Sumw2();
  W1File.Draw("BDT>>w1bdt","weight*splitFactor"*Coup);
  w1bdt->Scale(Lumi);
  TH1F *w2bdt = new TH1F("w2bdt","W+jets;BDT output",Xbin,Xmin,Xmax);
  w2bdt->Sumw2();
  W2File.Draw("BDT>>w2bdt","weight*splitFactor"*Coup);
  w2bdt->Scale(Lumi);
  TH1F *w3bdt = new TH1F("w3bdt","W+jets;BDT output",Xbin,Xmin,Xmax);
  w3bdt->Sumw2();
  W3File.Draw("BDT>>w3bdt","weight*splitFactor"*Coup);
  w3bdt->Scale(Lumi);
  TH1F *w4bdt = new TH1F("w4bdt","W+jets;BDT output",Xbin,Xmin,Xmax);
  w4bdt->Sumw2();
  W4File.Draw("BDT>>w4bdt","weight*splitFactor"*Coup);
  w4bdt->Scale(Lumi);
  TH1F *w5bdt = new TH1F("w5bdt","W+jets;BDT output",Xbin,Xmin,Xmax);
  w5bdt->Sumw2();
  W5File.Draw("BDT>>w5bdt","weight*splitFactor"*Coup);
  w5bdt->Scale(Lumi);
  TH1F *w6bdt = new TH1F("w6bdt","W+jets;BDT output",Xbin,Xmin,Xmax);
  w6bdt->Sumw2();
  W6File.Draw("BDT>>w6bdt","weight*splitFactor"*Coup);
  w6bdt->Scale(Lumi);
  TH1F *w7bdt = new TH1F("w7bdt","W+jets;BDT output",Xbin,Xmin,Xmax);
  w7bdt->Sumw2();
  W7File.Draw("BDT>>w7bdt","weight*splitFactor"*Coup);
  w7bdt->Scale(Lumi);

  TH1F *z1bdt = new TH1F("z1bdt","Z #rightarrow #nu#nu + jets;BDT output",Xbin,Xmin,Xmax);
  z1bdt->Sumw2();
  Z1File.Draw("BDT>>z1bdt","weight*splitFactor"*Coup);
  z1bdt->Scale(Lumi);
  TH1F *z2bdt = new TH1F("z2bdt","Z #rightarrow #nu#nu + jets;BDT output",Xbin,Xmin,Xmax);
  z2bdt->Sumw2();
  Z2File.Draw("BDT>>z2bdt","weight*splitFactor"*Coup);
  z2bdt->Scale(Lumi);
  TH1F *z3bdt = new TH1F("z3bdt","Z #rightarrow #nu#nu + jets;BDT output",Xbin,Xmin,Xmax);
  z3bdt->Sumw2();
  Z3File.Draw("BDT>>z3bdt","weight*splitFactor"*Coup);
  z3bdt->Scale(Lumi);
  TH1F *z4bdt = new TH1F("z4bdt","Z #rightarrow #nu#nu + jets;BDT output",Xbin,Xmin,Xmax);
  z4bdt->Sumw2();
  Z4File.Draw("BDT>>z4bdt","weight*splitFactor"*Coup);
  z4bdt->Scale(Lumi);
  TH1F *z5bdt = new TH1F("z5bdt","Z #rightarrow #nu#nu + jets;BDT output",Xbin,Xmin,Xmax);
  z5bdt->Sumw2();
  Z5File.Draw("BDT>>z5bdt","weight*splitFactor"*Coup);
  z5bdt->Scale(Lumi);
  TH1F *z6bdt = new TH1F("z6bdt","Z #rightarrow #nu#nu + jets;BDT output",Xbin,Xmin,Xmax);
  z6bdt->Sumw2();
  Z6File.Draw("BDT>>z6bdt","weight*splitFactor"*Coup);
  z6bdt->Scale(Lumi);
  TH1F *z7bdt = new TH1F("z7bdt","Z #rightarrow #nu#nu + jets;BDT output",Xbin,Xmin,Xmax);
  z7bdt->Sumw2();
  Z7File.Draw("BDT>>z7bdt","weight*splitFactor"*Coup);
  z7bdt->Scale(Lumi);

  TH1F *wjetsbdt = static_cast<TH1F*>(w1bdt->Clone("Background"));
  wjetsbdt->Add(w2bdt);
  wjetsbdt->Add(w3bdt);
  wjetsbdt->Add(w4bdt);
  wjetsbdt->Add(w5bdt);
  wjetsbdt->Add(w6bdt);
  wjetsbdt->Add(w7bdt);
  wjetsbdt->SetLineColor(2);

  TH1F *zinvbdt = static_cast<TH1F*>(z1bdt->Clone("Background"));
  zinvbdt->Add(z2bdt);
  zinvbdt->Add(z3bdt);
  zinvbdt->Add(z4bdt);
  zinvbdt->Add(z5bdt);
  zinvbdt->Add(z6bdt);
  zinvbdt->Add(z7bdt);
  zinvbdt->SetLineColor(1);

  wjetsbdt->SetMinimum(1.e-01);
  wjetsbdt->SetMaximum(1.5e04);

  ///////////////////////////////////////////////////////////////////////

  TCanvas *c1 = new TCanvas("c1"," ",1100,700);
  gStyle->SetOptStat(000000);
  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);
  c1->SetBorderSize(2);
  c1->SetFillColor(10);
  c1->SetBorderSize(2);
  //  c1->SetGridx();
  //  c1->SetGridy();
  c1->SetLogy();

  wjetsbdt->Draw("l");
  tbdt->Draw("lsame");
  zinvbdt->Draw("lsame");
  s1bdt->Draw("lsame");

  TLegend *legA;
  legA = gPad->BuildLegend(0.885,0.65,0.67,0.95,"");
  legA->SetFillColor(0);
  legA->SetFillStyle(0);
  legA->SetLineColor(0);
  legA->SetHeader("");
  legA->SetTextFont(42);

  char Buffer[1024];
  sprintf(Buffer, "%.1f fb^{-1} (%.1f TeV)", Lumi/1000, 13.0);
  TLatex *   tex  = new TLatex(0.805,0.975,Buffer);
  tex->SetNDC();
  tex->SetTextAlign(33);
  tex->SetTextFont(42);
  tex->SetTextSize(0.038);
  tex->SetLineWidth(2);
  tex->Draw();
  TLatex *   tex2 = new TLatex(0.15,0.965,"#font[61]{CMS}");
  tex2->SetNDC();
  tex2->SetTextAlign(13);
  tex2->SetTextFont(42);
  tex2->SetTextSize(0.038);
  tex2->SetLineWidth(2);
  tex2->Draw();

  std::string PlotTitle = "plots/BDToutput_DM"+dm+"_"+year+".pdf";
  c1->SaveAs(PlotTitle.c_str());

}
