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
#include <TLegend.h>
#include <TPad.h>
#include <TPaveText.h>
#include <TCanvas.h>
#include <TGraphErrors.h>

#include "TMVA/Factory.h"
#include "TMVA/Tools.h"

#include <iostream>
#include <vector>
#include <sstream>
#include <map>
#include <fstream>

#include "UserCode/Stop4Body/interface/json.hpp"
#include "UserCode/Stop4Body/interface/SampleReader.h"
#include "UserCode/Stop4Body/interface/VariableReader.h"

using json = nlohmann::json;

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
  gStyle->SetOptTitle(0);
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

  VariableJsonLoader variables(variablesJson);
  std::cout << "Processing variable plots" << std::endl;
  //std::cout << "  Preselection: " << presel << std::endl;
  for(auto & variable : variables)
  {
    //std::cout << "  Processing " << variable.name() << std::endl;

    auto dataH = Data.getHist(variable.name()+"_Data", variable.expression(), variable.label()+";Evt.", presel.GetTitle(), variable.bins(), variable.min(), variable.max());
    auto mcH = MC.getHist(variable.name()+"_MC", variable.expression(), variable.label()+";Evt.", mcWeight+"*("+presel.GetTitle()+")", variable.bins(), variable.min(), variable.max());
    auto sigH = Sig.getHist(variable.name()+"_Signal", variable.expression(), variable.label()+";Evt.", mcWeight+"*("+presel.GetTitle()+")", variable.bins(), variable.min(), variable.max());
    auto mcS = MC.getStack(variable.expression(), variable.label()+";Evt.", mcWeight+"*("+presel.GetTitle()+")", variable.bins(), variable.min(), variable.max());

    //std::cout << "    Got histograms, getting ratio" << std::endl;

    auto ratio = static_cast<TH1D*>(dataH->Clone("ratio"));
    ratio->SetTitle((";" + variable.label() + ";Data/#Sigma MC").c_str());
    ratio->Divide(mcH);

    //std::cout << "    Got ratio, creating canvas" << std::endl;

    /*TCanvas c2(variable.name().c_str(),"",800,600);
    mcS->Draw("hist");
    dataH->Draw("same");
    c2.SaveAs((outputDirectory+"/"+variable.name()+"_test.png").c_str());
    c2.SaveAs((outputDirectory+"/"+variable.name()+"_test.C").c_str()); // */

    TCanvas c1(variable.name().c_str(), "", 800, 800);
    gStyle->SetOptStat(0);
    TPad* t1 = new TPad("t1","t1", 0.0, 0.20, 1.0, 1.0);
    //TPad* t2 = nullptr;
    TPad* t2 = new TPad("t2","t2", 0.0, 0.0, 1.0, 0.2);
    t1->Draw();
    t1->cd();
    t1->SetLogy(true);
    mcS->Draw("hist");
    dataH->Draw("same");
    sigH->Draw("hist same");
    //TLegend *legA = gPad->BuildLegend(0.845,0.69,0.65,0.89, "NDC");
    TLegend *legA = gPad->BuildLegend(0.155,0.69,0.35,0.89, "NDC");
    legA->SetFillColor(0); legA->SetFillStyle(0); legA->SetLineColor(0);
    legA->SetHeader("");
    legA->SetTextFont(42);
    TPaveText* T = new TPaveText(0.1,0.995,0.84,0.95, "NDC");
    T->SetFillColor(0);
    T->SetFillStyle(0); T->SetLineColor(0);
    T->SetTextAlign(12);
    char Buffer[1024];
    sprintf(Buffer, "CMS preliminary, #sqrt{s}=%.1f TeV, #scale[0.5]{#int} L=%.1f fb^{-1}", 13.0, luminosity/1000);
    T->AddText(Buffer);
    T->Draw("same");
    T->SetBorderSize(0);
    c1.cd();
    t2->Draw();
    t2->cd();
    t2->SetGridy(true);
    t2->SetPad(0,0.0,1.0,0.2);
    t2->SetTopMargin(0);
    t2->SetBottomMargin(0.5);
    TH1D *bgUncH = static_cast<TH1D*>(mcH->Clone("bgUncH"));
    for(int xbin=1; xbin<=bgUncH->GetXaxis()->GetNbins(); xbin++)
    {
      if(bgUncH->GetBinContent(xbin)==0)
        continue;

      double unc = bgUncH->GetBinError(xbin) / bgUncH->GetBinContent(xbin);

      // Add systematic uncertainties
      unc = unc*unc;
      unc += 0.026*0.026; // Luminosity uncertainty
//      unc += 0.15*0.15; // Value assumed for total systematic uncertainty
      unc = std::sqrt(unc);

      bgUncH->SetBinContent(xbin,1);
      bgUncH->SetBinError(xbin,unc);
    }
    TGraphErrors *bgUnc=new TGraphErrors(bgUncH);
    bgUnc->SetLineColor(1);
    bgUnc->SetFillStyle(3001);
    bgUnc->SetFillColor(kGray);
    bgUnc->SetMarkerColor(1);
    bgUnc->SetMarkerStyle(1);
    bgUncH->Reset("ICE");
    bgUncH->Draw();
    bgUnc->Draw("3");
    double yscale = (1.0-0.2)/(0.18-0);
    bgUncH->GetYaxis()->SetTitle("Data/#Sigma MC");
    bgUncH->SetMinimum(0.4);
    bgUncH->SetMaximum(1.6);
    bgUncH->GetXaxis()->SetTitle("");
    bgUncH->GetXaxis()->SetTitleOffset(1.3);
    bgUncH->GetXaxis()->SetLabelSize(0.033*yscale);
    bgUncH->GetXaxis()->SetTitleSize(0.036*yscale);
    bgUncH->GetXaxis()->SetTickLength(0.03*yscale);
    bgUncH->GetYaxis()->SetTitleOffset(0.3);
    bgUncH->GetYaxis()->SetNdivisions(5);
    bgUncH->GetYaxis()->SetLabelSize(0.033*yscale);
    bgUncH->GetYaxis()->SetTitleSize(0.036*yscale);
    ratio->Draw("same");

    //std::cout << "    Finished drawing, saving output" << std::endl;
    //std::cout << "      Saving to: " << outputDirectory+"/"+variable.name()+".png" << std::endl;

    c1.SaveAs((outputDirectory+"/"+variable.name()+".png").c_str());
    c1.SaveAs((outputDirectory+"/"+variable.name()+".C").c_str());

    //std::cout << "    Finished saving plots" << std::endl;
  }

  std::cout << "Total MC yield: " << MC.getYield(presel.GetTitle(), mcWeight) << std::endl;
  for(auto & sample : MC)
    std::cout << "\t" << sample.label() << " yield: " << sample.getYield(presel.GetTitle(), mcWeight) << std::endl;
  std::cout << "Total Data yield: " << Data.getYield(presel.GetTitle(), "1") << std::endl;
  std::cout << "Total Signal yield: " << Sig.getYield(presel.GetTitle(), mcWeight) << std::endl;





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
