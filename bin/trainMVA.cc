#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TChain.h>
#include <TCut.h>
#include <TString.h>
#include <TObject.h>
#include <TRandom3.h>
#include <TDirectory.h>

#include "TMVA/Factory.h"
#include "TMVA/Tools.h"

#include <iostream>
#include <vector>
#include <sstream>
#include <map>
#include <fstream>

void printHelp();
bool fileExists(std::string);

int main(int argc, char** argv)
{
  std::string signalFileName = "signal.root";
  std::string backgroundFileName = "background.root";
  std::vector<std::string> methods;
  methods.clear();

  // Default MVA methods to be trained + tested
  std::map<std::string,int> Use;
  // --- Boosted Decision Trees
  Use["BDT"]             = 0; // uses Adaptive Boost
  Use["BDTG"]            = 0; // uses Gradient Boost
  Use["BDTB"]            = 0; // uses Bagging
  Use["BDTD"]            = 0; // decorrelation + Adaptive Boost
  Use["BDTF"]            = 0; // allow usage of fisher discriminant for node splitting
  //
  // --- Friedman's RuleFit method, ie, an optimised series of cuts ("rules")
  Use["RuleFit"]         = 0;
  Use["NN"]              = 0;

  if(argc < 2)
  {
    std::cout << "You did not pass enough parameters" << std::endl;
    printHelp();
    return 0;
  }

  for(int i = 1; i < argc; ++i)
  {
    std::string argument = argv[i];

    if(argument == "--help")
    {
      printHelp();
      return 0;
    }

    if(argument == "--signalFile")
      signalFileName = argv[++i];

    if(argument == "--backgroundFile")
      backgroundFileName = argv[++i];

    if(argument == "--method")
      methods.push_back(argv[++i]);
  }

  for(auto & method : methods)
  {
    if(Use.count(method) != 1)
    {
      std::cout << "Unknown method \"" << method << "\", ignoring the request" << std::endl;
    }
    else
      Use[method] = 1;
  }

  std::cout << "==> Start TMVAClassification" << std::endl;
  std::cout << "\tProcessing files:" << std::endl;
  std::cout << "\t  - Signal: " << signalFileName << std::endl;
  std::cout << "\t  - Background: " << backgroundFileName << std::endl;

  // Create a ROOT output file where TMVA will store ntuples, histograms, etc.
  TString outfileName( "TMVA.root" );
  TFile* outputFile = TFile::Open( outfileName, "RECREATE" );

  TMVA::Factory *factory = new TMVA::Factory( "TMVAClassification", outputFile,
                                              "!V:!Silent:Color:DrawProgressBar:Transformations=I:AnalysisType=Classification" );

  // Base BDT
  factory->AddVariable("Jet1Pt",'F');
  factory->AddVariable("mt",'F');
  factory->AddVariable("Met",'F');
  factory->AddVariable("HT20",'F');
  factory->AddVariable("NbLoose30",'I');
  factory->AddVariable("LepChg",'F');
  factory->AddVariable("LepEta",'F');
  factory->AddVariable("LepPt",'F');
  //   factory->AddVariable("Q80",'F');
  //   factory->AddVariable("CosDeltaPhi",'F');
  //   factory->AddVariable("Jet2Pt",'F');
  factory->AddVariable("JetHBpt",'F');
  factory->AddVariable("Njet",'I');
  //   factory->AddVariable("JetLepMass",'F');
  //   factory->AddVariable("J3Mass",'F');
  //   factory->AddVariable("DrJet1Lep",'F');
  factory->AddVariable("DrJetHBLep",'F');

  TFile *inputsignal = TFile::Open( signalFileName.c_str() );
  TFile *inputbkg= TFile::Open( backgroundFileName.c_str() );

  TTree *signal     = (TTree*)inputsignal->Get("bdttree");
  TTree *background = (TTree*)inputbkg->Get("bdttree");

  Double_t signalWeight     = 1.0;
  Double_t backgroundWeight = 1.0;

  factory->AddSignalTree    ( signal,     signalWeight     );
  factory->AddBackgroundTree( background, backgroundWeight );
  factory->SetBackgroundWeightExpression( "XS" );
  factory->SetSignalWeightExpression("XS");

  TCut mycuts = "Met > 100";
  TCut mycutb = "Met > 100";
  mycuts = "Met > 280";
  mycutb = "Met > 280";

  factory->PrepareTrainingAndTestTree( mycuts, mycutb, "nTrain_Signal=0:nTrain_Background=0:nTest_Signal=0:nTest_Background=0:SplitMode=Random:NormMode=EqualNumEvents" );

  // ---- Book MVA methods
  if (Use["BDTG"]) // Gradient Boost
    factory->BookMethod( TMVA::Types::kBDT, "BDTG",
                         "!H:!V:NTrees=500:MinNodeSize=2.5%:BoostType=Grad:Shrinkage=0.10:UseBaggedBoost:BaggedSampleFraction=0.5:nCuts=20:MaxDepth=2:IgnoreNegWeightsInTraining" );

  if (Use["BDT"])  // Adaptive Boost
  {
    factory->BookMethod( TMVA::Types::kBDT, "BDT",
                         "!H:!V:NTrees=400:MaxDepth=3:BoostType=AdaBoost:SeparationType=GiniIndex:nCuts=20:PruneMethod=NoPruning" );
    //factory->BookMethod( TMVA::Types::kBDT, "BDT",
    //                     "!H:!V:NTrees=400:nEventsMin=400:MaxDepth=3:BoostType=AdaBoost:SeparationType=GiniIndex:nCuts=20:PruneMethod=NoPruning" );
    for(int i = 1; i <= 20; ++i)
    //for(int i = 3; i <= 3; ++i)
    {
      double val = i/2.0;
      std::string valStr;

      std::stringstream converter;
      converter << val;
      converter >> valStr;

      factory->BookMethod( TMVA::Types::kBDT, ("BDT_NodeSize"+valStr).c_str(),
                           ("!H:!V:NTrees=400:MinNodeSize="+valStr+"%:MaxDepth=3:BoostType=AdaBoost:SeparationType=GiniIndex:nCuts=20:PruneMethod=NoPruning").c_str() );
    }
  }

  if(Use["NN"])
  {
    std::vector<std::string> firstLayer = {"N", "2N", "0.5N"};
    std::vector<std::string> secondLayer = {"N", "N-1", "0.5N", "2N"};
    std::vector<std::string> nCycles = {"50", "100", "150", "200", "300", "500", "1000", "2000"};
    std::vector<std::string> learningRate = {"0.01", "0.02", "0.04", "0.08", "0.12"};
    std::vector<std::string> decayRate = {"0.005", "0.01", "0.02"};
    std::vector<std::string> other = {};

    for(auto& layer1 : firstLayer)
    {
      for(auto& layer2 : secondLayer)
      {
        for(auto& cycles : nCycles)
        {
          for(auto& learn : learningRate)
          {
            for(auto& decay : decayRate)
            {
              std::string name = "MLP_" + layer1 + "_" + layer2 + "_" + cycles + "_" + learn + "_" + decay;
              std::string options = "!H:!V:VarTransform=N:TestRate=1:NCycles="+cycles+":HiddenLayers="+layer1+","+layer2+":LearningRate="+learn+":DecayRate="+decay;

              factory->BookMethod(TMVA::Types::kMLP, name.c_str(), options.c_str());
            }
          }
        }
      }
    }

    for(size_t i = 0; i < other.size(); ++i)
    {
      std::stringstream converter;
      converter << "other" << i;

      std::string name;
      converter >> name;
      std::string options = "!H:!V:VarTransform=N:TestRate=1" + other[i];

      factory->BookMethod(TMVA::Types::kMLP, name.c_str(), options.c_str());
    }

  }

  if (Use["BDTB"]) // Bagging
    factory->BookMethod( TMVA::Types::kBDT, "BDTB",
                         "!H:!V:NTrees=400:BoostType=Bagging:SeparationType=GiniIndex:nCuts=20" );

  if (Use["BDTD"]) // Decorrelation + Adaptive Boost
    factory->BookMethod( TMVA::Types::kBDT, "BDTD",
                         "!H:!V:NTrees=400:MinNodeSize=5%:MaxDepth=3:BoostType=AdaBoost:SeparationType=GiniIndex:nCuts=20:VarTransform=Decorrelate" );

  if (Use["BDTF"])  // Allow Using Fisher discriminant in node splitting for (strong) linearly correlated variables
    factory->BookMethod( TMVA::Types::kBDT, "BDTMitFisher",
                         "!H:!V:NTrees=50:MinNodeSize=2.5%:UseFisherCuts:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:SeparationType=GiniIndex:nCuts=20" );

  factory->TrainAllMethods();
  factory->TestAllMethods();
  factory->EvaluateAllMethods();

  outputFile->Close();

  std::cout << "==> Wrote root file: " << outputFile->GetName() << std::endl;
  std::cout << "==> TMVAClassification is done!" << std::endl;

  delete factory;

  return 0;
}


void printHelp()
{
  std::cout << "The program has the following parameters:" << std::endl;
  std::cout << "\t--help\t- Print this message" << std::endl;

  return;
}

bool fileExists(std::string fileName)
{
  std::ifstream infile(fileName);
  return infile.good();
}
