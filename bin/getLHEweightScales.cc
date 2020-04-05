#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <list>
#include <iterator>
#include <algorithm>
#include <iomanip>
#include <math.h>
#include <memory>
#include <cmath>
#include <cstdlib>

#include "TROOT.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TChain.h"
#include "TObject.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TLegend.h"
#include "TGraph.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TTree.h"
#include "TF1.h"
#include "TCutG.h"
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
#include "TMultiGraph.h"
#include "TPaveText.h"
#include "THStack.h"
#include "TStyle.h"
#include "TLatex.h"
#include "TLorentzVector.h"
#include "TVectorD.h"

#include "UserCode/Stop4Body-nanoAOD/interface/json.hpp"
#include "UserCode/Stop4Body-nanoAOD/interface/SampleReader.h"
#include "UserCode/Stop4Body-nanoAOD/interface/commonFunctions.h"
#include "UserCode/Stop4Body-nanoAOD/interface/doubleWithUncertainty.h"

#define LHEWEIGHT_LIMIT 9

using json = nlohmann::json;

bool replace(std::string&, const std::string&, const std::string&);

int main(int argc, char** argv)
{
  std::string jsonFileName = "";
  std::string outputDirectory = "./OUT/";

  if(argc < 2)
  {
    std::cout << "You did not pass enough parameters" << std::endl;
    //printHelp();
    return 0;
  }

  for(int i = 1; i < argc; ++i)
  {
    std::string argument = argv[i];

    if(argument == "--json")
      jsonFileName = argv[++i];

    if(argument == "--outDir")
      outputDirectory = argv[++i];
  }

  if(jsonFileName == "")
  {
    std::cout << "You must define a json file" << std::endl;
    return 1;
  }

  std::cout << "Reading JSON file" << std::endl;
  SampleReader samples(jsonFileName);

  gInterpreter->GenerateDictionary("map<int, double>","map");
  TClass *mapClass = gROOT->FindSTLClass("std::map<int,double>", true);
  TFile foutput((outputDirectory + "/lheWeights_" + getBaseName(jsonFileName) + ".root").c_str(), "RECREATE");

  for(auto &process : samples)
  {
    std::cout << "Processing process: " << process.tag() << std::endl;

    std::map<int,double> processSum;

    for(auto &sample : process)
    {
      std::cout << "\tProcessing sample: " << sample.tag() << std::endl;

      std::map<int,double> sampleSum;

      for(auto &file : sample)
      {
        TFile finput(file.c_str(), "READ");
        std::cout << "\t\tProcessing file: " << file.c_str() << std::endl;
        foutput.cd();
        TTree *inputtree;
        if(process.selection() != "")
          inputtree = static_cast<TTree*>(finput.Get("Events"))->CopyTree(process.selection().c_str());
        else
          inputtree = static_cast<TTree*>(finput.Get("Events"));

        std::map<int,double> fileSum;

        UInt_t nLHEScaleWeight;
        float LHEScaleWeight[LHEWEIGHT_LIMIT];

        inputtree->SetBranchAddress("nLHEScaleWeight" , &nLHEScaleWeight);
        inputtree->SetBranchAddress("LHEScaleWeight", &LHEScaleWeight);

        Int_t thisNevt = static_cast<Int_t>(inputtree->GetEntries());

        for(Int_t i = 0; i < thisNevt; ++i)
        {
          inputtree->GetEntry(i);

          for(UInt_t i = 0; i < nLHEScaleWeight; ++i)
          {
            //Integral of weight per variation
            fileSum[i] += LHEScaleWeight[i];
          }
        }

        for(auto& kv: fileSum)
        {
          sampleSum[kv.first] += kv.second;
        }

        if(process.selection() != "")
          delete inputtree;
      }

      foutput.WriteObjectAny(&sampleSum, mapClass, ("sample_"+sample.tag()+"_lhemap").c_str());

      for(auto& kv: sampleSum)
      {
        processSum[kv.first] += kv.second;
      }
    }

    foutput.WriteObjectAny(&processSum, mapClass, ("process_"+process.tag()+"_lhemap").c_str());
  }

  std::cout << "Done!" << std::endl << std::endl;

  return 0;
}

bool replace(std::string& str, const std::string& from, const std::string& to)
{
  size_t start_pos = str.find(from);
  if(start_pos == std::string::npos)
    return false;
  str.replace(start_pos, from.length(), to);
  return true;
}
