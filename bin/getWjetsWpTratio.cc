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
#include "UserCode/Stop4Body-nanoAOD/interface/ValueWithSystematics.h"

#define GENPART_LIMIT    40

using json = nlohmann::json;

struct FileInfo
{
  std::string path;
  double crossSection;
  double branchingRatio;
  std::string tag;
};

int main(int argc, char** argv)
{
  std::string jsonFileName = "";
  std::string outputDirectory = "./OUT/";
  std::string jsonFileNameWJets16 = "/lstore/cms/dbastos/REPOS/Stop4Body/CMSSW_8_0_14/src/UserCode/Stop4Body-nanoAOD/Macros/JSON/2016/WJets.json";


  if(argc < 2)
  {
    std::cout << "You did not pass enough parameters" << std::endl;
    printHelp();
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
  SampleReader samples(jsonFileNameWJets16);
  std::string selection = "";


  for(auto &process : samples)
  {
    std::cout << "Processing process: " << process.tag() << std::endl;
    for(auto &sample : process)
    {
      std::cout << "\tProcessing sample: " << sample.tag() << std::endl;
      for(auto &file : sample)
      {
      	TFile finput(file.c_str(), "READ");
      	TTree *inputtree;
      	inputtree = static_cast<TTree*>(finput.Get("Events"));
      	UInt_t nGenPart = Events->SetBranchAddress("nGenPart", &nGenPart);
      	Int_t GenPart_pdgId[40] = Events->SetBranchAddress("GenPart_pdgId", GenPart_pdgId);
      	size_t nentries = static_cast<size_t>(inputtree->GetEntries());
      	std::cout << "\t    The file has " << nentries << " events." << std::endl;
      	/*
      	for(size_t i = 0; i < nentries; i++)
      	{
      		inputtree->GetEntry(i);
      	}
      	*/
      	inputtree->GetEntry(1);
		for(UInt_t genPartIndex = 0; genPartIndex < nGenPart; ++genPartIndex)
		{
			std::cout << "pdgId: " << GenPart_pdgId[genPartIndex] << std::endl;
		}
      }
  	}
  }
}