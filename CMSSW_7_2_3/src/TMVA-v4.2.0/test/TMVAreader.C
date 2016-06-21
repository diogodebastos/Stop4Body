/**********************************************************************************
 * Project   : TMVA - a Root-integrated toolkit for multivariate data analysis    *
 * Package   : TMVA                                                               *
 * Exectuable: TMVAClassificationApplication                                      *
 *                                                                                *
 * This macro provides a simple example on how to use the trained classifiers     *
 * within an analysis module                                                      *
 **********************************************************************************/

#include <cstdlib>
#include <vector>
#include <iostream>
#include <map>
#include <string>

#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TStopwatch.h"

#include "TMVAGui.C"

#if not defined(__CINT__) || defined(__MAKECINT__)
#include "TMVA/Tools.h"
#include "TMVA/Reader.h"
#include "TMVA/MethodCuts.h"
#endif

using namespace TMVA;

void TMVAreader( TString fname ) 
{   
#ifdef __CINT__
   gROOT->ProcessLine( ".O0" ); // turn off optimization in CINT
#endif

 TString myMethodList="";
   //---------------------------------------------------------------

   // This loads the library
   TMVA::Tools::Instance();

   // Default MVA methods to be trained + tested
   std::map<std::string,int> Use;

   // 
   // --- Boosted Decision Trees
   Use["BDT"]             = 1; // uses Adaptive Boost

   std::cout << std::endl;
   std::cout << "==> Start TMVAClassificationApplication" << std::endl;

   // Select methods (don't look at this code - not of interest)
   if (myMethodList != "") {
      for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++) it->second = 0;

      std::vector<TString> mlist = gTools().SplitString( myMethodList, ',' );
      for (UInt_t i=0; i<mlist.size(); i++) {
         std::string regMethod(mlist[i]);

         if (Use.find(regMethod) == Use.end()) {
            std::cout << "Method \"" << regMethod 
                      << "\" not known in TMVA under this name. Choose among the following:" << std::endl;
            for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++) {
               std::cout << it->first << " ";
            }
            std::cout << std::endl;
            return;
         }
         Use[regMethod] = 1;
      }
   }

   // --------------------------------------------------------------------------------------------------

   // --- Create the Reader object

   TMVA::Reader *reader = new TMVA::Reader( "!Color:!Silent" );    

   // Create a set of variables and declare them to the reader
   // - the variable names MUST corresponds in name and type to those given in the weight file(s) used
   Float_t LepID, LepChg, LepPt, LepEta, LepDxy, LepDz, LepIso, Met, mt, Q80, CosDeltaPhi, NbLoose30, NbTight30;
   Float_t Njet,Jet1Pt,Jet1Eta,Jet2Pt,Jet2Eta,JetHBpt,DrJet1Lep,DrJet2Lep,DrJetHBLep,DrJet1Jet2,JetLepMass;
   Float_t J3Mass,HT20,XS,Event,Nevt;
  
   // This list should ONLY comprise variables used for the training

   // Base BDT
   reader->AddVariable("Jet1Pt", &Jet1Pt);
   reader->AddVariable("mt", &mt);
   reader->AddVariable("Met", &Met);
   reader->AddVariable("HT20", &HT20);
   reader->AddVariable("NbLoose30", &NbLoose30);   
   reader->AddVariable("LepChg", &LepChg);
   reader->AddVariable("LepEta", &LepEta);
   reader->AddVariable("LepPt", &LepPt);
   //
   //   reader->AddVariable("Q80", &Q80);
   //   reader->AddVariable("CosDeltaPhi", &CosDeltaPhi);
   //   reader->AddVariable("Jet2Pt", &Jet2Pt);
   reader->AddVariable("JetHBpt", &JetHBpt);
   reader->AddVariable("Njet", &Njet);
   //   reader->AddVariable("JetLepMass",&JetLepMass);
   //   reader->AddVariable("J3Mass",&J3Mass);
   //   reader->AddVariable("DrJet1Lep", &DrJet1Lep);
   reader->AddVariable("DrJetHBLep",&DrJetHBLep);

   /*
   reader->AddVariable("LepPt", &LepPt);
   reader->AddVariable("LepEta", &LepEta);
   //   reader->AddVariable("LepChg", &LepChg);
   reader->AddVariable("Met", &Met);
   reader->AddVariable("Q80", &Q80);
   reader->AddVariable("CosDeltaPhi",&CosDeltaPhi);
   reader->AddVariable("NbLoose30", &NbLoose30);   
   reader->AddVariable("Njet", &Njet);
   reader->AddVariable("Jet1Pt", &Jet1Pt);
   reader->AddVariable("Jet2Pt", &Jet2Pt);
   reader->AddVariable("JetHBpt", &JetHBpt);
   reader->AddVariable("HT20",&HT20);
   //   reader->AddVariable("HT25",&HT25);
   reader->AddVariable("DrJet1Lep", &DrJet1Lep);
   reader->AddVariable("DrJetHBLep",&DrJetHBLep);
   reader->AddVariable("DrJet1Jet2",&DrJet1Jet2);
   reader->AddVariable("JetLepMass",&JetLepMass);
   reader->AddVariable("J3Mass",&J3Mass);
   */

   // Spectator variables declared in the training have to be added to the reader, too
   Float_t spec1,spec2;
//   reader->AddSpectator( "spec1 := var1*2",   &spec1 );
//   reader->AddSpectator( "spec2 := var1*3",   &spec2 );


   // --- Book the MVA methods

   TString dir    = "weights/";
   TString prefix = "TMVAClassification";

   // Book method(s)
   for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++) {
      if (it->second) {
         TString methodName = TString(it->first) + TString(" method");
         cout << "method being used " << methodName << endl;
         TString weightfile = dir + prefix + TString("_") + TString(it->first) + TString(".weights.xml");
         cout << "weight file being used : "    <<  weightfile << endl;
         reader->BookMVA( methodName, weightfile ); 
      }
   }
   
   // Book output histograms
   UInt_t nbin = 100;
   TH1F   *histLk(0), *histLkD(0), *histLkPCA(0), *histLkKDE(0), *histLkMIX(0), *histPD(0), *histPDD(0);
   TH1F   *histPDPCA(0), *histPDEFoam(0), *histPDEFoamErr(0), *histPDEFoamSig(0), *histKNN(0), *histHm(0);
   TH1F   *histFi(0), *histFiG(0), *histFiB(0), *histLD(0), *histNn(0),*histNnbfgs(0),*histNnbnn(0);
   TH1F   *histNnC(0), *histNnT(0), *histBdt(0), *histBdtG(0), *histBdtD(0), *histRf(0), *histSVMG(0);
   TH1F   *histSVMP(0), *histSVML(0), *histFDAMT(0), *histFDAGA(0), *histCat(0), *histPBdt(0);

   if (Use["BDT"])           histBdt     = new TH1F( "MVA_BDT",           "MVA_BDT",           nbin, -0.8, 0.8 );


   // Prepare input tree (this must be replaced by your data source)
   // in this example, there is a toy tree with signal and one with background events
   // we'll later on use only the "signal" events for the test in this example.
   //   
   //TString fname = "T2DegStop_300_270_bdt.root";   
   TFile *input = new TFile(fname,"UPDATE");
   std::cout << "--- TMVAClassificationApp    : Using input file: " << input->GetName() << std::endl;
   
   // --- Event loop

   // Prepare the event tree
   // - here the variable names have to corresponds to your tree
   // - you can use the same variables as above which is slightly faster,
   //   but of course you can use different ones and copy the values inside the event loop
   //
   std::cout << "--- Select signal sample" << std::endl;
   TTree* InputTree = (TTree*)input->Get("bdttree");
    
   
   // This is the list of variables to be saved
   InputTree->SetBranchAddress("LepID", &LepID);
   InputTree->SetBranchAddress("LepChg", &LepChg);
   InputTree->SetBranchAddress("LepPt", &LepPt);
   InputTree->SetBranchAddress("LepEta", &LepEta);
   InputTree->SetBranchAddress("LepDxy", &LepDxy);
   InputTree->SetBranchAddress("LepDz", &LepDz);
   InputTree->SetBranchAddress("LepIso", &LepIso);
   InputTree->SetBranchAddress("Met", &Met);
   InputTree->SetBranchAddress("mt",  &mt);
   InputTree->SetBranchAddress("Q80", &Q80);
   InputTree->SetBranchAddress("CosDeltaPhi",&CosDeltaPhi);
   InputTree->SetBranchAddress("NbLoose30", &NbLoose30);   
   InputTree->SetBranchAddress("NbTight30", &NbTight30);
   InputTree->SetBranchAddress("Njet", &Njet);
   InputTree->SetBranchAddress("Jet1Pt", &Jet1Pt);
   InputTree->SetBranchAddress("Jet1Eta", &Jet1Eta);
   InputTree->SetBranchAddress("Jet2Pt", &Jet2Pt);
   InputTree->SetBranchAddress("Jet2Eta", &Jet2Eta);
   InputTree->SetBranchAddress("JetHBpt", &JetHBpt);
   InputTree->SetBranchAddress("DrJet1Lep", &DrJet1Lep);
   InputTree->SetBranchAddress("DrJet2Lep", &DrJet2Lep);
   InputTree->SetBranchAddress("DrJetHBLep",&DrJetHBLep);
   InputTree->SetBranchAddress("DrJet1Jet2",&DrJet1Jet2);
   InputTree->SetBranchAddress("JetLepMass",&JetLepMass);
   InputTree->SetBranchAddress("J3Mass",&J3Mass);
   InputTree->SetBranchAddress("HT20",&HT20);
   //   InputTree->SetBranchAddress("HT25",&HT25);
   InputTree->SetBranchAddress("XS", &XS);
   InputTree->SetBranchAddress("Event", &Event);
   InputTree->SetBranchAddress("Nevt", &Nevt);

   // new bdt branch
   Float_t bdt1;
   TBranch *bdtoutput = InputTree->Branch("bdt1",&bdt1,"bdt1/F");
   
   // Efficiency calculator for cut method
   Int_t    nSelCutsGA = 0;
   Double_t effS       = 0.7;

   std::vector<Float_t> vecVar(4); // vector for EvaluateMVA tests

   std::cout << "--- Processing: " << InputTree->GetEntries() << " events" << std::endl;
   TStopwatch sw;
   sw.Start();
   for (Long64_t ievt=0; ievt<InputTree->GetEntries();ievt++) {

      if (ievt%1000 == 0) std::cout << "--- ... Processing event: " << ievt << std::endl;

      InputTree->GetEntry(ievt);
      
      bdt1=reader->EvaluateMVA("BDT method") ;
      bdtoutput->Fill();      
      
   //   cout << " Met " <<  Met << endl;
   //   cout << " BDT output " << reader->EvaluateMVA("BDT method") << endl;
      if (Use["BDT"])   histBdt    ->Fill( reader->EvaluateMVA( "BDT method"           ) );

   }
    
  input->cd(); 

   // Get elapsed time
   sw.Stop();
   std::cout << "--- End of event loop: "; sw.Print();



   // --- Write histograms

//   TFile *target  = new TFile( "TMVApp.root","RECREATE" );
//    TFile *target= new TFile(fname,"UPDATE");
   if (Use["BDT"          ])   histBdt    ->Write();

  
  InputTree->Print();
  InputTree->Write();
  delete input ;

   std::cout << "--- Created root file: " + fname + " containing the MVA output histograms" << std::endl;
  
   delete reader;
    
   std::cout << "==> TMVAClassificationApplication is done!" << endl << std::endl;
} 
