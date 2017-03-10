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

using json = nlohmann::json;

void printHelp();
void naiveDD(std::ofstream &, ProcessInfo &, SampleReader &, SampleReader &, std::string, std::string, std::string);

int main(int argc, char** argv)
{
  std::string jsonFileName = "";
  std::string inputDirectory = "";
  std::string outputDirectory = "./OUT/";
  std::string suffix = "";
  double luminosity = -1.0;
  bool isPseudoData = false;
  bool verbose = false;

  std::string baseSelection = "(HT30 > 200) && (Jet1Pt > 100) && (Met > 300) && (nGoodEl + nGoodMu <= 2)";
  std::string wjetsControlRegion = "(BDT < -0.1) && (NbLoose30 == 0)";
  std::string ttbarControlRegion = "(BDT < -0.1) && (NbTight30 > 0)";
  std::string signalRegion       = "(BDT > 0.37)";

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

    if(argument == "--lumi")
    {
      std::stringstream convert;
      convert << argv[++i];
      convert >> luminosity;
    }

    if(argument == "--isPseudoData")
    {
      isPseudoData = true;
    }

    if(argument == "--verbose")
    {
      verbose = true;
    }
  }

  gStyle->SetOptStat(000000);
  gStyle->SetOptTitle(0);

  std::cout << "Reading json files" << std::endl;
  SampleReader samples(jsonFileName, inputDirectory, suffix);

  if(verbose)
    std::cout << "Splitting samples according to type" << std::endl;
  auto MC = samples.getMCBkg();
  auto Sig = samples.getMCSig();
  auto Data = samples.getData();

  if(verbose)
    std::cout << "Extracting ttbar and wjets from other backgrounds" << std::endl;
  size_t ttbarIndex = 0, wjetsIndex = 0;
  bool foundTTbar = false, foundWJets = false;
  for(size_t i = 0; i < MC.nProcesses(); ++i)
  {
    if(MC.process(i).tag() == "WJets")
    {
      wjetsIndex = i;
      foundWJets = true;
    }

    if(MC.process(i).tag() == "ttbar" || MC.process(i).tag() == "ttbar_lo")
    {
      ttbarIndex = i;
      foundTTbar = true;
    }
  }
  if(!foundTTbar)
  {
    std::cout << "There isn't a ttbar sample in the JSON file" << std::endl;
    return 1;
  }
  if(!foundWJets)
  {
    std::cout << "There isn't a wjets sample in the JSON file" << std::endl;
    return 1;
  }
  auto wjets = MC.process(wjetsIndex);
  auto ttbar = MC.process(ttbarIndex);

  if(verbose)
    std::cout << "Retrieving luminosity" << std::endl;
  if(luminosity <= 0)
    luminosity = Data.getLumi();

  if(verbose)
    std::cout << "Computing correct MC weight" << std::endl;
  std::string mcWeight;
  {
    std::stringstream converter;
    if(isPseudoData)
      converter << "splitFactor*XS*filterEfficiency*(genWeight/sumGenWeight)"; // The scale factors are not considered here
    else
      converter << "splitFactor*weight";
    converter << "*" << luminosity;
    converter >> mcWeight;
  }
  std::cout << "Using mcWeight: " << mcWeight << std::endl;


  // Make a plot of the BDToutput, just for control reasons
  if(verbose)
    std::cout << "Building control plot of BDT output" << std::endl;
  {
    auto dataH = Data.process(0).getHist("BDT", "BDT;Evt.",               baseSelection,     20, -1.0, 1.0);
    auto mcH   =        MC.getHist("MC", "BDT", "BDT;Evt.", mcWeight+"*("+baseSelection+")", 20, -1.0, 1.0);
    auto sigH  =  Sig.process(0).getHist("BDT", "BDT;Evt.", mcWeight+"*("+baseSelection+")", 20, -1.0, 1.0);
    auto mcS   =             MC.getStack("BDT", "BDT;Evt.", mcWeight+"*("+baseSelection+")", 20, -1.0, 1.0);

    auto ratio = static_cast<TH1D*>(dataH->Clone("Ratio"));
    ratio->SetTitle(";BDT;Data/#Sigma MC");
    ratio->Divide(mcH);

    TCanvas c1("BDT_canv", "", 800, 800);
    gStyle->SetOptStat(0);

    TPad* t1 = new TPad("t1","t1", 0.0, 0.20, 1.0, 1.0);
    TPad* t2 = new TPad("t2","t2", 0.0, 0.0, 1.0, 0.2);
    t1->Draw();
    t1->cd();
    t1->SetLogy(true);
    mcS->Draw("hist");
    dataH->Draw("same");
    sigH->Draw("hist same");

    TLegend *legA = gPad->BuildLegend(0.845,0.69,0.65,0.89, "NDC");
    legA->SetFillColor(0);
    legA->SetFillStyle(0);
    legA->SetLineColor(0);
    legA->SetHeader("");
    legA->SetTextFont(42);

    TPaveText* T = new TPaveText(0.1,0.995,0.84,0.95, "NDC");
    T->SetFillColor(0);
    T->SetFillStyle(0);
    T->SetLineColor(0);
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

    TH1D *bgUncH = static_cast<TH1D*>(mcH->Clone("BDT_bgUncH"));
    for(int xbin=1; xbin <= bgUncH->GetXaxis()->GetNbins(); xbin++)
    {
      if(bgUncH->GetBinContent(xbin)==0)
        continue;

      double unc = bgUncH->GetBinError(xbin) / bgUncH->GetBinContent(xbin);

      // Add systematic uncertainties
      unc = unc*unc;
      unc += 0.026*0.026; // Luminosity uncertainty
      unc = std::sqrt(unc);

      bgUncH->SetBinContent(xbin,1);
      bgUncH->SetBinError(xbin,unc);
    }

    TGraphErrors *bgUnc = new TGraphErrors(bgUncH);
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

    c1.SaveAs((outputDirectory+"/BDTout.png").c_str());
    c1.SaveAs((outputDirectory+"/BDTout.C").c_str());

    delete dataH;
    delete mcH;
    delete sigH;
    // Delete individual hists in the stack
    TList* histList = mcS->GetHists();
    histList->Delete();
    delete mcS;
    delete ratio;
    delete legA;
    delete T;
    delete bgUncH;
    delete bgUnc;
  }

  if(verbose)
    std::cout << "Filling table" << std::endl;
  std::ofstream outputTable(outputDirectory+"/table.tex");
  outputTable << "\\begin{tabular}{r|ccccc}\n";
  outputTable << " & SR & CR & Data in CR & other MC in CR & Estimate\\\\\n\\hline\n";

  naiveDD(outputTable, wjets, Data, MC, baseSelection + " && " + signalRegion, baseSelection + " && " + wjetsControlRegion, mcWeight);
  naiveDD(outputTable, ttbar, Data, MC, baseSelection + " && " + signalRegion, baseSelection + " && " + ttbarControlRegion, mcWeight);

  return 0;
}

void naiveDD(std::ofstream &outputTable, ProcessInfo &toEstimate, SampleReader &Data, SampleReader &MC, std::string signalRegion, std::string controlRegion, std::string mcWeight)
{
  doubleUnc NinSR = toEstimate.getYield(signalRegion, mcWeight);
  doubleUnc NinCR = toEstimate.getYield(controlRegion, mcWeight);
  doubleUnc DatainCR = Data.getYield(controlRegion, "1.0");
  doubleUnc otherMC (0,0);
  for(auto &process: MC)
  {
    if(process.tag() != toEstimate.tag())
      otherMC += process.getYield(controlRegion, mcWeight);
  }

  doubleUnc estimate = NinSR/NinCR * (DatainCR - otherMC);

  outputTable << "$" << toEstimate.label() << "$ & $";
  outputTable << NinSR << "$ & $";
  outputTable << NinCR << "$ & $";
  outputTable << DatainCR << "$ & $";
  outputTable << otherMC << "$ & $";
  outputTable << estimate << "$\\\\\n";

  return;
}

void printHelp()
{
  std::cout << "The program has the following parameters:" << std::endl;
  std::cout << "\t--help\t- Print this message" << std::endl;

  return;
}
