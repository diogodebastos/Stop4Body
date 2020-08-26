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

#define LEPCOLL_LIMIT  40

using json = nlohmann::json;

bool replace(std::string&, const std::string&, const std::string&);

int main(int argc, char** argv)
{
  std::string jsonFileName = "";
  std::string outputDirectory = "./OUT/";
  std::string suffix = "";
  std::string dataPUFileName = "";
  //int nBins = 100; //100 bins in 2017
  int nBins = 200;  // 200 bins in 2018

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
  TH1D* dataPUUp = nullptr;
  TH1D* dataPUDown = nullptr;

  std::string fileUp = dataPUFileName;
  replace(fileUp, ".root", "Up.root");
  TFile finputUp(fileUp.c_str(), "READ");
  dataPUUp = static_cast<TH1D*>(finputUp.Get("pileup"));
  dataPUUp->Scale(1/dataPUUp->Integral());

  std::string fileDown = dataPUFileName;
  replace(fileDown, ".root", "Down.root");
  TFile finputDown(fileDown.c_str(), "READ");
  dataPUDown = static_cast<TH1D*>(finputDown.Get("pileup"));
  dataPUDown->Scale(1/dataPUDown->Integral());

  TFile foutput((outputDirectory + "/preProcessSamples_" + getBaseName(jsonFileName) + ".root").c_str(), "RECREATE");

  for(auto &process : samples)
  {
    std::cout << "Processing process: " << process.tag() << std::endl;

    TH1D processNVTX(("process_"+process.tag()+"_nvtx").c_str(), "nvtx;Evt.", nBins, 0, nBins);
    processNVTX.Sumw2();
    TH1D processNTrue(("process_"+process.tag()+"_nTrueInt").c_str(), "nvtx;Evt.", nBins, 0, nBins);
    processNTrue.Sumw2();

    int nIsrBin_process[7]{0, 0, 0, 0, 0, 0, 0};
    int EWKpTBin_process[8]{0, 0, 0, 0, 0, 0, 0, 0};

    for(auto &sample : process)
    {
      std::cout << "\tProcessing sample: " << sample.tag() << std::endl;

      TH1D sampleNVTX(("sample_"+sample.tag()+"_nvtx").c_str(), "nvtx;Evt.", nBins, 0, nBins);
      sampleNVTX.Sumw2();
      TH1D sampleNTrue(("sample_"+sample.tag()+"_nTrueInt").c_str(), "nvtx;Evt.", nBins, 0, nBins);
      sampleNTrue.Sumw2();

      int nIsrBin_sample[7]{0, 0, 0, 0, 0, 0, 0};
      int EWKpTBin_sample[8]{0, 0, 0, 0, 0, 0, 0, 0};

      double sampleSumGenWeight = 0;
      Double_t thisSumGenWeight = 0;
      double Nevt = 0;

      for(auto &file : sample)
      {
        TFile finput(file.c_str(), "READ");
        std::cout << "\t\tProcessing file: " << file.c_str() << std::endl;
        foutput.cd();
        TTree *inputtree;
        TTree *inputruntree;
        if(process.selection() != ""){
          inputtree = static_cast<TTree*>(finput.Get("Events"))->CopyTree(process.selection().c_str());
        }
        else{
          inputtree = static_cast<TTree*>(finput.Get("Events"));
        }
        inputruntree = static_cast<TTree*>(finput.Get("Runs"));

        Int_t thisNentries = static_cast<Int_t>(inputruntree->GetEntries());
        Int_t thisNevt = static_cast<Int_t>(inputtree->GetEntries());
        Nevt += thisNevt;

        //PU
        Int_t nvtx = 0;
        Float_t nTrueInt = 0;
        Float_t xsec = 1;

        inputtree->SetBranchAddress("PV_npvsGood", &nvtx);
        inputtree->SetBranchAddress("Pileup_nTrueInt", &nTrueInt);
        inputtree->SetBranchAddress("xsec", &xsec);

        // Get sumGenWeight
        thisSumGenWeight = 0;
        double counter = 0;
        inputruntree->SetBranchAddress("genEventSumw", &thisSumGenWeight);
        if(process.selection() == ""){
          for(Int_t k = 0; k < thisNentries; ++k)
          {
            inputruntree->GetEntry(k);
            counter += thisSumGenWeight;
          }
          sampleSumGenWeight += counter;
        }
        //Get ISR Params
        Float_t nIsr;          inputtree->SetBranchAddress("nIsr", &nIsr);
        Float_t thisGenWeight; inputtree->SetBranchAddress("genWeight", &thisGenWeight);
        Float_t met_pt;        inputtree->SetBranchAddress("MET_pt"    , &met_pt);
        Float_t met_phi;       inputtree->SetBranchAddress("MET_phi",   &met_phi);
        UInt_t nLepGood;       inputtree->SetBranchAddress("nLepGood"   , &nLepGood);
        Float_t LepGood_pt[LEPCOLL_LIMIT];  inputtree->SetBranchAddress("LepGood_pt", &LepGood_pt);
        Float_t LepGood_phi[LEPCOLL_LIMIT];  inputtree->SetBranchAddress("LepGood_phi", &LepGood_phi);
        double smallCounter = 0;

        for(Int_t i = 0; i < thisNevt; ++i)
        {
          inputtree->GetEntry(i);

          //PU
          sampleNVTX.Fill(nvtx, thisGenWeight*xsec);
          processNVTX.Fill(nvtx, thisGenWeight*xsec);
          sampleNTrue.Fill(nTrueInt, thisGenWeight*xsec);
          processNTrue.Fill(nTrueInt, thisGenWeight*xsec);

          smallCounter += thisGenWeight;

          //Get ISR Params
          Int_t nIsr_switch = nIsr;
          if(nIsr_switch > 6)
            nIsr_switch = 6;
          nIsrBin_sample[nIsr_switch]++;
          nIsrBin_process[nIsr_switch]++;

          if(nLepGood > 0)
          {
            double lep_x = LepGood_pt[0] * std::cos(LepGood_phi[0]);
            double lep_y = LepGood_pt[0] * std::sin(LepGood_phi[0]);
            double met_x = met_pt * std::cos(met_phi);
            double met_y = met_pt * std::sin(met_phi);

            double w_pt = std::sqrt((lep_x + met_x)*(lep_x + met_x) + (lep_y + met_y)*(lep_y + met_y));

            int EWKindex = 7;
            if(w_pt < 600)
              EWKindex = 6;
            if(w_pt < 400)
              EWKindex = 5;
            if(w_pt < 300)
              EWKindex = 4;
            if(w_pt < 200)
              EWKindex = 3;
            if(w_pt < 150)
              EWKindex = 2;
            if(w_pt < 100)
              EWKindex = 1;
            if(w_pt < 50)
              EWKindex = 0;

            EWKpTBin_sample[EWKindex]++;
            EWKpTBin_process[EWKindex]++;
          }
        }

        if(process.selection() != "")
        {
          sampleSumGenWeight += smallCounter;
          delete inputtree;
        }
        delete inputruntree;
      }

      TVectorD sampleNevt(1);
      sampleNevt[0] = Nevt;
      sampleNevt.Write(("sample_"+sample.tag()+"_Nevt").c_str());

      TVectorD sumGenWeight(1);
      sumGenWeight[0] = sampleSumGenWeight;
      sumGenWeight.Write(("sample_"+sample.tag()+"_sumGenWeight").c_str());

      double ISRCParam_sample = (                 nIsrBin_sample[0] +
                                                  nIsrBin_sample[1] +
                                                  nIsrBin_sample[2] +
                                                  nIsrBin_sample[3] +
                                                  nIsrBin_sample[4] +
                                                  nIsrBin_sample[5] +
                                                  nIsrBin_sample[6]  ) /
      static_cast<double>(ISRweightFromNISRJet(0)*nIsrBin_sample[0] +
                          ISRweightFromNISRJet(1)*nIsrBin_sample[1] +
                          ISRweightFromNISRJet(2)*nIsrBin_sample[2] +
                          ISRweightFromNISRJet(3)*nIsrBin_sample[3] +
                          ISRweightFromNISRJet(4)*nIsrBin_sample[4] +
                          ISRweightFromNISRJet(5)*nIsrBin_sample[5] +
                          ISRweightFromNISRJet(6)*nIsrBin_sample[6]  );
      double EWKISRCParam_sample = (                   EWKpTBin_sample[0] +
                                                       EWKpTBin_sample[1] +
                                                       EWKpTBin_sample[2] +
                                                       EWKpTBin_sample[3] +
                                                       EWKpTBin_sample[4] +
                                                       EWKpTBin_sample[5] +
                                                       EWKpTBin_sample[6] +
                                                       EWKpTBin_sample[7]  ) /
      static_cast<double>(EWKISRweightFromISRpT( 25.0)*EWKpTBin_sample[0] +
                          EWKISRweightFromISRpT( 75.0)*EWKpTBin_sample[1] +
                          EWKISRweightFromISRpT(125.0)*EWKpTBin_sample[2] +
                          EWKISRweightFromISRpT(175.0)*EWKpTBin_sample[3] +
                          EWKISRweightFromISRpT(250.0)*EWKpTBin_sample[4] +
                          EWKISRweightFromISRpT(350.0)*EWKpTBin_sample[5] +
                          EWKISRweightFromISRpT(500.0)*EWKpTBin_sample[6] +
                          EWKISRweightFromISRpT(700.0)*EWKpTBin_sample[7]  );
      TVectorD ISRCParams(2);
      ISRCParams[0] = ISRCParam_sample;
      ISRCParams[1] = EWKISRCParam_sample;
      ISRCParams.Write(("sample_"+sample.tag()+"_ISRCParams").c_str());

      sampleNVTX.Write();
      sampleNTrue.Write();

      TH1D* samplePUweight = static_cast<TH1D*>(dataPU->Clone(("sample_"+sample.tag()+"_puWeight").c_str()));

      sampleNTrue.Scale(1/sampleNTrue.Integral());
      sampleNVTX.Scale(1/sampleNVTX.Integral());
      samplePUweight->Divide(&sampleNTrue); // This is the recommended, but for some reason it is giving weird results
      //samplePUweight->Divide(&sampleNVTX); // Trying this one instead
      samplePUweight->Write();

    }
    double ISRCParam_process = (                nIsrBin_process[0] +
                                                nIsrBin_process[1] +
                                                nIsrBin_process[2] +
                                                nIsrBin_process[3] +
                                                nIsrBin_process[4] +
                                                nIsrBin_process[5] +
                                                nIsrBin_process[6]  ) /
    static_cast<double>(ISRweightFromNISRJet(0)*nIsrBin_process[0] +
                        ISRweightFromNISRJet(1)*nIsrBin_process[1] +
                        ISRweightFromNISRJet(2)*nIsrBin_process[2] +
                        ISRweightFromNISRJet(3)*nIsrBin_process[3] +
                        ISRweightFromNISRJet(4)*nIsrBin_process[4] +
                        ISRweightFromNISRJet(5)*nIsrBin_process[5] +
                        ISRweightFromNISRJet(6)*nIsrBin_process[6]  );
    double EWKISRCParam_process = (                  EWKpTBin_process[0] +
                                                     EWKpTBin_process[1] +
                                                     EWKpTBin_process[2] +
                                                     EWKpTBin_process[3] +
                                                     EWKpTBin_process[4] +
                                                     EWKpTBin_process[5] +
                                                     EWKpTBin_process[6] +
                                                     EWKpTBin_process[7]  ) /
    static_cast<double>(EWKISRweightFromISRpT( 25.0)*EWKpTBin_process[0] +
                        EWKISRweightFromISRpT( 75.0)*EWKpTBin_process[1] +
                        EWKISRweightFromISRpT(125.0)*EWKpTBin_process[2] +
                        EWKISRweightFromISRpT(175.0)*EWKpTBin_process[3] +
                        EWKISRweightFromISRpT(250.0)*EWKpTBin_process[4] +
                        EWKISRweightFromISRpT(350.0)*EWKpTBin_process[5] +
                        EWKISRweightFromISRpT(500.0)*EWKpTBin_process[6] +
                        EWKISRweightFromISRpT(700.0)*EWKpTBin_process[7]  );
    TVectorD ISRCParams(2);
    ISRCParams[0] = ISRCParam_process;
    ISRCParams[1] = EWKISRCParam_process;
    ISRCParams.Write(("process_"+process.tag()+"_ISRCParams").c_str());

    processNVTX.Write();
    processNTrue.Write();

    TH1D* processPUweight = static_cast<TH1D*>(dataPU->Clone(("process_"+process.tag()+"_puWeight").c_str()));
    processNTrue.Scale(1/processNTrue.Integral());
    processNVTX.Scale(1/processNVTX.Integral());
    processPUweight->Divide(&processNTrue);
    //processPUweight->Divide(&processNVTX);
    processPUweight->Write();

    if(dataPUUp != nullptr)
    {
      TH1D* processPUweightUp = static_cast<TH1D*>(dataPUUp->Clone(("process_"+process.tag()+"_puWeight_Up").c_str()));
      TH1D* processPUweightDown = static_cast<TH1D*>(dataPUDown->Clone(("process_"+process.tag()+"_puWeight_Down").c_str()));

      processPUweightUp->Divide(&processNTrue);
      processPUweightDown->Divide(&processNTrue);

      processPUweightUp->Write();
      processPUweightDown->Write();
    }
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
