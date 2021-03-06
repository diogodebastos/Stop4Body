void DMv_Njet(){

gStyle->SetOptStat(000000);
gStyle->SetPalette(1);

std::string path17 = "/lstore/cms/dbastos/Stop4Body/nTuples17_nanoAOD_v2020-11-16_test";

TChain DM1("bdttree");
DM1.Add((path17+"_bdt10/T2DegStop_300_290_bdt.root").c_str());

TChain DM2("bdttree");
DM2.Add((path17+"_bdt30/T2DegStop_300_270_bdt.root").c_str());

TChain DM3("bdttree");
DM3.Add((path17+"_bdt50/T2DegStop_300_250_bdt.root").c_str());

TChain DM4("bdttree");
DM4.Add((path17+"_bdt80/T2DegStop_300_220_bdt.root").c_str());

TChain TT("bdttree");
TT.Add((path17+"/WJetsToLNu_Pt_250To400.root").c_str());


/////////////////////////////////////////////////////////////
// Define cuts

// Lepton selection
//TCut singlep = "(isTight==1)";
TCut singlep = "(nGoodEl_cutId_loose+nGoodMu_cutId_medium == 1)";
TCut lept = "LepPt < 1000.";

// Jets
TCut ISRjet = "Jet1Pt > 110.";
TCut dphij1j2 = "(DPhiJet1Jet2 < 2.5)||(Jet2Pt<60.)";

// HT
TCut HT = "HT > 200.";

// MET
TCut met    = "Met>280.";

// Preselection
TCut presel = singlep+dphij1j2+ISRjet+HT+met;

/////////////////////////////////////////////////////////////

const Int_t Xbin=10;
const Double_t Xmin=0.;
const Double_t Xmax=10.;

TH1F *tt = new TH1F("tt","Wjets;N(jet)",Xbin,Xmin,Xmax);
TT.Draw("Njet>>tt",presel);
double ttn = tt->GetEntries();
tt->Scale(1./ttn);
tt->SetFillColor(kGray);
//tt->SetLineWidth(2);
TH1F *dm1 = new TH1F("dm1","Signal=(300,290);N(jet)",Xbin,Xmin,Xmax);
DM1.Draw("Njet>>dm1",presel);
double tn = dm1->GetEntries();
dm1->Scale(1./tn);
dm1->SetLineColor(kRed);
dm1->SetLineWidth(2);
TH1F *dm2 = new TH1F("dm2","Signal=(300,270);N(jet)",Xbin,Xmin,Xmax);
DM2.Draw("Njet>>dm2",presel);
double sn = dm2->GetEntries();
dm2->Scale(1./sn);
dm2->SetLineColor(kRed+1);
dm2->SetLineWidth(2);
TH1F *dm3 = new TH1F("dm3","Signal=(300,250);N(jet)",Xbin,Xmin,Xmax);
DM3.Draw("Njet>>dm3",presel);
double w1n = dm3->GetEntries();
dm3->Scale(1./w1n);
dm3->SetLineColor(kRed+3);
dm3->SetLineWidth(2);
TH1F *dm4 = new TH1F("dm4","Signal=(300,220);N(jet)",Xbin,Xmin,Xmax);
DM4.Draw("Njet>>dm4",presel);
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
tt->SetMaximum(0.5);
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

 c1->SaveAs("DMv_Njet.pdf");

}
