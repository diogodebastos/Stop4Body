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
  std::string inputDirectory = "./IN/";
  std::string outputDirectory = "./OUT/";
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
    return 0;
  }

  std::cout << "Reading json file" << std::endl;
  json jsonFile;
  std::ifstream inputFile(jsonFileName);
  inputFile >> jsonFile;

  if(jsonFile.find("lines") == jsonFile.end())
  {
    std::cout << "The specified json file is not a valid sample descriptor" << std::endl;
    return 1;
  }

  std::map<std::string,ProcessInfo> processes;
  std::vector<std::string> emptyLines;
  std::vector<std::string> emptyPath;
  std::vector<std::string> invalidPath;

  for(auto &process : jsonFile["lines"])
  {
    if(process.find("files") != process.end())
    {
      ProcessInfo thisProcess;

      thisProcess.tag() = process["tag"];
      thisProcess.color() = process["color"];
      thisProcess.lcolor() = process["lcolor"];
      thisProcess.label() = process["label"];

      if(process.count("isdata") != 0)
        thisProcess.isdata() = process["isdata"];
      if(process.count("issignal") != 0)
        thisProcess.issignal() = process["issignal"];
      if(process.count("spimpose") != 0)
        thisProcess.spimpose() = process["spimpose"];
      if(process.count("lwidth") != 0)
        thisProcess.lwidth() = process["lwidth"];
      if(process.count("lstyle") != 0)
        thisProcess.lstyle() = process["lstyle"];
      if(process.count("fill") != 0)
        thisProcess.fill() = process["fill"];
      if(process.count("marker") != 0)
        thisProcess.marker() = process["marker"];
      if(process.count("isfastsim") != 0)
        thisProcess.isfastsim() = process["isfastsim"];

      for(auto &file : process["files"])
      {
        std::string path = inputDirectory + "/";
        path += file["tag"];
        path += "_bdt.root";
        std::string identifier = process["tag"];
        identifier += ":";
        identifier += file["tag"];
        if(path == "")
          emptyPath.push_back(identifier);
        else
        {
          if(fileExists(path))
          {
            FileInfo tmpFile;
            tmpFile.path = path;
            tmpFile.crossSection = file["xsec"];
            tmpFile.branchingRatio = file["br"];
            tmpFile.tag = file["tag"];
            thisProcess.files().push_back(tmpFile);
          }
          else
            invalidPath.push_back(identifier + "  ->  " + path);
        }
      }

      if(thisProcess.files().size() != 0)
      {
        processes[thisProcess.tag()] = thisProcess;
      }
    }
    else
      emptyLines.push_back(process["tag"]);
  }

  std::cout << "Found " << processes.size() << " processes with valid files" << std::endl;

  gStyle->SetOptStat(000000);
  TCut muon = "(nGoodMu==1)";
  TCut electron = "(nGoodEl==1)";
  TCut singlep = muon||electron;
  TCut ISRjet = "Jet1Pt > 110.";
  TCut met    = "Met>300.";
  TCut dphij1j2 = "DPhiJet1Jet2 < 2.5";
  TCut presel = singlep+ISRjet+dphij1j2+met;




  if(emptyLines.size() != 0)
  {
    std::cout << "The following lines did not have any files associated to them: ";
    for(auto &process : emptyLines)
      std::cout << process << "; ";
    std::cout << std::endl;
  }

  if(emptyPath.size() != 0)
  {
    std::cout << "The following files did not have any path defined, or the defined path was empty: ";
    for(auto &file : emptyPath)
      std::cout << file << "; ";
    std::cout << std::endl;
  }

  if(invalidPath.size() != 0)
  {
    std::cout << "The following files could not be found: " << std::endl;
    for(auto &file : invalidPath)
      std::cout << "\t" << file << std::endl;
    std::cout << std::endl;
  }

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
