void BDToutput(){

  gStyle->SetOptStat(0);
  gStyle->SetOptFit(1);

  TChain DataFile("bdttree");
  DataFile.Add("SET5/PseudoData_bdt.root");

  TChain TopFile("bdttree");
  TopFile.Add("SET5/TTJets_LO_bdt.root");
  float tjxs;
  TopFile.SetBranchAddress("XS",&tjxs);
  TopFile.GetEntry(0);
  TH1D *tjnev = new TH1D("tjnev","tjnev",10,0.,10000000.);
  TopFile.Draw("Nevt>>tjnev");
  double *tjDen = TopFile.GetV1();
  double tjDeno = tjDen[0];
  TChain W1File("bdttree");
  W1File.Add("SET5/Wjets_100to200_bdt.root");
  float w1xs;
  W1File.SetBranchAddress("XS",&w1xs);
  W1File.GetEntry(0);
  TH1D *w1nev = new TH1D("w1nev","w1nev",10,0.,10000000.);
  W1File.Draw("Nevt>>w1nev");
  double *w1Den = W1File.GetV1();
  double w1Deno = w1Den[0];
  TChain W2File("bdttree");
  W2File.Add("SET5/Wjets_200to400_bdt.root");
  float w2xs;
  W2File.SetBranchAddress("XS",&w2xs);
  W2File.GetEntry(0);
  TH1D *w2nev = new TH1D("w2nev","w2nev",10,0.,10000000.);
  W2File.Draw("Nevt>>w2nev");
  double *w2Den = W2File.GetV1();
  double w2Deno = w2Den[0];
  TChain W3File("bdttree");
  W3File.Add("SET5/Wjets_400to600_bdt.root");
  float w3xs;
  W3File.SetBranchAddress("XS",&w3xs);
  W3File.GetEntry(0);
  TH1D *w3nev = new TH1D("w3nev","w3nev",10,0.,10000000.);
  W3File.Draw("Nevt>>w3nev");
  double *w3Den = W3File.GetV1();
  double w3Deno = w3Den[0];
  TChain W4File("bdttree");
  W4File.Add("SET5/Wjets_600toInf_bdt.root");
  float w4xs;
  W4File.SetBranchAddress("XS",&w4xs);
  W4File.GetEntry(0);
  TH1D *w4nev = new TH1D("w4nev","w4nev",10,0.,10000000.);
  W4File.Draw("Nevt>>w4nev");
  double *w4Den = W4File.GetV1();
  double w4Deno = w4Den[0];

  const double Lumi = 5000.;


  // MC Triggers
  TCut trgM   = "Event>-9999.";
  // Data Triggers
  TCut trgD   = "Event>-9999.";

  // Lepton selection
  // Muon channel
  //  TCut muon = "(abs(LepID)==13)&&(abs(LepEta)<2.1)&&(LepChg<0.)&&(LepIso<0.2)&&(abs(LepDxy)<0.02)&&(abs(LepDz)<0.5)";
  //  TCut muon = "(abs(LepID)==13)&&(abs(LepEta)<2.1)&&(LepChg<0.)&&(LepIso<0.2)";
  //  TCut muon = "(abs(LepID)==13)&&(LepChg<0.)&&(LepIso<0.2)";
  //TCut muon = "(abs(LepID)==13)&&(LepIso<0.2)";
  TCut muon = "nGoodMu==1";
  // Electron channel
  //  TCut electron = "(abs(LepID)==11)&&(abs(LepEta)<2.1)&&(LepChg<0.)&&(LepIso<0.2)";
  //  TCut electron = "(abs(LepID)==11)&&(LepChg<0.)&&(LepIso<0.2)";
  //TCut electron = "(abs(LepID)==11)&&(LepIso<0.2)";
  TCut electron = "nGoodEl==1";
  // Both channels
  TCut emu = muon||electron;
  // Decide
  TCut singlep = emu;
 
  // MET
  TCut met    = "Met>300.";

  // MT
  TCut mta = "mt<60";
  TCut mtb = "(60<mt)&&(mt<88)";
  TCut mtc = "88<mt";

  // Jets
  TCut ISRjet = "Jet1Pt > 110.";
  TCut njet = "Njet30>1";

  // HT
  TCut HT = "HT20 > 400.";

  // btag
  TCut Bloose = "NbLoose30>0";
  TCut NoBloose = "NbLoose30==0";
  TCut Bhard = "NbTight30>0";
  TCut NoBhard = "NbTight30==0";

  // Signal regions
  TCut SR1a = met+NoBloose+HT+mta;
  TCut SR1b = met+NoBloose+HT+mtb;
  TCut SR1c = met+NoBloose+HT+mtc;
  TCut SR2  = met+ISRjet+Bloose+NoBhard;

  // BDT cut
  TCut BDT = "bdt1>0.5";

  TCut dphij1j2 = "DPhiJet1Jet2 < 2.5";

  TCut presel = singlep+ISRjet+dphij1j2+met;

  // Selection
  TCut Coup   = presel;
  //  TCut Coup   = singlep+ISRjt+Bhard;
  //  TCut Coup   = singlep+ISRjt+NoBhard;
  // Warning: When using Pseudo-data:
  //          Don't use single-lepton cut: It's been created w
  //          TCut Coup   = singlep+ISRjt;
  //TCut CoupD = ISRjt;
  //  TCut CoupD = ISRjt+Bhard;
  //  TCut CoupD = ISRjt+NoBhard;

  TH1F *dbdt = new TH1F("dbdt","Data/MC vs BDToutput",30,-0.50,0.70);
  dbdt->Sumw2();
  DataFile.Draw("BDT>>dbdt",Coup);
  dbdt->SetMarkerStyle(20);
  dbdt->SetMarkerSize(0.6);

  TH1F *tbdt = new TH1F("tbdt","Data/MC vs BDToutput",30,-0.50,0.70);
  tbdt->Sumw2();
  TopFile.Draw("BDT>>tbdt",Coup);
  tbdt->Scale(Lumi*tjxs/tjDeno);
  tbdt->SetFillColor(4);
  TH1F *w1bdt = new TH1F("w1bdt","Data/MC vs BDToutput",30,-0.50,0.70);
  w1bdt->Sumw2();
  W1File.Draw("BDT>>w1bdt",Coup);
  w1bdt->Scale(Lumi*w1xs/w1Deno);
  w1bdt->SetFillColor(3);
  TH1F *w2bdt = new TH1F("w2bdt","Data/MC vs BDToutput",30,-0.50,0.70);
  w2bdt->Sumw2();
  W2File.Draw("BDT>>w2bdt",Coup);
  w2bdt->Scale(Lumi*w2xs/w2Deno);
  w2bdt->SetFillColor(3);
  TH1F *w3bdt = new TH1F("w3bdt","Data/MC vs BDToutput",30,-0.50,0.70);
  w3bdt->Sumw2();
  W3File.Draw("BDT>>w3bdt",Coup);
  w3bdt->Scale(Lumi*w3xs/w3Deno);
  w3bdt->SetFillColor(3);
  TH1F *w4bdt = new TH1F("w4bdt","Data/MC vs BDToutput",30,-0.50,0.70);
  w4bdt->Sumw2();
  W4File.Draw("BDT>>w4bdt",Coup);
  w4bdt->Scale(Lumi*w4xs/w4Deno);
  w4bdt->SetFillColor(3);

  TH1F *bckgbdt = static_cast<TH1F*>(tbdt->Clone("Background"));
  bckgbdt->Add(w1bdt);
  bckgbdt->Add(w2bdt);
  bckgbdt->Add(w3bdt);
  bckgbdt->Add(w4bdt);
  TH1F *ratiobdt = static_cast<TH1F*>(dbdt->Clone("Ratio"));
  ratiobdt->Divide(bckgbdt);


  TCanvas *c1 = new TCanvas("c1"," ",1100,700);
  c1->SetBorderSize(2);
  THStack *ratioPlot = new THStack("RatSt", "Data/MC vs BDToutput; BDT output; Evt / 25 u. bdt");
  ratioPlot->SetMinimum(0.5);
  ratioPlot->SetMaximum(1.5);
  ratioPlot->Add(ratiobdt);

  c1->SetFillColor(10);
  c1->SetBorderSize(2);
  c1->SetGridx();
  c1->SetGridy();
  ratioPlot->Draw();
  ratiobdt->Fit("pol0","","",0.0,0.5);
  std::string PlotTitle = "plots/SF_presel.png";
  c1->SaveAs(PlotTitle.c_str());

}
