void DM_BPT(){

gStyle->SetOptStat(000000);
gStyle->SetPalette(1);

TChain DM1("bdttree");
DM1.Add("/lstore/cms/cbeiraod/Stop4Body/nTuples_v2017-10-19_test_bdt30/T2DegStop_250_220_bdt.root");
TChain DM2("bdttree");
DM2.Add("/lstore/cms/cbeiraod/Stop4Body/nTuples_v2017-10-19_test_bdt30/T2DegStop_300_270_bdt.root");
TChain DM3("bdttree");
DM3.Add("/lstore/cms/cbeiraod/Stop4Body/nTuples_v2017-10-19_test_bdt30/T2DegStop_400_370_bdt.root");
TChain DM4("bdttree");
DM4.Add("/lstore/cms/cbeiraod/Stop4Body/nTuples_v2017-10-19_test_bdt30/T2DegStop_500_470_bdt.root");

/////////////////////////////////////////////////////////////
// Define cuts

// Lepton selection
TCut singlep = "(isTight==1)";
TCut lept = "LepPt < 1000.";
// Jets
TCut ISRjet = "Jet1Pt > 110.";
TCut dphij1j2 = "DPhiJet1Jet2 < 2.5";
// MET
TCut met    = "Met>280.";
// Preselection
TCut presel = singlep+ISRjet+dphij1j2+met;

/////////////////////////////////////////////////////////////

const Int_t Xbin=50;
const Double_t Xmin=0.;
const Double_t Xmax=1000.;

TH1F *dm1 = new TH1F("dm1","pT(b)",Xbin,Xmin,Xmax);
 DM1.Draw("JetHBpt>>dm1",presel);
double tn = dm1->GetEntries();
dm1->Scale(1./tn);
dm1->SetLineColor(1);
dm1->SetLineWidth(2);
TH1F *dm2 = new TH1F("dm2","pT(b)",Xbin,Xmin,Xmax);
 DM2.Draw("JetHBpt>>dm2",presel);
double sn = dm2->GetEntries();
dm2->Scale(1./sn);
dm2->SetLineColor(2);
dm2->SetLineWidth(2);
TH1F *dm3 = new TH1F("dm3","pT(b)",Xbin,Xmin,Xmax);
 DM3.Draw("JetHBpt>>dm3",presel);
double w1n = dm3->GetEntries();
dm3->Scale(1./w1n);
dm3->SetLineColor(3);
dm3->SetLineWidth(2);
TH1F *dm4 = new TH1F("dm4","pT(b)",Xbin,Xmin,Xmax);
DM4.Draw("JetHBpt>>dm4",presel);
double w2n = dm4->GetEntries();
dm4->Scale(1./w2n);
dm4->SetLineColor(4);
dm4->SetLineWidth(2);

/////////////////////////////////////////////////////////////

TCanvas *c1 = new TCanvas("c1"," ",1400,1000);
c1->SetFillColor(10);
c1->SetBorderSize(2);

//c1->Divide(3,3);

//c1->cd(1);
c1->SetGridx();
c1->SetGridy();
dm1->Draw();
dm4->Draw("same");
dm3->Draw("same");
dm2->Draw("same");
 TLegend *lg = new TLegend(.65,.65,0.99,1.);
 lg->SetFillColor(0);
 lg->AddEntry(dm1,"Signal (250,220)","l");
 lg->AddEntry(dm2,"Signal (300,270)","l");
 lg->AddEntry(dm3,"Signal (400,370)","l");
 lg->AddEntry(dm4,"Signal (500,470)","l");
 lg->Draw();
 c1->SaveAs("DM_BPT.png");

}
