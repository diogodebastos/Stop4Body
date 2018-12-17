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
  double luminosity = -1;
  std::string jsonFileName = ${JSON_PATH}/DataJetHT.json;
  std::string inputDirectory = ${INPUT};
  std::string suffix = "";
  
  SampleReader samples(jsonFileName, inputDirectory, suffix);
  auto Data = samples.getData();
  
  luminosity = Data.getLumi();
  
  std::stringstream converter;
  converter << "luminosity: " << luminosity;
  
  std::cout << converter << std::endl;
  
  return 0;
}