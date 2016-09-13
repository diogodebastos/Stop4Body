#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TChain.h>
#include <TCut.h>
#include <TString.h>
#include <TObject.h>
#include <TRandom3.h>
#include <TDirectory.h>
#include <TStyle.h>

#include "TMVA/Factory.h"
#include "TMVA/Tools.h"

#include <iostream>
#include <vector>
#include <sstream>
#include <map>
#include <fstream>

#include "UserCode/Stop4Body/interface/json.hpp"
#include "UserCode/Stop4Body/interface/SampleReader.h"

using json = nlohmann::json;

struct FileInfo
{
  std::string path;
  double crossSection;
  double branchingRatio;
  std::string tag;
};

class ProcessInfo
{
public:
  ProcessInfo(): _tag(""), _label(""), _color(0), _lcolor(0), _lwidth(0), _lstyle(0), _fill(0), _marker(0), _isdata(false), _issignal(false), _isfastsim(false), _spimpose(false) {};

  std::string & tag() {return _tag;};
  std::string & label() {return _label;};
  int & color() {return _color;};
  int & lcolor() {return _lcolor;};
  int & lwidth() {return _lwidth;};
  int & lstyle() {return _lstyle;};
  int & fill() {return _fill;};
  int & marker() {return _marker;};
  bool & isdata() {return _isdata;};
  bool & issignal() {return _issignal;};
  bool & isfastsim() {return _isfastsim;};
  bool & spimpose() {return _spimpose;};

  std::vector<FileInfo> & files() {return _files;};

private:
  std::string _tag;
  std::string _label;
  int _color;
  int _lcolor;
  int _lwidth;
  int _lstyle;
  int _fill;
  int _marker;
  bool _isdata;
  bool _issignal;
  bool _isfastsim;
  bool _spimpose;

  std::vector<FileInfo> _files;
};

void printHelp();
bool fileExists(std::string);

int main(int argc, char** argv)
{
  std::string jsonFileName = "";
  std::string inputDirectory = "";
  std::string outputDirectory = "./OUT/";
  std::string suffix = "";
  std::string variablesJson = "";
  double luminosity = 5000;

  if(argc < 2)
  {
    std::cout << "You did not pass enough parameters" << std::endl;
    printHelp();
    return 0;
  }

  for(int i = 1; i < argc; ++i)
  {
    std::string argument = argv[i];

    if(argument == "--help")
    {
      printHelp();
      return 0;
    }

    if(argument == "--json")
      jsonFileName = argv[++i];

    if(argument == "--outDir")
      outputDirectory = argv[++i];

    if(argument == "--inDir")
      inputDirectory = argv[++i];

    if(argument == "--suffix")
      suffix = argv[++i];

    if(argument == "--variables")
      variablesJson = argv[++i];

    if(argument == "--lumi")
    {
      std::stringstream convert;
      convert << argv[++i];
      convert >> luminosity;
    }
  }

  if(jsonFileName == "")
  {
    std::cout << "You must define a json file" << std::endl;
    return 1;
  }

  if(variablesJson == "")
  {
    std::cout << "You must define a json file with the variables to plot" << std::endl;
    return 1;
  }

  if(inputDirectory == "")
  {
    std::cout << "You must define an input directory" << std::endl;
    return 1;
  }

  std::cout << "Reading json file" << std::endl;
  SampleReader samples(jsonFileName, inputDirectory, suffix);


  gStyle->SetOptStat(000000);
  TCut muon = "(nGoodMu==1)";
  TCut electron = "(nGoodEl==1)";
  TCut singlep = muon||electron;
  TCut ISRjet = "Jet1Pt > 110.";
  TCut met    = "Met>300.";
  TCut dphij1j2 = "DPhiJet1Jet2 < 2.5";
  TCut presel = singlep+ISRjet+dphij1j2+met;

  std::string mcWeight;
  {
    std::stringstream converter;
    converter << "XS*" << luminosity << "/Nevt";
    converter >> mcWeight;
  }

  auto MC = samples.getMCBkg();
  auto Sig = samples.getMCSig();
  auto Data = samples.getData();

  std::cout << "Total MC yield: " << MC.getYield() << std::endl;
  for(auto & sample : MC)
    std::cout << "\t" << sample.label() << " yield: " << sample.getYield() << std::endl;
  std::cout << "Total Data yield: " << Data.getYield() << std::endl;
  std::cout << "Total Signal yield: " << Sig.getYield() << std::endl;





  return 0;
}

void printHelp()
{
  std::cout << "The program has the following parameters:" << std::endl;
  std::cout << "\t--help\t- Print this message" << std::endl;

  return;
}

bool fileExists(std::string fileName)
{
  std::ifstream infile(fileName);
  return infile.good();
}
