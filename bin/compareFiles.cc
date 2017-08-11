#include <iostream>
#include <vector>

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"

#include "UserCode/Stop4Body/interface/commonFunctions.h"

int main(int argc, char** argv)
{
  std::string fileName1;
  std::string fileName2;
  std::string selection = "(DPhiJet1Jet2 < 2.5 || Jet2Pt < 60) && (HT > 200) && (Met > 280) && (isTight == 1) && (Jet1Pt > 110)";

  if(argc != 3)
  {
    std::cout << "You did not pass the right number of parameters" << std::endl;
    return 0;
  }

  fileName1 = argv[1];
  fileName2 = argv[2];

  std::cout << "File1: " << fileName1 << std::endl;
  std::cout << "File2: " << fileName2 << std::endl;

  if(!fileExists(fileName1))
  {
    std::cout << "File1 does not exist" << std::endl;
    return 1;
  }
  if(!fileExists(fileName2))
  {
    std::cout << "File2 does not exist" << std::endl;
    return 1;
  }

  TFile file1(fileName1.c_str(), "READ");
  TFile file2(fileName2.c_str(), "READ");
  TFile tmpErr("tmp.root", "RECREATE"); // Because root does not like memory resident trees

  TTree* tree1 = static_cast<TTree*>(file1.Get("bdttree"));
  TTree* tree2 = static_cast<TTree*>(file2.Get("bdttree"));

  if(tree1 == nullptr)
  {
    std::cout << "Unable to find the tree for File1" << std::endl;
    return 1;
  }
  if(tree2 == nullptr)
  {
    std::cout << "Unable to find the tree for File2" << std::endl;
    return 1;
  }

  TTree* tree1Filtered = tree1->CopyTree(selection.c_str());
  TTree* tree2Filtered = tree2->CopyTree(selection.c_str());

  std::vector<std::string> branches = {
    "Jet2Pt",
    "Jet1Pt",
    "mt",
    "Met",
    "LepChg",
    "LepEta",
    "LepPt",
    "HT",
    "NbLoose",
    "Njet",
    "JetHBpt",
    "DrJetHBLep",
    "JetHBCSV"
  };

  for(auto& branch : branches)
  {
    TH1D tmp1("tmp1", "tmp1", 40, 0, 100);
    TH1D tmp2("tmp2", "tmp2", 40, 0, 100);

    tree1Filtered->Draw((branch+">>tmp1").c_str(), "weight", "goff");
    tree2Filtered->Draw((branch+">>tmp2").c_str(), "weight", "goff");

    std::cout << " " << branch << ": ";
    std::cout << tmp1.GetMean() << "+-" << tmp1.GetRMS() << " (" << tmp1.GetEntries() << ") vs ";
    std::cout << tmp2.GetMean() << "+-" << tmp2.GetRMS() << " (" << tmp2.GetEntries() << ")" << std::endl;
  }

  return 0;
}
