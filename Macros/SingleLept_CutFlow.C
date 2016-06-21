// Macro calculating the yield of different channels
// The yield is provided with its:
// JES central-value, weighted by all efficiencies (Trigger efficiencies / MC scaling / ...)
// +1sigma(JES) value, weighted by all efficiencies
// -1sigma(JES) value, weighted by all efficiencies
//
// The user has to:
// * provide the luminosity Lumi
// * chose which MC contributions to use
// * modify the section "Define analysis" to modify analysis cuts

#include <iomanip>
#include <fstream>
#include "TMath.h"
#include <iostream>
#include "TStyle.h"
#include "TCut.h"
#include "TChain.h"
#include "TH1F.h"
#include "TROOT.h"
#include "TEventList.h"
#include "SingleLept_CutFlow.h"

using namespace std;

void SingleLept_CutFlow (){
  
  gStyle->SetOptStat(000000);

  //////////////////////////////////////////////////////////////////////////
  // Give luminosity (positive value ==> weighting whereas <=0 value ==> unweighting
  // Luminosity
  const double Lumi = 10000.;

  //////////////////////////////////////////////////////////////////////////
  // Define analysis

  // MC Triggers
  TCut trgM   = "Event>-999.";
  // Data Triggers
  TCut trgD   = "Event>-999.";

  // Lepton selection
  // Muon channel
  //  TCut muon = "(abs(LepID)==13)&&(abs(LepEta)<2.1)&&(LepChg<0.)&&(LepIso<0.2)&&(abs(LepDxy)<0.02)&&(abs(LepDz)<0.5)";
  //  TCut muon = "(abs(LepID)==13)&&(abs(LepEta)<2.1)&&(LepChg<0.)&&(LepIso<0.2)";
  TCut muon = "(abs(LepID)==13)&&(LepIso<0.2)";
  //  TCut muon = "(abs(LepID)==13)&&(LepIso<0.2)&&(Met>120.)";
  // Electron channel
  //  TCut electron = "(abs(LepID)==11)&&(abs(LepEta)<2.1)&&(LepChg<0.)&&(LepIso<0.2)";
  TCut electron = "(abs(LepID)==11)&&(LepIso<0.2)";
  //  TCut electron = "(abs(LepID)==11)&&(LepIso<0.2)&&(Met>300.)";
  // Both channels
  TCut emu = muon||electron;
  // Decide
  TCut singlep = emu;
 
  // MET
  TCut met = "Met>300";
  TCut MetMu = "Met>120.";
  TCut MetEl = "Met>300.";
  TCut LepMet    = (muon&&MetMu)||(electron&&MetEl);

  // MT
  TCut mta = "mt<60";
  TCut mtb = "(60<mt)&&(mt<88)";
  TCut mtc = "88<mt";

  // Jets
  TCut ISRjt = "Jet1Pt > 110.";

  // HT
  TCut HT = "HT20 > 400.";

  // btag
  TCut Bloose = "NbLoose30>0";
  TCut NoBloose = "NbLoose30==0";
  TCut Bhard = "NbTight30>0";
  TCut NoBhard = "NbTight30==0";

  // BDT cut
  TCut BDT = "bdt1>0.5";
  //  TCut BDT = "(bdtW>0.41)&&(bdtT>0.43)";

  // Signal regions
  TCut SR1a = met+NoBloose+HT+mta;
  TCut SR1b = met+NoBloose+HT+mtb;
  TCut SR1c = met+NoBloose+HT+mtc;
  TCut SR2  = met+ISRjt+Bloose+NoBhard;

  //////////////////////////////////////////////////////////////////////////
  
  // Prepare the .tex settings
  ofstream     tablesFile    ("SingleptCutFlow.tex");
  tablesFile.setf(ios::fixed);
  tablesFile.precision(1);
  tablesFile << "\\documentclass[amsmath,amssymb]{revtex4}" << endl;
  tablesFile << "\\oddsidemargin -0.5in" << endl;
  tablesFile << "\\usepackage{longtable}" << endl;
  tablesFile << "\\usepackage{color}" << endl;
  tablesFile << "\\begin{document}" << endl;
  tablesFile << "\\clearpage" << endl << endl << endl;
  tablesFile << "\\begin{table}[!htbp]" << endl;
  tablesFile << "\\begin{center}" << endl;
  tablesFile << "\\begin{tabular}{|l|ccc|c|c|c|}" << endl;
  tablesFile << "\\hline" << endl;
  tablesFile << "\\hline" << endl;
  tablesFile << "    &        &                  &        & Total       &      &   Signal  \\\\" << endl;
  tablesFile << "Cut & W+jets & $t\\overline{t}$ & Rare   & Background  & Data & (300,270) \\\\" << endl;
  tablesFile << "\\hline" << endl;

  // Loop over cuts
  TString CutName[10];
  int nbcutmax  = 3;
  //  bool RempPrecNul[100];
  //  for(int j=0; j<101; ++j) {    RempPrecNul[j] = false;  }
  
  for(int ic=0; ic<nbcutmax; ++ic) {
    
    // Cut specifics
    TCut Coup,Coupjp,Coupjm;
    if (ic==0){
      Coup = emu+ISRjt;
      Coupjp = emu+ISRjt;
      Coupjm = emu+ISRjt;
      CutName[ic] = "1 lepton + ISRjet";
    }
    if (ic==1){
      Coup   = LepMet+ISRjt;
      Coupjp = LepMet+ISRjt;
      Coupjm = LepMet+ISRjt;
      CutName[ic] = "MET(e/$\\mu$) $\\geq$ $300/120$ $GeV$";
    }
    if (ic==2){
      Coup   = LepMet+ISRjt+BDT;
      Coupjp = LepMet+ISRjt+BDT;
      Coupjm = LepMet+ISRjt+BDT;
      CutName[ic] = "$BDT \\geq 0.5$";
    }

    cout << " " <<  endl;
    cout << " " <<  endl;
    cout << "selection " << CutName[ic] << endl;
    //////////////////////////////////////////////////////////////////////////
    // Contribution / Stat. error / JES plus - minus for different channels
    cout.setf(ios::floatfield,ios::fixed);
    cout<<setprecision(2);

    //////////////////////////////////////////////////////////////////////////
    // Which MC contributions to use
    bool use_STP300CHI270 = true;
    bool use_STP600CHI150 = false;
    bool use_Zjets        = false;
    bool use_Zmumu        = false;
    bool use_Zee          = false;
    bool use_Ztautau      = false;
    bool use_W1Jets       = true;
    bool use_W2Jets       = true;
    bool use_W3Jets       = true;
    bool use_W4Jets       = true;
    bool use_Wenu         = false;
    bool use_Wmunu        = false;
    bool use_Wtaunu       = false;
    bool use_Vqq          = false;
    bool use_WW           = false;
    bool use_WZ           = false;
    bool use_ZZ           = false;
    bool use_TTBackg      = false;
    bool use_SingleToptW  = false;
    bool use_SingleToptC  = false;
    bool use_SingleTopsC  = false;
    bool use_TT           = true;
    bool use_Data         = false;

    // Data
    TH1F *mdata = new TH1F("mdata","Data",200,-100,100);
    if(use_Data){
      TH1F *mdem = new TH1F("mdem","Data",200,-100,100);
      TChain l1("bdttree");
      //  pb-1
      l1.Add("");
      l1.Add("");
      l1.Draw("nVert>>mdem",trgD+Coup);
      mdata->Add(mdem);
      mdata->Sumw2();
      mdata->SetMarkerStyle(8);
    }

  // Two reference signal points
  double S300N270=0, eS300N270=0, S300N270p=0, S300N270m=0;
  if ( use_STP300CHI270 ) {
    GetYield(Lumi,"STP300CHI270",trgM+Coup,trgM+Coupjp,trgM+Coupjm, S300N270, eS300N270, S300N270p, S300N270m);
  }   
  double S600N150=0, eS600N150=0, S600N150p=0, S600N150m=0;
  if ( use_STP600CHI150 ) {
    GetYield(Lumi,"STP600CHI150",trgM+Coup,trgM+Coupjp,trgM+Coupjm, S600N150, eS600N150, S600N150p, S600N150m);
  }   

  // 0- TTbar
  double TT=0, eTT=0, TTp=0, TTm=0;
  if ( use_TT ) {
    GetYield(Lumi,"TTbar",trgM+Coup,trgM+Coupjp,trgM+Coupjm, TT, eTT, TTp, TTm);
  }   

  // 1- Z+jets
  double Zj=0, eZj=0, Zjp=0, Zjm=0;
  if ( use_Zjets ) {
    GetYield(Lumi,"Zjets",trgM+Coup,trgM+Coupjp,trgM+Coupjm, Zj, eZj, Zjp, Zjm);
  }
  
  // 2- W+jets
  double W1j=0, eW1j=0, W1jp=0, W1jm=0;
  if ( use_W1Jets ) {
    GetYield(Lumi,"W1Jets",trgM+Coup,trgM+Coupjp,trgM+Coupjm, W1j, eW1j, W1jp, W1jm);
  }
  double W2j=0, eW2j=0, W2jp=0, W2jm=0;
  if ( use_W2Jets ) {
    GetYield(Lumi,"W2Jets",trgM+Coup,trgM+Coupjp,trgM+Coupjm, W2j, eW2j, W2jp, W2jm);
  }
  double W3j=0, eW3j=0, W3jp=0, W3jm=0;
  if ( use_W3Jets ) {
    GetYield(Lumi,"W3Jets",trgM+Coup,trgM+Coupjp,trgM+Coupjm, W3j, eW3j, W3jp, W3jm);
  }
  double W4j=0, eW4j=0, W4jp=0, W4jm=0;
  if ( use_W4Jets ) {
    GetYield(Lumi,"W4Jets",trgM+Coup,trgM+Coupjp,trgM+Coupjm, W4j, eW4j, W4jp, W4jm);
  }
  
  // 3- Vqq
  double vqq=0, evqq=0, vqqp=0, vqqm=0;
  if ( use_Vqq ) {
    GetYield(Lumi,"Vqq",trgM+Coup,trgM+Coupjp,trgM+Coupjm, vqq, evqq, vqqp, vqqm);
  }
  
  // 4- WW
  double WW=0, eWW=0, WWp=0, WWm=0;
  if ( use_WW ) {
    GetYield(Lumi,"WW",trgM+Coup,trgM+Coupjp,trgM+Coupjm, WW, eWW, WWp, WWm);
  }
  
  // 5- WZ
  double WZ=0, eWZ=0, WZp=0, WZm=0;
  if ( use_WZ ) {
    GetYield(Lumi,"WZ",trgM+Coup,trgM+Coupjp,trgM+Coupjm, WZ, eWZ, WZp, WZm);
  }
  
  // 6- TopBackg
  double tBckg=0, etBckg=0, tBckgp=0, tBckgm=0;
  if ( use_TTBackg ) {
//     GetYield(Lumi,"TTbar",trgM+Coup+SngLpTb,trgM+Coupjp+SngLpTb,trgM+Coupjm+SngLpTb, tBckg, etBckg, tBckgp, tBckgm);
  }  
  
  // 7- SingleToptW
  double sTw=0, esTw=0, sTwp=0, sTwm=0;
  if ( use_SingleToptW ) {
    GetYield(Lumi,"SingleToptW",trgM+Coup,trgM+Coupjp,trgM+Coupjm, sTw, esTw, sTwp, sTwm);
  }
  
  // 8- SingleToptChannel
  double sTtC=0, esTtC=0, sTtCp=0, sTtCm=0;
  if ( use_SingleToptC) {
    GetYield(Lumi,"SingleToptChannel",trgM+Coup,trgM+Coupjp,trgM+Coupjm, sTtC, esTtC, sTtCp, sTtCm);
  }
  
  // 9- SingleTopsChannel
  double sTsC=0, esTsC=0, sTsCp=0, sTsCm=0;
  if ( use_SingleTopsC) {
    GetYield(Lumi,"SingleTopsChannel",trgM+Coup,trgM+Coupjp,trgM+Coupjm, sTsC, esTsC, sTsCp, sTsCm);
  }
  
  // 12- Zmumu
  double Zmumu=0, eZmumu=0, Zmumup=0, Zmumum=0;
  if ( use_Zmumu ) {
    GetYield(Lumi,"Zmumu",trgM+Coup,trgM+Coupjp,trgM+Coupjm, Zmumu, eZmumu, Zmumup, Zmumum);
  }
  
  // 13- Zee
  double Zee=0, eZee=0, Zeep=0, Zeem=0;
  if ( use_Zee ) {
    GetYield(Lumi,"Zee",trgM+Coup,trgM+Coupjp,trgM+Coupjm, Zee, eZee, Zeep, Zeem);
  }
  
  // 14- Ztautau
  double Ztautau=0, eZtautau=0, Ztautaup=0, Ztautaum=0;
  if ( use_Ztautau ) {
    GetYield(Lumi,"Ztautau",trgM+Coup,trgM+Coupjp,trgM+Coupjm, Ztautau, eZtautau, Ztautaup, Ztautaum);
  }
  
  // 15- Wmunu
  double Wmunu=0, eWmunu=0, Wmunup=0, Wmunum=0;
  if ( use_Wmunu ) {
    GetYield(Lumi,"Wmunu",trgM+Coup,trgM+Coupjp,trgM+Coupjm, Wmunu, eWmunu, Wmunup, Wmunum);
  }
  
  // 16- Wenu
  double Wenu=0, eWenu=0, Wenup=0, Wenum=0;
  if ( use_Wenu ) {
    GetYield(Lumi,"Wenu",trgM+Coup,trgM+Coupjp,trgM+Coupjm, Wenu, eWenu, Wenup, Wenum);
  }
  
  // 17- Wtaunu
  double Wtaunu=0, eWtaunu=0, Wtaunup=0, Wtaunum=0;
  if ( use_Wtaunu ) {
    GetYield(Lumi,"Wtaunu",trgM+Coup,trgM+Coupjp,trgM+Coupjm, Wtaunu, eWtaunu, Wtaunup, Wtaunum);
  }

  // 18- ZZ
  double ZZ=0, eZZ=0, ZZp=0, ZZm=0;
  if ( use_ZZ ) {
    GetYield(Lumi,"ZZ",trgM+Coup,trgM+Coupjp,trgM+Coupjm, ZZ, eZZ, ZZp, ZZm);
  }

  //19- Total background
  double bckg  = tBckg +Zj +W1j +W2j +W3j +W4j +WW +WZ +vqq +sTw +sTtC +sTsC +Zmumu +Zee +Ztautau +Wenu +Wmunu +Wtaunu +TT;
  double bckgp = tBckgp+Zjp+W1jp+W2jp+W3jp+W4jp+WWp+WZp+vqqp+sTwp+sTtCp+sTsCp+Zmumup+Zeep+Ztautaup+Wenup+Wmunup+Wtaunup+TTp;
  double bckgm = tBckgm+Zjm+W1jm+W2jm+W3jm+W4jm+WWm+WZm+vqqm+sTwm+sTtCm+sTsCm+Zmumum+Zeem+Ztautaum+Wenup+Wmunup+Wtaunup+TTm;
  double ebckg =
    sqrt(pow(etBckg,2)+pow(eZj,2)+pow(eW1j,2)+pow(eW2j,2)+pow(eW3j,2)+pow(eW4j,2)+pow(eWW,2)+pow(eWZ,2)+pow(evqq,2)+pow(esTw,2)
	 +pow(esTtC,2)+pow(esTsC,2)+pow(eZmumu,2)+pow(eZee,2)+pow(eZtautau,2)+pow(eWenu,2)+pow(eWmunu,2)+pow(eWtaunu,2)+pow(eTT,2));

  tablesFile << CutName[ic] << " & "
	     << W1j+W2j+W3j+W4j          << " & "
	     << TT          << " & "
	     << bckg-TT-(W1j+W2j+W3j+W4j)         << " & "
	     << bckg << " $\\pm$ " << ebckg << " $\\pm$ " << AbsV(Max(bckgp-bckg,bckg-bckgm)) << " & "
             << mdata->GetEntries() << " & "
	     << S300N270 << " $\\pm$ " << eS300N270 << " \\\\" << endl;

  }

tablesFile << "\\hline " << endl;
tablesFile << "\\hline" << endl;
tablesFile << "\\end{tabular}" << endl;
tablesFile << "\\caption{xxx}" << endl;
//tablesFile << "\\label{tab:SingleLeptCutFlow}" << endl;
tablesFile << "\\end{center}" << endl;
tablesFile << "\\end{table}" << endl;
tablesFile << endl << endl << "\\end{document}" << endl;
tablesFile.close();


}


void GetYield(
	      double lumi,     // Input: Luminosity
	      string pc,       // Input: Name of the channel
	      TCut Coupure,    // Input: Analysis cut
	      TCut Coupurejp,  // Input: +1sigma(JES) analysis cut
	      TCut Coupurejm,  // Input: -1sigma(JES) analysis cut
	      double& Yield,   // Output: Weighted yield of the channel
	      double& eYield,  // Output: Error on the wighted yield
	      double& YieldP,  // Output: Weighted yield of the channel +1sigma(JES)
	      double& YieldM  // Output: Weighted yield of the channel -1sigma(JES)
	      ){ 

  // Which file to read
  TChain ProcFile("bdttree");

  if (pc=="STP300CHI270"){    ProcFile.Add("SET5/T2DegStop_300_270_bdt.root");  }
  if (pc=="STP600CHI150"){    ProcFile.Add("");  }
  if (pc=="TTbar"){    ProcFile.Add("SET5/TTJets_bdt.root");  }
  if (pc=="Zjets"){    ProcFile.Add("");  }
  if (pc=="Zmumu"){    ProcFile.Add("");  }
  if (pc=="Zee"){    ProcFile.Add("");  }
  if (pc=="Ztautau"){     ProcFile.Add("");  }
  if (pc=="W1Jets"){    ProcFile.Add("SET5/WJets_HT100to200_bdt.root");  }
  if (pc=="W2Jets"){    ProcFile.Add("SET5/WJets_HT200to400_bdt.root");  }
  if (pc=="W3Jets"){    ProcFile.Add("SET5/WJets_HT400to600_bdt.root");  }
  if (pc=="W4Jets"){    ProcFile.Add("SET5/WJets_HT600toInf_bdt.root");  }
  if (pc=="Wmunu"){    ProcFile.Add("");  }
  if (pc=="Wenu"){    ProcFile.Add("");  }
  if (pc=="Wtaunu"){    ProcFile.Add("");  }
  if (pc=="WW"){    ProcFile.Add("");  }
  if (pc=="WZ"){    ProcFile.Add("");  }
  if (pc=="ZZ"){    ProcFile.Add("");  }
  if (pc=="Vqq"){    ProcFile.Add("");  }
  if (pc=="SingleToptW"){    ProcFile.Add("");  }
  if (pc=="SingleToptChannel"){    ProcFile.Add("");  }
  if (pc=="SingleTopsChannel"){    ProcFile.Add("");  }

  // Get the numerator
  TH1D *nbcut = new TH1D("nbcut","nbcut",200,-100,100);
  TH1D *nbcutjp = new TH1D("nbcutjp","nbcutjp",200,-100,100);
  TH1D *nbcutjm = new TH1D("nbcutjm","nbcutjm",200,-100,100);
  ProcFile.Draw("Njet30>>nbcut",Coupure);    // Pick-up a variable which is always filled
  ProcFile.Draw("Njet30>>nbcutjp",Coupurejp);
  ProcFile.Draw("Njet30>>nbcutjm",Coupurejm);
  double NumWeight = nbcut->GetEntries();
  double NumWeightP = nbcutjp->GetEntries();
  double NumWeightM = nbcutjm->GetEntries();
  // Get the weight
  float xsec;
  ProcFile.SetBranchAddress("XS",&xsec);
  ProcFile.GetEntry(0);
  // Get the denominator
  TH1D *nbt = new TH1D("nbt","nbt",10,0.,10000000.);
  ProcFile.Draw("Nevt>>nbt");
  double *Den = ProcFile.GetV1();
  //  cout << pc << " " << Den[0] << endl;

  // Calculate yields (weighted/unweighted)
  Yield  = lumi*xsec*NumWeight/Den[0];
  eYield = lumi*xsec*uneff(NumWeight,Den[0]);
  YieldP = lumi*xsec*NumWeightP/Den[0];
  YieldM = lumi*xsec*NumWeightM/Den[0];

  // Delete histograms
  delete nbt;
  delete nbcut;
  delete nbcutjp;
  delete nbcutjm;

}


double eff(double a, double b){ 
  double af = double(a);
  double bf = double(b);   
  double effi = af/bf;
  return effi;
}
double uneff(double a, double b){
  double af = double(a);
  double bf = double(b);
  double r = af/bf;
  double unc = sqrt(af + (r*r*bf) )/bf;
  return unc;
}

double Max(double a, double b){
  if (a>b) return a;
  else return b;
}

double AbsV(double a){
  return sqrt(pow(a,2));
}
