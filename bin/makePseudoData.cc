#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TChain.h>
#include <TCut.h>
#include <TString.h>
#include <TObject.h>
#include <TRandom3.h>

#include <iostream>
#include <vector>
#include <sstream>
#include <map>

#include "json.hpp"

using json = nlohmann::json;

struct FileInfo
{
  std::string path;
  double crossSection;
  double branchingRatio;
  std::string tag;
};

void GetYield(
	      double lumi,     // Input: Luminosity
	      std::string pc,       // Input: Name of the channel
	      TCut Coupure,    // Input: Analysis cut
	      double& Yield   // Output: Weighted yield of the channel
	      );
void SetBranchAddress(TString varname, float *varaddress, TTree* tree1, TTree* tree2, TTree* tree3, TTree* tree4, TTree* tree5);
void SetBranchAddressI(TString varname, int *varaddress, TTree* tree1, TTree* tree2, TTree* tree3, TTree* tree4, TTree* tree5);
std::string convertDouble(double value);
void printHelp();
bool fileExists(std::string);

int main(int argc, char** argv)
{
  std::string jsonFileName = "";
  std::string outputDirectory = "./OUT/";
  std::string inputDirectory = "./OUT/";
  double luminosity = 10000;

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
      std::stringstream conv;
      conv << argv[++i];
      conv >> luminosity;
    }
  }

  if(jsonFileName == "")
  {
    std::cout << "You must define a json file" << std::endl;
  }
  std::cout << "Producing PseudoData from the file '" << jsonFileName << "' with an integrated luminosity of " << luminosity << " fb" <<std::endl;

  json jsonFile;
  std::ifstream inputFile(jsonFileName);
  inputFile >> jsonFile;

  if(jsonFile.find("lines") == jsonFile.end())
  {
    std::cout << "The specified json file is not a valid sample descriptor" << std::endl;
    return 1;
  }

  std::vector<FileInfo> filesToProcess;
  std::vector<std::string> emptyLines;
  std::vector<std::string> emptyPath;
  std::vector<std::string> invalidPath;

  for(auto &process : jsonFile["lines"])
  {
    if(process.find("files") != process.end())
    {
      for(auto &file : process["files"])
      {
        //std::string path = file["path"];
        std::string path = inputDirectory + "/" + file["tag"] + "_bdt.root";
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

            filesToProcess.push_back(tmpFile);
          }
          else
            invalidPath.push_back(identifier + "  ->  " + path);
        }
      }
    }
    else
      emptyLines.push_back(process["tag"]);
  }


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
  std::cout << "\t--json\t- Used to specify the json file which describes the samples, this option is obligatory. The parameter immediately after this one should be the desired json file" << std::endl;
  std::cout << "\t--inDir\t- Used to specify the input directory. The parameter immediately after this one should be the desired directory. By default the directory './OUT'" << std::endl;
  std::cout << "\t--outDir\t- Used to specify the output directory. The parameter immediately after this one should be the desired directory. By default the directory './OUT/' is used" << std::endl;

  return;
}

bool fileExists(std::string fileName)
{
  std::ifstream infile(fileName);
  return infile.good();
}

/*void PseudoData(){

  gROOT->ProcessLine(".L MapLoader.C+");

  using std::cout;

  TDirectory* cwd = gDirectory;
  TFile *TTfile = new TFile("SET5/TTJets_bdt.root");
  TFile *W1file = new TFile("SET5/WJets_HT100to200_bdt.root");
  TFile *W2file = new TFile("SET5/WJets_HT200to400_bdt.root");
  TFile *W3file = new TFile("SET5/WJets_HT400to600_bdt.root");
  TFile *W4file = new TFile("SET5/WJets_HT600toInf_bdt.root");
  cwd->cd();

  const double Lumi = 2300.;

  // MC Triggers
  TCut trgM   = "Event>-999.";
  // Data Triggers
  TCut trgD   = "Event>-999.";

  // Lepton selection
  // Muon channel
  TCut muon = "(abs(LepID)==13)&&(LepIso<0.2)&&(LepPt<30.)";
  // Electron channel
  TCut electron = "(abs(LepID)==11)&&(LepIso<0.2)&&(LepPt<30.)";
  // Both channels
  TCut emu = muon||electron;
  // Decide
  TCut singlep = emu;
 
  // MET
  TCut met    = "Met>100.";

  // Jets
  TCut ISRjt = "Jet1Pt > 110.";

  // Selection
  //  TCut Coup   = singlep;
  TCut Coup   = singlep+ISRjt;
  //  TCut Coup   = singlep+met+ISRjt;


  // Get the trees as provided...
  TTree* TTtreetemp = (TTree*)TTfile->Get("bdttree");
  TTree* W1treetemp = (TTree*)W1file->Get("bdttree");
  TTree* W2treetemp = (TTree*)W2file->Get("bdttree");
  TTree* W3treetemp = (TTree*)W3file->Get("bdttree");
  TTree* W4treetemp = (TTree*)W4file->Get("bdttree");
  // ...to make new ones with cut
  TTree* TTtree = (TTree*)TTtreetemp->CopyTree(Coup);
  TTree* W1tree = (TTree*)W1treetemp->CopyTree(Coup);
  TTree* W2tree = (TTree*)W2treetemp->CopyTree(Coup);
  TTree* W3tree = (TTree*)W3treetemp->CopyTree(Coup);
  TTree* W4tree = (TTree*)W4treetemp->CopyTree(Coup);

  // Calculate yields of processes with same cut as above
  double Ytt, Yw1, Yw2, Yw3, Yw4;
  GetYield(Lumi, "TTbar", Coup, Ytt);
  GetYield(Lumi, "W1Jets", Coup, Yw1);
  GetYield(Lumi, "W2Jets", Coup, Yw2);
  GetYield(Lumi, "W3Jets", Coup, Yw3);
  GetYield(Lumi, "W4Jets", Coup, Yw4);
  cout << "Yield0: " << Ytt << "\n";
  cout << "Yield1: " << Yw1 << "\n";
  cout << "Yield2: " << Yw2 << "\n";
  cout << "Yield3: " << Yw3 << "\n";
  cout << "Yield4: " << Yw4 << "\n";
  cout << "MaxEvts0: " << TTtree->GetEntries() << "\n";
  cout << "MaxEvts1: " << W1tree->GetEntries() << "\n";
  cout << "MaxEvts2: " << W2tree->GetEntries() << "\n";
  cout << "MaxEvts3: " << W3tree->GetEntries() << "\n";
  cout << "MaxEvts4: " << W4tree->GetEntries() << "\n";


  // New file: Pseudo-data
  TFile PseudoData("PseudoData.root","recreate");
  // To be filled with a tree of same name as input files
  TTree *PDtree = new TTree("bdttree","Same tree");
  std::map<TString, float> VarMap;
  std::map<TString, int> VarMapI;
  VarMap["LepPt"] = 0.;
  // Pick a variable by value from all processes and create one w same name in new file
  SetBranchAddress("LepPt", &VarMap["LepPt"], TTtree, W1tree, W2tree, W3tree, W4tree);
  // Fill it in the new tree PDtree, with same name
  PDtree->Branch("LepPt", &VarMap["LepPt"]);
  VarMap["LepEta"] = 0.;
  SetBranchAddress("LepEta", &VarMap["LepEta"], TTtree, W1tree, W2tree, W3tree, W4tree);
  PDtree->Branch("LepEta", &VarMap["LepEta"]);
  VarMap["Met"] = 0.;
  SetBranchAddress("Met", &VarMap["Met"], TTtree, W1tree, W2tree, W3tree, W4tree);
  PDtree->Branch("Met", &VarMap["Met"]);
  VarMap["HT20"] = 0.;
  SetBranchAddress("HT20", &VarMap["HT20"], TTtree, W1tree, W2tree, W3tree, W4tree);
  PDtree->Branch("HT20", &VarMap["HT20"]);
  VarMap["Jet1Pt"] = 0.;
  SetBranchAddress("Jet1Pt", &VarMap["Jet1Pt"], TTtree, W1tree, W2tree, W3tree, W4tree);
  PDtree->Branch("Jet1Pt", &VarMap["Jet1Pt"]);
  VarMap["Jet2Pt"] = 0.;
  SetBranchAddress("Jet2Pt", &VarMap["Jet2Pt"], TTtree, W1tree, W2tree, W3tree, W4tree);
  PDtree->Branch("Jet2Pt", &VarMap["Jet2Pt"]);
  VarMap["JetHBpt"] = 0.;
  SetBranchAddress("JetHBpt", &VarMap["JetHBpt"], TTtree, W1tree, W2tree, W3tree, W4tree);
  PDtree->Branch("JetHBpt", &VarMap["JetHBpt"]);
  VarMapI["Njet30"] = 0.;
  SetBranchAddressI("Njet30", &VarMapI["Njet30"], TTtree, W1tree, W2tree, W3tree, W4tree);
  PDtree->Branch("Njet30", &VarMapI["Njet30"]);
  VarMapI["NbLoose30"] = 0.;
  SetBranchAddressI("NbLoose30", &VarMapI["NbLoose30"], TTtree, W1tree, W2tree, W3tree, W4tree);
  PDtree->Branch("NbLoose30", &VarMapI["NbLoose30"]);
  VarMap["Q80"] = 0.;
  SetBranchAddress("Q80", &VarMap["Q80"], TTtree, W1tree, W2tree, W3tree, W4tree);
  PDtree->Branch("Q80", &VarMap["Q80"]);
  VarMap["CosDeltaPhi"] = 0.;
  SetBranchAddress("CosDeltaPhi", &VarMap["CosDeltaPhi"], TTtree, W1tree, W2tree, W3tree, W4tree);
  PDtree->Branch("CosDeltaPhi", &VarMap["CosDeltaPhi"]);
  VarMap["DrJet1Lep"] = 0.;
  SetBranchAddress("DrJet1Lep", &VarMap["DrJet1Lep"], TTtree, W1tree, W2tree, W3tree, W4tree);
  PDtree->Branch("DrJet1Lep", &VarMap["DrJet1Lep"]);
  VarMap["DrJet1Jet2"] = 0.;
  SetBranchAddress("DrJet1Jet2", &VarMap["DrJet1Jet2"], TTtree, W1tree, W2tree, W3tree, W4tree);
  PDtree->Branch("DrJet1Jet2", &VarMap["DrJet1Jet2"]);
  VarMap["DrJetHBLep"] = 0.;
  SetBranchAddress("DrJetHBLep", &VarMap["DrJetHBLep"], TTtree, W1tree, W2tree, W3tree, W4tree);
  PDtree->Branch("DrJetHBLep", &VarMap["DrJetHBLep"]);
  VarMap["JetLepMass"] = 0.;
  SetBranchAddress("JetLepMass", &VarMap["JetLepMass"], TTtree, W1tree, W2tree, W3tree, W4tree);
  PDtree->Branch("JetLepMass", &VarMap["JetLepMass"]);
  VarMap["J3Mass"] = 0.;
  SetBranchAddress("J3Mass", &VarMap["J3Mass"], TTtree, W1tree, W2tree, W3tree, W4tree);
  PDtree->Branch("J3Mass", &VarMap["J3Mass"]);
  VarMapI["NbTight30"] = 0.;
  SetBranchAddressI("NbTight30", &VarMapI["NbTight30"], TTtree, W1tree, W2tree, W3tree, W4tree);
  PDtree->Branch("NbTight30", &VarMapI["NbTight30"]);
  // Add the other variables, w dummy values, to have the same data structure
  VarMap["LepID"] = -999;
  PDtree->Branch("LepID", &VarMap["LepID"]);
  VarMap["LepChg"] = -999;
  PDtree->Branch("LepChg", &VarMap["LepChg"]);
  VarMap["LepDxy"] = -999;
  PDtree->Branch("LepDxy", &VarMap["LepDxy"]);
  VarMap["LepDz"] = -999;
  PDtree->Branch("LepDz", &VarMap["LepDz"]);
  VarMap["LepIso"] = -999;
  PDtree->Branch("LepIso", &VarMap["LepIso"]);
  VarMap["mt"] = -999;
  PDtree->Branch("mt", &VarMap["mt"]);
  //  VarMap["NbTight30"] = -999;
  //  PDtree->Branch("NbTight30", &VarMap["NbTight30"]);
  VarMap["Jet1Eta"] = -999;
  PDtree->Branch("Jet1Eta", &VarMap["Jet1Eta"]);
  VarMap["Jet1Phi"] = -999;
  PDtree->Branch("Jet1Phi", &VarMap["Jet1Phi"]);
  VarMap["Jet2Eta"] = -999;
  PDtree->Branch("Jet2Eta", &VarMap["Jet2Eta"]);
  VarMap["Jet2Phi"] = -999;
  PDtree->Branch("Jet2Phi", &VarMap["Jet2Phi"]);
  VarMap["DrJet2Lep"] = -999;
  PDtree->Branch("DrJet2Lep", &VarMap["DrJet2Lep"]);
  VarMap["XS"] = -999;
  PDtree->Branch("XS", &VarMap["XS"]);
  VarMap["Event"] = -999;
  PDtree->Branch("Event", &VarMap["Event"]);
  VarMap["Nevt"] = -999;
  PDtree->Branch("Nevt", &VarMap["Nevt"]);
  //  VarMap["Wght"] = 1.;
  //  PDtree->Branch("Wght", &VarMap["Wght"]);

  // Yield: Y = Lumi . xs . (Ns / Ng)
  // Sanity condition: Y < Ns
  // Pick-up randomly Y events out of Ns selected events
  // Shape of the variable: OK
  // The very number of events = Y, by definition
  TRandom3 *haz = new TRandom3();
  std::vector<size_t> TTevt, W1evt, W2evt, W3evt, W4evt;
  // Randomly generating the indices of the events to keep, 
  // not allowing the index to be higher than the size of the tree
  int PYtt = haz->Poisson(Ytt); 
  for(int i = 0; i < PYtt; i++){
    size_t max = TTtree->GetEntries();
    size_t evtindex = max;
    while(evtindex == max)
      evtindex = haz->Uniform(max);
    // Avoid to pick-up the same event twice
    if(std::find(TTevt.begin(), TTevt.end(), evtindex) == TTevt.end())
      TTevt.push_back(evtindex);
    else
      --i;
  }
  int PYw1 = haz->Poisson(Yw1); 
  for(int i = 0; i < PYw1; i++){
    size_t max = W1tree->GetEntries();
    size_t evtindex = max;
    while(evtindex == max)
      evtindex = haz->Uniform(max);
    if(std::find(W1evt.begin(), W1evt.end(), evtindex) == W1evt.end())
      W1evt.push_back(evtindex);
    else
      --i;
  }
  int PYw2 = haz->Poisson(Yw2); 
  for(int i = 0; i < PYw2; i++){
    size_t max = W2tree->GetEntries();
    size_t evtindex = max;
    while(evtindex == max)
      evtindex = haz->Uniform(max);
    if(std::find(W2evt.begin(), W2evt.end(), evtindex) == W2evt.end())
      W2evt.push_back(evtindex);
    else
      --i;
  }
  int PYw3 = haz->Poisson(Yw3); 
  for(int i = 0; i < PYw3; i++){
    size_t max = W3tree->GetEntries();
    size_t evtindex = max;
    while(evtindex == max)
      evtindex = haz->Uniform(max);
    if(std::find(W3evt.begin(), W3evt.end(), evtindex) == W3evt.end())
      W3evt.push_back(evtindex);
    else
      --i;
  }
  int PYw4 = haz->Poisson(Yw4); 
  for(int i = 0; i < PYw4; i++){
    size_t max = W4tree->GetEntries();
    size_t evtindex = max;
    while(evtindex == max)
      evtindex = haz->Uniform(max);
    if(std::find(W4evt.begin(), W4evt.end(), evtindex) == W4evt.end())
      W4evt.push_back(evtindex);
    else
      --i;
  }

  // Retrieving the selected events from the tree, 
  // and filling the output tree
  for (int i=0; i<TTevt.size(); i++){
    TTtree->GetEntry(TTevt[i]);
    PDtree->Fill(); // Since the addresses of PDtree have been defined 
                    // with the same address than TTtree, the evt info is correctly filled
  }
  for (int i=0; i<W1evt.size(); i++){
    W1tree->GetEntry(W1evt[i]);
    PDtree->Fill();
  }
  for (int i=0; i<W2evt.size(); i++){
    W2tree->GetEntry(W2evt[i]);
    PDtree->Fill();
  }
  for (int i=0; i<W3evt.size(); i++){
    W3tree->GetEntry(W3evt[i]);
    PDtree->Fill();
  }
  for (int i=0; i<W4evt.size(); i++){
    W4tree->GetEntry(W4evt[i]);
    PDtree->Fill();
  }


  PDtree->Write();
  cout << "*** THE END ***" << "\n";

}// */

void GetYield(
	      double lumi,     // Input: Luminosity
	      std::string pc,       // Input: Name of the channel
	      TCut Coupure,    // Input: Analysis cut
	      double& Yield    // Output: Weighted yield of the channel
	      ){ 

  // Which file to read
  TChain ProcFile("bdttree");

  if (pc=="STP300CHI270"){    ProcFile.Add("SET5/T2DegStop_300_270_bdt.root");  }
  if (pc=="STP600CHI150"){    ProcFile.Add("");  }
  if (pc=="TTbar"){    ProcFile.Add("SET5/TTJets_bdt.root");  }
  if (pc=="Zjets"){    ProcFile.Add("");  }
  if (pc=="Zmumu"){    ProcFile.Add("");  }
  if (pc=="Zee"){    ProcFile.Add("");  }
  if (pc=="Ztautau"){     ProcFile.Add("");  }
  if (pc=="W1Jets"){    ProcFile.Add("SET5/WJets_HT100to200_bdt.root");  }
  if (pc=="W2Jets"){    ProcFile.Add("SET5/WJets_HT200to400_bdt.root");  }
  if (pc=="W3Jets"){    ProcFile.Add("SET5/WJets_HT400to600_bdt.root");  }
  if (pc=="W4Jets"){    ProcFile.Add("SET5/WJets_HT600toInf_bdt.root");  }
  if (pc=="Wmunu"){    ProcFile.Add("");  }
  if (pc=="Wenu"){    ProcFile.Add("");  }
  if (pc=="Wtaunu"){    ProcFile.Add("");  }
  if (pc=="WW"){    ProcFile.Add("");  }
  if (pc=="WZ"){    ProcFile.Add("");  }
  if (pc=="ZZ"){    ProcFile.Add("");  }
  if (pc=="Vqq"){    ProcFile.Add("");  }
  if (pc=="SingleToptW"){    ProcFile.Add("");  }
  if (pc=="SingleToptChannel"){    ProcFile.Add("");  }
  if (pc=="SingleTopsChannel"){    ProcFile.Add("");  }

  // Get the numerator
  TH1D *nbcut = new TH1D("nbcut","nbcut",200,-100,100);
  ProcFile.Draw("Njet30>>nbcut",Coupure);    // Pick-up a variable which is always filled
  double NumWeight = nbcut->GetEntries();
  // Get the weight
  float xsec;
  ProcFile.SetBranchAddress("XS",&xsec);
  ProcFile.GetEntry(0);
  // Get the denominator
  TH1D *nbt = new TH1D("nbt","nbt",10,0.,10000000.);
  ProcFile.Draw("Nevt>>nbt");
  double *Den = ProcFile.GetV1();
//  cout << pc << " " << Den[0] << endl;

  // Calculate yields (weighted/unweighted)
  Yield  = lumi*xsec*NumWeight/Den[0];

  // Delete histograms
  delete nbt;
  delete nbcut;

}

void SetBranchAddress(TString varname, float *varaddress, TTree* tree1, TTree* tree2, TTree* tree3, TTree* tree4, TTree* tree5){
  tree1->SetBranchAddress(varname, varaddress);
  tree2->SetBranchAddress(varname, varaddress);
  tree3->SetBranchAddress(varname, varaddress);
  tree4->SetBranchAddress(varname, varaddress);
  tree5->SetBranchAddress(varname, varaddress);
}

void SetBranchAddressI(TString varname, int *varaddress, TTree* tree1, TTree* tree2, TTree* tree3, TTree* tree4, TTree* tree5){
  tree1->SetBranchAddress(varname, varaddress);
  tree2->SetBranchAddress(varname, varaddress);
  tree3->SetBranchAddress(varname, varaddress);
  tree4->SetBranchAddress(varname, varaddress);
  tree5->SetBranchAddress(varname, varaddress);
}

std::string convertDouble(double value)
{
  std::ostringstream o;
  if (!(o << value))
    return "";
  return o.str();
}
