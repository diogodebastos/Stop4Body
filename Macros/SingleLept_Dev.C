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
//#include <cmath>
//#include <ctgmath>
#include "TStyle.h"
#include "TCut.h"
#include "TChain.h"
#include "TH1F.h"
#include "TROOT.h"
#include "TEventList.h"
#include "SingleLept_Dev.h"

using namespace std;

void SingleLept_Dev (){
  
  gStyle->SetOptStat(000000);

  //////////////////////////////////////////////////////////////////////////
  // Give luminosity (positive value ==> weighting whereas <=0 value ==> unweighting
  // Luminosity [pb]
  const double Lumi = 10000.;

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

  //////////////////////////////////////////////////////////////////////////
  // Define analysis

  // MC Triggers
  TCut trgM   = "Event>-999.";
  // Data Triggers
  TCut trgD   = "Event>-999.";

  // Lepton selection
  // Muon channel
  //  TCut muon = "(nGoodMu==1)&&(nGoodEl==0)";
  TCut muon = "(nGoodMu==1)";
  // Electron channel
  TCut electron = "(nGoodEl==1)";
  // Decide
  //  TCut singlep = muon;
  //  TCut singlep = electron;
  TCut singlep = muon||electron;
  TCut etal = "abs(LepEta[0])<1.5";
  TCut chgl = "LepChg[0]<0.";

  // Jets
  TCut ISRjet = "Jet1Pt > 110.";
  //  TCut ISRjet = "Jet1Pt > 325.";
  TCut dphij1j2 = "DPhiJet1Jet2 < 2.5";

  // MET
  //  TCut met    = "Met>100.";
  TCut met    = "Met>300.";
  //  TCut met    = "Met>500.";

  // Preselection
  //  TCut presel = trgM;
  //  TCut presel = singlep;
  //  TCut presel = singlep+ISRjet;
  //  TCut presel = singlep+ISRjet+dphij1j2;
  TCut presel = singlep+ISRjet+dphij1j2+met;

  presel = singlep+ISRjet+dphij1j2+("Met<300");
  presel = singlep+ISRjet+dphij1j2+("Met<200");
  presel = singlep+ISRjet+dphij1j2+met+("NbLoose30>=2");
  presel = singlep+ISRjet+dphij1j2+met+("NbTight30>=1");
  presel = singlep+ISRjet+dphij1j2+met+("NbTight30>=2");
  presel = singlep+ISRjet+dphij1j2+("Met<300")+("NbLoose30>=2");
  presel = singlep+ISRjet+dphij1j2+("Met<200")+("NbLoose30>=2");
  presel = singlep+ISRjet+dphij1j2+("Met<300")+("NbTight30>=1");
  presel = singlep+ISRjet+dphij1j2+("Met<200")+("NbTight30>=1");
  presel = singlep+ISRjet+dphij1j2+("Met<300")+("NbTight30>=2");
  presel = singlep+ISRjet+dphij1j2+("Met<200")+("NbTight30>=2");
  presel = singlep+ISRjet+dphij1j2+("Met<300")+("NbLoose30==0");
  presel = singlep+ISRjet+dphij1j2+("Met<200")+("NbLoose30==0");

  presel = singlep+ISRjet+dphij1j2+met+("BDT<=0.2")+("NbTight30>0");
  presel = singlep+ISRjet+dphij1j2+met+("BDT<=0.2")+("NbTight30==0");
  presel = singlep+ISRjet+dphij1j2+met+("BDT<=0.2")+("NbTight30>1");
  presel = singlep+ISRjet+dphij1j2+met+("BDT<=0.2")+("NbLoose30==0");

  presel = singlep+ISRjet+dphij1j2+("Met<300")+("NbLoose30>=2");
  presel = singlep+ISRjet+dphij1j2+("Met<300")+("NbTight30>=1");
  presel = singlep+ISRjet+dphij1j2+("Met<300")+("NbTight30>=2");
  presel = singlep+ISRjet+dphij1j2+("Met<300")+("NbLoose30==0");
  presel = singlep+ISRjet+dphij1j2+("Met<300");

  presel = singlep+ISRjet+dphij1j2+met+("BDT>0.53");

  // MT
  TCut mta = "mt<60";
  TCut mtb = "(60<mt)&&(mt<88)";
  TCut mtc = "88<mt";

  // HT
  TCut HT = "HT30 > 400.";

  // btag
  TCut Bloose = "NbLoose30>0";
  TCut NoBloose = "NbLoose30==0";
  TCut Bhard = "NbTight30>0";
  TCut NoBhard = "NbTight30==0";

  // BDT cut
  TCut BDT = "bdt1>0.4";

  // Signal regions a la 8 TeV
  TCut SR1a = etal+chgl+met+NoBloose+HT+mta;
  TCut SR1b = etal+chgl+met+NoBloose+HT+mtb;
  TCut SR1c = etal+chgl+met+NoBloose+HT+mtc;
  TCut SR2  = met+ISRjet+Bloose+NoBhard;

  // 3 parallel JES varied final analysis
  TCut Coup   = presel;
  TCut Coupjp = presel;
  TCut Coupjm = presel;
  //  TCut Coup   = presel+SR1a;
  //  TCut Coupjp = presel+SR1a;
  //  TCut Coupjm = presel+SR1a;
  //  TCut Coup   = presel+BDT;
  //  TCut Coupjp = presel+BDT;
  //  TCut Coupjm = presel+BDT;
  //  TCut Coup   = presel+BDT+Bhard;
  //  TCut Coupjp = presel+BDT+Bhard;
  //  TCut Coupjm = presel+BDT+Bhard;


  //////////////////////////////////////////////////////////////////////////
  // Contribution / Stat. error / JES plus - minus for different channels
  cout.setf(ios::floatfield,ios::fixed);
  cout<<setprecision(2);
  
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

  cout << " " <<  endl;
  if ( use_Zjets ) {
    cout << "Zjets   : " << Zj << " +- " << eZj << " (stat) +" << Zjp-Zj << " -" << Zj-Zjm << " (jes)" << "\n";
  }
  if ( use_Zmumu ) {
    cout << "Zmumu   : " << Zmumu << " +- " << eZmumu << " (stat) +" << Zmumup-Zmumu << " -" << Zmumu-Zmumum << " (jes)" << "\n";
  }
  if ( use_Zee ) {
    cout << "Zee     : " << Zee << " +- " << eZee << " (stat) +" << Zeep-Zee << " -" << Zee-Zeem << " (jes)" << "\n";
  }
  if ( use_Ztautau ) {
    cout << "Ztautau : " << Ztautau << " +- " << eZtautau << " (stat) +" << Ztautaup-Ztautau << " -" << Ztautau-Ztautaum << " (jes)" << "\n";
  }
  if ( use_W1Jets ) {
    cout << "W1Jets  : " << W1j << " +- " << eW1j << " (stat) +" << W1jp-W1j << " -" << W1j-W1jm << " (jes)" << "\n";
  }
  if ( use_W2Jets ) {
    cout << "W2Jets  : " << W2j << " +- " << eW2j << " (stat) +" << W2jp-W2j << " -" << W2j-W2jm << " (jes)" << "\n";
  }
  if ( use_W3Jets ) {
    cout << "W3Jets  : " << W3j << " +- " << eW3j << " (stat) +" << W3jp-W3j << " -" << W3j-W3jm << " (jes)" << "\n";
  }
  if ( use_W4Jets ) {
    cout << "W4Jets  : " << W4j << " +- " << eW4j << " (stat) +" << W4jp-W4j << " -" << W4j-W4jm << " (jes)" << "\n";
  }
  if ( use_Wenu ) {
    cout << "Wenu    : " << Wenu << " +- " << eWenu << " (stat) +" << Wenup-Wenu << " -" << Wenu-Wenum << " (jes)" << "\n";
  }
  if ( use_Wmunu ) {
    cout << "Wmunu   : " << Wmunu << " +- " << eWmunu << " (stat) +" << Wmunup-Wmunu << " -" << Wmunu-Wmunum << " (jes)" << "\n";
  }
  if ( use_Wtaunu ) {
    cout << "Wtaunu  : " << Wtaunu << " +- " << eWtaunu << " (stat) +" << Wtaunup-Wtaunu << " -" << Wtaunu-Wtaunum << " (jes)" << "\n";
  }
  if ( use_Vqq ) {
    cout << "W/Zbb   : " << vqq << " +- " << evqq << " (stat) +" << vqqp-vqq << " -" << vqq-vqqm << " (jes)" << "\n";
  }
  if ( use_WW ) {
    cout << "WW      : " << WW << " +- " << eWW << " (stat) +" << WWp-WW << " -" << WW-WWm << " (jes)" << "\n";
  }
  if ( use_WZ ) {
    cout << "WZ      : " << WZ << " +- " << eWZ << " (stat) +" << WZp-WZ << " -" << WZ-WZm << " (jes)" << "\n";
  }
  if ( use_ZZ ) {
    cout << "ZZ      : " << ZZ << " +- " << eZZ << " (stat) +" << ZZp-ZZ << " -" << ZZ-ZZm << " (jes)" << "\n";
  }
  if ( use_TT ) {
    cout << "TTbar   : " << TT << " +- " << eTT << " (stat) +" << TTp-TT << " -" << TT-TTm << " (jes)" << "\n";
  }
  if ( use_TTBackg ) {
    cout << "TopBckg : " << tBckg << " +- " << etBckg << " (stat) +" << tBckgp-tBckg << " -" << tBckg-tBckgm << " (jes)" << "\n";
  }
  if ( use_SingleToptW ) {
    cout << "SToptW  : " << sTw << " +- " << esTw << " (stat) +" << sTwp-sTw << " -" << sTw-sTwm << " (jes)" << "\n";
  }
  if ( use_SingleToptC) {
    cout << "SToptC  : " << sTtC << " +- " << esTtC << " (stat) +" << sTtCp-sTtC << " -" << sTtC-sTtCm << " (jes)" << "\n";
  }
  if ( use_SingleTopsC) {
    cout << "STopsC  : " << sTsC << " +- " << esTsC << " (stat) +" << sTsCp-sTsC << " -" << sTsC-sTsCm << " (jes)" << "\n";
  }

  cout << "---------------------------------------------------------" << "\n"; 
  cout << "BACKGROUND : " << bckg << " +- " << ebckg << " (stat) +" << bckgp-bckg << " -" << bckg-bckgm << " (jes)"  << "\n";
  cout << "---------------------------------------------------------" << "\n"; 
  cout << "S300N270         : " << S300N270 << " +- " << eS300N270 << " (stat) +" << S300N270p-S300N270 << " -" << S300N270-S300N270m << " (jes)" << "\n";
  cout << "*********************************************************" << "\n"; 
  if (use_Data) {
    cout << "Data : " << mdata->GetEntries() << "\n";
    cout << "*********************************************************" << "\n";
  }
  
// cout << "\n"; 

// Choose which signal should be looked upon
  double Sgn = S300N270;
// FOM
  double f = 0.2;
  // A la 8 TeV
  //  double FOM = Sgn/sqrt(bckg+pow(f*bckg,2));
  //  double dFdS = 1./(bckg + pow(f*bckg,2));
  //  double dFdB = pow(Sgn,2)*pow(1.+(2.*f*f*bckg),2)/(4.*pow(bckg+pow(f*bckg,2),3));
  //  double sS = Sgn;
  //  double sB = pow(ebckg,2);
  //  double dFOM = sqrt((dFdS*sS) + (dFdB*sB));
 // Exclusion-oriented
  //  double FOM = Sgn/sqrt(Sgn+bckg+(pow(f*bckg,2)));
  //  double dFdS = pow((Sgn/2.)+bckg+pow(f*bckg,2),2)/pow(Sgn+bckg+pow(f*bckg,2),3);
  //  double dFdB = pow(Sgn,2)*pow(1.+(2.*f*f*bckg),2)/pow(Sgn+bckg+pow(f*bckg,2),3);
  //  double sS = Sgn;
  //  double sB = pow(ebckg,2);
  //  double dFOM = sqrt((dFdS*sS) + (dFdB*sB));
 // Discovery-oriented
  //  double FOM = Sgn/sqrt(Sgn+(pow(f*bckg,2)));
  //  double dFdS1 = sqrt(Sgn+(pow(f*bckg,2)));
  //  dFdS1 = dFdS1 - (Sgn/(2.*pow(Sgn+(pow(f*bckg,2)),1.5)));
  //  dFdS1 = dFdS1 / (Sgn+(pow(f*bckg,2)));
  //  double dFdS = pow(dFdS1,2);
  //  double dFdB = pow(f,4)*pow(Sgn*bckg,2)/pow(Sgn+(pow(f*bckg,2)),3);
  //  double sS = Sgn;
  //  double sB = pow(ebckg,2);
  //  double dFOM = sqrt((dFdS*sS) + (dFdB*sB));
  // Stat. comm. recommendation: https://indico.cern.ch/event/301310/session/5/contribution/49/material/slides/0.pptx
  //  double FOM = 2.* (sqrt(Sgn + bckg) - sqrt(bckg));
  //  double dF1 = (Sgn/(Sgn+bckg));
  //  double dF2 = ((1./sqrt(Sgn+bckg)) - (1./sqrt(bckg)));
  //  dF2 = pow(dF2,2) * bckg;
  //  double dFOM = sqrt(dF1+dF2);
  // http://www.pp.rhul.ac.uk/~cowan/stat/notes/medsigNote.pdf
  double SysB = f * bckg;
  double FOMn1 = (Sgn+bckg)*(bckg+pow(SysB,2));
  double FOMd1 = pow(bckg,2) + (Sgn+bckg)*pow(SysB,2);
  double FOM1 = (Sgn+bckg)*log(FOMn1/FOMd1);
  double FOMn2 = pow(SysB,2)*Sgn;
  double FOMd2 = bckg*(bckg+pow(SysB,2));
  double FOM2 = pow(bckg/SysB,2)*log(1.+(FOMn2/FOMd2));
  double FOM = sqrt(2.*(FOM1 - FOM2));

  double sS2 = Sgn;
  double sB2 = pow(ebckg,2);
  double f2 = pow(f,2);
  double lnu = 1. + (f2*(Sgn+bckg));
  double lde = (1. + (bckg*f2))*(Sgn+bckg);
  double lognum = log(lnu/lde);
  double numbr = bckg*(1.+(bckg*f2))*(sS2+sB2)*lognum;
  double numb =  (2.*Sgn*sB2) + numbr;
  double nnum = (pow(Sgn,2)*sB2) + ( bckg*(1.+(bckg*f2))*lognum*numb );

  double lnd = (1.+(bckg*f2))*(bckg+Sgn);
  double ldd = bckg*(1. + (f2*(bckg+Sgn)));
  double nden1 = f2*(Sgn+bckg)*log(lnd/ldd);
  double nden2 = log( 1. + (Sgn*f2/(1.+(bckg*f2))) );
  double nden = nden1 - nden2;

  double num = f * sqrt(nnum/nden);
  double den = sqrt(2.)*bckg*(1.+(bckg*f2));
  double dFOM = num/den;

  cout << "FOM = " << FOM << " +- " << dFOM << "\n";
cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << "\n"; 


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

  //  if (pc=="STP300CHI270"){    ProcFile.Add("FrigoRoottuple/Summer2015/Jet20/T2DegStop_300_270_bdt.root");  }
  if (pc=="STP300CHI270"){    ProcFile.Add("SET7013/T2DegStop_300_270_bdt.root");  }
  if (pc=="STP600CHI150"){    ProcFile.Add("");  }
  //  if (pc=="TTbar"){    ProcFile.Add("FrigoRoottuple/Summer2015/Jet20/TTJets_LO_bdt.root");  }
  if (pc=="TTbar"){    ProcFile.Add("SET7013/TTJets_LO_bdt.root");  }
  if (pc=="Zjets"){    ProcFile.Add("");  }
  if (pc=="Zmumu"){    ProcFile.Add("");  }
  if (pc=="Zee"){    ProcFile.Add("");  }
  if (pc=="Ztautau"){     ProcFile.Add("");  }
  //  if (pc=="W1Jets"){    ProcFile.Add("FrigoRoottuple/Summer2015/Jet20/Wjets100to200_bdt.root");  }
  //  if (pc=="W2Jets"){    ProcFile.Add("FrigoRoottuple/Summer2015/Jet20/Wjets200to400_bdt.root");  }
  //  if (pc=="W3Jets"){    ProcFile.Add("FrigoRoottuple/Summer2015/Jet20/Wjets400to600_bdt.root");  }
  //  if (pc=="W4Jets"){    ProcFile.Add("FrigoRoottuple/Summer2015/Jet20/Wjets600toInf_bdt.root");  }
  if (pc=="W1Jets"){    ProcFile.Add("SET7013/Wjets_100to200_bdt.root");  }
  if (pc=="W2Jets"){    ProcFile.Add("SET7013/Wjets_200to400_bdt.root");  }
  if (pc=="W3Jets"){    ProcFile.Add("SET7013/Wjets_400to600_bdt.root");  }
  if (pc=="W4Jets"){    ProcFile.Add("SET7013/Wjets_600toInf_bdt.root");  }
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
  ProcFile.Draw("Njet>>nbcut",Coupure);    // Pick-up a variable which is always filled
  ProcFile.Draw("Njet>>nbcutjp",Coupurejp);
  ProcFile.Draw("Njet>>nbcutjm",Coupurejm);
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
  //  cout << pc << " " << NumWeight << endl;
  //  cout << "XS = " << xsec << "\n";
  //  cout << "Total number of evets of " << pc << " " << Den[0] << endl;

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
