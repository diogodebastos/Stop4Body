void DMv_JetbPT(){

gStyle->SetOptStat(000000);
gStyle->SetPalette(1);

TChain DM1("bdttree");
DM1.Add("/lstore/cms/cbeiraod/Stop4Body/nTuples_v2017-10-19_test/T2DegStop_300_290.root");
TChain DM2("bdttree");
DM2.Add("/lstore/cms/cbeiraod/Stop4Body/nTuples_v2017-10-19_test/T2DegStop_300_270.root");
TChain DM3("bdttree");
DM3.Add("/lstore/cms/cbeiraod/Stop4Body/nTuples_v2017-10-19_test/T2DegStop_300_250.root");
TChain DM4("bdttree");
DM4.Add("/lstore/cms/cbeiraod/Stop4Body/nTuples_v2017-10-19_test/T2DegStop_350_270.root");


/////////////////////////////////////////////////////////////
// Define cuts

// Lepton selection
TCut singlep = "LepPt < 30.";

// Jets
TCut ISRjet = "Jet1Pt > 100.";
TCut dphij1j2 = "(DPhiJet1Jet2 < 2.5)||((30.<Jet2Pt)&&(Jet2Pt<60.))";

// MET
TCut met    = "Met>300.";

// Preselection
TCut presel = singlep+dphij1j2+ISRjet;

/////////////////////////////////////////////////////////////

const Int_t Xbin=50;
const Double_t Xmin=50.;
const Double_t Xmax=550.;

TH1F *dm1 = new TH1F("dm1","pT(b)",Xbin,Xmin,Xmax);
DM1.Draw("Jet1Pt>>dm1",presel);
double tn = dm1->GetEntries();
dm1->Scale(1./tn);
dm1->SetLineColor(kRed);
dm1->SetLineWidth(2);
TH1F *dm2 = new TH1F("dm2","pT(b)",Xbin,Xmin,Xmax);
DM2.Draw("Jet1Pt>>dm2",presel);
double sn = dm2->GetEntries();
dm2->Scale(1./sn);
dm2->SetLineColor(kRed+1);
dm2->SetLineWidth(2);
TH1F *dm3 = new TH1F("dm3","pT(b)",Xbin,Xmin,Xmax);
DM3.Draw("Jet1Pt>>dm3",presel);
double w1n = dm3->GetEntries();
dm3->Scale(1./w1n);
dm3->SetLineColor(kRed+3);
dm3->SetLineWidth(2);
TH1F *dm4 = new TH1F("dm4","pT(b)",Xbin,Xmin,Xmax);
DM4.Draw("Jet1Pt>>dm4",presel);
double w2n = dm4->GetEntries();
dm4->Scale(1./w2n);
dm4->SetLineColor(kRed+4);
dm4->SetLineWidth(2);

/////////////////////////////////////////////////////////////

TCanvas *c1 = new TCanvas("c1"," ",1400,1000);
c1->SetFillColor(10);
c1->SetBorderSize(2);

c1->SetGridx();
c1->SetGridy();
dm4->Draw();
dm3->Draw("same");
dm2->Draw("same");
dm1->Draw("same");
 TLegend *lg = new TLegend(.65,.65,0.99,1.);
 lg->SetFillColor(0);
 lg->AddEntry(dm1,"Signal (300,290)","l");
 lg->AddEntry(dm2,"Signal (300,270)","l");
 lg->AddEntry(dm3,"Signal (300,250)","l");
 lg->AddEntry(dm4,"Signal (350,270)","l");
 lg->Draw();
 c1->SaveAs("DMv_JetbPT.pdf");

}
