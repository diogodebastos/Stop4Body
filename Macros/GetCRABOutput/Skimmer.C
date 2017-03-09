#include <iostream>

#include <TROOT.h>
#include <TString.h>
#include <TFile.h>
#include <TTree.h>

void Skimmer(TString cname)
{
  TFile* file=TFile::Open(cname + "/treeProducerStop4Body/tree.root", "read");
  TString cutString = "nLepGood >= 1"; // Place custom cut string for the skim here

  if(!file)
  {
    std::cout << "Corrupted file! : " << cname << std::endl;
    return;
  }

  TTree* tree = static_cast<TTree*>(file->Get("tree"));
  if(!tree)
  {
    std::cout << "Corrupted file! : " << cname << std::endl;
    return;
  }

  TFile* ofile = new TFile(cname + "/treeProducerStop4Body/tree_skimmed.root", "recreate");
  TTree* otree = tree->CopyTree(cutString);

  ofile->Write();
  ofile->Close();

  gROOT->ProcessLine(".q");
}
