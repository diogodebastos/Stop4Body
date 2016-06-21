// @(#)root/tmva $Id$
/**********************************************************************************
 * Project   : TMVA - a ROOT-integrated toolkit for multivariate data analysis    *
 * Package   : TMVA                                                               *
 * Root Macro: TMVAClassification                                                 *
 *                                                                                *
 * This macro provides examples for the training and testing of the               *
 * TMVA classifiers.                                                              *
 *                                                                                *
 * As input data is used a toy-MC sample consisting of four Gaussian-distributed  *
 * and linearly correlated input variables.                                       *
 *                                                                                *
 * The methods to be used can be switched on and off by means of booleans, or     *
 * via the prompt command, for example:                                           *
 *                                                                                *
 *    root -l ./TMVAClassification.C\(\"Fisher,Likelihood\"\)                     *
 *                                                                                *
 * (note that the backslashes are mandatory)                                      *
 * If no method given, a default set of classifiers is used.                      *
 *                                                                                *
 * The output file "TMVA.root" can be analysed with the use of dedicated          *
 * macros (simply say: root -l <macro.C>), which can be conveniently              *
 * invoked through a GUI that will appear at the end of the run of this macro.    *
 * Launch the GUI via the command:                                                *
 *                                                                                *
 *    root -l ./TMVAGui.C                                                         *
 *                                                                                *
 **********************************************************************************/

#include <cstdlib>
#include <iostream>
#include <map>
#include <string>

#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TROOT.h"

#include "TMVAGui.C"
//#include "TMVAMultiClassGui.C"

#if not defined(__CINT__) || defined(__MAKECINT__)
// needs to be included when makecint runs (ACLIC)
#include "TMVA/Factory.h"
#include "TMVA/Tools.h"
#endif

void TMVAFourBody( TString myMethodList = "" )
{
   // The explicit loading of the shared libTMVA is done in TMVAlogon.C, defined in .rootrc
   // if you use your private .rootrc, or run from a different directory, please copy the
   // corresponding lines from .rootrc

   // methods to be processed can be given as an argument; use format:
   //
   // mylinux~> root -l TMVAClassification.C\(\"myMethod1,myMethod2,myMethod3\"\)
   //
   // if you like to use a method via the plugin mechanism, we recommend using
   //
   // mylinux~> root -l TMVAClassification.C\(\"P_myMethod\"\)
   // (an example is given for using the BDT as plugin (see below),
   // but of course the real application is when you write your own
   // method based)

   //---------------------------------------------------------------
   // This loads the library
   TMVA::Tools::Instance();

   // Default MVA methods to be trained + tested
   std::map<std::string,int> Use;

   // --- Cut optimisation
   // 
   // --- Boosted Decision Trees
   Use["BDT"]             = 1; // uses Adaptive Boost
   Use["BDTG"]            = 1; // uses Gradient Boost
   Use["BDTB"]            = 0; // uses Bagging
   Use["BDTD"]            = 0; // decorrelation + Adaptive Boost
   Use["BDTF"]            = 0; // allow usage of fisher discriminant for node splitting 
   // 
   // --- Friedman's RuleFit method, ie, an optimised series of cuts ("rules")
   Use["RuleFit"]         = 1;
   // ---------------------------------------------------------------

   std::cout << std::endl;
   std::cout << "==> Start TMVAClassification" << std::endl;

   // Select methods (don't look at this code - not of interest)
   if (myMethodList != "") {
      for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++) it->second = 0;

      std::vector<TString> mlist = TMVA::gTools().SplitString( myMethodList, ',' );
      for (UInt_t i=0; i<mlist.size(); i++) {
         std::string regMethod(mlist[i]);

         if (Use.find(regMethod) == Use.end()) {
            std::cout << "Method \"" << regMethod << "\" not known in TMVA under this name. Choose among the following:" << std::endl;
            for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++) std::cout << it->first << " ";
            std::cout << std::endl;
            return;
         }
         Use[regMethod] = 1;
      }
   }

   // --------------------------------------------------------------------------------------------------

   // --- Here the preparation phase begins

   // Create a ROOT output file where TMVA will store ntuples, histograms, etc.
   TString outfileName( "TMVA.root" );
   TFile* outputFile = TFile::Open( outfileName, "RECREATE" );

   // Create the factory object. Later you can choose the methods
   // whose performance you'd like to investigate. The factory is 
   // the only TMVA object you have to interact with
   //
   // The first argument is the base of the name of all the
   // weightfiles in the directory weight/
   //
   // The second argument is the output file for the training results
   // All TMVA output can be suppressed by removing the "!" (not) in
   // front of the "Silent" argument in the option string
   TMVA::Factory *factory = new TMVA::Factory( "TMVAClassification", outputFile,
					       "!V:!Silent:Color:DrawProgressBar:Transformations=I;D;P;G,D:AnalysisType=Classification" );
   //   TMVA::Factory *factory = new TMVA::Factory( "TMVAMulticlass", outputFile,
   //                                          "!V:!Silent:Color:DrawProgressBar:Transformations=I;P:AnalysisType=multiclass" );

   // If you wish to modify default settings
   // (please check "src/Config.h" to see all available global options)
   //    (TMVA::gConfig().GetVariablePlotting()).fTimesRMS = 8.0;
   //    (TMVA::gConfig().GetIONames()).fWeightFileDir = "myWeightDirectory";

   // Define the input variables that shall be used for the MVA training
   // note that you may also use variable expressions, such as: "3*var1/var2*abs(var3)"
   // [all types of expressions that can also be parsed by TTree::Draw( "expression" )]

   // Base BDT
   factory->AddVariable("Jet1Pt",'F');
   factory->AddVariable("mt",'F');
   factory->AddVariable("Met",'F');
   factory->AddVariable("HT20",'F');
   factory->AddVariable("NbLoose30",'I');   
   factory->AddVariable("LepChg",'F');
   factory->AddVariable("LepEta",'F');
   factory->AddVariable("LepPt",'F');
   //
   //   factory->AddVariable("Q80",'F');
   //   factory->AddVariable("CosDeltaPhi",'F');
   //   factory->AddVariable("Jet2Pt",'F');
   factory->AddVariable("JetHBpt",'F');
   factory->AddVariable("Njet",'I');
   //   factory->AddVariable("JetLepMass",'F');
   //   factory->AddVariable("J3Mass",'F');
   //   factory->AddVariable("DrJet1Lep",'F');
   factory->AddVariable("DrJetHBLep",'F');


   /*
   factory->AddVariable("LepPt",'F');
   factory->AddVariable("LepEta",'F');
   factory->AddVariable("Met",'F');
   factory->AddVariable("Q80",'F');
   factory->AddVariable("CosDeltaPhi",'F');
   factory->AddVariable("NbLoose30",'I');   
   factory->AddVariable("Njet",'I');
   factory->AddVariable("Jet1Pt",'F');
   factory->AddVariable("Jet2Pt",'F');
   factory->AddVariable("JetHBpt",'F');
   //   factory->AddVariable("HT25",'F');
   factory->AddVariable("HT20",'F');
   factory->AddVariable("DrJet1Lep",'F');
   factory->AddVariable("DrJetHBLep",'F');
   factory->AddVariable("DrJet1Jet2",'F');
   factory->AddVariable("JetLepMass",'F');
   factory->AddVariable("J3Mass",'F');
   */

   // You can add so-called "Spectator variables", which are not used in the MVA training,
   // but will appear in the final "TestTree" produced by TMVA. This TestTree will contain the
   // input variables, the response values of all trained MVAs, and the spectator variables
   //factory->AddSpectator( "Q80",  "Spectator 1", "units", 'F' );
   //factory->AddSpectator( "HT20",  "Spectator 2", "units", 'F' );

   // Read training and test data
   // (it is also possible to use ASCII format as input -> see TMVA Users Guide)
   TString fnamesignal = "../../../../Macros/FrigoRoottuple/Summer2015/Jet20/HTjet/T2DegStop_300_270_bdt.root";
   TString fnamebkg="../../../../Macros/FrigoRoottuple/Summer2015/Jet20/HTjet/Bckg_bdt.root";
   //   TString fnamesignal = "../../../../Macros/FrigoRoottuple/Spring2015/T2DegStop_300_270_bdt.root";
   //   TString fnamebkg="../../../../Macros/FrigoRoottuple/Spring2015/Bckg_bdt.root";
   //   TString fnamebkg="../../../../Macros/FrigoRoottuple/TTJets_bdt.root";
   //   TString fnamebkg="../../../../Macros/FrigoRoottuple/WJets_bdt.root";
   //   if (gSystem->AccessPathName( fname ))  // file does not exist in local directory
   //      gSystem->Exec("wget http://root.cern.ch/files/tmva_class_example.root");
   
   TFile *inputsignal = TFile::Open( fnamesignal );
   TFile *inputbkg= TFile::Open(fnamebkg);
   //   TFile *inputbkgT= TFile::Open(fnamebkgT);
   //   TFile *inputbkgW= TFile::Open(fnamebkgW);
   //   std::cout << "--- TMVAClassification       : Using input file signal: " << inputsignal->GetName() << std::endl;
   //   std::cout << "--- TMVAClassification       : Using input file background: " << inputbkg->GetName() << std::endl;
   // --- Register the training and test trees

   TTree *signal     = (TTree*)inputsignal->Get("bdttree");
   TTree *background = (TTree*)inputbkg->Get("bdttree");
   //   TTree *backgroundT = (TTree*)inputbkgT->Get("bdttree");
   //   TTree *backgroundW = (TTree*)inputbkgW->Get("bdttree");
   
   // global event weights per tree (see below for setting event-wise weights)
   Double_t signalWeight     = 1.0;
   Double_t backgroundWeight = 1.0;
   
   // You can add an arbitrary number of signal or background trees
   factory->AddSignalTree    ( signal,     signalWeight     );
   factory->AddBackgroundTree( background, backgroundWeight );
   //   factory->AddTree( signal, "Signal" );
   //   factory->AddTree( backgroundT, "TTbar" );
   //   factory->AddTree( backgroundW, "Wjets" );
   

   // Set individual event weights (the variables must exist in the original TTree)
   //    for signal    : factory->SetSignalWeightExpression    ("weight1*weight2");
   //    for background: factory->SetBackgroundWeightExpression("weight1*weight2");
   factory->SetBackgroundWeightExpression( "XS" );
   //   factory->SetSignalWeightExpression("XS");
   //   factory->SetWeightExpression( "XS" , "Signal");
   //   factory->SetWeightExpression( "XS" , "TTbar");
   //   factory->SetWeightExpression( "XS" , "Wjets");

   // Apply additional cuts on the signal and background samples (can be different): Cuts applied just before starting training
   TCut mycuts = "Met > 100"; // for example: TCut mycuts = "abs(var1)<0.5 && abs(var2-0.5)<1";
   TCut mycutb = "Met > 100"; // for example: TCut mycutb = "abs(var1)<0.5";

   // Tell the factory how to use the training and testing events
   //
   // If no numbers of events are given, half of the events in the tree are used 
   // for training, and the other half for testing:
   //    factory->PrepareTrainingAndTestTree( mycut, "SplitMode=random:!V" );
   // To also specify the number of testing events, use:
   // Use ALL events for training
   //   factory->PrepareTrainingAndTestTree( mycuts, mycutb,
   //                                        "nTrain_Signal=0:nTrain_Background=0:SplitMode=Random:NormMode=NumEvents:!V" );
   // Choose the number of events used for training & testing
   factory->PrepareTrainingAndTestTree( mycuts, mycutb,"nTrain_Signal=0:nTrain_Background=0:nTest_Signal=0:nTest_Background=0:SplitMode=Random:NormMode=EqualNumEvents");

   // ---- Book MVA methods
   //
   // Please lookup the various method configuration options in the corresponding cxx files, eg:
   // src/MethoCuts.cxx, etc, or here: http://tmva.sourceforge.net/optionRef.html
   // it is possible to preset ranges in the option string in which the cut optimisation should be done:
   // "...:CutRangeMin[2]=-1:CutRangeMax[2]=1"...", where [2] is the third input variable


   // Boosted Decision Trees
   //   if (Use["BDTG"]) // Gradient Boost
   //   factory->BookMethod( TMVA::Types::kBDT, "BDTG",
   //			"!H:!V:NTrees=500:MinNodeSize=2.5%:BoostType=Grad:Shrinkage=0.10:UseBaggedBoost:BaggedSampleFraction=0.5:nCuts=20:MaxDepth=2:IgnoreNegWeightsInTraining" );

   if (Use["BDT"])  // Adaptive Boost
     //     factory->BookMethod( TMVA::Types::kBDT, "BDT",
     //			  "!H:!V:NTrees=300:MinNodeSize=2.5%:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:UseBaggedBoost:BaggedSampleFraction=0.5:SeparationType=GiniIndex:nCuts=20" );
     factory->BookMethod( TMVA::Types::kBDT, "BDT",
     			  "!H:!V:NTrees=400:nEventsMin=400:MaxDepth=3:BoostType=AdaBoost:SeparationType=GiniIndex:nCuts=20:PruneMethod=NoPruning");

   //   if (Use["BDTB"]) // Bagging
	  // factory->BookMethod( TMVA::Types::kBDT, "BDTB",
   //               "!H:!V:NTrees=400:BoostType=Bagging:SeparationType=GiniIndex:nCuts=20" );

   //   if (Use["BDTD"]) // Decorrelation + Adaptive Boost
	  // factory->BookMethod( TMVA::Types::kBDT, "BDTD",
   //               "!H:!V:NTrees=400:MinNodeSize=5%:MaxDepth=3:BoostType=AdaBoost:SeparationType=GiniIndex:nCuts=20:VarTransform=Decorrelate" );

   //   if (Use["BDTF"])  // Allow Using Fisher discriminant in node splitting for (strong) linearly correlated variables
	  // factory->BookMethod( TMVA::Types::kBDT, "BDTMitFisher",
   //               "!H:!V:NTrees=50:MinNodeSize=2.5%:UseFisherCuts:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:SeparationType=GiniIndex:nCuts=20" );


   // For an example of the category classifier usage, see: TMVAClassificationCategory

   // --------------------------------------------------------------------------------------------------

   // ---- Now you can optimize the setting (configuration) of the MVAs using the set of training events

   // ---- STILL EXPERIMENTAL and only implemented for BDT's ! 
   // factory->OptimizeAllMethods("SigEffAt001","Scan");
   // factory->OptimizeAllMethods("ROCIntegral","FitGA");

   // --------------------------------------------------------------------------------------------------

   // ---- Now you can tell the factory to train, test, and evaluate the MVAs

   // Train MVAs using the set of training events
   factory->TrainAllMethods();

   // ---- Evaluate all MVAs using the set of test events
   factory->TestAllMethods();

   // ----- Evaluate and compare performance of all configured MVAs
   factory->EvaluateAllMethods();

   // --------------------------------------------------------------

   // Save the output
   outputFile->Close();

   std::cout << "==> Wrote root file: " << outputFile->GetName() << std::endl;
   std::cout << "==> TMVAClassification is done!" << std::endl;

   delete factory;

   // Launch the GUI for the root macros
   if (!gROOT->IsBatch()) TMVAGui( outfileName );
}
