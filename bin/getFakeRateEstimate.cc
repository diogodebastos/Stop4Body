#include <iostream>
#include <vector>
#include <sstream>
#include <map>
#include <fstream>

#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TStyle.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TLegend.h>
#include <TPaveText.h>
#include <TGraphErrors.h>

#include "UserCode/Stop4Body/interface/json.hpp"
#include "UserCode/Stop4Body/interface/SampleReader.h"
#include "UserCode/Stop4Body/interface/doubleWithUncertainty.h"
#include "UserCode/Stop4Body/interface/ValueWithSystematics.h"

using json = nlohmann::json;

int main(int argc, char** argv)
{
  std::string debug;
  double luminosity = -1;
  // Placeholder for ${JSON_PATH}
  std::string jsonFileName = "/lstore/cms/dbastos/REPOS/Stop4Body/CMSSW_8_0_14/src/UserCode/Stop4Body/Macros/JSON/2017/DataJetHT.json";
  // Placeholder for ${INPUT}
  std::string inputDirectory = "/lstore/cms/dbastos/Stop4Body/nTuples_v2018-12-14/";
  std::string suffix = "";
  
  SampleReader samples(jsonFileName, inputDirectory, suffix);
  auto Data = samples.getData();
  
  luminosity = Data.getLumi();
  
  std::stringstream converter;
  converter << "luminosity: " << luminosity;
  converter >> debug;
  std::cout << debug << std::endl;
  
  return 0;
}