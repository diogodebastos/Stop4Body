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

  TFile foutput((outputDirectory + "/preProcessSamples_" + getBaseName(jsonFileName) + ".root").c_str(), "RECREATE");

  for(auto &process : samples)
  {
    std::cout << "Processing process: " << process.tag() << std::endl;

    int nIsrBin_process[7]{0, 0, 0, 0, 0, 0, 0};
    int EWKpTBin_process[8]{0, 0, 0, 0, 0, 0, 0, 0};

    for(auto &sample : process)
    {
      std::cout << "\tProcessing sample: " << sample.tag() << std::endl;

      int nIsrBin_sample[7]{0, 0, 0, 0, 0, 0, 0};
      int EWKpTBin_sample[8]{0, 0, 0, 0, 0, 0, 0, 0};

      double sampleSumGenWeight = 0;
      Double_t thisSumGenWeight = 0;

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

        TTree *inputruntree;
        inputruntree = static_cast<TTree*>(finput.Get("Runs"));

        Int_t thisNevt = static_cast<Int_t>(inputtree->GetEntries());

        // Get sumGenWeight
        thisSumGenWeight = 0;
        inputruntree->SetBranchAddress("genEventSumw", &thisSumGenWeight);
        inputruntree->GetEntry(0);
        sampleSumGenWeight += thisSumGenWeight;

        //Get ISR Params
        Float_t nIsr;        inputtree->SetBranchAddress("nIsr", &nIsr);
        Float_t met_pt;      inputtree->SetBranchAddress("MET_pt"    , &met_pt);
        Float_t met_phi;     inputtree->SetBranchAddress("MET_phi",   &met_phi);
        UInt_t nLepGood;     inputtree->SetBranchAddress("nLepGood"   , &nLepGood);
        Float_t LepGood_pt[LEPCOLL_LIMIT];  inputtree->SetBranchAddress("LepGood_pt", &LepGood_pt);
        Float_t LepGood_phi[LEPCOLL_LIMIT];  inputtree->SetBranchAddress("LepGood_phi", &LepGood_phi);

        for(Int_t i = 0; i < thisNevt; ++i)
        {
          inputtree->GetEntry(i);

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
          delete inputtree;
          delete inputruntree;
        }
      }

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
