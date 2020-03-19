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

void eraseFileLine(std::string, std::string);

int main(int argc, char** argv){

  std::string inputDirectory = "";
  std::string outputDirectory = "./OUT/";

  for(int i = 1; i < argc; ++i)
  {
    std::string argument = argv[i];

    if(argument == "--inDir")
      inputDirectory = argv[++i];

    if(argument == "--outDir")
      outputDirectory = argv[++i];
  }

  TFile file((inputDirectory+"/tree.root").c_str(),"");
  TTree* lumiTree = static_cast<TTree*>(file.Get("LuminosityBlocks"));

  size_t nentries = static_cast<size_t>(lumiTree->GetEntries());
  std::cout << inputDirectory  <<" has:  " << nentries << " events." << std::endl;

  UInt_t run;
  UInt_t luminosityBlock;
  lumiTree->SetBranchAddress("run", &run);
  lumiTree->SetBranchAddress("luminosityBlock", &luminosityBlock);
/*
  std::cout << "For DEBUG" << std::endl;
  UInt_t debugBegin = 0;
  UInt_t debugLength = nentries;//nentries;
  for(size_t i = debugBegin; i < debugBegin+debugLength; i++)
  {
    lumiTree->GetEntry(i);
    std::cout << " Entry: " << i << "  " << run << " : " << luminosityBlock << std::endl;
  }
  std::cout << std::endl;
*/
  lumiTree->GetEntry(0);

  UInt_t lastRun = run;
  UInt_t lastLumiBlock = luminosityBlock-1;
  UInt_t lastLastLumiBlock = luminosityBlock-2;
  UInt_t firstLumi = 1;

  std::ofstream jsonFile;
  jsonFile.open ((outputDirectory+"/lumiSummary.json").c_str());
  jsonFile << "{" << '"' << lastRun << '"' << ": [[" << luminosityBlock << ",";

  for(size_t i = 0; i < nentries; i++)
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
        else if ((luminosityBlock-lastLumiBlock)!=1) {
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
      jsonFile << "\n " << '"' << run << '"' << ": [";
      firstLumi=0;
    }
    lastRun=run;
    lastLastLumiBlock=lastLumiBlock;
    lastLumiBlock=luminosityBlock;
  }
  if ((lastLumiBlock-lastLastLumiBlock)==1) {
    jsonFile << lastLumiBlock << "]]}";
  }
  else{
    //jsonFile << "[" << lastLumiBlock << "]]}";
  }
  jsonFile << std::endl;
  jsonFile.close();
  eraseFileLine(outputDirectory,": [],");
}


//https://stackoverflow.com/questions/26576714/deleting-specific-line-from-file
void eraseFileLine(std::string path, std::string eraseLine) {
  std::string line;
  std::ifstream fin;

  fin.open(path+"/lumiSummary.json");
  std::ofstream temp;
  temp.open(path+"/temp.json");

  while (getline(fin, line)) {
      if (line.substr( line.length()-5) != eraseLine){
        temp << line << std::endl;
      }
  }

  temp.close();
  fin.close();

  std::string phStr = path+"/lumiSummary.json";
  const char * p = phStr.c_str(); // required conversion for remove and rename functions
  remove(p);
  rename((path+"/temp.json").c_str(), p);
}
