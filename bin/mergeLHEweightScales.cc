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
#include "TKey.h"
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
#include "UserCode/Stop4Body/interface/commonFunctions.h"
#include "UserCode/Stop4Body/interface/doubleWithUncertainty.h"

#define LHEWEIGHT_LIMIT  500

int main(int argc, char** argv)
{
  std::string outFile = "";
  std::vector<std::string> inFiles;
  inFiles.clear();

  if(argc < 3)
  {
    std::cout << "You did not pass enough parameters" << std::endl;
    //printHelp();
    return 0;
  }

  outFile = argv[1];

  for(int i = 2; i < argc; ++i)
  {
    std::string argument = argv[i];

    if(fileExists(argument))
      inFiles.push_back(argument);
    else
    {
      std::cout << "The file " << argument << " does not exist." << std::endl;
    }
  }

  if(outFile == "")
  {
    std::cout << "You must define an output file" << std::endl;
    return 1;
  }

  if(inFiles.size() <= 0)
  {
    std::cout << "You must define one or more files to be merged" << std::endl;
    return 1;
  }

  std::cout << "Merging input files into output file:" << std::endl;
  std::cout << "  Output: " << outFile << std::endl;
  for(size_t i = 0; i < inFiles.size(); ++i)
  {
    std::cout << "  Input[" << i << "]: " << inFiles[i] << std::endl;
  }

  gInterpreter->GenerateDictionary("map<int, double>","map");
  TClass *mapClass = gROOT->FindSTLClass("std::map<int,double>", true);
  TFile foutput((outFile).c_str(), "RECREATE");

  for(auto& fileName: inFiles)
  {
    TFile file(fileName.c_str(), "READ");
    foutput.cd();

    TIter next(file.GetListOfKeys());
    TKey *key;

    while( (key = (TKey*)next()) )
    {
      std::cout << "Getting " << key->GetName();
      std::map<int, double> *tmpMap;

      tmpMap = static_cast<std::map<int, double>*>(key->ReadObjectAny(mapClass));

      std::cout << " with " << tmpMap->size() << " entries" << std::endl;

      foutput.WriteObjectAny(&tmpMap, mapClass, key->GetName());
    }

    break;
  }

  return 0;
}
