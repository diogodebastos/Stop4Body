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
#include "UserCode/Stop4Body/interface/commonFunctions.h"
#include "UserCode/Stop4Body/interface/doubleWithUncertainty.h"

#define LEPCOLL_LIMIT  40

using json = nlohmann::json;

bool replace(std::string&, const std::string&, const std::string&);

int main(int argc, char** argv)
{
  gInterpreter->GenerateDictionary("map<int, double>","map");
  TFile test("test.root", "RECREATE");

  std::map<int,double> myMap = { {1, 1.02}, {2, 0.9} };
  TClass *c = gROOT->FindSTLClass("std::map<int,double>", true);

  test.WriteObjectAny(&myMap, c, "myMap");

  return 0;

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
  TFile foutput((outputDirectory + "/lheWeights_" + getBaseName(jsonFileName) + ".root").c_str(), "RECREATE");

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
