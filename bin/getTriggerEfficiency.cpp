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
#include <TEfficiency.h>
#include <TGraphAsymmErrors.h>

#include "UserCode/Stop4Body-nanoAOD/interface/json.hpp"
#include "UserCode/Stop4Body-nanoAOD/interface/SampleReader.h"
#include "UserCode/Stop4Body-nanoAOD/interface/doubleWithUncertainty.h"
#include "UserCode/Stop4Body-nanoAOD/interface/ValueWithSystematics.h"

using json = nlohmann::json;

class CutInfo
{
public:
  CutInfo(std::string name, std::string cut, std::string latex): name_(name), cut_(cut), latex_(latex) {}

  std::string name()  const {return name_;}
  std::string cut()   const {return cut_;}
  std::string latex() const {return latex_;}

private:
protected:
  std::string name_;
  std::string cut_;
  std::string latex_;
};

int main(int argc, char** argv)
{
  std::string jsonFileName = "";
  std::string inputDirectory = "";
  std::string outputDirectory = "./OUT/";
  bool verbose = false;
  int year = 2017;
  double luminosity = -1.0;

  for(int i = 1; i < argc; ++i)
  {
    std::string argument = argv[i];

    if(argument == "--json")
      jsonFileName = argv[++i];

    if(argument == "--inDir")
      inputDirectory = argv[++i];

    if(argument == "--outDir")
      outputDirectory = argv[++i];

    if(argument == "--year")
    {
      std::stringstream converter;
      converter << argv[++i];
      converter >> year;
    }
    if(argument == "--verbose")
    {
      verbose = true;
    } 
  }

  std::cout << "Reading json files" << std::endl;
  SampleReader samples(jsonFileName, inputDirectory, suffix);

  std::string auxTrigger;
  if(year==2017){
  	auxTrigger = "HLT_el35";
  }
  else if(year==2018){
  	auxTrigger = "HLT_el32";
  }
  std::string selection = "HT>200 && Jet1Pt > 110";
  std::string triggers = "HLT_PFMET120_PFMHT120";
  if(year==2018){
  	triggers += " || HLT_PFMET120_PFMHT120_PFHT60";
  }

  std::string sel = auxTrigger + " && " + selection;
  std::string trigSel = auxTrigger + " && " + selection + " && " + triggers;

  if(verbose)
    std::cout << "Splitting samples according to type" << std::endl;
  auto Data = samples.getData();
  auto MC = samples.getMCBkg();

  if(verbose)
    std::cout << "Retrieving luminosity" << std::endl;
  if(luminosity <= 0)
    luminosity = Data.getLumi();

  auto psl = Data.getHist("Met", Met, sel*"1.01");
  auto hlt = Data.getHist("Met", Met, trigSel*"1.01");

  TCanvas c1("DEBUG", "", 1200, 1350);
  gStyle->SetOptStat(0);
  c1.cd();
  c1.SetGridx();
  c1.SetGridy();

  hlt->SetTitle("")
  hlt->GetYaxis()->SetTitle("Trigger Efficiency");
  hlt->GetXaxis()->SetTitle("MET");
  hlt->Divide(psl)
  hlt->Draw()

  TF1  *f1 = new TF1("f1","[0]*0.5*(1+TMath::Erf((x-[1])/[2]))")
  hlt->Fit(f1,"","",150,500)

  TPaveText* T = new TPaveText(0.6,0.55,0.85,0.4, "NDC");
  T->SetFillColor(0);
  T->SetFillStyle(0);
  T->SetLineColor(0);
  T->SetTextAlign(12);
  Char_t chindf[80];
  sprintf(chindf,"Total : #chi^{2}/NDF = %.2f",f1->GetChisquare()/f1->GetNDF());
  Char_t p0[80];
  sprintf(p0,"p0 = %.2f #pm %.2f",f1->GetParameter(0),f1->GetParError(0));
  Char_t p1[80];
  sprintf(p1,"p1 = %.2f #pm %.2f",f1->GetParameter(1),f1->GetParError(1));
  Char_t p2[80];
  sprintf(p2,"p2 = %.2f #pm %.2f",f1->GetParameter(2),f1->GetParError(2));

  T->AddText(chindf);
  T->AddText(p0);
  T->AddText(p1);
  T->AddText(p2);
  T->Draw("same");
  T->SetBorderSize(0);

  c1.SaveAs(("trigEff_"+std::to_string(YEAR)+".pdf").c_str());

  return 0;
}