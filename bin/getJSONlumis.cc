#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <sstream>
#include <map>

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TCut.h"
#include "TChain.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "string"
#include "vector"

void fixFile(std::string);
bool cleanFile(std::string);

int main(int argc, char** argv){

  std::string inputDirectory = "";
  std::string outputDirectory = "./OUT/";
  bool debug = false;

  for(int i = 1; i < argc; ++i)
  {
    std::string argument = argv[i];

    if(argument == "--inDir")
      inputDirectory = argv[++i];

    if(argument == "--outDir")
      outputDirectory = argv[++i];
 
    if(argument == "--debug")
      debug = true;
  }

  TFile file((inputDirectory+"/tree.root").c_str(),"");
  TTree* lumiTree = static_cast<TTree*>(file.Get("LuminosityBlocks"));

  size_t nentries = static_cast<size_t>(lumiTree->GetEntries());
  std::cout << inputDirectory  <<" has:  " << nentries << " events." << std::endl;

  UInt_t run;
  UInt_t luminosityBlock;
  lumiTree->SetBranchAddress("run", &run);
  lumiTree->SetBranchAddress("luminosityBlock", &luminosityBlock);
  UInt_t debugBegin = 0;
  UInt_t debugEnd = 1000;

  if (debug) {
    std::cout << "For DEBUG" << std::endl;
    for(size_t i = debugBegin; i < debugEnd; i++)
    {
      lumiTree->GetEntry(i);
      std::cout << " Entry: " << i << "  " << run << " : " << luminosityBlock << std::endl;
    }
    std::cout << std::endl;
  }
  lumiTree->GetEntry(0);

  UInt_t lastRun = run;
  UInt_t lastLastRun = run;
  UInt_t lastLumiBlock = luminosityBlock-1;
  UInt_t lastLastLumiBlock = luminosityBlock-2;
  UInt_t firstLumi = 1;

  std::ofstream jsonFile;
  jsonFile.open ((outputDirectory+"/lumiSummary.json").c_str());
  jsonFile << "{" << '"' << lastRun << '"' << ": [[" << luminosityBlock << ",";

  for(size_t i = debugBegin; i < debugEnd; i++)
  {
    lumiTree->GetEntry(i);
    if (lastRun==run) {
      if ((luminosityBlock-lastLumiBlock)!=1 || (lastLumiBlock-lastLastLumiBlock)!=1) {
        if ((luminosityBlock-lastLumiBlock)!=1 && (lastLumiBlock-lastLastLumiBlock)!=1) {
          //jsonFile << "[" << lastLumiBlock << "],";
        }
        else if ((lastLumiBlock-lastLastLumiBlock)!=1) {
          if (firstLumi==0) {
            jsonFile << "[" << lastLumiBlock << ", ";
          }
          else{
            jsonFile << ",[" << lastLumiBlock << ", ";
          }
          ++firstLumi;
        }
        else if ((luminosityBlock-lastLumiBlock)!=1 && lastLastRun == lastRun) {
          jsonFile << lastLumiBlock << "]";
        }
      }
    }
    else{
      if ((lastLumiBlock-lastLastLumiBlock)==1) {
        jsonFile << lastLumiBlock << "]], ";
      }
      else if ((luminosityBlock-lastLumiBlock)!=1) {
        jsonFile << "],";
      }
      else if ((luminosityBlock-lastLumiBlock)==1 && (lastLumiBlock-lastLastLumiBlock)!=1) {
        jsonFile << "],";
      }
      jsonFile << "\n " << '"' << run << '"' << ": [";
      firstLumi=0;
    }
    lastLastRun=lastRun;
    lastRun=run;
    lastLastLumiBlock=lastLumiBlock;
    lastLumiBlock=luminosityBlock;
  }
  if ((lastLumiBlock-lastLastLumiBlock)==1) {
    jsonFile << lastLumiBlock << "]]}";
  }
  else{
    //jsonFile << "[" << lastLumiBlock << "]]}";
    jsonFile << "]}";
  }
  jsonFile.close();
  bool removeLastLine = cleanFile(outputDirectory);
  
  if (removeLastLine){
    fixFile(outputDirectory);
  }

}

void fixFile(std::string path) {
  std::string line;
  std::ifstream fin;

  fin.open(path+"/lumiSummary.json");
  std::ofstream temp;
  temp.open(path+"/temp.json");

  UInt_t nLines=0;
  UInt_t counter=0;

  while (getline(fin, line)) {
    ++nLines;  
  }
  fin.close();
  fin.open(path+"/lumiSummary.json");

  while (getline(fin, line)) {
    ++counter;
    if (counter == nLines - 1) {
      temp << line.substr(0, line.length()-2) << "}" << std::endl; 
    }
    else if (counter == nLines){
      temp << "";
    }
    else {
      temp << line << std::endl;
    }
  }
 
  temp.close();
  fin.close();

  std::string phStr = path+"/lumiSummary.json";
  const char * p = phStr.c_str();
  remove(p);
  rename((path+"/temp.json").c_str(), p);
}

//https://stackoverflow.com/questions/26576714/deleting-specific-line-from-file
bool cleanFile(std::string path) {
  std::string line;
  std::ifstream fin;

  fin.open(path+"/lumiSummary.json");
  std::ofstream temp;
  temp.open(path+"/temp.json");

  bool fixLastLine = false;
  while (getline(fin, line)) {
    if (line.length() > 5 ){
      if ((line.substr( line.length()-5) == ": [],") || (line.substr( line.length()-4) == ": []") || (line.substr( line.length()-3) == ": [") ){
        temp << "";
      }
      else{
        if(line.substr(line.length()-5) == ": []}"){
          fixLastLine = true;
        }
        temp << line << std::endl;
      }
    }
  }

  temp.close();
  fin.close();

  std::string phStr = path+"/lumiSummary.json";
  const char * p = phStr.c_str();
  remove(p);
  rename((path+"/temp.json").c_str(), p);
  return fixLastLine;
}
