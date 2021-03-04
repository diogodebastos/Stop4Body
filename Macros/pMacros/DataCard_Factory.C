// The user has to:
// * provide the luminosity Lumi
// * chose which MC contributions to use
// * modify the section "Define analysis" to modify analysis cuts

#include <iomanip>
#include <fstream>
#include "TMath.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include "string"
#include "TStyle.h"
#include "TCut.h"
#include "TChain.h"
#include "TH1F.h"
#include "TROOT.h"
#include "TEventList.h"
#include "DataCard_Factory.h"

using namespace std;

void DataCard_Factory (){
  
  gStyle->SetOptStat(000000);

  //////////////////////////////////////////////////////////////////////////
  // Give luminosity (positive value ==> weighting whereas <=0 value ==> unweighting
  // Luminosity [pb]
  const double Lumi = 35866.0;

  //////////////////////////////////////////////////////////////////////////
  // Which MC contributions to use
  bool use_SIGNAL = true;
  bool use_TT           = false;
  bool use_W1Jets       = false;
  bool use_W2Jets       = false;
  bool use_W3Jets       = false;
  bool use_W4Jets       = false;
  bool use_W5Jets       = false;
  bool use_W6Jets       = false;
  bool use_W7Jets       = false;
  bool use_Zinv1        = false;
  bool use_Zinv2        = false;
  bool use_Zinv3        = false;
  bool use_Zinv4        = false;
  bool use_Zinv5        = false;
  bool use_Zinv6        = false;
  bool use_Zinv7        = false;
  bool use_DY1          = true;
  bool use_DY2          = true;
  bool use_DY3          = true;
  bool use_DY4          = true;
  bool use_DY5          = true;
  bool use_DY6          = true;
  bool use_DY7          = true;
  bool use_DY8          = true;
  bool use_DY9          = true;
  bool use_DY10         = true;
  bool use_DY11         = true;
  bool use_WW           = true;
  bool use_WZ           = true;
  bool use_ZZ           = true;
  bool use_SingleToptW  = true;
  bool use_SingleToptC  = true;
  bool use_SingleTopBtW = true;
  bool use_SingleTopBtC = true;
  bool use_TTgj         = true;
  bool use_TTwl         = true;
  bool use_TTwqq        = true;
  bool use_TTzl         = true;
  bool use_TTzl1m10     = true;
  bool use_TTzqq        = true;
  bool use_Data         = false;

  //////////////////////////////////////////////////////////////////////////
  cout.setf(ios::floatfield,ios::fixed);
  //  cout<<setprecision(3);

  // Get Delta_M to cover and corresponding signal points
  string idm = " ";
  cout << "Which Delta_M to cover ?" << std::endl ;
  cin >> idm ;
  string dm = "DM" + idm;
  ifstream dmFile;
  dmFile.open(dm + ".txt",ifstream::in);
  string dmline;
  std::vector<std::string> dmlines;
  while (dmFile.good()) {
    dmFile >> dmline;
    dmlines.push_back(dmline);
  }
  dmFile.close();

  // Read the DD backgrounds
  string dd = "DD" + idm;
  ifstream ddFile;
  ddFile.open(dd + ".txt",ifstream::in);
  string ddline;
  std::vector<std::string> ddlines;
  while (ddFile.good()) {
    ddFile >> ddline;
    ddlines.push_back(ddline);
  }
  ddFile.close();
  double DDwj = std::stod(ddlines[0]);
  double eDDwj = std::stod(ddlines[1]);
  double DDtt = std::stod(ddlines[2]);
  double eDDtt = std::stod(ddlines[3]);
  double DDfk = std::stod(ddlines[4]);
  double eDDfk = std::stod(ddlines[5]);

  double EISRwj1u = std::stod(ddlines[6]);
  double EISRwj1d = std::stod(ddlines[7]);
  double EISRwj2u = std::stod(ddlines[8]);
  double EISRwj2d = std::stod(ddlines[9]);
  double EISRwj3u = std::stod(ddlines[10]);
  double EISRwj3d = std::stod(ddlines[11]);
  double EISRwj4u = std::stod(ddlines[12]);
  double EISRwj4d = std::stod(ddlines[13]);
  double EISRwj5u = std::stod(ddlines[14]);
  double EISRwj5d = std::stod(ddlines[15]);
  double EISRwj6u = std::stod(ddlines[16]);
  double EISRwj6d = std::stod(ddlines[17]);
  double EISRwj7u = std::stod(ddlines[18]);
  double EISRwj7d = std::stod(ddlines[19]);
  double EISRtt1u = std::stod(ddlines[20]);
  double EISRtt1d = std::stod(ddlines[21]);
  double EISRtt2u = std::stod(ddlines[22]);
  double EISRtt2d = std::stod(ddlines[23]);
  double EISRtt3u = std::stod(ddlines[24]);
  double EISRtt3d = std::stod(ddlines[25]);
  double EISRtt4u = std::stod(ddlines[26]);
  double EISRtt4d = std::stod(ddlines[27]);
  double EISRtt5u = std::stod(ddlines[28]);
  double EISRtt5d = std::stod(ddlines[29]);
  double EISRtt6u = std::stod(ddlines[30]);
  double EISRtt6d = std::stod(ddlines[31]);
  double EISRtt7u = std::stod(ddlines[32]);
  double EISRtt7d = std::stod(ddlines[33]);
  double EISRfk1u = std::stod(ddlines[34]);
  double EISRfk1d = std::stod(ddlines[35]);
  double EISRfk2u = std::stod(ddlines[36]);
  double EISRfk2d = std::stod(ddlines[37]);
  double EISRfk3u = std::stod(ddlines[38]);
  double EISRfk3d = std::stod(ddlines[39]);
  double EISRfk4u = std::stod(ddlines[40]);
  double EISRfk4d = std::stod(ddlines[41]);
  double EISRfk5u = std::stod(ddlines[42]);
  double EISRfk5d = std::stod(ddlines[43]);
  double EISRfk6u = std::stod(ddlines[44]);
  double EISRfk6d = std::stod(ddlines[45]);
  double EISRfk7u = std::stod(ddlines[46]);
  double EISRfk7d = std::stod(ddlines[47]);

  double ISRwj1u = std::stod(ddlines[48]);
  double ISRwj1d = std::stod(ddlines[49]);
  double ISRwj2u = std::stod(ddlines[50]);
  double ISRwj2d = std::stod(ddlines[51]);
  double ISRwj3u = std::stod(ddlines[52]);
  double ISRwj3d = std::stod(ddlines[53]);
  double ISRwj4u = std::stod(ddlines[54]);
  double ISRwj4d = std::stod(ddlines[55]);
  double ISRwj5u = std::stod(ddlines[56]);
  double ISRwj5d = std::stod(ddlines[57]);
  double ISRwj6u = std::stod(ddlines[58]);
  double ISRwj6d = std::stod(ddlines[59]);
  double ISRtt1u = std::stod(ddlines[60]);
  double ISRtt1d = std::stod(ddlines[61]);
  double ISRtt2u = std::stod(ddlines[62]);
  double ISRtt2d = std::stod(ddlines[63]);
  double ISRtt3u = std::stod(ddlines[64]);
  double ISRtt3d = std::stod(ddlines[65]);
  double ISRtt4u = std::stod(ddlines[66]);
  double ISRtt4d = std::stod(ddlines[67]);
  double ISRtt5u = std::stod(ddlines[68]);
  double ISRtt5d = std::stod(ddlines[69]);
  double ISRtt6u = std::stod(ddlines[70]);
  double ISRtt6d = std::stod(ddlines[71]);
  double ISRfk1u = std::stod(ddlines[72]);
  double ISRfk1d = std::stod(ddlines[73]);
  double ISRfk2u = std::stod(ddlines[74]);
  double ISRfk2d = std::stod(ddlines[75]);
  double ISRfk3u = std::stod(ddlines[76]);
  double ISRfk3d = std::stod(ddlines[77]);
  double ISRfk4u = std::stod(ddlines[78]);
  double ISRfk4d = std::stod(ddlines[79]);
  double ISRfk5u = std::stod(ddlines[80]);
  double ISRfk5d = std::stod(ddlines[81]);
  double ISRfk6u = std::stod(ddlines[82]);
  double ISRfk6d = std::stod(ddlines[83]);

  double VVtoFK  = std::stod(ddlines[84]);
  double STtoFK  = std::stod(ddlines[85]);
  double DYtoFK  = std::stod(ddlines[86]);
  double TTXtoFK = std::stod(ddlines[87]);
  double TTtoFK  = std::stod(ddlines[88]);
  double WJtoFK  = std::stod(ddlines[89]);
  double VVtoWJ  = std::stod(ddlines[90]);
  double STtoWJ  = std::stod(ddlines[91]);
  double DYtoWJ  = std::stod(ddlines[92]);
  double TTXtoWJ = std::stod(ddlines[93]);
  double TTtoWJ  = std::stod(ddlines[94]);
  double WJtoWJ  = std::stod(ddlines[95]);
  double VVtoTT  = std::stod(ddlines[96]);
  double STtoTT  = std::stod(ddlines[97]);
  double DYtoTT  = std::stod(ddlines[98]);
  double TTXtoTT = std::stod(ddlines[99]);
  double TTtoTT  = std::stod(ddlines[100]);
  double WJtoTT  = std::stod(ddlines[101]);

  double DATA  = std::stod(ddlines[102]);


  //////////////////////////////////////////////////////////////////////////
  // Define analysis

  TCut trgD   = "Event>-999.";

  TCut NoSpike = "(badMuonMoriond2017 == 1) && (badCloneMuonMoriond2017 == 1)";

  TCut Promp = "isPrompt==1";
  //  TCut singlep = "(isTight==1)&&((nGoodEl + nGoodMu) <= 2)";
  TCut singlep = "(isTight==1)";
  TCut lept = "LepPt<10000.";
  TCut bdt = "BDT>-999.";
  TCut bdtp = "BDT>-999.";
  TCut bdtm = "BDT>-999.";
  if (idm=="10"){
    lept = "LepPt<30.";
    bdt = "BDT>0.31";
    bdtp = "BDT_JES_Up>0.31";
    bdtm = "BDT_JES_Down>0.31";
  }
  if (idm=="20"){
    lept = "LepPt<30.";
    bdt = "BDT>0.39";
    bdtp = "BDT_JES_Up>0.39";
    bdtm = "BDT_JES_Down>0.39";
  }
  if (idm=="30"){
    lept = "LepPt<30.";
    bdt = "BDT>0.47";
    bdtp = "BDT_JES_Up>0.47";
    bdtm = "BDT_JES_Down>0.47";
  }
  if (idm=="40"){
    lept = "LepPt<30.";
    bdt = "BDT>0.48";
    bdtp = "BDT_JES_Up>0.48";
    bdtm = "BDT_JES_Down>0.48";
  }
  if (idm=="50"){
    lept = "LepPt<30.";
    bdt = "BDT>0.45";
    bdtp = "BDT_JES_Up>0.45";
    bdtm = "BDT_JES_Down>0.45";
  }
  if (idm=="60"){
    lept = "LepPt<30.";
    bdt = "BDT>0.50";
    bdtp = "BDT_JES_Up>0.50";
    bdtm = "BDT_JES_Down>0.50";
  }
  if (idm=="70"){ 
    bdt = "BDT>0.46"; 
    bdtp = "BDT_JES_Up>0.46"; 
    bdtm = "BDT_JES_Down>0.46"; 
  }
  if (idm=="80"){ 
    bdt = "BDT>0.44"; 
    bdtp = "BDT_JES_Up>0.44"; 
    bdtm = "BDT_JES_Down>0.44"; 
  }

  // 3 JES-varied serie of cuts on jet-related variables
  // Central JESR
  TCut ISRjet = "Jet1Pt>110.";
  TCut dphij1j2 = "(DPhiJet1Jet2 < 2.5)||(Jet2Pt<60.)";
  TCut met    = "Met>280.";
  TCut HT = "HT > 200.";
  // +1 JES
  TCut ISRjetjp = "Jet1Pt_JES_Up > 110.";
  TCut metjp    = "Met_JES_Up > 280.";
  TCut HTjp = "HT_JES_Up > 200.";
  // -1 JES
  TCut ISRjetjm = "Jet1Pt_JES_Down > 110.";
  TCut metjm    = "Met_JES_Down > 280.";
  TCut HTjm = "HT_JES_Down > 200.";
  // +1 JESR
  TCut ISRjetjrp = "Jet1Pt_JER_Up > 110.";
  TCut metjrp    = "Met_JER_Up > 280.";
  TCut HTjrp = "HT_JER_Up > 200.";
  // -1 JESR
  TCut ISRjetjrm = "Jet1Pt_JER_Down > 110.";
  TCut metjrm    = "Met_JER_Down > 280.";
  TCut HTjrm = "HT_JER_Down > 200.";

  // 3 JESR-varied preselection
  TCut presel = singlep+lept+ISRjet+dphij1j2+met+HT;
  TCut preseljp = singlep+lept+ISRjetjp+dphij1j2+metjp+HTjp;
  TCut preseljm = singlep+lept+ISRjetjm+dphij1j2+metjm+HTjm;
  TCut preseljrp = singlep+lept+ISRjetjrp+dphij1j2+metjrp+HTjrp;
  TCut preseljrm = singlep+lept+ISRjetjrm+dphij1j2+metjrm+HTjrm;

  // JESR-varied final selection
  TCut Coup   = NoSpike+presel+bdt;
  TCut Coupjp = NoSpike+preseljp+bdt;
  TCut Coupjm = NoSpike+preseljm+bdt;
  TCut Coupjrp = NoSpike+preseljrp+bdt;
  TCut Coupjrm = NoSpike+preseljrm+bdt;
  // For background: Take only the prompt component
  TCut CoupB   = Promp+Coup;
  TCut CoupBjp = Promp+Coupjp;
  TCut CoupBjm = Promp+Coupjm;
  TCut CoupBjrp = Promp+Coupjrp;
  TCut CoupBjrm = Promp+Coupjrm;

  //////////////////////////////////////////////////////////////////////////

  // TTbar
  double TT=0, eTT=0, TTp=0, TTm=0, TTrp=0, TTrm=0, TTpup=0, TTpum=0;
  if ( use_TT ) { GetYield(Lumi,idm,"TTbar",CoupB,CoupBjp,CoupBjm,CoupBjrp,CoupBjrm, TT, eTT, TTp, TTm, TTrp, TTrm, TTpup, TTpum); }   
  // W+jets
  double W1j=0, eW1j=0, W1jp=0, W1jm=0, W1jrp=0, W1jrm=0, W1jpup=0, W1jpum=0;
  if ( use_W1Jets ) { GetYield(Lumi,idm,"W1Jets",CoupB,CoupBjp,CoupBjm,CoupBjrp,CoupBjrm, W1j, eW1j, W1jp, W1jm, W1jrp, W1jrm, W1jpup, W1jpum); }
  double W2j=0, eW2j=0, W2jp=0, W2jm=0, W2jrp=0, W2jrm=0, W2jpup=0, W2jpum=0;
  if ( use_W2Jets ) { GetYield(Lumi,idm,"W2Jets",CoupB,CoupBjp,CoupBjm,CoupBjrp,CoupBjrm, W2j, eW2j, W2jp, W2jm, W2jrp, W2jrm, W2jpup, W2jpum); }
  double W3j=0, eW3j=0, W3jp=0, W3jm=0, W3jrp=0, W3jrm=0, W3jpup=0, W3jpum=0;
  if ( use_W3Jets ) { GetYield(Lumi,idm,"W3Jets",CoupB,CoupBjp,CoupBjm,CoupBjrp,CoupBjrm, W3j, eW3j, W3jp, W3jm, W3jrp, W3jrm, W3jpup, W3jpum); }
  double W4j=0, eW4j=0, W4jp=0, W4jm=0, W4jrp=0, W4jrm=0, W4jpup=0, W4jpum=0;
  if ( use_W4Jets ) { GetYield(Lumi,idm,"W4Jets",CoupB,CoupBjp,CoupBjm,CoupBjrp,CoupBjrm, W4j, eW4j, W4jp, W4jm, W4jrp, W4jrm, W4jpup, W4jpum); }
  double W5j=0, eW5j=0, W5jp=0, W5jm=0, W5jrp=0, W5jrm=0, W5jpup=0, W5jpum=0;
  if ( use_W5Jets ) { GetYield(Lumi,idm,"W5Jets",CoupB,CoupBjp,CoupBjm,CoupBjrp,CoupBjrm, W5j, eW5j, W5jp, W5jm, W5jrp, W5jrm, W5jpup, W5jpum); }
  double W6j=0, eW6j=0, W6jp=0, W6jm=0, W6jrp=0, W6jrm=0, W6jpup=0, W6jpum=0;
  if ( use_W6Jets ) { GetYield(Lumi,idm,"W6Jets",CoupB,CoupBjp,CoupBjm,CoupBjrp,CoupBjrm, W6j, eW6j, W6jp, W6jm, W6jrp, W6jrm, W6jpup, W6jpum); }
  double W7j=0, eW7j=0, W7jp=0, W7jm=0, W7jrp=0, W7jrm=0, W7jpup=0, W7jpum=0;
  if ( use_W7Jets ) { GetYield(Lumi,idm,"W7Jets",CoupB,CoupBjp,CoupBjm,CoupBjrp,CoupBjrm, W7j, eW7j, W7jp, W7jm, W7jrp, W7jrm, W7jpup, W7jpum); }
  // Zinv
  double Zinv1=0, eZinv1=0, Zinv1p=0, Zinv1m=0, Zinv1rp=0, Zinv1rm=0, Zinv1pup=0, Zinv1pum=0;
  if ( use_Zinv1 ) { GetYield(Lumi,idm,"Zinv1",CoupB,CoupBjp,CoupBjm,CoupBjrp,CoupBjrm, Zinv1, eZinv1, Zinv1p, Zinv1m, Zinv1rp, Zinv1rm, Zinv1pup, Zinv1pum); }  
  double Zinv2=0, eZinv2=0, Zinv2p=0, Zinv2m=0, Zinv2rp=0, Zinv2rm=0, Zinv2pup=0, Zinv2pum=0;
  if ( use_Zinv2 ) { GetYield(Lumi,idm,"Zinv2",CoupB,CoupBjp,CoupBjm,CoupBjrp,CoupBjrm, Zinv2, eZinv2, Zinv2p, Zinv2m, Zinv2rp, Zinv2rm, Zinv2pup, Zinv2pum); }  
  double Zinv3=0, eZinv3=0, Zinv3p=0, Zinv3m=0, Zinv3rp=0, Zinv3rm=0, Zinv3pup=0, Zinv3pum=0;
  if ( use_Zinv3 ) { GetYield(Lumi,idm,"Zinv3",CoupB,CoupBjp,CoupBjm,CoupBjrp,CoupBjrm, Zinv3, eZinv3, Zinv3p, Zinv3m, Zinv2rp, Zinv3rm, Zinv3pup, Zinv3pum); }
  double Zinv4=0, eZinv4=0, Zinv4p=0, Zinv4m=0, Zinv4rp=0, Zinv4rm=0, Zinv4pup=0, Zinv4pum=0;
  if ( use_Zinv4 ) { GetYield(Lumi,idm,"Zinv4",CoupB,CoupBjp,CoupBjm,CoupBjrp,CoupBjrm, Zinv4, eZinv4, Zinv4p, Zinv4m, Zinv2rp, Zinv4rm, Zinv4pup, Zinv4pum); }
  double Zinv5=0, eZinv5=0, Zinv5p=0, Zinv5m=0, Zinv5rp=0, Zinv5rm=0, Zinv5pup=0, Zinv5pum=0;
  if ( use_Zinv5 ) { GetYield(Lumi,idm,"Zinv5",CoupB,CoupBjp,CoupBjm,CoupBjrp,CoupBjrm, Zinv5, eZinv5, Zinv5p, Zinv5m, Zinv2rp, Zinv5rm, Zinv5pup, Zinv5pum); }
  double Zinv6=0, eZinv6=0, Zinv6p=0, Zinv6m=0, Zinv6rp=0, Zinv6rm=0, Zinv6pup=0, Zinv6pum=0;
  if ( use_Zinv6 ) { GetYield(Lumi,idm,"Zinv6",CoupB,CoupBjp,CoupBjm,CoupBjrp,CoupBjrm, Zinv6, eZinv6, Zinv6p, Zinv6m, Zinv2rp, Zinv6rm, Zinv6pup, Zinv6pum); }
  double Zinv7=0, eZinv7=0, Zinv7p=0, Zinv7m=0, Zinv7rp=0, Zinv7rm=0, Zinv7pup=0, Zinv7pum=0;
  if ( use_Zinv7 ) { GetYield(Lumi,idm,"Zinv7",CoupB,CoupBjp,CoupBjm,CoupBjrp,CoupBjrm, Zinv7, eZinv7, Zinv7p, Zinv7m, Zinv2rp, Zinv7rm, Zinv7pup, Zinv7pum); }
  // WW
  double WW=0, eWW=0, WWp=0, WWm=0, WWrp=0, WWrm=0, WWpup=0, WWpum=0;
  double q1WW=0, q2WW=0, q3WW=0, q4WW=0, q6WW=0, q8WW=0;
  double sB1WWp=0, sB1WWm=0, sB2WWp=0, sB2WWm=0, sB3WWp=0, sB3WWm=0, sB4WWp=0, sB4WWm=0, 
    sB5WWp=0, sB5WWm=0, sB6WWp=0, sB6WWm=0, sB7WWp=0, sB7WWm=0, sB8WWp=0, sB8WWm=0;
  if ( use_WW ) {     
    GetYield(Lumi,idm,"WW",CoupB,CoupBjp,CoupBjm,CoupBjrp,CoupBjrm, WW, eWW, WWp, WWm, WWrp, WWrm, WWpup, WWpum);   
    GetSysQ2(Lumi,idm,"WW",CoupB, q1WW, q2WW, q3WW, q4WW, q6WW, q8WW);
    GetSysBTAG(Lumi,idm,"WW",CoupB, sB1WWp, sB1WWm, sB2WWp, sB2WWm, sB3WWp, sB3WWm, sB4WWp, sB4WWm, 
	       sB5WWp, sB5WWm, sB6WWp, sB6WWm, sB7WWp, sB7WWm, sB8WWp, sB8WWm);
  }
  std::vector<double> WWQ2 = {Abs(q1WW-WW), Abs(q2WW-WW), Abs(q3WW-WW), Abs(q4WW-WW), Abs(q6WW-WW), Abs(q8WW-WW)};
  double SysWWQ2 = *std::max_element(WWQ2.begin(), WWQ2.end());
  // WZ
  double WZ=0, eWZ=0, WZp=0, WZm=0, WZrp=0, WZrm=0, WZpup=0, WZpum=0;
  double q1WZ=0, q2WZ=0, q3WZ=0, q4WZ=0, q6WZ=0, q8WZ=0;
  double sB1WZp=0, sB1WZm=0, sB2WZp=0, sB2WZm=0, sB3WZp=0, sB3WZm=0, sB4WZp=0, sB4WZm=0, 
    sB5WZp=0, sB5WZm=0, sB6WZp=0, sB6WZm=0, sB7WZp=0, sB7WZm=0, sB8WZp=0, sB8WZm=0;
  if ( use_WZ ) {     
    GetYield(Lumi,idm,"WZ",CoupB,CoupBjp,CoupBjm,CoupBjrp,CoupBjrm, WZ, eWZ, WZp, WZm, WZrp, WZrm, WZpup, WZpum);   
    GetSysQ2(Lumi,idm,"WZ",CoupB, q1WZ, q2WZ, q3WZ, q4WZ, q6WZ, q8WZ);
    GetSysBTAG(Lumi,idm,"WZ",CoupB, sB1WZp, sB1WZm, sB2WZp, sB2WZm, sB3WZp, sB3WZm, sB4WZp, sB4WZm, 
	       sB5WZp, sB5WZm, sB6WZp, sB6WZm, sB7WZp, sB7WZm, sB8WZp, sB8WZm);
  }
  std::vector<double> WZQ2 = {Abs(q1WZ-WZ), Abs(q2WZ-WZ), Abs(q3WZ-WZ), Abs(q4WZ-WZ), Abs(q6WZ-WZ), Abs(q8WZ-WZ)};
  double SysWZQ2 = *std::max_element(WZQ2.begin(), WZQ2.end());
  // ZZ
  double ZZ=0, eZZ=0, ZZp=0, ZZm=0, ZZrp=0, ZZrm=0, ZZpup=0, ZZpum=0;
  double q1ZZ=0, q2ZZ=0, q3ZZ=0, q4ZZ=0, q6ZZ=0, q8ZZ=0;
  double sB1ZZp=0, sB1ZZm=0, sB2ZZp=0, sB2ZZm=0, sB3ZZp=0, sB3ZZm=0, sB4ZZp=0, sB4ZZm=0, 
    sB5ZZp=0, sB5ZZm=0, sB6ZZp=0, sB6ZZm=0, sB7ZZp=0, sB7ZZm=0, sB8ZZp=0, sB8ZZm=0;
  if ( use_ZZ ) {     
    GetYield(Lumi,idm,"ZZ",CoupB,CoupBjp,CoupBjm,CoupBjrp,CoupBjrm, ZZ, eZZ, ZZp, ZZm, ZZrp, ZZrm, ZZpup, ZZpum);   
    GetSysQ2(Lumi,idm,"ZZ",CoupB, q1ZZ, q2ZZ, q3ZZ, q4ZZ, q6ZZ, q8ZZ);
    GetSysBTAG(Lumi,idm,"ZZ",CoupB, sB1ZZp, sB1ZZm, sB2ZZp, sB2ZZm, sB3ZZp, sB3ZZm, sB4ZZp, sB4ZZm, 
	       sB5ZZp, sB5ZZm, sB6ZZp, sB6ZZm, sB7ZZp, sB7ZZm, sB8ZZp, sB8ZZm);
  }
  std::vector<double> ZZQ2 = {Abs(q1ZZ-ZZ), Abs(q2ZZ-ZZ), Abs(q3ZZ-ZZ), Abs(q4ZZ-ZZ), Abs(q6ZZ-ZZ), Abs(q8ZZ-ZZ)};
  double SysZZQ2 = *std::max_element(ZZQ2.begin(), ZZQ2.end());

  // SingleToptW
  double sTw=0, esTw=0, sTwp=0, sTwm=0, sTwrp=0, sTwrm=0, sTwpup=0, sTwpum=0;
  double q1sTw=0, q2sTw=0, q3sTw=0, q4sTw=0, q6sTw=0, q8sTw=0;
  double sB1Twp=0, sB1Twm=0, sB2Twp=0, sB2Twm=0, sB3Twp=0, sB3Twm=0, sB4Twp=0, sB4Twm=0, 
    sB5Twp=0, sB5Twm=0, sB6Twp=0, sB6Twm=0, sB7Twp=0, sB7Twm=0, sB8Twp=0, sB8Twm=0;
  if ( use_SingleToptW ) { 
    GetYield(Lumi,idm,"SingleToptW",CoupB,CoupBjp,CoupBjm,CoupBjrp,CoupBjrm, sTw, esTw, sTwp, sTwm, sTwrp, sTwrm, sTwpup, sTwpum); 
    GetSysQ2(Lumi,idm,"SingleToptW",CoupB, q1sTw, q2sTw, q3sTw, q4sTw, q6sTw, q8sTw);
    GetSysBTAG(Lumi,idm,"SingleToptW",CoupB, sB1Twp, sB1Twm, sB2Twp, sB2Twm, sB3Twp, sB3Twm, sB4Twp, sB4Twm, 
	       sB5Twp, sB5Twm, sB6Twp, sB6Twm, sB7Twp, sB7Twm, sB8Twp, sB8Twm);
  }
  std::vector<double> sTwQ2 = {Abs(q1sTw-sTw), Abs(q2sTw-sTw), Abs(q3sTw-sTw), Abs(q4sTw-sTw), Abs(q6sTw-sTw), Abs(q8sTw-sTw)};
  double SyssTwQ2 = *std::max_element(sTwQ2.begin(), sTwQ2.end());
  // SingleToptChannel
  double sTtC=0, esTtC=0, sTtCp=0, sTtCm=0, sTtCrp=0, sTtCrm=0, sTtCpup=0, sTtCpum=0;
  double q1sTtC=0, q2sTtC=0, q3sTtC=0, q4sTtC=0, q6sTtC=0, q8sTtC=0;
  double sB1TtCp=0, sB1TtCm=0, sB2TtCp=0, sB2TtCm=0, sB3TtCp=0, sB3TtCm=0, sB4TtCp=0, sB4TtCm=0, 
    sB5TtCp=0, sB5TtCm=0, sB6TtCp=0, sB6TtCm=0, sB7TtCp=0, sB7TtCm=0, sB8TtCp=0, sB8TtCm=0;
 if ( use_SingleToptC) { 
    GetYield(Lumi,idm,"SingleToptChannel",CoupB,CoupBjp,CoupBjm,CoupBjrp,CoupBjrm, sTtC, esTtC, sTtCp, sTtCm, sTtCrp, sTtCrm, sTtCpup, sTtCpum); 
    GetSysQ2(Lumi,idm,"SingleToptChannel",CoupB, q1sTtC, q2sTtC, q3sTtC, q4sTtC, q6sTtC, q8sTtC);
    GetSysBTAG(Lumi,idm,"SingleToptChannel",CoupB, sB1TtCp, sB1TtCm, sB2TtCp, sB2TtCm, sB3TtCp, sB3TtCm, sB4TtCp, sB4TtCm, 
	       sB5TtCp, sB5TtCm, sB6TtCp, sB6TtCm, sB7TtCp, sB7TtCm, sB8TtCp, sB8TtCm);
  }
  std::vector<double> sTtCQ2 = {Abs(q1sTtC-sTtC), Abs(q2sTtC-sTtC), Abs(q3sTtC-sTtC), Abs(q4sTtC-sTtC), Abs(q6sTtC-sTtC), Abs(q8sTtC-sTtC)};
  double SyssTtCQ2 = *std::max_element(sTtCQ2.begin(), sTtCQ2.end());
  // SingleToptW
  double sTBw=0, esTBw=0, sTBwp=0, sTBwm=0, sTBwrp=0, sTBwrm=0, sTBwpup=0, sTBwpum=0;
  double q1sTBw=0, q2sTBw=0, q3sTBw=0, q4sTBw=0, q6sTBw=0, q8sTBw=0;
  double sB1TBwp=0, sB1TBwm=0, sB2TBwp=0, sB2TBwm=0, sB3TBwp=0, sB3TBwm=0, sB4TBwp=0, sB4TBwm=0, 
    sB5TBwp=0, sB5TBwm=0, sB6TBwp=0, sB6TBwm=0, sB7TBwp=0, sB7TBwm=0, sB8TBwp=0, sB8TBwm=0;
  if ( use_SingleTopBtW ) { 
    GetYield(Lumi,idm,"SingleTopBtW",CoupB,CoupBjp,CoupBjm,CoupBjrp,CoupBjrm, sTBw, esTBw, sTBwp, sTBwm, sTBwrp, sTBwrm, sTBwpup, sTBwpum); 
    GetSysQ2(Lumi,idm,"SingleTopBtW",CoupB, q1sTBw, q2sTBw, q3sTBw, q4sTBw, q6sTBw, q8sTBw);
    GetSysBTAG(Lumi,idm,"SingleTopBtW",CoupB, sB1TBwp, sB1TBwm, sB2TBwp, sB2TBwm, sB3TBwp, sB3TBwm, sB4TBwp, sB4TBwm, 
	       sB5TBwp, sB5TBwm, sB6TBwp, sB6TBwm, sB7TBwp, sB7TBwm, sB8TBwp, sB8TBwm);
  }
  std::vector<double> sTBwQ2 = {Abs(q1sTBw-sTBw), Abs(q2sTBw-sTBw), Abs(q3sTBw-sTBw), Abs(q4sTBw-sTBw), Abs(q6sTBw-sTBw), Abs(q8sTBw-sTBw)};
  double SyssTBwQ2 = *std::max_element(sTBwQ2.begin(), sTBwQ2.end());
  // SingleToptChannel
  double sTBtC=0, esTBtC=0, sTBtCp=0, sTBtCm=0, sTBtCrp=0, sTBtCrm=0, sTBtCpup=0, sTBtCpum=0;
  double q1sTBtC=0, q2sTBtC=0, q3sTBtC=0, q4sTBtC=0, q6sTBtC=0, q8sTBtC=0;
  double sB1TBtCp=0, sB1TBtCm=0, sB2TBtCp=0, sB2TBtCm=0, sB3TBtCp=0, sB3TBtCm=0, sB4TBtCp=0, sB4TBtCm=0, 
    sB5TBtCp=0, sB5TBtCm=0, sB6TBtCp=0, sB6TBtCm=0, sB7TBtCp=0, sB7TBtCm=0, sB8TBtCp=0, sB8TBtCm=0;
  if ( use_SingleTopBtC) { 
    GetYield(Lumi,idm,"SingleTopBtChannel",CoupB,CoupBjp,CoupBjm,CoupBjrp,CoupBjrm, sTBtC, esTBtC, sTBtCp, sTBtCm, sTBtCrp, sTBtCrm, sTBtCpup, sTBtCpum); 
    GetSysQ2(Lumi,idm,"SingleTopBtChannel",CoupB, q1sTBtC, q2sTBtC, q3sTBtC, q4sTBtC, q6sTBtC, q8sTBtC);
    GetSysBTAG(Lumi,idm,"SingleTopBtChannel",CoupB, sB1TBtCp, sB1TBtCm, sB2TBtCp, sB2TBtCm, sB3TBtCp, sB3TBtCm, sB4TBtCp, sB4TBtCm, 
	       sB5TBtCp, sB5TBtCm, sB6TBtCp, sB6TBtCm, sB7TBtCp, sB7TBtCm, sB8TBtCp, sB8TBtCm);
  }
  std::vector<double> sTBtCQ2 = {Abs(q1sTBtC-sTBtC), Abs(q2sTBtC-sTBtC), Abs(q3sTBtC-sTBtC), Abs(q4sTBtC-sTBtC), Abs(q6sTBtC-sTBtC), Abs(q8sTBtC-sTBtC)};
  double SyssTBtCQ2 = *std::max_element(sTBtCQ2.begin(), sTBtCQ2.end());

  // DY1
  double DY1=0, eDY1=0, DY1p=0, DY1m=0, DY1rp=0, DY1rm=0, DY1pup=0, DY1pum=0;
  double sISR1pDY1=0, sISR2pDY1=0, sISR3pDY1=0, sISR4pDY1=0, sISR5pDY1=0, sISR6pDY1=0, sISR7pDY1=0;
  double sISR1mDY1=0, sISR2mDY1=0, sISR3mDY1=0, sISR4mDY1=0, sISR5mDY1=0, sISR6mDY1=0, sISR7mDY1=0;
  double q1DY1=0, q2DY1=0, q3DY1=0, q4DY1=0, q6DY1=0, q8DY1=0;
  double sB1DY1p=0, sB1DY1m=0, sB2DY1p=0, sB2DY1m=0, sB3DY1p=0, sB3DY1m=0, sB4DY1p=0, sB4DY1m=0, 
    sB5DY1p=0, sB5DY1m=0, sB6DY1p=0, sB6DY1m=0, sB7DY1p=0, sB7DY1m=0, sB8DY1p=0, sB8DY1m=0;
  if ( use_DY1 ) { 
    GetYield(Lumi,idm,"DY1",CoupB,CoupBjp,CoupBjm,CoupBjrp,CoupBjrm, DY1, eDY1, DY1p, DY1m, DY1rp, DY1rm, DY1pup, DY1pum); 
    GetSysISR(Lumi,idm,"DY1",CoupB,
	      sISR1pDY1,sISR2pDY1,sISR3pDY1,sISR4pDY1,sISR5pDY1,sISR6pDY1,sISR7pDY1,
	      sISR1mDY1,sISR2mDY1,sISR3mDY1,sISR4mDY1,sISR5mDY1,sISR6mDY1,sISR7mDY1); 
    GetSysQ2(Lumi,idm,"DY1",CoupB, q1DY1, q2DY1, q3DY1, q4DY1, q6DY1, q8DY1);
    GetSysBTAG(Lumi,idm,"DY1",CoupB, sB1DY1p, sB1DY1m, sB2DY1p, sB2DY1m, sB3DY1p, sB3DY1m, sB4DY1p, sB4DY1m, 
	       sB5DY1p, sB5DY1m, sB6DY1p, sB6DY1m, sB7DY1p, sB7DY1m, sB8DY1p, sB8DY1m);
  }
  std::vector<double> DY1Q2 = {Abs(q1DY1-DY1), Abs(q2DY1-DY1), Abs(q3DY1-DY1), Abs(q4DY1-DY1), Abs(q6DY1-DY1), Abs(q8DY1-DY1)};
  double SysDY1Q2 = *std::max_element(DY1Q2.begin(), DY1Q2.end());
  // DY2
  double DY2=0, eDY2=0, DY2p=0, DY2m=0, DY2rp=0, DY2rm=0, DY2pup=0, DY2pum=0;
  double sISR1pDY2=0, sISR2pDY2=0, sISR3pDY2=0, sISR4pDY2=0, sISR5pDY2=0, sISR6pDY2=0, sISR7pDY2=0;
  double sISR1mDY2=0, sISR2mDY2=0, sISR3mDY2=0, sISR4mDY2=0, sISR5mDY2=0, sISR6mDY2=0, sISR7mDY2=0;
  double q1DY2=0, q2DY2=0, q3DY2=0, q4DY2=0, q6DY2=0, q8DY2=0;
  double sB1DY2p=0, sB1DY2m=0, sB2DY2p=0, sB2DY2m=0, sB3DY2p=0, sB3DY2m=0, sB4DY2p=0, sB4DY2m=0, 
    sB5DY2p=0, sB5DY2m=0, sB6DY2p=0, sB6DY2m=0, sB7DY2p=0, sB7DY2m=0, sB8DY2p=0, sB8DY2m=0;
  if ( use_DY2 ) { 
    GetYield(Lumi,idm,"DY2",CoupB,CoupBjp,CoupBjm,CoupBjrp,CoupBjrm, DY2, eDY2, DY2p, DY2m, DY2rp, DY2rm, DY2pup, DY2pum); 
    GetSysISR(Lumi,idm,"DY2",CoupB,
	      sISR1pDY2,sISR2pDY2,sISR3pDY2,sISR4pDY2,sISR5pDY2,sISR6pDY2,sISR7pDY2,
	      sISR1mDY2,sISR2mDY2,sISR3mDY2,sISR4mDY2,sISR5mDY2,sISR6mDY2,sISR7mDY2); 
    GetSysQ2(Lumi,idm,"DY2",CoupB, q1DY2, q2DY2, q3DY2, q4DY2, q6DY2, q8DY2);
    GetSysBTAG(Lumi,idm,"DY2",CoupB, sB1DY2p, sB1DY2m, sB2DY2p, sB2DY2m, sB3DY2p, sB3DY2m, sB4DY2p, sB4DY2m, 
	       sB5DY2p, sB5DY2m, sB6DY2p, sB6DY2m, sB7DY2p, sB7DY2m, sB8DY2p, sB8DY2m);
  }
  std::vector<double> DY2Q2 = {Abs(q1DY2-DY2), Abs(q2DY2-DY2), Abs(q3DY2-DY2), Abs(q4DY2-DY2), Abs(q6DY2-DY2), Abs(q8DY2-DY2)};
  double SysDY2Q2 = *std::max_element(DY2Q2.begin(), DY2Q2.end());
  // DY3
  double DY3=0, eDY3=0, DY3p=0, DY3m=0, DY3rp=0, DY3rm=0, DY3pup=0, DY3pum=0;
  double sISR1pDY3=0, sISR2pDY3=0, sISR3pDY3=0, sISR4pDY3=0, sISR5pDY3=0, sISR6pDY3=0, sISR7pDY3=0;
  double sISR1mDY3=0, sISR2mDY3=0, sISR3mDY3=0, sISR4mDY3=0, sISR5mDY3=0, sISR6mDY3=0, sISR7mDY3=0;
  double q1DY3=0, q2DY3=0, q3DY3=0, q4DY3=0, q6DY3=0, q8DY3=0;
  double sB1DY3p=0, sB1DY3m=0, sB2DY3p=0, sB2DY3m=0, sB3DY3p=0, sB3DY3m=0, sB4DY3p=0, sB4DY3m=0, 
    sB5DY3p=0, sB5DY3m=0, sB6DY3p=0, sB6DY3m=0, sB7DY3p=0, sB7DY3m=0, sB8DY3p=0, sB8DY3m=0;
  if ( use_DY3 ) { 
    GetYield(Lumi,idm,"DY3",CoupB,CoupBjp,CoupBjm,CoupBjrp,CoupBjrm, DY3, eDY3, DY3p, DY3m, DY3rp, DY3rm, DY3pup, DY3pum); 
    GetSysISR(Lumi,idm,"DY3",CoupB,
	      sISR1pDY3,sISR2pDY3,sISR3pDY3,sISR4pDY3,sISR5pDY3,sISR6pDY3,sISR7pDY3,
	      sISR1mDY3,sISR2mDY3,sISR3mDY3,sISR4mDY3,sISR5mDY3,sISR6mDY3,sISR7mDY3); 
    GetSysQ2(Lumi,idm,"DY3",CoupB, q1DY3, q2DY3, q3DY3, q4DY3, q6DY3, q8DY3);
    GetSysBTAG(Lumi,idm,"DY3",CoupB, sB1DY3p, sB1DY3m, sB2DY3p, sB2DY3m, sB3DY3p, sB3DY3m, sB4DY3p, sB4DY3m, 
	       sB5DY3p, sB5DY3m, sB6DY3p, sB6DY3m, sB7DY3p, sB7DY3m, sB8DY3p, sB8DY3m);
  }
  std::vector<double> DY3Q2 = {Abs(q1DY3-DY3), Abs(q2DY3-DY3), Abs(q3DY3-DY3), Abs(q4DY3-DY3), Abs(q6DY3-DY3), Abs(q8DY3-DY3)};
  double SysDY3Q2 = *std::max_element(DY3Q2.begin(), DY3Q2.end());
  // DY4
  double DY4=0, eDY4=0, DY4p=0, DY4m=0, DY4rp=0, DY4rm=0, DY4pup=0, DY4pum=0;
  double sISR1pDY4=0, sISR2pDY4=0, sISR3pDY4=0, sISR4pDY4=0, sISR5pDY4=0, sISR6pDY4=0, sISR7pDY4=0;
  double sISR1mDY4=0, sISR2mDY4=0, sISR3mDY4=0, sISR4mDY4=0, sISR5mDY4=0, sISR6mDY4=0, sISR7mDY4=0;
  double q1DY4=0, q2DY4=0, q3DY4=0, q4DY4=0, q6DY4=0, q8DY4=0;
  double sB1DY4p=0, sB1DY4m=0, sB2DY4p=0, sB2DY4m=0, sB3DY4p=0, sB3DY4m=0, sB4DY4p=0, sB4DY4m=0, 
    sB5DY4p=0, sB5DY4m=0, sB6DY4p=0, sB6DY4m=0, sB7DY4p=0, sB7DY4m=0, sB8DY4p=0, sB8DY4m=0;
  if ( use_DY4 ) { 
    GetYield(Lumi,idm,"DY4",CoupB,CoupBjp,CoupBjm,CoupBjrp,CoupBjrm, DY4, eDY4, DY4p, DY4m, DY4rp, DY4rm, DY4pup, DY4pum); 
    GetSysISR(Lumi,idm,"DY4",CoupB,
	      sISR1pDY4,sISR2pDY4,sISR3pDY4,sISR4pDY4,sISR5pDY4,sISR6pDY4,sISR7pDY4,
	      sISR1mDY4,sISR2mDY4,sISR3mDY4,sISR4mDY4,sISR5mDY4,sISR6mDY4,sISR7mDY4); 
    GetSysQ2(Lumi,idm,"DY4",CoupB, q1DY4, q2DY4, q3DY4, q4DY4, q6DY4, q8DY4);
    GetSysBTAG(Lumi,idm,"DY4",CoupB, sB1DY4p, sB1DY4m, sB2DY4p, sB2DY4m, sB3DY4p, sB3DY4m, sB4DY4p, sB4DY4m, 
	       sB5DY4p, sB5DY4m, sB6DY4p, sB6DY4m, sB7DY4p, sB7DY4m, sB8DY4p, sB8DY4m);
 }
  std::vector<double> DY4Q2 = {Abs(q1DY4-DY4), Abs(q2DY4-DY4), Abs(q3DY4-DY4), Abs(q4DY4-DY4), Abs(q6DY4-DY4), Abs(q8DY4-DY4)};
  double SysDY4Q2 = *std::max_element(DY4Q2.begin(), DY4Q2.end());
  // DY5
  double DY5=0, eDY5=0, DY5p=0, DY5m=0, DY5rp=0, DY5rm=0, DY5pup=0, DY5pum=0;
  double sISR1pDY5=0, sISR2pDY5=0, sISR3pDY5=0, sISR4pDY5=0, sISR5pDY5=0, sISR6pDY5=0, sISR7pDY5=0;
  double sISR1mDY5=0, sISR2mDY5=0, sISR3mDY5=0, sISR4mDY5=0, sISR5mDY5=0, sISR6mDY5=0, sISR7mDY5=0;
  double q1DY5=0, q2DY5=0, q3DY5=0, q4DY5=0, q6DY5=0, q8DY5=0;
  double sB1DY5p=0, sB1DY5m=0, sB2DY5p=0, sB2DY5m=0, sB3DY5p=0, sB3DY5m=0, sB4DY5p=0, sB4DY5m=0, 
    sB5DY5p=0, sB5DY5m=0, sB6DY5p=0, sB6DY5m=0, sB7DY5p=0, sB7DY5m=0, sB8DY5p=0, sB8DY5m=0;
  if ( use_DY5 ) { 
    GetYield(Lumi,idm,"DY5",CoupB,CoupBjp,CoupBjm,CoupBjrp,CoupBjrm, DY5, eDY5, DY5p, DY5m, DY5rp, DY5rm, DY5pup, DY5pum); 
    GetSysISR(Lumi,idm,"DY5",CoupB,
	      sISR1pDY5,sISR2pDY5,sISR3pDY5,sISR4pDY5,sISR5pDY5,sISR6pDY5,sISR7pDY5,
	      sISR1mDY5,sISR2mDY5,sISR3mDY5,sISR4mDY5,sISR5mDY5,sISR6mDY5,sISR7mDY5); 
    GetSysQ2(Lumi,idm,"DY5",CoupB, q1DY5, q2DY5, q3DY5, q4DY5, q6DY5, q8DY5);
    GetSysBTAG(Lumi,idm,"DY5",CoupB, sB1DY5p, sB1DY5m, sB2DY5p, sB2DY5m, sB3DY5p, sB3DY5m, sB4DY5p, sB4DY5m, 
	       sB5DY5p, sB5DY5m, sB6DY5p, sB6DY5m, sB7DY5p, sB7DY5m, sB8DY5p, sB8DY5m);
  }
  std::vector<double> DY5Q2 = {Abs(q1DY5-DY5), Abs(q2DY5-DY5), Abs(q3DY5-DY5), Abs(q4DY5-DY5), Abs(q6DY5-DY5), Abs(q8DY5-DY5)};
  double SysDY5Q2 = *std::max_element(DY5Q2.begin(), DY5Q2.end());
  // DY6
  double DY6=0, eDY6=0, DY6p=0, DY6m=0, DY6rp=0, DY6rm=0, DY6pup=0, DY6pum=0;
  double sISR1pDY6=0, sISR2pDY6=0, sISR3pDY6=0, sISR4pDY6=0, sISR5pDY6=0, sISR6pDY6=0, sISR7pDY6=0;
  double sISR1mDY6=0, sISR2mDY6=0, sISR3mDY6=0, sISR4mDY6=0, sISR5mDY6=0, sISR6mDY6=0, sISR7mDY6=0;
  double q1DY6=0, q2DY6=0, q3DY6=0, q4DY6=0, q6DY6=0, q8DY6=0;
  double sB1DY6p=0, sB1DY6m=0, sB2DY6p=0, sB2DY6m=0, sB3DY6p=0, sB3DY6m=0, sB4DY6p=0, sB4DY6m=0, 
    sB5DY6p=0, sB5DY6m=0, sB6DY6p=0, sB6DY6m=0, sB7DY6p=0, sB7DY6m=0, sB8DY6p=0, sB8DY6m=0;
  if ( use_DY6 ) { 
    GetYield(Lumi,idm,"DY6",CoupB,CoupBjp,CoupBjm,CoupBjrp,CoupBjrm, DY6, eDY6, DY6p, DY6m, DY6rp, DY6rm, DY6pup, DY6pum); 
    GetSysISR(Lumi,idm,"DY6",CoupB,
	      sISR1pDY6,sISR2pDY6,sISR3pDY6,sISR4pDY6,sISR5pDY6,sISR6pDY6,sISR7pDY6,
	      sISR1mDY6,sISR2mDY6,sISR3mDY6,sISR4mDY6,sISR5mDY6,sISR6mDY6,sISR7mDY6); 
    GetSysQ2(Lumi,idm,"DY6",CoupB, q1DY6, q2DY6, q3DY6, q4DY6, q6DY6, q8DY6);
    GetSysBTAG(Lumi,idm,"DY6",CoupB, sB1DY6p, sB1DY6m, sB2DY6p, sB2DY6m, sB3DY6p, sB3DY6m, sB4DY6p, sB4DY6m, 
	       sB5DY6p, sB5DY6m, sB6DY6p, sB6DY6m, sB7DY6p, sB7DY6m, sB8DY6p, sB8DY6m);
  }
  std::vector<double> DY6Q2 = {Abs(q1DY6-DY6), Abs(q2DY6-DY6), Abs(q3DY6-DY6), Abs(q4DY6-DY6), Abs(q6DY6-DY6), Abs(q8DY6-DY6)};
  double SysDY6Q2 = *std::max_element(DY6Q2.begin(), DY6Q2.end());
  // DY7
  double DY7=0, eDY7=0, DY7p=0, DY7m=0, DY7rp=0, DY7rm=0, DY7pup=0, DY7pum=0;
  double sISR1pDY7=0, sISR2pDY7=0, sISR3pDY7=0, sISR4pDY7=0, sISR5pDY7=0, sISR6pDY7=0, sISR7pDY7=0;
  double sISR1mDY7=0, sISR2mDY7=0, sISR3mDY7=0, sISR4mDY7=0, sISR5mDY7=0, sISR6mDY7=0, sISR7mDY7=0;
  double q1DY7=0, q2DY7=0, q3DY7=0, q4DY7=0, q6DY7=0, q8DY7=0;
  double sB1DY7p=0, sB1DY7m=0, sB2DY7p=0, sB2DY7m=0, sB3DY7p=0, sB3DY7m=0, sB4DY7p=0, sB4DY7m=0, 
    sB5DY7p=0, sB5DY7m=0, sB6DY7p=0, sB6DY7m=0, sB7DY7p=0, sB7DY7m=0, sB8DY7p=0, sB8DY7m=0;
  if ( use_DY7 ) { 
    GetYield(Lumi,idm,"DY7",CoupB,CoupBjp,CoupBjm,CoupBjrp,CoupBjrm, DY7, eDY7, DY7p, DY7m, DY7rp, DY7rm, DY7pup, DY7pum); 
    GetSysISR(Lumi,idm,"DY7",CoupB,
	      sISR1pDY7,sISR2pDY7,sISR3pDY7,sISR4pDY7,sISR5pDY7,sISR6pDY7,sISR7pDY7,
	      sISR1mDY7,sISR2mDY7,sISR3mDY7,sISR4mDY7,sISR5mDY7,sISR6mDY7,sISR7mDY7); 
    GetSysQ2(Lumi,idm,"DY7",CoupB, q1DY7, q2DY7, q3DY7, q4DY7, q6DY7, q8DY7);
    GetSysBTAG(Lumi,idm,"DY7",CoupB, sB1DY7p, sB1DY7m, sB2DY7p, sB2DY7m, sB3DY7p, sB3DY7m, sB4DY7p, sB4DY7m, 
	       sB5DY7p, sB5DY7m, sB6DY7p, sB6DY7m, sB7DY7p, sB7DY7m, sB8DY7p, sB8DY7m);
  }
  std::vector<double> DY7Q2 = {Abs(q1DY7-DY7), Abs(q2DY7-DY7), Abs(q3DY7-DY7), Abs(q4DY7-DY7), Abs(q6DY7-DY7), Abs(q8DY7-DY7)};
  double SysDY7Q2 = *std::max_element(DY7Q2.begin(), DY7Q2.end());
  // DY8
  double DY8=0, eDY8=0, DY8p=0, DY8m=0, DY8rp=0, DY8rm=0, DY8pup=0, DY8pum=0;
  double sISR1pDY8=0, sISR2pDY8=0, sISR3pDY8=0, sISR4pDY8=0, sISR5pDY8=0, sISR6pDY8=0, sISR7pDY8=0;
  double sISR1mDY8=0, sISR2mDY8=0, sISR3mDY8=0, sISR4mDY8=0, sISR5mDY8=0, sISR6mDY8=0, sISR7mDY8=0;
  double q1DY8=0, q2DY8=0, q3DY8=0, q4DY8=0, q6DY8=0, q8DY8=0;
  double sB1DY8p=0, sB1DY8m=0, sB2DY8p=0, sB2DY8m=0, sB3DY8p=0, sB3DY8m=0, sB4DY8p=0, sB4DY8m=0, 
    sB5DY8p=0, sB5DY8m=0, sB6DY8p=0, sB6DY8m=0, sB7DY8p=0, sB7DY8m=0, sB8DY8p=0, sB8DY8m=0;
  if ( use_DY8 ) { 
    GetYield(Lumi,idm,"DY8",CoupB,CoupBjp,CoupBjm,CoupBjrp,CoupBjrm, DY8, eDY8, DY8p, DY8m, DY8rp, DY8rm, DY8pup, DY8pum); 
    GetSysISR(Lumi,idm,"DY8",CoupB,
	      sISR1pDY8,sISR2pDY8,sISR3pDY8,sISR4pDY8,sISR5pDY8,sISR6pDY8,sISR7pDY8,
	      sISR1mDY8,sISR2mDY8,sISR3mDY8,sISR4mDY8,sISR5mDY8,sISR6mDY8,sISR7mDY8); 
    GetSysQ2(Lumi,idm,"DY8",CoupB, q1DY8, q2DY8, q3DY8, q4DY8, q6DY8, q8DY8);
    GetSysBTAG(Lumi,idm,"DY8",CoupB, sB1DY8p, sB1DY8m, sB2DY8p, sB2DY8m, sB3DY8p, sB3DY8m, sB4DY8p, sB4DY8m, 
	       sB5DY8p, sB5DY8m, sB6DY8p, sB6DY8m, sB7DY8p, sB7DY8m, sB8DY8p, sB8DY8m);
  }
  std::vector<double> DY8Q2 = {Abs(q1DY8-DY8), Abs(q2DY8-DY8), Abs(q3DY8-DY8), Abs(q4DY8-DY8), Abs(q6DY8-DY8), Abs(q8DY8-DY8)};
  double SysDY8Q2 = *std::max_element(DY8Q2.begin(), DY8Q2.end());
  // DY9
  double DY9=0, eDY9=0, DY9p=0, DY9m=0, DY9rp=0, DY9rm=0, DY9pup=0, DY9pum=0;
  double sISR1pDY9=0, sISR2pDY9=0, sISR3pDY9=0, sISR4pDY9=0, sISR5pDY9=0, sISR6pDY9=0, sISR7pDY9=0;
  double sISR1mDY9=0, sISR2mDY9=0, sISR3mDY9=0, sISR4mDY9=0, sISR5mDY9=0, sISR6mDY9=0, sISR7mDY9=0;
  double q1DY9=0, q2DY9=0, q3DY9=0, q4DY9=0, q6DY9=0, q8DY9=0;
  double sB1DY9p=0, sB1DY9m=0, sB2DY9p=0, sB2DY9m=0, sB3DY9p=0, sB3DY9m=0, sB4DY9p=0, sB4DY9m=0, 
    sB5DY9p=0, sB5DY9m=0, sB6DY9p=0, sB6DY9m=0, sB7DY9p=0, sB7DY9m=0, sB8DY9p=0, sB8DY9m=0;
  if ( use_DY9 ) { 
    GetYield(Lumi,idm,"DY9",CoupB,CoupBjp,CoupBjm,CoupBjrp,CoupBjrm, DY9, eDY9, DY9p, DY9m, DY9rp, DY9rm, DY9pup, DY9pum); 
    GetSysISR(Lumi,idm,"DY9",CoupB,
	      sISR1pDY9,sISR2pDY9,sISR3pDY9,sISR4pDY9,sISR5pDY9,sISR6pDY9,sISR7pDY9,
	      sISR1mDY9,sISR2mDY9,sISR3mDY9,sISR4mDY9,sISR5mDY9,sISR6mDY9,sISR7mDY9); 
    GetSysQ2(Lumi,idm,"DY9",CoupB, q1DY9, q2DY9, q3DY9, q4DY9, q6DY9, q8DY9);
    GetSysBTAG(Lumi,idm,"DY9",CoupB, sB1DY9p, sB1DY9m, sB2DY9p, sB2DY9m, sB3DY9p, sB3DY9m, sB4DY9p, sB4DY9m, 
	       sB5DY9p, sB5DY9m, sB6DY9p, sB6DY9m, sB7DY9p, sB7DY9m, sB8DY9p, sB8DY9m);
  }
  std::vector<double> DY9Q2 = {Abs(q1DY9-DY9), Abs(q2DY9-DY9), Abs(q3DY9-DY9), Abs(q4DY9-DY9), Abs(q6DY9-DY9), Abs(q8DY9-DY9)};
  double SysDY9Q2 = *std::max_element(DY9Q2.begin(), DY9Q2.end());
  // DY10
  double DY10=0, eDY10=0, DY10p=0, DY10m=0, DY10rp=0, DY10rm=0, DY10pup=0, DY10pum=0;
  double sISR1pDY10=0, sISR2pDY10=0, sISR3pDY10=0, sISR4pDY10=0, sISR5pDY10=0, sISR6pDY10=0, sISR7pDY10=0;
  double sISR1mDY10=0, sISR2mDY10=0, sISR3mDY10=0, sISR4mDY10=0, sISR5mDY10=0, sISR6mDY10=0, sISR7mDY10=0;
  double q1DY10=0, q2DY10=0, q3DY10=0, q4DY10=0, q6DY10=0, q8DY10=0;
  double sB1DY10p=0, sB1DY10m=0, sB2DY10p=0, sB2DY10m=0, sB3DY10p=0, sB3DY10m=0, sB4DY10p=0, sB4DY10m=0, 
    sB5DY10p=0, sB5DY10m=0, sB6DY10p=0, sB6DY10m=0, sB7DY10p=0, sB7DY10m=0, sB8DY10p=0, sB8DY10m=0;
  if ( use_DY10 ) { 
    GetYield(Lumi,idm,"DY10",CoupB,CoupBjp,CoupBjm,CoupBjrp,CoupBjrm, DY10, eDY10, DY10p, DY10m, DY10rp, DY10rm, DY10pup, DY10pum); 
    GetSysISR(Lumi,idm,"DY10",CoupB,
	      sISR1pDY10,sISR2pDY10,sISR3pDY10,sISR4pDY10,sISR5pDY10,sISR6pDY10,sISR7pDY10,
	      sISR1mDY10,sISR2mDY10,sISR3mDY10,sISR4mDY10,sISR5mDY10,sISR6mDY10,sISR7mDY10); 
    GetSysQ2(Lumi,idm,"DY10",CoupB, q1DY10, q2DY10, q3DY10, q4DY10, q6DY10, q8DY10);
    GetSysBTAG(Lumi,idm,"DY10",CoupB, sB1DY10p, sB1DY10m, sB2DY10p, sB2DY10m, sB3DY10p, sB3DY10m, sB4DY10p, sB4DY10m, 
	       sB5DY10p, sB5DY10m, sB6DY10p, sB6DY10m, sB7DY10p, sB7DY10m, sB8DY10p, sB8DY10m);
  }
  std::vector<double> DY10Q2 = {Abs(q1DY10-DY10), Abs(q2DY10-DY10), Abs(q3DY10-DY10), Abs(q4DY10-DY10), Abs(q6DY10-DY10), Abs(q8DY10-DY10)};
  double SysDY10Q2 = *std::max_element(DY10Q2.begin(), DY10Q2.end());
  // DY11
  double DY11=0, eDY11=0, DY11p=0, DY11m=0, DY11rp=0, DY11rm=0, DY11pup=0, DY11pum=0;
  double sISR1pDY11=0, sISR2pDY11=0, sISR3pDY11=0, sISR4pDY11=0, sISR5pDY11=0, sISR6pDY11=0, sISR7pDY11=0;
  double sISR1mDY11=0, sISR2mDY11=0, sISR3mDY11=0, sISR4mDY11=0, sISR5mDY11=0, sISR6mDY11=0, sISR7mDY11=0;
  double q1DY11=0, q2DY11=0, q3DY11=0, q4DY11=0, q6DY11=0, q8DY11=0;
  double sB1DY11p=0, sB1DY11m=0, sB2DY11p=0, sB2DY11m=0, sB3DY11p=0, sB3DY11m=0, sB4DY11p=0, sB4DY11m=0, 
    sB5DY11p=0, sB5DY11m=0, sB6DY11p=0, sB6DY11m=0, sB7DY11p=0, sB7DY11m=0, sB8DY11p=0, sB8DY11m=0;
  if ( use_DY11 ) { 
    GetYield(Lumi,idm,"DY11",CoupB,CoupBjp,CoupBjm,CoupBjrp,CoupBjrm, DY11, eDY11, DY11p, DY11m, DY11rp, DY11rm, DY11pup, DY11pum); 
    GetSysISR(Lumi,idm,"DY11",CoupB,
	      sISR1pDY11,sISR2pDY11,sISR3pDY11,sISR4pDY11,sISR5pDY11,sISR6pDY11,sISR7pDY11,
	      sISR1mDY11,sISR2mDY11,sISR3mDY11,sISR4mDY11,sISR5mDY11,sISR6mDY11,sISR7mDY11); 
    GetSysQ2(Lumi,idm,"DY11",CoupB, q1DY10, q2DY10, q3DY10, q4DY10, q6DY10, q8DY10);
    GetSysBTAG(Lumi,idm,"DY11",CoupB, sB1DY11p, sB1DY11m, sB2DY11p, sB2DY11m, sB3DY11p, sB3DY11m, sB4DY11p, sB4DY11m, 
	       sB5DY11p, sB5DY11m, sB6DY11p, sB6DY11m, sB7DY11p, sB7DY11m, sB8DY11p, sB8DY11m);
  }
  std::vector<double> DY11Q2 = {Abs(q1DY11-DY11), Abs(q2DY11-DY11), Abs(q3DY11-DY11), Abs(q4DY11-DY11), Abs(q6DY11-DY11), Abs(q8DY11-DY11)};
  double SysDY11Q2 = *std::max_element(DY10Q2.begin(), DY10Q2.end());

  // TTX
  double TTgj=0, eTTgj=0, TTgjp=0, TTgjm=0, TTgjrp=0, TTgjrm=0, TTgjpup=0, TTgjpum=0;
  double q1TTgj=0, q2TTgj=0, q3TTgj=0, q4TTgj=0, q6TTgj=0, q8TTgj=0;
  double sB1TTgjp=0, sB1TTgjm=0, sB2TTgjp=0, sB2TTgjm=0, sB3TTgjp=0, sB3TTgjm=0, sB4TTgjp=0, sB4TTgjm=0, 
    sB5TTgjp=0, sB5TTgjm=0, sB6TTgjp=0, sB6TTgjm=0, sB7TTgjp=0, sB7TTgjm=0, sB8TTgjp=0, sB8TTgjm=0;
  if ( use_TTgj ) { 
    GetYield(Lumi,idm,"TTgj",CoupB,CoupBjp,CoupBjm,CoupBjrp,CoupBjrm, TTgj, eTTgj, TTgjp, TTgjm, TTgjrp, TTgjrm, TTgjpup, TTgjpum); 
    GetSysQ2(Lumi,idm,"TTgj",CoupB, q1TTgj, q2TTgj, q3TTgj, q4TTgj, q6TTgj, q8TTgj);
    GetSysBTAG(Lumi,idm,"TTgj",CoupB, sB1TTgjp, sB1TTgjm, sB2TTgjp, sB2TTgjm, sB3TTgjp, sB3TTgjm, sB4TTgjp, sB4TTgjm, 
	       sB5TTgjp, sB5TTgjm, sB6TTgjp, sB6TTgjm, sB7TTgjp, sB7TTgjm, sB8TTgjp, sB8TTgjm);
  }   
  std::vector<double> TTgjQ2 = {Abs(q1TTgj-TTgj), Abs(q2TTgj-TTgj), Abs(q3TTgj-TTgj), Abs(q4TTgj-TTgj), Abs(q6TTgj-TTgj), Abs(q8TTgj-TTgj)};
  double SysTTgjQ2 = *std::max_element(TTgjQ2.begin(), TTgjQ2.end());

  double TTwl=0, eTTwl=0, TTwlp=0, TTwlm=0, TTwlrp=0, TTwlrm=0, TTwlpup=0, TTwlpum=0;
  double q1TTwl=0, q2TTwl=0, q3TTwl=0, q4TTwl=0, q6TTwl=0, q8TTwl=0;
  double sB1TTwlp=0, sB1TTwlm=0, sB2TTwlp=0, sB2TTwlm=0, sB3TTwlp=0, sB3TTwlm=0, sB4TTwlp=0, sB4TTwlm=0, 
    sB5TTwlp=0, sB5TTwlm=0, sB6TTwlp=0, sB6TTwlm=0, sB7TTwlp=0, sB7TTwlm=0, sB8TTwlp=0, sB8TTwlm=0;
  if ( use_TTwl ) { 
    GetYield(Lumi,idm,"TTwl",CoupB,CoupBjp,CoupBjm,CoupBjrp,CoupBjrm, TTwl, eTTwl, TTwlp, TTwlm, TTwlrp, TTwlrm, TTwlpup, TTwlpum); 
    GetSysQ2(Lumi,idm,"TTwl",CoupB, q1TTwl, q2TTwl, q3TTwl, q4TTwl, q6TTwl, q8TTwl);
    GetSysBTAG(Lumi,idm,"TTwl",CoupB, sB1TTwlp, sB1TTwlm, sB2TTwlp, sB2TTwlm, sB3TTwlp, sB3TTwlm, sB4TTwlp, sB4TTwlm, 
	       sB5TTwlp, sB5TTwlm, sB6TTwlp, sB6TTwlm, sB7TTwlp, sB7TTwlm, sB8TTwlp, sB8TTwlm);
  }   
  std::vector<double> TTwlQ2 = {Abs(q1TTwl-TTwl), Abs(q2TTwl-TTwl), Abs(q3TTwl-TTwl), Abs(q4TTwl-TTwl), Abs(q6TTwl-TTwl), Abs(q8TTwl-TTwl)};
  double SysTTwlQ2 = *std::max_element(TTwlQ2.begin(), TTwlQ2.end());

  double TTwqq=0, eTTwqq=0, TTwqqp=0, TTwqqm=0, TTwqqrp=0, TTwqqrm=0, TTwqqpup=0, TTwqqpum=0;
  double q1TTwqq=0, q2TTwqq=0, q3TTwqq=0, q4TTwqq=0, q6TTwqq=0, q8TTwqq=0;
  double sB1TTwqqp=0, sB1TTwqqm=0, sB2TTwqqp=0, sB2TTwqqm=0, sB3TTwqqp=0, sB3TTwqqm=0, sB4TTwqqp=0, sB4TTwqqm=0, 
    sB5TTwqqp=0, sB5TTwqqm=0, sB6TTwqqp=0, sB6TTwqqm=0, sB7TTwqqp=0, sB7TTwqqm=0, sB8TTwqqp=0, sB8TTwqqm=0;
  if ( use_TTwqq ) { 
    GetYield(Lumi,idm,"TTwqq",CoupB,CoupBjp,CoupBjm,CoupBjrp,CoupBjrm, TTwqq, eTTwqq, TTwqqp, TTwqqm, TTwqqrp, TTwqqrm, TTwqqpup, TTwqqpum); 
    GetSysQ2(Lumi,idm,"TTwqq",CoupB, q1TTwqq, q2TTwqq, q3TTwqq, q4TTwqq, q6TTwqq, q8TTwqq);
    GetSysBTAG(Lumi,idm,"TTwqq",CoupB, sB1TTwqqp, sB1TTwqqm, sB2TTwqqp, sB2TTwqqm, sB3TTwqqp, sB3TTwqqm, sB4TTwqqp, sB4TTwqqm, 
	       sB5TTwqqp, sB5TTwqqm, sB6TTwqqp, sB6TTwqqm, sB7TTwqqp, sB7TTwqqm, sB8TTwqqp, sB8TTwqqm);
  }   
  std::vector<double> TTwqqQ2 = {Abs(q1TTwqq-TTwqq), Abs(q2TTwqq-TTwqq), Abs(q3TTwqq-TTwqq), Abs(q4TTwqq-TTwqq), Abs(q6TTwqq-TTwqq), Abs(q8TTwqq-TTwqq)};
  double SysTTwqqQ2 = *std::max_element(TTwqqQ2.begin(), TTwqqQ2.end());

  double TTzl=0, eTTzl=0, TTzlp=0, TTzlm=0, TTzlrp=0, TTzlrm=0, TTzlpup=0, TTzlpum=0;
  double q1TTzl=0, q2TTzl=0, q3TTzl=0, q4TTzl=0, q6TTzl=0, q8TTzl=0;
  double sB1TTzlp=0, sB1TTzlm=0, sB2TTzlp=0, sB2TTzlm=0, sB3TTzlp=0, sB3TTzlm=0, sB4TTzlp=0, sB4TTzlm=0, 
    sB5TTzlp=0, sB5TTzlm=0, sB6TTzlp=0, sB6TTzlm=0, sB7TTzlp=0, sB7TTzlm=0, sB8TTzlp=0, sB8TTzlm=0;
  if ( use_TTzl ) { 
    GetYield(Lumi,idm,"TTzl",CoupB,CoupBjp,CoupBjm,CoupBjrp,CoupBjrm, TTzl, eTTzl, TTzlp, TTzlm, TTzlrp, TTzlrm, TTzlpup, TTzlpum); 
    GetSysQ2(Lumi,idm,"TTzl",CoupB, q1TTzl, q2TTzl, q3TTzl, q4TTzl, q6TTzl, q8TTzl);
    GetSysBTAG(Lumi,idm,"TTzl",CoupB, sB1TTzlp, sB1TTzlm, sB2TTzlp, sB2TTzlm, sB3TTzlp, sB3TTzlm, sB4TTzlp, sB4TTzlm, 
	       sB5TTzlp, sB5TTzlm, sB6TTzlp, sB6TTzlm, sB7TTzlp, sB7TTzlm, sB8TTzlp, sB8TTzlm);
  }   
  std::vector<double> TTzlQ2 = {Abs(q1TTzl-TTzl), Abs(q2TTzl-TTzl), Abs(q3TTzl-TTzl), Abs(q4TTzl-TTzl), Abs(q6TTzl-TTzl), Abs(q8TTzl-TTzl)};
  double SysTTzlQ2 = *std::max_element(TTzlQ2.begin(), TTzlQ2.end());

  double TTzl1m10=0, eTTzl1m10=0, TTzl1m10p=0, TTzl1m10m=0, TTzl1m10rp=0, TTzl1m10rm=0, TTzl1m10pup=0, TTzl1m10pum=0;
  double q1TTzl1m10=0, q2TTzl1m10=0, q3TTzl1m10=0, q4TTzl1m10=0, q6TTzl1m10=0, q8TTzl1m10=0;
  double sB1TTzl1m10p=0, sB1TTzl1m10m=0, sB2TTzl1m10p=0, sB2TTzl1m10m=0, sB3TTzl1m10p=0, sB3TTzl1m10m=0, sB4TTzl1m10p=0, sB4TTzl1m10m=0, 
    sB5TTzl1m10p=0, sB5TTzl1m10m=0, sB6TTzl1m10p=0, sB6TTzl1m10m=0, sB7TTzl1m10p=0, sB7TTzl1m10m=0, sB8TTzl1m10p=0, sB8TTzl1m10m=0;
  if ( use_TTzl1m10 ) { 
    GetYield(Lumi,idm,"TTzl1m10",CoupB,CoupBjp,CoupBjm,CoupBjrp,CoupBjrm, TTzl1m10, eTTzl1m10, TTzl1m10p, TTzl1m10m, TTzl1m10rp, TTzl1m10rm, TTzl1m10pup, TTzl1m10pum); 
    GetSysQ2(Lumi,idm,"TTzl1m10",CoupB, q1TTzl1m10, q2TTzl1m10, q3TTzl1m10, q4TTzl1m10, q6TTzl1m10, q8TTzl1m10);
    GetSysBTAG(Lumi,idm,"TTzl1m10",CoupB, sB1TTzl1m10p, sB1TTzl1m10m, sB2TTzl1m10p, sB2TTzl1m10m, sB3TTzl1m10p, sB3TTzl1m10m, sB4TTzl1m10p, sB4TTzl1m10m, 
	       sB5TTzl1m10p, sB5TTzl1m10m, sB6TTzl1m10p, sB6TTzl1m10m, sB7TTzl1m10p, sB7TTzl1m10m, sB8TTzl1m10p, sB8TTzl1m10m);
  }   
  std::vector<double> TTzl1m10Q2 = {Abs(q1TTzl1m10-TTzl1m10), Abs(q2TTzl1m10-TTzl1m10), Abs(q3TTzl1m10-TTzl1m10), Abs(q4TTzl1m10-TTzl1m10), Abs(q6TTzl1m10-TTzl1m10), Abs(q8TTzl1m10-TTzl1m10)};
  double SysTTzl1m10Q2 = *std::max_element(TTzl1m10Q2.begin(), TTzl1m10Q2.end());

  double TTzqq=0, eTTzqq=0, TTzqqp=0, TTzqqm=0, TTzqqrp=0, TTzqqrm=0, TTzqqpup=0, TTzqqpum=0;
  double q1TTzqq=0, q2TTzqq=0, q3TTzqq=0, q4TTzqq=0, q6TTzqq=0, q8TTzqq=0;
  double sB1TTzqqp=0, sB1TTzqqm=0, sB2TTzqqp=0, sB2TTzqqm=0, sB3TTzqqp=0, sB3TTzqqm=0, sB4TTzqqp=0, sB4TTzqqm=0, 
    sB5TTzqqp=0, sB5TTzqqm=0, sB6TTzqqp=0, sB6TTzqqm=0, sB7TTzqqp=0, sB7TTzqqm=0, sB8TTzqqp=0, sB8TTzqqm=0;
  if ( use_TTzqq ) { 
    GetYield(Lumi,idm,"TTzqq",CoupB,CoupBjp,CoupBjm,CoupBjrp,CoupBjrm, TTzqq, eTTzqq, TTzqqp, TTzqqm, TTzqqrp, TTzqqrm, TTzqqpup, TTzqqpum); 
    GetSysQ2(Lumi,idm,"TTzqq",CoupB, q1TTzqq, q2TTzqq, q3TTzqq, q4TTzqq, q6TTzqq, q8TTzqq);
    GetSysBTAG(Lumi,idm,"TTzqq",CoupB, sB1TTzqqp, sB1TTzqqm, sB2TTzqqp, sB2TTzqqm, sB3TTzqqp, sB3TTzqqm, sB4TTzqqp, sB4TTzqqm, 
	       sB5TTzqqp, sB5TTzqqm, sB6TTzqqp, sB6TTzqqm, sB7TTzqqp, sB7TTzqqm, sB8TTzqqp, sB8TTzqqm);
  }   
  std::vector<double> TTzqqQ2 = {Abs(q1TTzqq-TTzqq), Abs(q2TTzqq-TTzqq), Abs(q3TTzqq-TTzqq), Abs(q4TTzqq-TTzqq), Abs(q6TTzqq-TTzqq), Abs(q8TTzqq-TTzqq)};
  double SysTTzqqQ2 = *std::max_element(TTzqqQ2.begin(), TTzqqQ2.end());

  // Data
  TH1F *mdata = new TH1F("mdata","Data",200,-100,100);
  if(use_Data){
    TH1F *mdem = new TH1F("mdem","Data",200,-100,100);
    TChain l1("bdttree");
    l1.Draw("nVert>>mdem",trgD+Coup);
    mdata->Add(mdem);
    mdata->Sumw2();
    mdata->SetMarkerStyle(8);
  }
  // Backgrounds
  double WjT = W1j+W2j+W3j+W4j+W5j+W6j+W7j;
  double ZiT = Zinv1+Zinv2+Zinv3+Zinv4+Zinv5+Zinv6+Zinv7;
  double VVT = WW+WZ+ZZ;
  double SysVVQ2 = sqrt(pow(SysWWQ2,2)+pow(SysWZQ2,2)+pow(SysZZQ2,2));
  double VVB1p = sB1WWp+sB1WZp+sB1ZZp;
  double VVB1m = sB1WWm+sB1WZm+sB1ZZm;
  double VVB2p = sB2WWp+sB2WZp+sB2ZZp;
  double VVB2m = sB2WWm+sB2WZm+sB2ZZm;
  double VVB3p = sB3WWp+sB3WZp+sB3ZZp;
  double VVB3m = sB3WWm+sB3WZm+sB3ZZm;
  double VVB4p = sB4WWp+sB4WZp+sB4ZZp;
  double VVB4m = sB4WWm+sB4WZm+sB4ZZm;
  double VVB5p = sB5WWp+sB5WZp+sB5ZZp;
  double VVB5m = sB5WWm+sB5WZm+sB5ZZm;
  double VVB6p = sB6WWp+sB6WZp+sB6ZZp;
  double VVB6m = sB6WWm+sB6WZm+sB6ZZm;
  double VVB7p = sB7WWp+sB7WZp+sB7ZZp;
  double VVB7m = sB7WWm+sB7WZm+sB7ZZm;
  double VVB8p = sB8WWp+sB8WZp+sB8ZZp;
  double VVB8m = sB8WWm+sB8WZm+sB8ZZm;
  double VVTpup = WWpup+WZpup+ZZpup;
  double VVTpum = WWpum+WZpum+ZZpum;
  double VVTp = WWp+WZp+ZZp;
  double VVTm = WWm+WZm+ZZm;
  double VVTrp = WWrp+WZrp+ZZrp;
  double VVTrm = WWrm+WZrm+ZZrm;
  double STT = sTw+sTtC+sTBw+sTBtC;
  //  double SysSTTQ2 = sqrt(pow(SyssTwQ2,2)+pow(SyssTtCQ2,2)+pow(SyssTBwQ2,2)+pow(SyssTBtCQ2,2));
  double SysSTTQ2 = sqrt(pow(SyssTwQ2,2)+pow(SyssTBwQ2,2)+pow(SyssTBtCQ2,2));
  double STTpup = sTwpup+sTtCpup+sTBwpup+sTBtCpup;
  double STTpum = sTwpum+sTtCpum+sTBwpum+sTBtCpum;
  double STTp = sTwp+sTtCp+sTBwp+sTBtCp;
  double STTm = sTwm+sTtCm+sTBwm+sTBtCm;
  double STTrp = sTwrp+sTtCrp+sTBwrp+sTBtCrp;
  double STTrm = sTwrm+sTtCrm+sTBwrm+sTBtCrm;
  double STTB1p = sB1Twp+sB1TtCp+sB1TBwp+sB1TBtCp;
  double STTB2p = sB2Twp+sB2TtCp+sB2TBwp+sB2TBtCp;
  double STTB3p = sB3Twp+sB3TtCp+sB3TBwp+sB3TBtCp;
  double STTB4p = sB4Twp+sB4TtCp+sB4TBwp+sB4TBtCp;
  double STTB5p = sB5Twp+sB5TtCp+sB5TBwp+sB5TBtCp;
  double STTB6p = sB6Twp+sB6TtCp+sB6TBwp+sB6TBtCp;
  double STTB7p = sB7Twp+sB7TtCp+sB7TBwp+sB7TBtCp;
  double STTB8p = sB8Twp+sB8TtCp+sB8TBwp+sB8TBtCp;
  double STTB1m = sB1Twm+sB1TtCm+sB1TBwm+sB1TBtCm;
  double STTB2m = sB2Twm+sB2TtCm+sB2TBwm+sB2TBtCm;
  double STTB3m = sB3Twm+sB3TtCm+sB3TBwm+sB3TBtCm;
  double STTB4m = sB4Twm+sB4TtCm+sB4TBwm+sB4TBtCm;
  double STTB5m = sB5Twm+sB5TtCm+sB5TBwm+sB5TBtCm;
  double STTB6m = sB6Twm+sB6TtCm+sB6TBwm+sB6TBtCm;
  double STTB7m = sB7Twm+sB7TtCm+sB7TBwm+sB7TBtCm;
  double STTB8m = sB8Twm+sB8TtCm+sB8TBwm+sB8TBtCm;
  double DYT = DY1+DY2+DY3+DY4+DY5+DY6+DY7+DY8+DY9+DY10+DY11;
  double SysDYTQ2 = sqrt(pow(SysDY1Q2,2)+pow(SysDY2Q2,2)+pow(SysDY3Q2,2)+pow(SysDY4Q2,2)+pow(SysDY5Q2,2)+
			 pow(SysDY6Q2,2)+pow(SysDY7Q2,2)+pow(SysDY8Q2,2)+pow(SysDY9Q2,2)+pow(SysDY10Q2,2)+pow(SysDY11Q2,2));
  double sISR1pDYT = sISR1pDY1+sISR1pDY2+sISR1pDY3+sISR1pDY4+sISR1pDY5+sISR1pDY6+sISR1pDY7+sISR1pDY8+sISR1pDY9+sISR1pDY10+sISR1pDY11;
  double sISR2pDYT = sISR1pDY2+sISR2pDY2+sISR2pDY3+sISR2pDY4+sISR2pDY5+sISR2pDY6+sISR2pDY7+sISR2pDY8+sISR2pDY9+sISR2pDY10+sISR2pDY11;
  double sISR3pDYT = sISR1pDY3+sISR3pDY2+sISR3pDY3+sISR3pDY4+sISR3pDY5+sISR3pDY6+sISR3pDY7+sISR3pDY8+sISR3pDY9+sISR3pDY10+sISR3pDY11;
  double sISR4pDYT = sISR1pDY4+sISR4pDY2+sISR4pDY3+sISR4pDY4+sISR4pDY5+sISR4pDY6+sISR4pDY7+sISR4pDY8+sISR4pDY9+sISR4pDY10+sISR4pDY11;
  double sISR5pDYT = sISR1pDY5+sISR5pDY2+sISR5pDY3+sISR5pDY4+sISR5pDY5+sISR5pDY6+sISR5pDY7+sISR5pDY8+sISR5pDY9+sISR5pDY10+sISR5pDY11;
  double sISR6pDYT = sISR1pDY6+sISR6pDY2+sISR6pDY3+sISR6pDY4+sISR6pDY5+sISR6pDY6+sISR6pDY7+sISR6pDY8+sISR6pDY9+sISR6pDY10+sISR6pDY11;
  double sISR7pDYT = sISR1pDY7+sISR7pDY2+sISR7pDY3+sISR7pDY4+sISR7pDY5+sISR7pDY6+sISR7pDY7+sISR7pDY8+sISR7pDY9+sISR7pDY10+sISR7pDY11;
  double sISR1mDYT = sISR1mDY1+sISR1mDY2+sISR1mDY3+sISR1mDY4+sISR1mDY5+sISR1mDY6+sISR1mDY7+sISR1mDY8+sISR1mDY9+sISR1mDY10+sISR1mDY11;
  double sISR2mDYT = sISR1mDY2+sISR2mDY2+sISR2mDY3+sISR2mDY4+sISR2mDY5+sISR2mDY6+sISR2mDY7+sISR2mDY8+sISR2mDY9+sISR2mDY10+sISR2mDY11;
  double sISR3mDYT = sISR1mDY3+sISR3mDY2+sISR3mDY3+sISR3mDY4+sISR3mDY5+sISR3mDY6+sISR3mDY7+sISR3mDY8+sISR3mDY9+sISR3mDY10+sISR3mDY11;
  double sISR4mDYT = sISR1mDY4+sISR4mDY2+sISR4mDY3+sISR4mDY4+sISR4mDY5+sISR4mDY6+sISR4mDY7+sISR4mDY8+sISR4mDY9+sISR4mDY10+sISR4mDY11;
  double sISR5mDYT = sISR1mDY5+sISR5mDY2+sISR5mDY3+sISR5mDY4+sISR5mDY5+sISR5mDY6+sISR5mDY7+sISR5mDY8+sISR5mDY9+sISR5mDY10+sISR5mDY11;
  double sISR6mDYT = sISR1mDY6+sISR6mDY2+sISR6mDY3+sISR6mDY4+sISR6mDY5+sISR6mDY6+sISR6mDY7+sISR6mDY8+sISR6mDY9+sISR6mDY10+sISR6mDY11;
  double sISR7mDYT = sISR1mDY7+sISR7mDY2+sISR7mDY3+sISR7mDY4+sISR7mDY5+sISR7mDY6+sISR7mDY7+sISR7mDY8+sISR7mDY9+sISR7mDY10+sISR7mDY11;
  double DYTpup = DY1pup+DY2pup+DY3pup+DY4pup+DY5pup+DY6pup+DY7pup+DY8pup+DY9pup+DY10pup+DY11pup;
  double DYTpum = DY1pum+DY2pum+DY3pum+DY4pum+DY5pum+DY6pum+DY7pum+DY8pum+DY9pum+DY10pum+DY11pum;
  double DYTp = DY1p+DY2p+DY3p+DY4p+DY5p+DY6p+DY7p+DY8p+DY9p+DY10p+DY11p;
  double DYTm = DY1m+DY2m+DY3m+DY4m+DY5m+DY6m+DY7m+DY8m+DY9m+DY10m+DY11m;
  double DYTrp = DY1rp+DY2rp+DY3rp+DY4rp+DY5rp+DY6rp+DY7rp+DY8rp+DY9rp+DY10rp+DY11rp;
  double DYTrm = DY1rm+DY2rm+DY3rm+DY4rm+DY5rm+DY6rm+DY7rm+DY8rm+DY9rm+DY10rm+DY11rm;
  double DYB1p = sB1DY1p+sB1DY2p+sB1DY3p+sB1DY4p+sB1DY5p+sB1DY6p+sB1DY7p+sB1DY8p+sB1DY9p+sB1DY10p+sB1DY11p;
  double DYB1m = sB1DY1m+sB1DY2m+sB1DY3m+sB1DY4m+sB1DY5m+sB1DY6m+sB1DY7m+sB1DY8m+sB1DY9m+sB1DY10m+sB1DY11m;
  double DYB2p = sB2DY1p+sB2DY2p+sB2DY3p+sB2DY4p+sB2DY5p+sB2DY6p+sB2DY7p+sB2DY8p+sB2DY9p+sB2DY10p+sB2DY11p;
  double DYB2m = sB2DY1m+sB2DY2m+sB2DY3m+sB2DY4m+sB2DY5m+sB2DY6m+sB2DY7m+sB2DY8m+sB2DY9m+sB2DY10m+sB2DY11m;
  double DYB3p = sB3DY1p+sB3DY2p+sB3DY3p+sB3DY4p+sB3DY5p+sB3DY6p+sB3DY7p+sB3DY8p+sB3DY9p+sB3DY10p+sB3DY11p;
  double DYB3m = sB3DY1m+sB3DY2m+sB3DY3m+sB3DY4m+sB3DY5m+sB3DY6m+sB3DY7m+sB3DY8m+sB3DY9m+sB3DY10m+sB3DY11m;
  double DYB4p = sB4DY1p+sB4DY2p+sB4DY3p+sB4DY4p+sB4DY5p+sB4DY6p+sB4DY7p+sB4DY8p+sB4DY9p+sB4DY10p+sB4DY11p;
  double DYB4m = sB4DY1m+sB4DY2m+sB4DY3m+sB4DY4m+sB4DY5m+sB4DY6m+sB4DY7m+sB4DY8m+sB4DY9m+sB4DY10m+sB4DY11m;
  double DYB5p = sB5DY1p+sB5DY2p+sB5DY3p+sB5DY4p+sB5DY5p+sB5DY6p+sB5DY7p+sB5DY8p+sB5DY9p+sB5DY10p+sB5DY11p;
  double DYB5m = sB5DY1m+sB5DY2m+sB5DY3m+sB5DY4m+sB5DY5m+sB5DY6m+sB5DY7m+sB5DY8m+sB5DY9m+sB5DY10m+sB5DY11m;
  double DYB6p = sB6DY1p+sB6DY2p+sB6DY3p+sB6DY4p+sB6DY5p+sB6DY6p+sB6DY7p+sB6DY8p+sB6DY9p+sB6DY10p+sB6DY11p;
  double DYB6m = sB6DY1m+sB6DY2m+sB6DY3m+sB6DY4m+sB6DY5m+sB6DY6m+sB6DY7m+sB6DY8m+sB6DY9m+sB6DY10m+sB6DY11m;
  double DYB7p = sB7DY1p+sB7DY2p+sB7DY3p+sB7DY4p+sB7DY5p+sB7DY6p+sB7DY7p+sB7DY8p+sB7DY9p+sB7DY10p+sB7DY11p;
  double DYB7m = sB7DY1m+sB7DY2m+sB7DY3m+sB7DY4m+sB7DY5m+sB7DY6m+sB7DY7m+sB7DY8m+sB7DY9m+sB7DY10m+sB7DY11m;
  double DYB8p = sB8DY1p+sB8DY2p+sB8DY3p+sB8DY4p+sB8DY5p+sB8DY6p+sB8DY7p+sB8DY8p+sB8DY9p+sB8DY10p+sB8DY11p;
  double DYB8m = sB8DY1m+sB8DY2m+sB8DY3m+sB8DY4m+sB8DY5m+sB8DY6m+sB8DY7m+sB8DY8m+sB8DY9m+sB8DY10m+sB8DY11m;
  double TTX = TTgj+TTwl+TTwqq+TTzl+TTzl1m10+TTzqq;
  double TTXpup = TTgjpup+TTwlpup+TTwqqpup+TTzlpup+TTzl1m10pup+TTzqqpup;
  double TTXpum = TTgjpum+TTwlpum+TTwqqpum+TTzlpum+TTzl1m10pum+TTzqqpum;
  double TTXp = TTgjp+TTwlp+TTwqqp+TTzlp+TTzl1m10p+TTzqqp;
  double TTXm = TTgjm+TTwlm+TTwqqm+TTzlm+TTzl1m10m+TTzqqm;
  double TTXrp = TTgjrp+TTwlrp+TTwqqrp+TTzlrp+TTzl1m10rp+TTzqqrp;
  double TTXrm = TTgjrm+TTwlrm+TTwqqrm+TTzlrm+TTzl1m10rm+TTzqqrm;
  double SysTTXQ2 = sqrt(pow(SysTTgjQ2,2)+pow(SysTTwlQ2,2)+pow(SysTTwqqQ2,2)+pow(SysTTzlQ2,2)+pow(SysTTzl1m10Q2,2)+pow(SysTTzqqQ2,2));
  double TTXB1p = sB1TTgjp+sB1TTwlp+sB1TTwqqp+sB1TTzlp+sB1TTzl1m10p+sB1TTzqqp;
  double TTXB1m = sB1TTgjm+sB1TTwlm+sB1TTwqqm+sB1TTzlm+sB1TTzl1m10m+sB1TTzqqm;
  double TTXB2p = sB2TTgjp+sB2TTwlp+sB2TTwqqp+sB2TTzlp+sB2TTzl1m10p+sB2TTzqqp;
  double TTXB2m = sB2TTgjm+sB2TTwlm+sB2TTwqqm+sB2TTzlm+sB2TTzl1m10m+sB2TTzqqm;
  double TTXB3p = sB3TTgjp+sB3TTwlp+sB3TTwqqp+sB3TTzlp+sB3TTzl1m10p+sB3TTzqqp;
  double TTXB3m = sB3TTgjm+sB3TTwlm+sB3TTwqqm+sB3TTzlm+sB3TTzl1m10m+sB3TTzqqm;
  double TTXB4p = sB4TTgjp+sB4TTwlp+sB4TTwqqp+sB4TTzlp+sB4TTzl1m10p+sB4TTzqqp;
  double TTXB4m = sB4TTgjm+sB4TTwlm+sB4TTwqqm+sB4TTzlm+sB4TTzl1m10m+sB4TTzqqm;
  double TTXB5p = sB5TTgjp+sB5TTwlp+sB5TTwqqp+sB5TTzlp+sB5TTzl1m10p+sB5TTzqqp;
  double TTXB5m = sB5TTgjm+sB5TTwlm+sB5TTwqqm+sB5TTzlm+sB5TTzl1m10m+sB5TTzqqm;
  double TTXB6p = sB6TTgjp+sB6TTwlp+sB6TTwqqp+sB6TTzlp+sB6TTzl1m10p+sB6TTzqqp;
  double TTXB6m = sB6TTgjm+sB6TTwlm+sB6TTwqqm+sB6TTzlm+sB6TTzl1m10m+sB6TTzqqm;
  double TTXB7p = sB7TTgjp+sB7TTwlp+sB7TTwqqp+sB7TTzlp+sB7TTzl1m10p+sB7TTzqqp;
  double TTXB7m = sB7TTgjm+sB7TTwlm+sB7TTwqqm+sB7TTzlm+sB7TTzl1m10m+sB7TTzqqm;
  double TTXB8p = sB8TTgjp+sB8TTwlp+sB8TTwqqp+sB8TTzlp+sB8TTzl1m10p+sB8TTzqqp;
  double TTXB8m = sB8TTgjm+sB8TTwlm+sB8TTwqqm+sB8TTzlm+sB8TTzl1m10m+sB8TTzqqm;

  double bckg  = DDwj + DDtt + DDfk + VVT + STT + DYT + TTX;

  // Some stat uncertainties
  double eVVT = sqrt(pow(eWW,2)+pow(eWZ,2)+pow(eZZ,2));
  double eSTT = sqrt(pow(sTw,2)+pow(sTtC,2)+pow(sTBw,2)+pow(sTBtC,2));
  double eDYT = sqrt(pow(DY1,2)+pow(DY2,2)+pow(DY3,2)+pow(DY4,2)+pow(DY5,2)+pow(DY6,2)+pow(DY7,2)+pow(DY8,2)+
		     pow(DY9,2)+pow(DY10,2)+pow(DY11,2));
  double eTTX = sqrt(pow(TTgj,2)+pow(TTwl,2)+pow(TTwqq,2)+pow(TTzl,2)+pow(TTzl1m10,2)+pow(TTzqq,2));

  double eBMC = sqrt(pow(eWW,2)+pow(eWZ,2)+pow(eZZ,2)+pow(esTw,2)+pow(esTtC,2)+pow(esTBw,2)+pow(esTBtC,2)+
		     pow(eDY1,2)+pow(eDY2,2)+pow(eDY3,2)+pow(eDY4,2)+pow(eDY5,2)+pow(eDY6,2)+pow(eDY7,2)+pow(eDY8,2)+pow(eDY9,2)+pow(eDY10,2)+pow(eDY11,2)+
		     pow(eTTgj,2)+pow(eTTwl,2)+pow(eTTwqq,2)+pow(eTTzl,2)+pow(eTTzl1m10,2)+pow(eTTzqq,2));
  cout << "Bckg(Other MC) = " << (VVT + STT + DYT + TTX) << " +- " << eBMC << std::endl;
  cout << "VV  = " << VVT << " +- " << eVVT << std::endl;
  cout << "ST  = " << STT << " +- " << eSTT << std::endl;
  cout << "DY  = " << DYT << " +- " << eDYT << std::endl;
  cout << "TTX = " << TTX << " +- " << eTTX << std::endl;
  //  cout << "SysSTTQ2 = " << SyssTwQ2 << " " << SyssTtCQ2 << " " << SyssTBwQ2 << " " << SyssTBtCQ2 << std::endl;

  // Signal point to cover
  string Spt = " ";

  for (int i=0; i<dmlines.size()-1; i++){ 

    Spt = dmlines[i];
    double SIGNAL=0, eSIGNAL=0, SIGNALp=0, SIGNALm=0, SIGNALrp=0, SIGNALrm=0, SIGNALpup=0, SIGNALpum=0;
    double sISR1pS=0, sISR2pS=0, sISR3pS=0, sISR4pS=0, sISR5pS=0, sISR6pS=0, sISR7pS=0;
    double sISR1mS=0, sISR2mS=0, sISR3mS=0, sISR4mS=0, sISR5mS=0, sISR6mS=0, sISR7mS=0;
    double SIGNALidp=0, SIGNALidm=0, SIGNALisop=0, SIGNALisom=0;
    double SIGNALfsidp=0, SIGNALfsidm=0, SIGNALfshiipp=0, SIGNALfshiipm=0;
    double SIGNALq1=0, SIGNALq2=0, SIGNALq3=0, SIGNALq4=0, SIGNALq6=0, SIGNALq8=0;
    double sB1SGNp=0, sB1SGNm=0, sB2SGNp=0, sB2SGNm=0, sB3SGNp=0, sB3SGNm=0, sB4SGNp=0, sB4SGNm=0, 
      sB5SGNp=0, sB5SGNm=0, sB6SGNp=0, sB6SGNm=0, sB7SGNp=0, sB7SGNm=0, sB8SGNp=0, sB8SGNm=0;
   if ( use_SIGNAL ) { 
      GetYield(Lumi,idm,Spt,Coup,Coupjp,Coupjm,Coupjrp,Coupjrm, SIGNAL, eSIGNAL, SIGNALp, SIGNALm, SIGNALrp, SIGNALrm, SIGNALpup, SIGNALpum); 
      GetSysISR(Lumi,idm,Spt,Coup,
		sISR1pS,sISR2pS,sISR3pS,sISR4pS,sISR5pS,sISR6pS,sISR7pS,
		sISR1mS,sISR2mS,sISR3mS,sISR4mS,sISR5mS,sISR6mS,sISR7mS); 
      GetSysIDISO(Lumi,idm,Spt,Coup, SIGNALidp, SIGNALidm, SIGNALisop, SIGNALisom); 
      GetSysFastSim(Lumi,idm,Spt,Coup, SIGNALfsidp, SIGNALfsidm, SIGNALfshiipp, SIGNALfshiipm); 
      GetSysQ2(Lumi,idm,Spt,Coup, SIGNALq1, SIGNALq2, SIGNALq3, SIGNALq4, SIGNALq6, SIGNALq8); 
      GetSysBTAG(Lumi,idm,Spt,Coup, sB1SGNp, sB1SGNm, sB2SGNp, sB2SGNm, sB3SGNp, sB3SGNm, sB4SGNp, sB4SGNm, 
		 sB5SGNp, sB5SGNm, sB6SGNp, sB6SGNm, sB7SGNp, sB7SGNm, sB8SGNp, sB8SGNm);
    }
    cout << "Signal point: " << Spt << std::endl;
    double SIGNALfsp = sqrt( pow((SIGNALfsidp-SIGNAL)/SIGNAL,2) + pow((SIGNALfshiipp-SIGNAL)/SIGNAL,2) + pow(0.02,2));
    double SIGNALfsm = -1. * sqrt( pow((SIGNALfsidm-SIGNAL)/SIGNAL,2) + pow((SIGNALfshiipm-SIGNAL)/SIGNAL,2) + pow(0.02,2));
    std::vector<double> SQ2 = {Abs(SIGNALq1-SIGNAL), Abs(SIGNALq2-SIGNAL), Abs(SIGNALq3-SIGNAL), 
			       Abs(SIGNALq4-SIGNAL), Abs(SIGNALq6-SIGNAL), Abs(SIGNALq8-SIGNAL)};
    double SgnQ2 = *std::max_element(SQ2.begin(), SQ2.end());

    // Dirty fix
    double STTsta=1., STTsysq2=1., STTsysJSp=1., STTsysJSm=1., STTsysJRp=1., STTsysJRm=1.;
    double STTsysB1p=1.,STTsysB2p=1.,STTsysB3p=1.,STTsysB4p=1.,STTsysB5p=1.,STTsysB6p=1.,STTsysB7p=1.,STTsysB8p=1.;
    double STTsysB1m=1.,STTsysB2m=1.,STTsysB3m=1.,STTsysB4m=1.,STTsysB5m=1.,STTsysB6m=1.,STTsysB7m=1.,STTsysB8m=1.;
    if (STT!=0.) {
      STTsta=eSTT/STT;
      STTsysq2=SysSTTQ2/STT;
      STTsysB1p=STTB1p/STT;
      STTsysB2p=STTB2p/STT;
      STTsysB3p=STTB3p/STT;
      STTsysB4p=STTB4p/STT;
      STTsysB5p=STTB5p/STT;
      STTsysB6p=STTB6p/STT;
      STTsysB7p=STTB7p/STT;
      STTsysB8p=STTB8p/STT;
      STTsysB1m=STTB1m/STT;
      STTsysB2m=STTB2m/STT;
      STTsysB3m=STTB3m/STT;
      STTsysB4m=STTB4m/STT;
      STTsysB5m=STTB5m/STT;
      STTsysB6m=STTB6m/STT;
      STTsysB7m=STTB7m/STT;
      STTsysB8m=STTB8m/STT;
      STTsysJSp = STTp/STT;
      STTsysJSm = STTm/STT;
      STTsysJRp = STTrp/STT;
      STTsysJRm = STTrm/STT;
    }

   // Create & write the DataCard file
    ofstream outFile;
    outFile.open(("DataCards/" + Spt + ".txt").c_str());
    outFile << "imax 1 number of channels" << std::endl;
    outFile << "jmax * number of backgrounds" << std::endl;
    outFile << "jmax * number of backgrounds" << std::endl;
    outFile << "----------------------------" << std::endl;
    Spt = "B_" + Spt;
    outFile << "bin " << Spt << "_SR" << std::endl;
    outFile << "observation " << DATA << std::endl;
    //    outFile << "observation " << bckg << std::endl;
    outFile << "----------------------------" << std::endl;
    outFile << "bin " << Spt << "_SR " << Spt << "_SR " << Spt << "_SR " << Spt << "_SR " << Spt << "_SR " << Spt << "_SR " << Spt << "_SR " << Spt << "_SR " << std::endl;
    outFile << "process Sgn Wj tt Fake VV ST DY TTX" << std::endl;
    outFile << "process 0 1 2 3 4 5 6 7" << std::endl;
    outFile << "rate   " << SIGNAL << " " << DDwj << " " << DDtt << " " << DDfk << " " << VVT << " " << STT << " " << DYT << " " << TTX << std::endl;
    outFile << "Sst   lnN " << 1.+(eSIGNAL/SIGNAL) << " - - - - - - -" << std::endl;
    outFile << "FastS lnN " << 1.+SIGNALfsp << "/" << 1.+SIGNALfsm << " - - - - - - -" << std::endl;
    outFile << "Wsy   lnN - " << 1.+(eDDwj/DDwj) << " - - - - - -" << std::endl;
    outFile << "ttsy  lnN - - " << 1.+(eDDtt/DDtt) << " - - - - -" << std::endl;
    outFile << "Fksy  lnN - - - " << 1.+(eDDfk/DDfk) << " - - - -" << std::endl;
    outFile << "VVsy  lnN - " << (1.-(VVtoWJ/DDwj)) << " " << (1.-(VVtoTT/DDtt)) << " " << (1.-(VVtoFK/DDfk)) << " 1.5 - - -" << std::endl;
    outFile << "VVst  lnN - - - - " << 1.+(eVVT/VVT) << " - - -" << std::endl;
    outFile << "STsy  lnN - " << (1.-(STtoWJ/DDwj)) << " " << (1.-(STtoTT/DDtt)) << " " << (1.-(STtoFK/DDfk)) << " - 1.5 - -" << std::endl;
    outFile << "STst  lnN - - - - - " << 1.+STTsta << " - -" << std::endl;
    outFile << "DYsy  lnN - " << (1.-(DYtoWJ/DDwj)) << " " << (1.-(DYtoTT/DDtt)) << " " << (1.-(DYtoFK/DDfk)) << " - - 1.5 -" << std::endl;
    outFile << "DYst  lnN - - - - - - " << 1.+(eDYT/DYT) << " -" << std::endl;
    outFile << "TTXsy lnN - " << (1.-(TTXtoWJ/DDwj)) << " " << (1.-(TTXtoTT/DDtt)) << " " << (1.-(TTXtoFK/DDfk)) << " - - - 1.5" << std::endl;
    outFile << "TTXst lnN - - - - - - - " << 1.+(eTTX/TTX) << std::endl;
    outFile << "Lum   lnN 1.025 - - - 1.025 1.025 1.025 1.025" << std::endl;
    outFile << "Trg   lnN 1.010 - - - 1.010 1.010 1.010 1.010" << std::endl;
    outFile << "PU    lnN 1.01 - - - 1.01 1.01 1.01 1.01" << std::endl;
    /*outFile << "PU    lnN " << (SIGNALpup/SIGNAL) << "/" << (SIGNALpum/SIGNAL) << " - - - " << (VVTpup/VVT) << "/" << (VVTpum/VVT) 
	    << " " << (STTpup/STT) << "/" << (STTpum/STT) << " " << (DYTpup/DYT) << "/" << (DYTpum/DYT) << " " << (TTXpup/TTX) 
	    << "/" << (TTXpum/TTX) << std::endl;*/
    outFile << "Q2    lnN " << 1.+(SgnQ2/SIGNAL) << " - - - " <<  1.+(SysVVQ2/VVT) << " " << 1.+STTsysq2 << " " << 1.+(SysDYTQ2/DYT) << " " << 1.+(SysTTXQ2/TTX) << std::endl;

    outFile << "JES   lnN " << (SIGNALp/SIGNAL) << "/" << (SIGNALm/SIGNAL) << " - - - " << (VVTp/VVT) << "/" << (VVTm/VVT) << " " << STTsysJSp << "/" << STTsysJSm << " " << (DYTp/DYT) << "/" << (DYTm/DYT) << " " << (TTXp/TTX) << "/" << (TTXm/TTX) << std::endl;

    outFile << "JER   lnN " << (SIGNALrp/SIGNAL) << "/" << (SIGNALrm/SIGNAL) << " - - - " << (VVTrp/VVT) << "/" << (VVTrm/VVT) << " " << STTsysJRp << "/" << STTsysJRm << " " << (DYTrp/DYT) << "/" << (DYTrm/DYT) << " " << (TTXrp/TTX) << "/" << (TTXrm/TTX) << std::endl;

    outFile << "BTAG1 lnN " << (sB1SGNp/SIGNAL) << "/" << (sB1SGNm/SIGNAL) << " - - - " << (VVB1p/VVT) << "/" << (VVB1m/VVT) << " " << STTsysB1p << "/" << STTsysB1m << " " << (DYB1p/DYT) << "/" << (DYB1m/DYT) << " " << (TTXB1p/TTX) << "/" << (TTXB1m/TTX) << std::endl;
    outFile << "BTAG2 lnN " << (sB2SGNp/SIGNAL) << "/" << (sB2SGNm/SIGNAL) << " - - - " << (VVB2p/VVT) << "/" << (VVB2m/VVT) << " " << STTsysB2p << "/" << STTsysB2m << " " << (DYB2p/DYT) << "/" << (DYB2m/DYT) << " " << (TTXB2p/TTX) << "/" << (TTXB2m/TTX) << std::endl;
    outFile << "BTAG3 lnN " << (sB3SGNp/SIGNAL) << "/" << (sB3SGNm/SIGNAL) << " - - - " << (VVB3p/VVT) << "/" << (VVB3m/VVT) << " " << STTsysB3p << "/" << STTsysB3m << " " << (DYB3p/DYT) << "/" << (DYB3m/DYT) << " " << (TTXB3p/TTX) << "/" << (TTXB3m/TTX) << std::endl;
    outFile << "BTAG4 lnN " << (sB4SGNp/SIGNAL) << "/" << (sB4SGNm/SIGNAL) << " - - - " << (VVB4p/VVT) << "/" << (VVB4m/VVT) << " " << STTsysB4p << "/" << STTsysB4m << " " << (DYB4p/DYT) << "/" << (DYB4m/DYT) << " " << (TTXB4p/TTX) << "/" << (TTXB4m/TTX) << std::endl;
    outFile << "BTAG5 lnN " << (sB5SGNp/SIGNAL) << "/" << (sB5SGNm/SIGNAL) << " - - - " << (VVB5p/VVT) << "/" << (VVB5m/VVT) << " " << STTsysB5p << "/" << STTsysB5m << " " << (DYB5p/DYT) << "/" << (DYB5m/DYT) << " " << (TTXB5p/TTX) << "/" << (TTXB5m/TTX) << std::endl;
    outFile << "BTAG6 lnN " << (sB6SGNp/SIGNAL) << "/" << (sB6SGNm/SIGNAL) << " - - - " << (VVB6p/VVT) << "/" << (VVB6m/VVT) << " " << STTsysB6p << "/" << STTsysB6m << " " << (DYB6p/DYT) << "/" << (DYB6m/DYT) << " " << (TTXB6p/TTX) << "/" << (TTXB6m/TTX) << std::endl;
    outFile << "BTAG7 lnN " << (sB7SGNp/SIGNAL) << "/" << (sB7SGNm/SIGNAL) << " - - - " << (VVB7p/VVT) << "/" << (VVB7m/VVT) << " " << STTsysB7p << "/" << STTsysB7m << " " << (DYB7p/DYT) << "/" << (DYB7m/DYT) << " " << (TTXB7p/TTX) << "/" << (TTXB7m/TTX) << std::endl;
    outFile << "BTAG8 lnN " << (sB8SGNp/SIGNAL) << "/" << (sB8SGNm/SIGNAL) << " - - - " << (VVB8p/VVT) << "/" << (VVB8m/VVT) << " " << STTsysB8p << "/" << STTsysB8m << " " << (DYB8p/DYT) << "/" << (DYB8m/DYT) << " " << (TTXB8p/TTX) << "/" << (TTXB8m/TTX) << std::endl;
    outFile << "ID    lnN " << (SIGNALidp/SIGNAL)  << "/" << (SIGNALidm/SIGNAL) <<  " - - - 1.01 1.01 1.01 1.01" << std::endl;
    outFile << "ISO   lnN " << (SIGNALisop/SIGNAL) << "/" << (SIGNALisom/SIGNAL) << " - - - 1.01 1.01 1.01 1.01" << std::endl;

    outFile << "ISR1  lnN " << (sISR1pS/SIGNAL) << "/" << (sISR1mS/SIGNAL) << " " << ((DDwj+ISRwj1u)/DDwj) << "/" << ((DDwj+ISRwj1d)/DDwj) << " " << ((DDtt+ISRtt1u)/DDtt) << "/" << ((DDtt+ISRtt1d)/DDtt) << " " << ((DDfk+ISRfk1u)/DDfk) << "/" << ((DDfk+ISRfk1d)/DDfk) << " - - - -" << std::endl;
    outFile << "ISR2  lnN " << (sISR2pS/SIGNAL) << "/" << (sISR2mS/SIGNAL) << " " << ((DDwj+ISRwj2u)/DDwj) << "/" << ((DDwj+ISRwj2d)/DDwj) << " " << ((DDtt+ISRtt2u)/DDtt) << "/" << ((DDtt+ISRtt2d)/DDtt) << " " << ((DDfk+ISRfk2u)/DDfk) << "/" << ((DDfk+ISRfk2d)/DDfk) << " - - - -" << std::endl;
    outFile << "ISR3  lnN " << (sISR3pS/SIGNAL) << "/" << (sISR3mS/SIGNAL) << " " << ((DDwj+ISRwj3u)/DDwj) << "/" << ((DDwj+ISRwj3d)/DDwj) << " " << ((DDtt+ISRtt3u)/DDtt) << "/" << ((DDtt+ISRtt3d)/DDtt) << " " << ((DDfk+ISRfk3u)/DDfk) << "/" << ((DDfk+ISRfk3d)/DDfk) << " - - - -" << std::endl;
    outFile << "ISR4  lnN " << (sISR4pS/SIGNAL) << "/" << (sISR4mS/SIGNAL) << " " << ((DDwj+ISRwj4u)/DDwj) << "/" << ((DDwj+ISRwj4d)/DDwj) << " " << ((DDtt+ISRtt4u)/DDtt) << "/" << ((DDtt+ISRtt4d)/DDtt) << " " << ((DDfk+ISRfk4u)/DDfk) << "/" << ((DDfk+ISRfk4d)/DDfk) << " - - - -" << std::endl;
    outFile << "ISR5  lnN " << (sISR5pS/SIGNAL) << "/" << (sISR5mS/SIGNAL) << " " << ((DDwj+ISRwj5u)/DDwj) << "/" << ((DDwj+ISRwj5d)/DDwj) << " " << ((DDtt+ISRtt5u)/DDtt) << "/" << ((DDtt+ISRtt5d)/DDtt) << " " << ((DDfk+ISRfk5u)/DDfk) << "/" << ((DDfk+ISRfk5d)/DDfk) << " - - - -" << std::endl;
    outFile << "ISR6  lnN " << (sISR6pS/SIGNAL) << "/" << (sISR6mS/SIGNAL) << " " << ((DDwj+ISRwj6u)/DDwj) << "/" << ((DDwj+ISRwj6d)/DDwj) << " " << ((DDtt+ISRtt6u)/DDtt) << "/" << ((DDtt+ISRtt6d)/DDtt) << " " << ((DDfk+ISRfk6u)/DDfk) << "/" << ((DDfk+ISRfk6d)/DDfk) << " - - - -" << std::endl;

    outFile << "EISR1 lnN " << "- " << ((DDwj+EISRwj1u)/DDwj) << "/" << ((DDwj+EISRwj1d)/DDwj) << " " << ((DDtt+EISRtt1u)/DDtt) << "/" << ((DDtt+EISRtt1d)/DDtt) << " " << ((DDfk+EISRfk1u)/DDfk) << "/" << ((DDfk+EISRfk1d)/DDfk) << " - - " << (sISR1pDYT/DYT) << "/" << (sISR1mDYT/DYT) << " -" << std::endl;
    outFile << "EISR2 lnN " << "- " << ((DDwj+EISRwj2u)/DDwj) << "/" << ((DDwj+EISRwj2d)/DDwj) << " " << ((DDtt+EISRtt2u)/DDtt) << "/" << ((DDtt+EISRtt2d)/DDtt) << " " << ((DDfk+EISRfk2u)/DDfk) << "/" << ((DDfk+EISRfk2d)/DDfk) << " - - " << (sISR2pDYT/DYT) << "/" << (sISR2mDYT/DYT) << " -" << std::endl;
    outFile << "EISR3 lnN " << "- " << ((DDwj+EISRwj3u)/DDwj) << "/" << ((DDwj+EISRwj3d)/DDwj) << " " << ((DDtt+EISRtt3u)/DDtt) << "/" << ((DDtt+EISRtt3d)/DDtt) << " " << ((DDfk+EISRfk3u)/DDfk) << "/" << ((DDfk+EISRfk3d)/DDfk) << " - - " << (sISR3pDYT/DYT) << "/" << (sISR3mDYT/DYT) << " -" << std::endl;
    outFile << "EISR4 lnN " << "- " << ((DDwj+EISRwj4u)/DDwj) << "/" << ((DDwj+EISRwj4d)/DDwj) << " " << ((DDtt+EISRtt4u)/DDtt) << "/" << ((DDtt+EISRtt4d)/DDtt) << " " << ((DDfk+EISRfk4u)/DDfk) << "/" << ((DDfk+EISRfk4d)/DDfk) << " - - " << (sISR4pDYT/DYT) << "/" << (sISR4mDYT/DYT) << " -" << std::endl;
    outFile << "EISR5 lnN " << "- " << ((DDwj+EISRwj5u)/DDwj) << "/" << ((DDwj+EISRwj5d)/DDwj) << " " << ((DDtt+EISRtt5u)/DDtt) << "/" << ((DDtt+EISRtt5d)/DDtt) << " " << ((DDfk+EISRfk5u)/DDfk) << "/" << ((DDfk+EISRfk5d)/DDfk) << " - - " << (sISR5pDYT/DYT) << "/" << (sISR5mDYT/DYT) << " -" << std::endl;
    outFile << "EISR6 lnN " << "- " << ((DDwj+EISRwj6u)/DDwj) << "/" << ((DDwj+EISRwj6d)/DDwj) << " " << ((DDtt+EISRtt6u)/DDtt) << "/" << ((DDtt+EISRtt6d)/DDtt) << " " << ((DDfk+EISRfk6u)/DDfk) << "/" << ((DDfk+EISRfk6d)/DDfk) << " - - " << (sISR6pDYT/DYT) << "/" << (sISR6mDYT/DYT) << " -" << std::endl;
    outFile << "EISR7 lnN " << "- " << ((DDwj+EISRwj7u)/DDwj) << "/" << ((DDwj+EISRwj7d)/DDwj) << " " << ((DDtt+EISRtt7u)/DDtt) << "/" << ((DDtt+EISRtt7d)/DDtt) << " " << ((DDfk+EISRfk7u)/DDfk) << "/" << ((DDfk+EISRfk7d)/DDfk) << " - - " << (sISR7pDYT/DYT) << "/" << (sISR7mDYT/DYT) << " -" << std::endl;
    outFile << "" << std::endl;

  }

  delete mdata;

}


void GetYield(
	      double lumi,     // Input: Luminosity
	      string dm,
	      string pc,       // Input: Name of the channel
	      TCut Coupure,    // Input: Analysis cut
	      TCut Coupurejp,  // Input: +1sigma(JES) analysis cut
	      TCut Coupurejm,  // Input: -1sigma(JES) analysis cut
	      TCut Coupurejrp,  // Input: +1sigma(JER) analysis cut
	      TCut Coupurejrm,  // Input: -1sigma(JER) analysis cut
	      double& Yield,   // Output: Weighted yield of the channel
	      double& eYield,  // Output: Error on the weighted yield
	      double& YieldP,  // Output: Weighted yield of the channel +1sigma(JES)
	      double& YieldM,  // Output: Weighted yield of the channel -1sigma(JES)
	      double& YieldRP,  // Output: Weighted yield of the channel +1sigma(JER)
	      double& YieldRM,  // Output: Weighted yield of the channel -1sigma(JER)
	      double& YPUP,    // Output: Weighted yield of the channel +1sigma(PU)
	      double& YPUM     // Output: Weighted yield of the channel -1sigma(PU)
	      ){ 

  TChain ProcFile("bdttree");
  // Directory of roottuples
  string BaseDir = "/lstore/cms/cbeiraod/Stop4Body/nTuples_v2017-10-19_test_bdt" + dm + "/";
  string FFile = " ";

  // Files to read
  if (pc==pc){     FFile = BaseDir + "T2DegStop_" + pc + "_bdt.root";  }

  if (pc=="TTbar"){     FFile = BaseDir + "TT_pow_bdt.root";  }
  if (pc=="W1Jets"){     FFile = BaseDir + "Wjets_100to200_bdt.root";  }
  if (pc=="W2Jets"){     FFile = BaseDir + "Wjets_200to400_bdt.root";  }
  if (pc=="W3Jets"){     FFile = BaseDir + "Wjets_400to600_bdt.root";  }
  if (pc=="W4Jets"){     FFile = BaseDir + "Wjets_600to800_bdt.root";  }
  if (pc=="W5Jets"){     FFile = BaseDir + "Wjets_800to1200_bdt.root";  }
  if (pc=="W6Jets"){     FFile = BaseDir + "Wjets_1200to2500_bdt.root";  }
  if (pc=="W7Jets"){     FFile = BaseDir + "Wjets_2500toInf_bdt.root";  }
  if (pc=="Zinv1"){     FFile = BaseDir + "ZJetsToNuNu_HT100to200_bdt.root";  }
  if (pc=="Zinv2"){     FFile = BaseDir + "ZJetsToNuNu_HT200to400_bdt.root";  }
  if (pc=="Zinv3"){     FFile = BaseDir + "ZJetsToNuNu_HT400to600_bdt.root";  }
  if (pc=="Zinv4"){     FFile = BaseDir + "ZJetsToNuNu_HT600to800_bdt.root";  }
  if (pc=="Zinv5"){     FFile = BaseDir + "ZJetsToNuNu_HT800to1200_bdt.root";  }
  if (pc=="Zinv6"){     FFile = BaseDir + "ZJetsToNuNu_HT1200to2500_bdt.root";  }
  if (pc=="Zinv7"){     FFile = BaseDir + "ZJetsToNuNu_HT2500toInf_bdt.root";  }

  if (pc=="DY1"){    FFile = BaseDir + "DYJetsToLL_M50_HT100to200_bdt.root";  }
  if (pc=="DY2"){    FFile = BaseDir + "DYJetsToLL_M50_HT200to400_bdt.root";  }
  if (pc=="DY3"){     FFile = BaseDir + "DYJetsToLL_M50_HT400to600_bdt.root";  }
  if (pc=="DY4"){    FFile = BaseDir + "DYJetsToLL_M50_HT600to800_bdt.root";  }
  if (pc=="DY5"){    FFile = BaseDir + "DYJetsToLL_M50_HT800to1200_bdt.root";  }
  if (pc=="DY6"){     FFile = BaseDir + "DYJetsToLL_M50_HT1200to2500_bdt.root";  }
  if (pc=="DY7"){     FFile = BaseDir + "DYJetsToLL_M50_HT2500toInf_bdt.root";  }
  if (pc=="DY8"){     FFile = BaseDir + "DYJetsToLL_M5to50_HT100to200_bdt.root";  }
  if (pc=="DY9"){     FFile = BaseDir + "DYJetsToLL_M5to50_HT200to400_bdt.root";  }
  if (pc=="DY10"){     FFile = BaseDir + "DYJetsToLL_M5to50_HT400to600_bdt.root";  }
  if (pc=="DY11"){     FFile = BaseDir + "DYJetsToLL_M5to50_HT600toInf_bdt.root";  }

  if (pc=="WW"){     FFile = BaseDir + "WW_bdt.root";  }
  if (pc=="WZ"){     FFile = BaseDir + "WZ_bdt.root";  }
  if (pc=="ZZ"){     FFile = BaseDir + "ZZ_bdt.root";  }

  if (pc=="SingleToptW"){     FFile = BaseDir + "T_tWch_ext_bdt.root";  }
  if (pc=="SingleToptChannel"){     FFile = BaseDir + "T_tch_powheg_bdt.root";  }
  if (pc=="SingleTopBtW"){     FFile = BaseDir + "TBar_tWch_ext_bdt.root";  }
  if (pc=="SingleTopBtChannel"){     FFile = BaseDir + "TBar_tch_powheg_bdt.root";  }

  if (pc=="TTgj"){     FFile = BaseDir + "TTGJets_bdt.root";  }
  if (pc=="TTwl"){     FFile = BaseDir + "TTWToLNu_bdt.root";  }
  if (pc=="TTwqq"){     FFile = BaseDir + "TTWToQQ_bdt.root";  }
  if (pc=="TTzl"){     FFile = BaseDir + "TTZToLLNuNu_bdt.root";  }
  if (pc=="TTzl1m10"){     FFile = BaseDir + "TTZToLLNuNu_m1to10_bdt.root";  }
  if (pc=="TTzqq"){     FFile = BaseDir + "TTZToQQ_bdt.root";  }

  ProcFile.Add((FFile).c_str());
  
  // Get the weighted numerator, where division by denominator has been done
  TH1D *nbcut = new TH1D("nbcut","nbcut",200,-100,100);
  nbcut->Sumw2();
  TH1D *nbcutjp = new TH1D("nbcutjp","nbcutjp",200,-100,100);
  TH1D *nbcutjm = new TH1D("nbcutjm","nbcutjm",200,-100,100);
  TH1D *nbcutjrp = new TH1D("nbcutjrp","nbcutjrp",200,-100,100);
  TH1D *nbcutjrm = new TH1D("nbcutjrm","nbcutjrm",200,-100,100);
  TH1D *nbcutpup = new TH1D("nbcutpup","nbcutpup",200,-100,100);
  TH1D *nbcutpum = new TH1D("nbcutpum","nbcutpum",200,-100,100);
  ProcFile.Draw("Njet>>nbcut","weight*splitFactor"*Coupure);
  ProcFile.Draw("Njet>>nbcutjp","weight_JES_Up*splitFactor"*Coupurejp);
  ProcFile.Draw("Njet>>nbcutjm","weight_JES_Down*splitFactor"*Coupurejm);
  ProcFile.Draw("Njet>>nbcutjrp","weight_JER_Up*splitFactor"*Coupurejrp);
  ProcFile.Draw("Njet>>nbcutjrm","weight_JER_Down*splitFactor"*Coupurejrm);
  ProcFile.Draw("Njet>>nbcutpup","weight_PU_Up*splitFactor"*Coupure);
  ProcFile.Draw("Njet>>nbcutpum","weight_PU_Down*splitFactor"*Coupure);
  //
  double NumWeight = 0., NumErr2 = 0.;
  double NumWeightP = 0., NumWeightM = 0.;
  double NumWeightRP = 0., NumWeightRM = 0.;
  double NumWeightPUP = 0., NumWeightPUM = 0.;
  for(int i=0; i<=(nbcut->GetNbinsX()+1); i++){ 
    NumWeight += nbcut->GetBinContent(i); 
    double NumErr = nbcut->GetBinError(i);
    NumErr2 += (NumErr*NumErr);
    NumWeightPUP += nbcutpup->GetBinContent(i);
    NumWeightPUM += nbcutpum->GetBinContent(i);
  }
  for(int i=0; i<=(nbcutjp->GetNbinsX()+1); i++){ 
    NumWeightP += nbcutjp->GetBinContent(i); 
    NumWeightRP += nbcutjrp->GetBinContent(i); 
  }
  for(int i=0; i<=(nbcutjm->GetNbinsX()+1); i++){ 
    NumWeightM += nbcutjm->GetBinContent(i); 
    NumWeightRM += nbcutjrm->GetBinContent(i); 
  }
  // Calculate yields 
  Yield  = lumi*NumWeight;
  eYield = lumi*sqrt(NumErr2);
  if (Yield==0.){eYield=0.;}
  YieldP = lumi*NumWeightP;
  YieldM = lumi*NumWeightM;
  YieldRP = lumi*NumWeightRP;
  YieldRM = lumi*NumWeightRM;
  YPUP   = lumi*NumWeightPUP;
  YPUM   = lumi*NumWeightPUM;

  // Delete histograms
  delete nbcut;
  delete nbcutjp;
  delete nbcutjm;
  delete nbcutjrp;
  delete nbcutjrm;
  delete nbcutpup;
  delete nbcutpum;

}


void GetSysISR(
		  double lumi,     // Input: Luminosity
		  string dm,
		  string pc,       // Input: Name of the channel
		  TCut Coupure,    // Input: Analysis cut
		  double& YISR1p,  // 
		  double& YISR2p,  // 
		  double& YISR3p,  // 
		  double& YISR4p,  // 
		  double& YISR5p,  // 
		  double& YISR6p,  // 
		  double& YISR7p,  // 
		  double& YISR1m,  // 
		  double& YISR2m,  // 
		  double& YISR3m,  // 
		  double& YISR4m,  // 
		  double& YISR5m,  // 
		  double& YISR6m,  // 
		  double& YISR7m   // 
		  ){ 

  TChain ProcFile("bdttree");
  // Directory of roottuples
  string BaseDir = "/lstore/cms/cbeiraod/Stop4Body/nTuples_v2017-10-19_test_bdt" + dm + "/";
  string FFile = " ";

  // File to read
  // File to read
  if (pc==pc){     FFile = BaseDir + "T2DegStop_" + pc + "_bdt.root";  }

  if (pc=="DY1"){    FFile = BaseDir + "DYJetsToLL_M50_HT100to200_bdt.root";  }
  if (pc=="DY2"){    FFile = BaseDir + "DYJetsToLL_M50_HT200to400_bdt.root";  }
  if (pc=="DY3"){     FFile = BaseDir + "DYJetsToLL_M50_HT400to600_bdt.root";  }
  if (pc=="DY4"){    FFile = BaseDir + "DYJetsToLL_M50_HT600to800_bdt.root";  }
  if (pc=="DY5"){    FFile = BaseDir + "DYJetsToLL_M50_HT800to1200_bdt.root";  }
  if (pc=="DY6"){     FFile = BaseDir + "DYJetsToLL_M50_HT1200to2500_bdt.root";  }
  if (pc=="DY7"){     FFile = BaseDir + "DYJetsToLL_M50_HT2500toInf_bdt.root";  }
  if (pc=="DY8"){     FFile = BaseDir + "DYJetsToLL_M5to50_HT100to200_bdt.root";  }
  if (pc=="DY9"){     FFile = BaseDir + "DYJetsToLL_M5to50_HT200to400_bdt.root";  }
  if (pc=="DY10"){     FFile = BaseDir + "DYJetsToLL_M5to50_HT400to600_bdt.root";  }
  if (pc=="DY11"){     FFile = BaseDir + "DYJetsToLL_M5to50_HT600toInf_bdt.root";  }

  ProcFile.Add((FFile).c_str());
  
  // sysISR
  TH1D *sISR1p = new TH1D("sISR1p","sISR1p",200,-100,100);
  TH1D *sISR2p = new TH1D("sISR2p","sISR1p",200,-100,100);
  TH1D *sISR3p = new TH1D("sISR3p","sISR1p",200,-100,100);
  TH1D *sISR4p = new TH1D("sISR4p","sISR1p",200,-100,100);
  TH1D *sISR5p = new TH1D("sISR5p","sISR1p",200,-100,100);
  TH1D *sISR6p = new TH1D("sISR6p","sISR1p",200,-100,100);
  TH1D *sISR7p = new TH1D("sISR7p","sISR1p",200,-100,100);
  TH1D *sISR1m = new TH1D("sISR1m","sISR1m",200,-100,100);
  TH1D *sISR2m = new TH1D("sISR2m","sISR1m",200,-100,100);
  TH1D *sISR3m = new TH1D("sISR3m","sISR1m",200,-100,100);
  TH1D *sISR4m = new TH1D("sISR4m","sISR1m",200,-100,100);
  TH1D *sISR5m = new TH1D("sISR5m","sISR1m",200,-100,100);
  TH1D *sISR6m = new TH1D("sISR6m","sISR1m",200,-100,100);
  TH1D *sISR7m = new TH1D("sISR7m","sISR1m",200,-100,100);
  if ((pc=="DY1")||(pc=="DY2")||(pc=="DY3")||(pc=="DY4")||(pc=="DY5")||
      (pc=="DY6")||(pc=="DY7")||(pc=="DY8")||(pc=="DY9")||(pc=="DY10")||(pc=="DY11")||
      (pc=="W1Jets")||(pc=="W2Jets")||(pc=="W3Jets")||(pc=="W4Jets")
      ||(pc=="W5Jets")||(pc=="W6Jets")||(pc=="W7Jets")){
    ProcFile.Draw("Njet>>sISR1p","weight_EWKISRweight_Bin1_Up*splitFactor"*Coupure);
    ProcFile.Draw("Njet>>sISR2p","weight_EWKISRweight_Bin2_Up*splitFactor"*Coupure);
    ProcFile.Draw("Njet>>sISR3p","weight_EWKISRweight_Bin3_Up*splitFactor"*Coupure);
    ProcFile.Draw("Njet>>sISR4p","weight_EWKISRweight_Bin4_Up*splitFactor"*Coupure);
    ProcFile.Draw("Njet>>sISR5p","weight_EWKISRweight_Bin5_Up*splitFactor"*Coupure);
    ProcFile.Draw("Njet>>sISR6p","weight_EWKISRweight_Bin6_Up*splitFactor"*Coupure);
    ProcFile.Draw("Njet>>sISR7p","weight_EWKISRweight_Bin7_Up*splitFactor"*Coupure);
    ProcFile.Draw("Njet>>sISR1m","weight_EWKISRweight_Bin1_Down*splitFactor"*Coupure);
    ProcFile.Draw("Njet>>sISR2m","weight_EWKISRweight_Bin2_Down*splitFactor"*Coupure);
    ProcFile.Draw("Njet>>sISR3m","weight_EWKISRweight_Bin3_Down*splitFactor"*Coupure);
    ProcFile.Draw("Njet>>sISR4m","weight_EWKISRweight_Bin4_Down*splitFactor"*Coupure);
    ProcFile.Draw("Njet>>sISR5m","weight_EWKISRweight_Bin5_Down*splitFactor"*Coupure);
    ProcFile.Draw("Njet>>sISR6m","weight_EWKISRweight_Bin6_Down*splitFactor"*Coupure);
    ProcFile.Draw("Njet>>sISR7m","weight_EWKISRweight_Bin7_Down*splitFactor"*Coupure);
  }
  else if ((pc==pc)||(pc=="TTbar")){
    ProcFile.Draw("Njet>>sISR1p","weight_ISRweight_Bin1_Up*splitFactor"*Coupure);
    ProcFile.Draw("Njet>>sISR2p","weight_ISRweight_Bin2_Up*splitFactor"*Coupure);
    ProcFile.Draw("Njet>>sISR3p","weight_ISRweight_Bin3_Up*splitFactor"*Coupure);
    ProcFile.Draw("Njet>>sISR4p","weight_ISRweight_Bin4_Up*splitFactor"*Coupure);
    ProcFile.Draw("Njet>>sISR5p","weight_ISRweight_Bin5_Up*splitFactor"*Coupure);
    ProcFile.Draw("Njet>>sISR6p","weight_ISRweight_Bin6_Up*splitFactor"*Coupure);
    ProcFile.Draw("Njet>>sISR1m","weight_ISRweight_Bin1_Down*splitFactor"*Coupure);
    ProcFile.Draw("Njet>>sISR2m","weight_ISRweight_Bin2_Down*splitFactor"*Coupure);
    ProcFile.Draw("Njet>>sISR3m","weight_ISRweight_Bin3_Down*splitFactor"*Coupure);
    ProcFile.Draw("Njet>>sISR4m","weight_ISRweight_Bin4_Down*splitFactor"*Coupure);
    ProcFile.Draw("Njet>>sISR5m","weight_ISRweight_Bin5_Down*splitFactor"*Coupure);
    ProcFile.Draw("Njet>>sISR6m","weight_ISRweight_Bin6_Down*splitFactor"*Coupure);
  }
  double ISR1p=0,ISR2p=0,ISR3p=0,ISR4p=0,ISR5p=0,ISR6p=0,ISR7p=0;
  double ISR1m=0,ISR2m=0,ISR3m=0,ISR4m=0,ISR5m=0,ISR6m=0,ISR7m=0;
  for(int i=0; i<=(sISR1p->GetNbinsX()+1); i++){ 
    ISR1p += sISR1p->GetBinContent(i);
    ISR2p += sISR2p->GetBinContent(i);
    ISR3p += sISR3p->GetBinContent(i);
    ISR4p += sISR4p->GetBinContent(i);
    ISR5p += sISR5p->GetBinContent(i);
    ISR6p += sISR6p->GetBinContent(i);
    ISR7p += sISR7p->GetBinContent(i);
    ISR1m += sISR1m->GetBinContent(i);
    ISR2m += sISR2m->GetBinContent(i);
    ISR3m += sISR3m->GetBinContent(i);
    ISR4m += sISR4m->GetBinContent(i);
    ISR5m += sISR5m->GetBinContent(i);
    ISR6m += sISR6m->GetBinContent(i);
    ISR7m += sISR7m->GetBinContent(i);
  }
  YISR1p = lumi*ISR1p;
  YISR2p = lumi*ISR2p;
  YISR3p = lumi*ISR3p;
  YISR4p = lumi*ISR4p;
  YISR5p = lumi*ISR5p;
  YISR6p = lumi*ISR6p;
  YISR7p = lumi*ISR7p;
  YISR1m = lumi*ISR1m;
  YISR2m = lumi*ISR2m;
  YISR3m = lumi*ISR3m;
  YISR4m = lumi*ISR4m;
  YISR5m = lumi*ISR5m;
  YISR6m = lumi*ISR6m;
  YISR7m = lumi*ISR7m;

  // Delete histograms
  delete sISR1p;
  delete sISR2p;
  delete sISR3p;
  delete sISR4p;
  delete sISR5p;
  delete sISR6p;
  delete sISR7p;
  delete sISR1m;
  delete sISR2m;
  delete sISR3m;
  delete sISR4m;
  delete sISR5m;
  delete sISR6m;
  delete sISR7m;

}


void GetSysIDISO(
		 double lumi,     // Input: Luminosity
		 string dm,
		 string pc,       // Input: Name of the channel
		 TCut Coupure,    // Input: Analysis cut
		 double& YIDP,    // Output: Weighted yield of the channel +1sigma(ID)
		 double& YIDM,    // Output: Weighted yield of the channel -1sigma(ID)
		 double& YISOP,   // Output: Weighted yield of the channel +1sigma(ISO)
		 double& YISOM    // Output: Weighted yield of the channel -1sigma(ISO)
		 ){ 

  TChain ProcFile("bdttree");
  // Directory of roottuples
  string BaseDir = "/lstore/cms/cbeiraod/Stop4Body/nTuples_v2017-10-19_test_bdt" + dm + "/";
  string FFile = " ";

  // File to read
  if (pc==pc){     FFile = BaseDir + "T2DegStop_" + pc + "_bdt.root";  }

  if (pc=="DY1"){    FFile = BaseDir + "DYJetsToLL_M50_HT100to200_bdt.root";  }
  if (pc=="DY2"){    FFile = BaseDir + "DYJetsToLL_M50_HT200to400_bdt.root";  }
  if (pc=="DY3"){     FFile = BaseDir + "DYJetsToLL_M50_HT400to600_bdt.root";  }
  if (pc=="DY4"){    FFile = BaseDir + "DYJetsToLL_M50_HT600to800_bdt.root";  }
  if (pc=="DY5"){    FFile = BaseDir + "DYJetsToLL_M50_HT800to1200_bdt.root";  }
  if (pc=="DY6"){     FFile = BaseDir + "DYJetsToLL_M50_HT1200to2500_bdt.root";  }
  if (pc=="DY7"){     FFile = BaseDir + "DYJetsToLL_M50_HT2500toInf_bdt.root";  }
  if (pc=="DY8"){     FFile = BaseDir + "DYJetsToLL_M5to50_HT100to200_bdt.root";  }
  if (pc=="DY9"){     FFile = BaseDir + "DYJetsToLL_M5to50_HT200to400_bdt.root";  }
  if (pc=="DY10"){     FFile = BaseDir + "DYJetsToLL_M5to50_HT400to600_bdt.root";  }
  if (pc=="DY11"){     FFile = BaseDir + "DYJetsToLL_M5to50_HT600toInf_bdt.root";  }

  if (pc=="WW"){     FFile = BaseDir + "WW_bdt.root";  }
  if (pc=="WZ"){     FFile = BaseDir + "WZ_bdt.root";  }
  if (pc=="ZZ"){     FFile = BaseDir + "ZZ_bdt.root";  }

  if (pc=="SingleToptW"){     FFile = BaseDir + "T_tWch_ext_bdt.root";  }
  if (pc=="SingleToptChannel"){     FFile = BaseDir + "T_tch_powheg_bdt.root";  }
  if (pc=="SingleTopBtW"){     FFile = BaseDir + "TBar_tWch_ext_bdt.root";  }
  if (pc=="SingleTopBtChannel"){     FFile = BaseDir + "TBar_tch_powheg_bdt.root";  }
  if (pc=="TTgj"){     FFile = BaseDir + "TTGJets_bdt.root";  }
  if (pc=="TTwl"){     FFile = BaseDir + "TTWToLNu_bdt.root";  }
  if (pc=="TTwqq"){     FFile = BaseDir + "TTWToQQ_bdt.root";  }
  if (pc=="TTzl"){     FFile = BaseDir + "TTZToLLNuNu_bdt.root";  }
  if (pc=="TTzl1m10"){     FFile = BaseDir + "TTZToLLNuNu_m1to10_bdt.root";  }
  if (pc=="TTzqq"){     FFile = BaseDir + "TTZToQQ_bdt.root";  }
  ProcFile.Add((FFile).c_str());
  
  // Get the weighted numerator, where division by denominator has been done
  TH1D *nbcutidp = new TH1D("nbcutidp","nbcutidp",200,-100,100);
  TH1D *nbcutidm = new TH1D("nbcutidm","nbcutidm",200,-100,100);
  TH1D *nbcutisop = new TH1D("nbcutisop","nbcutisop",200,-100,100);
  TH1D *nbcutisom = new TH1D("nbcutisom","nbcutisom",200,-100,100);
  ProcFile.Draw("Njet>>nbcutidp","weight_LeptonIDSF_AltCorr_Up*splitFactor"*Coupure);
  ProcFile.Draw("Njet>>nbcutidm","weight_LeptonIDSF_AltCorr_Down*splitFactor"*Coupure);
  ProcFile.Draw("Njet>>nbcutisop","weight_LeptonISOSF_AltCorr_Up*splitFactor"*Coupure);
  ProcFile.Draw("Njet>>nbcutisom","weight_LeptonISOSF_AltCorr_Down*splitFactor"*Coupure);
  //
  double NumWeightIDP = 0., NumWeightIDM = 0., NumWeightISOP = 0., NumWeightISOM = 0.;
  for(int i=0; i<=(nbcutidp->GetNbinsX()+1); i++){ 
    NumWeightIDP += nbcutidp->GetBinContent(i);
    NumWeightIDM += nbcutidm->GetBinContent(i);
    NumWeightISOP += nbcutisop->GetBinContent(i);
    NumWeightISOM += nbcutisom->GetBinContent(i);
  }
  // Calculate yields 
  YIDP   = lumi*NumWeightIDP;
  YIDM   = lumi*NumWeightIDM;
  YISOP  = lumi*NumWeightISOP;
  YISOM  = lumi*NumWeightISOM;

  // Delete histograms
  delete nbcutidp;
  delete nbcutidm;
  delete nbcutisop;
  delete nbcutisom;

}

void GetSysQ2(
	      double lumi,     // Input: Luminosity
	      string dm,
	      string pc,       // Input: Name of the channel
	      TCut Coupure,    // Input: Analysis cut
	      double& Y1,    // Output: Weighted yield of the channel 
	      double& Y2,    // Output: Weighted yield of the channel 
	      double& Y3,    // Output: Weighted yield of the channel 
	      double& Y4,    // Output: Weighted yield of the channel 
	      double& Y6,    // Output: Weighted yield of the channel 
	      double& Y8     // Output: Weighted yield of the channel 
	      ){ 

  TChain ProcFile("bdttree");
  // Directory of roottuples
  string BaseDir = "/lstore/cms/cbeiraod/Stop4Body/nTuples_v2017-10-19_test_bdt" + dm + "/";
  string FFile = " ";

  // File to read
  if (pc==pc){     FFile = BaseDir + "T2DegStop_" + pc + "_bdt.root";  }

  if (pc=="DY1"){    FFile = BaseDir + "DYJetsToLL_M50_HT100to200_bdt.root";  }
  if (pc=="DY2"){    FFile = BaseDir + "DYJetsToLL_M50_HT200to400_bdt.root";  }
  if (pc=="DY3"){     FFile = BaseDir + "DYJetsToLL_M50_HT400to600_bdt.root";  }
  if (pc=="DY4"){    FFile = BaseDir + "DYJetsToLL_M50_HT600to800_bdt.root";  }
  if (pc=="DY5"){    FFile = BaseDir + "DYJetsToLL_M50_HT800to1200_bdt.root";  }
  if (pc=="DY6"){     FFile = BaseDir + "DYJetsToLL_M50_HT1200to2500_bdt.root";  }
  if (pc=="DY7"){     FFile = BaseDir + "DYJetsToLL_M50_HT2500toInf_bdt.root";  }
  if (pc=="DY8"){     FFile = BaseDir + "DYJetsToLL_M5to50_HT100to200_bdt.root";  }
  if (pc=="DY9"){     FFile = BaseDir + "DYJetsToLL_M5to50_HT200to400_bdt.root";  }
  if (pc=="DY10"){     FFile = BaseDir + "DYJetsToLL_M5to50_HT400to600_bdt.root";  }
  if (pc=="DY11"){     FFile = BaseDir + "DYJetsToLL_M5to50_HT600toInf_bdt.root";  }

  if (pc=="WW"){     FFile = BaseDir + "WW_bdt.root";  }
  if (pc=="WZ"){     FFile = BaseDir + "WZ_bdt.root";  }
  if (pc=="ZZ"){     FFile = BaseDir + "ZZ_bdt.root";  }

  if (pc=="SingleToptW"){     FFile = BaseDir + "T_tWch_ext_bdt.root";  }
  if (pc=="SingleToptChannel"){     FFile = BaseDir + "T_tch_powheg_bdt.root";  }
  if (pc=="SingleTopBtW"){     FFile = BaseDir + "TBar_tWch_ext_bdt.root";  }
  if (pc=="SingleTopBtChannel"){     FFile = BaseDir + "TBar_tch_powheg_bdt.root";  }
  if (pc=="TTgj"){     FFile = BaseDir + "TTGJets_bdt.root";  }
  if (pc=="TTwl"){     FFile = BaseDir + "TTWToLNu_bdt.root";  }
  if (pc=="TTwqq"){     FFile = BaseDir + "TTWToQQ_bdt.root";  }
  if (pc=="TTzl"){     FFile = BaseDir + "TTZToLLNuNu_bdt.root";  }
  if (pc=="TTzl1m10"){     FFile = BaseDir + "TTZToLLNuNu_m1to10_bdt.root";  }
  if (pc=="TTzqq"){     FFile = BaseDir + "TTZToQQ_bdt.root";  }
  ProcFile.Add((FFile).c_str());
  
  // Get the weighted numerator, where division by denominator has been done
  TH1D *nbcut1 = new TH1D("nbcut1","nbcut1",200,-100,100);
  TH1D *nbcut2 = new TH1D("nbcut2","nbcut2",200,-100,100);
  TH1D *nbcut3 = new TH1D("nbcut3","nbcut3",200,-100,100);
  TH1D *nbcut4 = new TH1D("nbcut4","nbcut4",200,-100,100);
  TH1D *nbcut6 = new TH1D("nbcut6","nbcut6",200,-100,100);
  TH1D *nbcut8 = new TH1D("nbcut8","nbcut8",200,-100,100);
  ProcFile.Draw("Njet>>nbcut1","weight_Q2_1*splitFactor"*Coupure);
  ProcFile.Draw("Njet>>nbcut2","weight_Q2_2*splitFactor"*Coupure);
  ProcFile.Draw("Njet>>nbcut3","weight_Q2_3*splitFactor"*Coupure);
  ProcFile.Draw("Njet>>nbcut4","weight_Q2_4*splitFactor"*Coupure);
  ProcFile.Draw("Njet>>nbcut6","weight_Q2_6*splitFactor"*Coupure);
  ProcFile.Draw("Njet>>nbcut8","weight_Q2_8*splitFactor"*Coupure);
  //
  double NumWeight1 = 0., NumWeight2 = 0., NumWeight3 = 0.;
  double NumWeight4 = 0., NumWeight6 = 0., NumWeight8 = 0.;
  for(int i=0; i<=(nbcut1->GetNbinsX()+1); i++){ 
    NumWeight1 += nbcut1->GetBinContent(i);
    NumWeight2 += nbcut2->GetBinContent(i);
    NumWeight3 += nbcut3->GetBinContent(i);
    NumWeight4 += nbcut4->GetBinContent(i);
    NumWeight6 += nbcut6->GetBinContent(i);
    NumWeight8 += nbcut8->GetBinContent(i);
  }
  // Calculate yields 
  Y1   = lumi*NumWeight1;
  Y2   = lumi*NumWeight2;
  Y3   = lumi*NumWeight3;
  Y4   = lumi*NumWeight4;
  Y6   = lumi*NumWeight6;
  Y8   = lumi*NumWeight8;
  /*if (pc=="SingleToptChannel"){
    cout << "NBinX = " << nbcut1->GetNbinsX() << std::endl;
    cout << "SyssTtCQ2 = " << NumWeight1 << " " << NumWeight2 << " " << NumWeight3 << " " << NumWeight4 << " " << NumWeight6 <<  " " << NumWeight8 << std::endl;
    }*/

  // Delete histograms
  delete nbcut1;
  delete nbcut2;
  delete nbcut3;
  delete nbcut4;
  delete nbcut6;
  delete nbcut8;

}

void GetSysFastSim(
		 double lumi,     // Input: Luminosity
		 string dm,
		 string pc,         // Input: Name of the channel
		 TCut Coupure,      // Input: Analysis cut
		 double& YFSIDP,    // Output: Weighted yield of the channel +1sigma(FS_ID)
		 double& YFSIDM,    // Output: Weighted yield of the channel -1sigma(FS_ID)
		 double& YFSHIIPP,  // Output: Weighted yield of the channel +1sigma(FS_HybIso-IP)
		 double& YFSHIIPM   // Output: Weighted yield of the channel -1sigma(FS_HybIso-IP)
		 ){ 

  TChain ProcFile("bdttree");
  // Directory of roottuples
  string BaseDir = "/lstore/cms/cbeiraod/Stop4Body/nTuples_v2017-10-19_test_bdt" + dm + "/";
  string FFile = " ";

  // File to read
  if (pc==pc){     FFile = BaseDir + "T2DegStop_" + pc + "_bdt.root";  }

  ProcFile.Add((FFile).c_str());
  
  // Get the weighted numerator, where division by denominator has been done
  TH1D *nbcutfsidp = new TH1D("nbcutfsidp","nbcutfsidp",200,-100,100);
  TH1D *nbcutfsidm = new TH1D("nbcutfsidm","nbcutfsidm",200,-100,100);
  ProcFile.Draw("Njet>>nbcutfsidp","weight_FullFast_ID_AltCorr_Up*splitFactor"*Coupure);
  ProcFile.Draw("Njet>>nbcutfsidm","weight_FullFast_ID_AltCorr_Down*splitFactor"*Coupure);
  TH1D *nbcutfshiipp = new TH1D("nbcutfshiipp","nbcutfshiipp",200,-100,100);
  TH1D *nbcutfshiipm = new TH1D("nbcutfshiipm","nbcutfshiipm",200,-100,100);
  ProcFile.Draw("Njet>>nbcutfshiipp","weight_FullFast_HIIP_AltCorr_Up*splitFactor"*Coupure);
  ProcFile.Draw("Njet>>nbcutfshiipm","weight_FullFast_HIIP_AltCorr_Down*splitFactor"*Coupure);
  //
  double NumWeightFSIDP = 0., NumWeightFSIDM = 0., NumWeightFSHIIPP = 0., NumWeightFSHIIPM = 0.;
  for(int i=0; i<=(nbcutfsidp->GetNbinsX()+1); i++){ 
    NumWeightFSIDP += nbcutfsidp->GetBinContent(i);
    NumWeightFSIDM += nbcutfsidm->GetBinContent(i);
  }
  for(int i=0; i<=(nbcutfshiipp->GetNbinsX()+1); i++){ 
    NumWeightFSHIIPP += nbcutfshiipp->GetBinContent(i);
    NumWeightFSHIIPM += nbcutfshiipm->GetBinContent(i);
  }
  // Calculate yields 
  YFSIDP   = lumi*NumWeightFSIDP;
  YFSIDM   = lumi*NumWeightFSIDM;
  YFSHIIPP   = lumi*NumWeightFSHIIPP;
  YFSHIIPM   = lumi*NumWeightFSHIIPM;

  // Delete histograms
  delete nbcutfsidp;
  delete nbcutfsidm;
  delete nbcutfshiipp;
  delete nbcutfshiipm;

}


void GetSysBTAG(
		double lumi,     // Input: Luminosity
		string dm,
		string pc,       // Input: Name of the channel
		TCut Coupure,    // Input: Analysis cut
		double& YB1P,    // Output: Weighted yield of the channel +1sigma(CFErr1)
		double& YB1M,    // Output: Weighted yield of the channel -1sigma(CFErr1)
		double& YB2P,    // Output: Weighted yield of the channel +1sigma(CFErr2)
		double& YB2M,    // Output: Weighted yield of the channel -1sigma(CFErr2)
		double& YB3P,    // Output: Weighted yield of the channel +1sigma(LF)
		double& YB3M,    // Output: Weighted yield of the channel -1sigma(LF)
		double& YB4P,    // Output: Weighted yield of the channel +1sigma(LFStats1)
		double& YB4M,    // Output: Weighted yield of the channel -1sigma(LFStats1)
		double& YB5P,    // Output: Weighted yield of the channel +1sigma(LFStats2)
		double& YB5M,    // Output: Weighted yield of the channel -1sigma(LFStats2)
		double& YB6P,    // Output: Weighted yield of the channel +1sigma(HF)
		double& YB6M,    // Output: Weighted yield of the channel -1sigma(HF)
		double& YB7P,    // Output: Weighted yield of the channel +1sigma(HFStats1)
		double& YB7M,    // Output: Weighted yield of the channel -1sigma(HFStats1)
		double& YB8P,    // Output: Weighted yield of the channel +1sigma(HFStats2)
		double& YB8M     // Output: Weighted yield of the channel -1sigma(HFStats2)
		){ 

  TChain ProcFile("bdttree");
  // Directory of roottuples
  string BaseDir = "/lstore/cms/cbeiraod/Stop4Body/nTuples_v2017-10-19_test_bdt" + dm + "/";
  string FFile = " ";

  // File to read
  if (pc==pc){     FFile = BaseDir + "T2DegStop_" + pc + "_bdt.root";  }

  if (pc=="DY1"){    FFile = BaseDir + "DYJetsToLL_M50_HT100to200_bdt.root";  }
  if (pc=="DY2"){    FFile = BaseDir + "DYJetsToLL_M50_HT200to400_bdt.root";  }
  if (pc=="DY3"){     FFile = BaseDir + "DYJetsToLL_M50_HT400to600_bdt.root";  }
  if (pc=="DY4"){    FFile = BaseDir + "DYJetsToLL_M50_HT600to800_bdt.root";  }
  if (pc=="DY5"){    FFile = BaseDir + "DYJetsToLL_M50_HT800to1200_bdt.root";  }
  if (pc=="DY6"){     FFile = BaseDir + "DYJetsToLL_M50_HT1200to2500_bdt.root";  }
  if (pc=="DY7"){     FFile = BaseDir + "DYJetsToLL_M50_HT2500toInf_bdt.root";  }
  if (pc=="DY8"){     FFile = BaseDir + "DYJetsToLL_M5to50_HT100to200_bdt.root";  }
  if (pc=="DY9"){     FFile = BaseDir + "DYJetsToLL_M5to50_HT200to400_bdt.root";  }
  if (pc=="DY10"){     FFile = BaseDir + "DYJetsToLL_M5to50_HT400to600_bdt.root";  }
  if (pc=="DY11"){     FFile = BaseDir + "DYJetsToLL_M5to50_HT600toInf_bdt.root";  }

  if (pc=="WW"){     FFile = BaseDir + "WW_bdt.root";  }
  if (pc=="WZ"){     FFile = BaseDir + "WZ_bdt.root";  }
  if (pc=="ZZ"){     FFile = BaseDir + "ZZ_bdt.root";  }

  if (pc=="SingleToptW"){     FFile = BaseDir + "T_tWch_ext_bdt.root";  }
  if (pc=="SingleToptChannel"){     FFile = BaseDir + "T_tch_powheg_bdt.root";  }
  if (pc=="SingleTopBtW"){     FFile = BaseDir + "TBar_tWch_ext_bdt.root";  }
  if (pc=="SingleTopBtChannel"){     FFile = BaseDir + "TBar_tch_powheg_bdt.root";  }
  if (pc=="TTgj"){     FFile = BaseDir + "TTGJets_bdt.root";  }
  if (pc=="TTwl"){     FFile = BaseDir + "TTWToLNu_bdt.root";  }
  if (pc=="TTwqq"){     FFile = BaseDir + "TTWToQQ_bdt.root";  }
  if (pc=="TTzl"){     FFile = BaseDir + "TTZToLLNuNu_bdt.root";  }
  if (pc=="TTzl1m10"){     FFile = BaseDir + "TTZToLLNuNu_m1to10_bdt.root";  }
  if (pc=="TTzqq"){     FFile = BaseDir + "TTZToQQ_bdt.root";  }
  ProcFile.Add((FFile).c_str());
  
  // Get the weighted numerator, where division by denominator has been done
  TH1D *nbcutb1p = new TH1D("nbcutb1p","nbcutb1p",200,-100,100);
  TH1D *nbcutb2p = new TH1D("nbcutb2p","nbcutb2p",200,-100,100);
  TH1D *nbcutb3p = new TH1D("nbcutb3p","nbcutb3p",200,-100,100);
  TH1D *nbcutb4p = new TH1D("nbcutb4p","nbcutb4p",200,-100,100);
  TH1D *nbcutb5p = new TH1D("nbcutb5p","nbcutb5p",200,-100,100);
  TH1D *nbcutb6p = new TH1D("nbcutb6p","nbcutb6p",200,-100,100);
  TH1D *nbcutb7p = new TH1D("nbcutb7p","nbcutb7p",200,-100,100);
  TH1D *nbcutb8p = new TH1D("nbcutb8p","nbcutb8p",200,-100,100);
  TH1D *nbcutb1m = new TH1D("nbcutb1m","nbcutb1m",200,-100,100);
  TH1D *nbcutb2m = new TH1D("nbcutb2m","nbcutb2m",200,-100,100);
  TH1D *nbcutb3m = new TH1D("nbcutb3m","nbcutb3m",200,-100,100);
  TH1D *nbcutb4m = new TH1D("nbcutb4m","nbcutb4m",200,-100,100);
  TH1D *nbcutb5m = new TH1D("nbcutb5m","nbcutb5m",200,-100,100);
  TH1D *nbcutb6m = new TH1D("nbcutb6m","nbcutb6m",200,-100,100);
  TH1D *nbcutb7m = new TH1D("nbcutb7m","nbcutb7m",200,-100,100);
  TH1D *nbcutb8m = new TH1D("nbcutb8m","nbcutb8m",200,-100,100);
  ProcFile.Draw("Njet>>nbcutb1p","weight_CFErr1_Up*splitFactor"*Coupure);
  ProcFile.Draw("Njet>>nbcutb2p","weight_CFErr2_Up*splitFactor"*Coupure);
  ProcFile.Draw("Njet>>nbcutb3p","weight_LF_Up*splitFactor"*Coupure);
  ProcFile.Draw("Njet>>nbcutb4p","weight_LFStats1_Up*splitFactor"*Coupure);
  ProcFile.Draw("Njet>>nbcutb5p","weight_LFStats2_Up*splitFactor"*Coupure);
  ProcFile.Draw("Njet>>nbcutb6p","weight_HF_Up*splitFactor"*Coupure);
  ProcFile.Draw("Njet>>nbcutb7p","weight_HFStats1_Up*splitFactor"*Coupure);
  ProcFile.Draw("Njet>>nbcutb8p","weight_HFStats2_Up*splitFactor"*Coupure);
  ProcFile.Draw("Njet>>nbcutb1m","weight_CFErr1_Down*splitFactor"*Coupure);
  ProcFile.Draw("Njet>>nbcutb2m","weight_CFErr2_Down*splitFactor"*Coupure);
  ProcFile.Draw("Njet>>nbcutb3m","weight_LF_Down*splitFactor"*Coupure);
  ProcFile.Draw("Njet>>nbcutb4m","weight_LFStats1_Down*splitFactor"*Coupure);
  ProcFile.Draw("Njet>>nbcutb5m","weight_LFStats2_Down*splitFactor"*Coupure);
  ProcFile.Draw("Njet>>nbcutb6m","weight_HF_Down*splitFactor"*Coupure);
  ProcFile.Draw("Njet>>nbcutb7m","weight_HFStats1_Down*splitFactor"*Coupure);
  ProcFile.Draw("Njet>>nbcutb8m","weight_HFStats2_Down*splitFactor"*Coupure);
  //
  double NumWeightB1P = 0., NumWeightB2P = 0., NumWeightB3P = 0., NumWeightB4P = 0.;
  double NumWeightB5P = 0., NumWeightB6P = 0., NumWeightB7P = 0., NumWeightB8P = 0.;
  double NumWeightB1M = 0., NumWeightB2M = 0., NumWeightB3M = 0., NumWeightB4M = 0.;
  double NumWeightB5M = 0., NumWeightB6M = 0., NumWeightB7M = 0., NumWeightB8M = 0.;
  for(int i=0; i<=(nbcutb1p->GetNbinsX()+1); i++){ 
    NumWeightB1P += nbcutb1p->GetBinContent(i);
    NumWeightB2P += nbcutb2p->GetBinContent(i);
    NumWeightB3P += nbcutb3p->GetBinContent(i);
    NumWeightB4P += nbcutb4p->GetBinContent(i);
    NumWeightB5P += nbcutb5p->GetBinContent(i);
    NumWeightB6P += nbcutb6p->GetBinContent(i);
    NumWeightB7P += nbcutb7p->GetBinContent(i);
    NumWeightB8P += nbcutb8p->GetBinContent(i);
    NumWeightB1M += nbcutb1m->GetBinContent(i);
    NumWeightB2M += nbcutb2m->GetBinContent(i);
    NumWeightB3M += nbcutb3m->GetBinContent(i);
    NumWeightB4M += nbcutb4m->GetBinContent(i);
    NumWeightB5M += nbcutb5m->GetBinContent(i);
    NumWeightB6M += nbcutb6m->GetBinContent(i);
    NumWeightB7M += nbcutb7m->GetBinContent(i);
    NumWeightB8M += nbcutb8m->GetBinContent(i);
  }
  // Calculate yields 
  YB1P   = lumi*NumWeightB1P;
  YB2P   = lumi*NumWeightB2P;
  YB3P   = lumi*NumWeightB3P;
  YB4P   = lumi*NumWeightB4P;
  YB5P   = lumi*NumWeightB5P;
  YB6P   = lumi*NumWeightB6P;
  YB7P   = lumi*NumWeightB7P;
  YB8P   = lumi*NumWeightB8P;
  YB1M   = lumi*NumWeightB1M;
  YB2M   = lumi*NumWeightB2M;
  YB3M   = lumi*NumWeightB3M;
  YB4M   = lumi*NumWeightB4M;
  YB5M   = lumi*NumWeightB5M;
  YB6M   = lumi*NumWeightB6M;
  YB7M   = lumi*NumWeightB7M;
  YB8M   = lumi*NumWeightB8M;

  // Delete histograms
  delete nbcutb1p;
  delete nbcutb2p;
  delete nbcutb3p;
  delete nbcutb4p;
  delete nbcutb5p;
  delete nbcutb6p;
  delete nbcutb7p;
  delete nbcutb8p;
  delete nbcutb1m;
  delete nbcutb2m;
  delete nbcutb3m;
  delete nbcutb4m;
  delete nbcutb5m;
  delete nbcutb6m;
  delete nbcutb7m;
  delete nbcutb8m;

}


double Max(double a, double b){
  if (a>b) return a;
  else return b;
}

double Abs(double a){
  return sqrt(pow(a,2));
}

