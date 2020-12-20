void DMv_M3j(){

gStyle->SetOptStat(000000);
gStyle->SetPalette(1);

TChain DM1("bdttree");
DM1.Add("/lstore/cms/cbeiraod/Stop4Body/nTuples_v2017-10-19_test_bdt10/T2DegStop_300_290_bdt.root");
TChain DM2("bdttree");
DM2.Add("/lstore/cms/cbeiraod/Stop4Body/nTuples_v2017-10-19_test_bdt30/T2DegStop_300_270_bdt.root");
TChain DM3("bdttree");
DM3.Add("/lstore/cms/cbeiraod/Stop4Body/nTuples_v2017-10-19_test_bdt50/T2DegStop_300_250_bdt.root");
TChain DM4("bdttree");
DM4.Add("/lstore/cms/cbeiraod/Stop4Body/nTuples_v2017-10-19_test_bdt80/T2DegStop_300_220_bdt.root");
TChain TT("bdttree");
TT.Add("/lstore/cms/cbeiraod/Stop4Body/nTuples_v2017-10-19_test_skimmed/TT_pow_skimmed.root");


/////////////////////////////////////////////////////////////
// Define cuts

// Lepton selection
TCut singlep = "LepPt < 1000.";

// Jets
TCut ISRjet = "Jet1Pt > 110.";
TCut dphij1j2 = "(DPhiJet1Jet2 < 2.5)||(Jet2Pt<60.)";
TCut njet = "Njet>=4";

// HT
TCut HT = "HT > 200.";

// MET
TCut met    = "Met>280.";

// Preselection
TCut presel = singlep+dphij1j2+ISRjet+HT+met+njet;


/////////////////////////////////////////////////////////////

const Int_t Xbin=50;
const Double_t Xmin=0.;
const Double_t Xmax=500.;

TH1F *tt = new TH1F("tt","t#bar{t};M(3j)",Xbin,Xmin,Xmax);
TT.Draw("J3Mass>>tt",presel);
double ttn = tt->GetEntries();
tt->Scale(1./ttn);
tt->SetFillColor(kGray);
//tt->SetLineWidth(2);
TH1F *dm1 = new TH1F("dm1","Signal=(300,290);M(3j)",Xbin,Xmin,Xmax);
DM1.Draw("J3Mass>>dm1",presel);
double tn = dm1->GetEntries();
dm1->Scale(1./tn);
dm1->SetLineColor(kRed);
dm1->SetLineWidth(2);
TH1F *dm2 = new TH1F("dm2","Signal=(300,270);M(3j)",Xbin,Xmin,Xmax);
DM2.Draw("J3Mass>>dm2",presel);
double sn = dm2->GetEntries();
dm2->Scale(1./sn);
dm2->SetLineColor(kRed+1);
dm2->SetLineWidth(2);
TH1F *dm3 = new TH1F("dm3","Signal=(300,250);M(3j)",Xbin,Xmin,Xmax);
DM3.Draw("J3Mass>>dm3",presel);
double w1n = dm3->GetEntries();
dm3->Scale(1./w1n);
dm3->SetLineColor(kRed+3);
dm3->SetLineWidth(2);
TH1F *dm4 = new TH1F("dm4","Signal=(300,220);M(3j)",Xbin,Xmin,Xmax);
DM4.Draw("J3Mass>>dm4",presel);
double w2n = dm4->GetEntries();
dm4->Scale(1./w2n);
dm4->SetLineColor(kRed+4);
dm4->SetLineWidth(2);

/////////////////////////////////////////////////////////////

TCanvas *c1 = new TCanvas("c1"," ",1400,1000);

gStyle->SetOptStat(000000);
gStyle->SetOptTitle(0);
gStyle->SetOptStat(0);

c1->SetFillColor(10);
c1->SetBorderSize(2);

//c1->SetGridx();
//c1->SetGridy();
//c1->SetLogy();
tt->SetMaximum(0.035);
//tt->SetMinimum(0.001);
tt->Draw();
dm1->Draw("same");
dm2->Draw("same");
dm3->Draw("same");
dm4->Draw("same");

 TLegend *legA;
 legA = gPad->BuildLegend(0.865,0.65,0.65,0.95,"");
 legA->SetFillColor(0);
 legA->SetFillStyle(0);
 legA->SetLineColor(0);
 legA->SetHeader("");
 legA->SetTextFont(42);

 char Buffer[1024];
 sprintf(Buffer, "%.1f fb^{-1} (%.1f TeV)", 41.2, 13.0);
 TLatex *   tex  = new TLatex(0.805,0.975,Buffer);
 tex->SetNDC();
 tex->SetTextAlign(33);
 tex->SetTextFont(42);
 tex->SetTextSize(0.038);
 tex->SetLineWidth(2);
 tex->Draw();
 TLatex *   tex2 = new TLatex(0.15,0.965,"#font[61]{CMS} #font[52]{Preliminary}");
 tex2->SetNDC();
 tex2->SetTextAlign(13);
 tex2->SetTextFont(42);
 tex2->SetTextSize(0.038);
 tex2->SetLineWidth(2);
 tex2->Draw();

 c1->SaveAs("DMv_M3j.pdf");

}
