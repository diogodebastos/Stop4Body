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

std::string getBaseName(const std::string& s)
{
  char sep = '/';

  size_t i = s.rfind(sep, s.length());
  if (i != std::string::npos)
  {
    return s.substr(i+1, s.length() - i);
  }

  return "";
}

int main(int argc, char** argv)
{
  std::string jsonFileName = "";
  std::string outputDirectory = "./OUT/";
  std::string suffix = "";
  std::string dataPUFileName = "";
  std::string mcPUFileName = "";
  int nBins = 50;

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

    if(argument == "--dataPU")
      dataPUFileName = argv[++i];

    if(argument == "--otherMCPU")
      mcPUFileName = argv[++i];
  }

  if(jsonFileName == "")
  {
    std::cout << "You must define a json file" << std::endl;
    return 1;
  }

  if(dataPUFileName == "")
  {
    std::cout << "You must define a root file with the data PU distribution" << std::endl;
    return 1;
  }

  std::cout << "Reading JSON file" << std::endl;
  SampleReader samples(jsonFileName);

  TFile finput(dataPUFileName.c_str(), "READ");
  TH1D* dataPU = static_cast<TH1D*>(finput.Get("pileup"));
  dataPU->Scale(1/dataPU->Integral());

  TH1D* mcPU = nullptr;
  if(mcPUFileName != "")
  {
    mcPU = static_cast<TH1D*>( dataPU->Clone("mcPU") );
    TFile finput2(mcPUFileName.c_str(), "READ");
    TH1D* tmp = static_cast<TH1D*>(finput2.Get("pileup"));
    tmp->Scale(1/mcPU->Integral());
    mcPU->Divide(tmp);
  }

  TFile foutput((outputDirectory + "/puWeights_" + getBaseName(jsonFileName) + ".root").c_str(), "RECREATE");

  for(auto &process : samples)
  {
    std::cout << "Processing process: " << process.tag() << std::endl;

    TH1D processNVTX(("process_"+process.tag()+"_nvtx").c_str(), "nvtx;Evt.", nBins, 0, nBins);
    processNVTX.Sumw2();
    TH1D processNTrue(("process_"+process.tag()+"_nTrueInt").c_str(), "nvtx;Evt.", nBins, 0, nBins);
    processNTrue.Sumw2();
    for(auto &sample : process)
    {
      std::cout << "\tProcessing sample: " << sample.tag() << std::endl;

      TH1D sampleNVTX(("sample_"+sample.tag()+"_nvtx").c_str(), "nvtx;Evt.", nBins, 0, nBins);
      sampleNVTX.Sumw2();
      TH1D sampleNTrue(("sample_"+sample.tag()+"_nTrueInt").c_str(), "nvtx;Evt.", nBins, 0, nBins);
      sampleNTrue.Sumw2();

      double sampleSumGenWeight = 0;
      double Nevt = 0;

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
        Nevt += thisNevt;

        Float_t thisGenWeight = 0;
        Float_t nIsr;
        Int_t nvtx = 0;
        Float_t nTrueInt = 0;
        Float_t xsec = 1;
        inputtree->SetBranchAddress("nVert", &nvtx);
        if(process.isdata())
        {
          thisGenWeight = 1;
          nTrueInt = 1;
        }
        else
        {
          inputtree->SetBranchAddress("genWeight", &thisGenWeight);
          inputtree->SetBranchAddress("nTrueInt", &nTrueInt);
          inputtree->SetBranchAddress("xsec", &xsec);
          inputtree->SetBranchAddress("nIsr", &nIsr);
        }
        Float_t met_pt;      inputtree->SetBranchAddress("met_pt"    , &met_pt);
        Float_t met_phi;     inputtree->SetBranchAddress("met_phi",   &met_phi);
        Int_t nLepGood;      inputtree->SetBranchAddress("nLepGood"   , &nLepGood);
        Float_t LepGood_pt[LEPCOLL_LIMIT];  inputtree->SetBranchAddress("LepGood_pt", &LepGood_pt);
        Float_t LepGood_phi[LEPCOLL_LIMIT];  inputtree->SetBranchAddress("LepGood_phi", &LepGood_phi);
        double smallCounter = 0;

        for(Int_t i = 0; i < thisNevt; ++i)
        {
          inputtree->GetEntry(i);

          sampleNVTX.Fill(nvtx, thisGenWeight*xsec);
          processNVTX.Fill(nvtx, thisGenWeight*xsec);
          sampleNTrue.Fill(nTrueInt, thisGenWeight*xsec);
          processNTrue.Fill(nTrueInt, thisGenWeight*xsec);

          smallCounter += thisGenWeight;
        }

        sampleSumGenWeight += smallCounter;

        if(process.selection() != "")
          delete inputtree;
      }

      TVectorD sumGenWeight(1);
      sumGenWeight[0] = sampleSumGenWeight;
      sumGenWeight.Write(("sample_"+sample.tag()+"_sumGenWeight").c_str());

      TVectorD sampleNevt(1);
      sampleNevt[0] = Nevt;
      sampleNevt.Write(("sample_"+sample.tag()+"_Nevt").c_str());

      sampleNVTX.Write();
      sampleNTrue.Write();

      mcPU->Clone(("sample_"+sample.tag()+"_puWeight_alt").c_str())->Write();
      TH1D* samplePUweight = static_cast<TH1D*>(dataPU->Clone(("sample_"+sample.tag()+"_puWeight").c_str()));
      sampleNTrue.Scale(1/sampleNTrue.Integral());
      sampleNVTX.Scale(1/sampleNVTX.Integral());
      samplePUweight->Divide(&sampleNTrue); // This is the recommended, but for some reason it is giving weird results
      //samplePUweight->Divide(&sampleNVTX); // Trying this one instead
      samplePUweight->Write();
    }

    processNVTX.Write();
    processNTrue.Write();

    mcPU->Clone(("process_"+process.tag()+"_puWeight_alt").c_str())->Write();
    TH1D* processPUweight = static_cast<TH1D*>(dataPU->Clone(("process_"+process.tag()+"_puWeight").c_str()));
    processNTrue.Scale(1/processNTrue.Integral());
    processNVTX.Scale(1/processNVTX.Integral());
    processPUweight->Divide(&processNTrue);
    //processPUweight->Divide(&processNVTX);
    processPUweight->Write();
  }

  std::cout << "Done!" << std::endl << std::endl;

  return 0;
}
