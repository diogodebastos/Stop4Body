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

#include "UserCode/Stop4Body/interface/json.hpp"
#include "UserCode/Stop4Body/interface/SampleReader.h"
#include "UserCode/Stop4Body/interface/doubleWithUncertainty.h"

using json = nlohmann::json;

int main(int argc, char** argv)
{
  std::string jsonFileName = "";
  std::string outputDirectory = "./OUT/";
  std::string suffix = "";

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

    if(argument == "--suffix")
      suffix = argv[++i];
  }

  if(jsonFileName == "")
  {
    std::cout << "You must define a json file" << std::endl;
    return 1;
  }

  std::cout << "Reading JSON file" << std::endl;
  SampleReader samples(jsonFileName);

  TFile foutput((outputDirectory+"/puWeights.root").c_str(), "RECREATE");

  for(auto &process : samples)
  {
    std::cout << "Processing process: " << process.tag() << std::endl;

    TH1D processNVTX(("process_"+process.tag()+"_nvtx").c_str(), "nvtx;Evt.", 100, 0, 100);
    for(auto &sample : process)
    {
      std::cout << "\tProcessing sample: " << sample.tag() << std::endl;

      TH1D sampleNVTX(("sample_"+sample.tag()+"_nvtx").c_str(), "nvtx;Evt.", 100, 0, 100);

      for(auto &file : sample)
      {
        TFile finput(file.c_str(), "READ");
        foutput.cd();
        TTree *inputtree;
        if(process.selection() != "")
          inputtree = static_cast<TTree*>(finput.Get("tree"))->CopyTree(process.selection().c_str());
        else
          inputtree = static_cast<TTree*>(finput.Get("tree"));

        Int_t thisNevt = static_cast<Int_t>(inputtree->GetEntries());

        Float_t thisGenWeight = 0;
        Int_t nvtx = 0;
        inputtree->SetBranchAddress("genWeight", &thisGenWeight);
        inputtree->SetBranchAddress("nVert", &nvtx);
        for(Int_t i = 0; i < thisNevt; ++i)
        {
          inputtree->GetEntry(i);
          sampleNVTX.Fill(nvtx, thisGenWeight);
          processNVTX.Fill(nvtx, thisGenWeight);
        }

        if(process.selection() != "")
          delete inputtree;
      }

      sampleNVTX.Write();
    }

    processNVTX.Write();
  }

  std::cout << "Done!" << std::endl << std::endl;

  return 0;
}
