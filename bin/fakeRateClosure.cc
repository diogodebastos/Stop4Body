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
#include "UserCode/Stop4Body/interface/ValueWithSystematics.h"

using json = nlohmann::json;

class NullBuffer : public std::streambuf
{
  public:
    int overflow(int c) { return c; }
};

class NullStream : public std::ostream
{
  public:
    NullStream():std::ostream(&m_sb) {}
  private:
    NullBuffer m_sb;
};

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


void printHelp();
TH1* histsSumFromStack(THStack*);
doubleUnc fakeDD_MCClosure(std::ostream &, SampleReader &, std::string, std::string);
doubleUnc MCClosure(SampleReader &, VariableInfo&, std::string, std::string, std::string, std::string, std::string, std::string);
ValueWithSystematics<doubleUnc> fakeDDSys(std::ostream &, SampleReader &, SampleReader &, std::string, const ValueWithSystematics<std::string>&);

int main(int argc, char** argv)
{
  std::string variablesJson = "";
  std::string cutsJson = "";
  std::string jsonFileName = "";
  std::string inputDirectory = "";
  std::string outputDirectory = "./OUT/";
  std::string suffix = "";
  double luminosity = -1.0;
  bool isPseudoData = false;
  bool verbose = false;
  bool isSwap = false;
  bool isHighDM = false;
  double SRCut = 0.4;
  bool invertMet = false;
  bool invertLepPt = false;
  bool special = false;
  bool doMCFakeClosure = false;
  bool doPlots = false;

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

    if(argument == "--variables")
      variablesJson = argv[++i];

    if(argument == "--cuts")
      cutsJson = argv[++i];

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

    if(argument == "--signalRegionCut")
    {
      std::stringstream convert;
      convert << argv[++i];
      convert >> SRCut;
    }

    if(argument == "--isPseudoData")
    {
      isPseudoData = true;
    }

    if(argument == "--isSwap")
    {
      isSwap = true;
    }

    if(argument == "--isHighDeltaM")
    {
      isHighDM = true;
    }

    if(argument == "--verbose")
    {
      verbose = true;
    }

    if(argument == "--invertMet")
    {
      invertMet = true;
    }

    if(argument == "--invertLepPt")
    {
      invertLepPt = true;
    }

    if(argument == "--isSpecial")
    {
      special = true;
    }

    if(argument == "--doMCFakeClosure")
    {
      doMCFakeClosure = true;
    }
    if(argument == "--doPlots")
    {
      doPlots = true;
    }
  }

  if(SRCut < 0.2 && !special)
  {
    std::cout << "You have defined an invalid signal region cut." << std::endl;
    std::cout << "Using \"BDT > 0.2\" instead" << std::endl;
    SRCut = 0.2;
  }

  gStyle->SetOptStat(000000);
  gStyle->SetOptTitle(0);

  std::cout << "Reading json files" << std::endl;
  VariableJsonLoader variables(variablesJson);
  SampleReader samples(jsonFileName, inputDirectory, suffix);

  std::vector<CutInfo> cutFlow;
  json jsonFile;
  std::ifstream inputFile(cutsJson);
  inputFile >> jsonFile;

  std::string tightSelection = "(isTight == 1)";
  std::string looseSelection = "(isLoose == 1) && !(isTight == 1)";
  //std::string looseSelection = "(isLooseNotTight)";

  std::string leptonChannel = "(nGoodEl == 0) && (nGoodMu == 1)";
  //std::string analysisLeptonFinalState = "(nGoodEl_cutId_veto == 1) && (nGoodMu== 0)";
  std::string analysisLeptonFinalState = "(nGoodEl == 0) && (nGoodMu_cutId_loose == 1)";

  //std::string analysisLeptonFinalState = "(nGoodEl_cutId_veto + nGoodMu_cutId_loose == 1)";
  //std::string promptSelection = "(isPrompt == 1)";
  //std::string fakeSelsLoose == 1) && !(isTight == 1)"ection = "!(isPrompt == 1)";
  std::string metSelection = " && (Met < 100)";
  if(invertMet)
    metSelection = " && (Met > 200 && Met < 280)";
  //std::string baseSelection = "(badCloneMuonMoriond2017 == 1) && (badMuonMoriond2017 == 1) && (DPhiJet1Jet2 < 2.5 || Jet2Pt < 60) && (HT > 200) && (Jet1Pt > 110)" + metSelection;
  std::string baseSelection = "(badCloneMuonMoriond2017 == 1) && (badMuonMoriond2017 == 1) && (HT > 200) && (Jet1Pt > 110)" + metSelection;
  //std::string baseSelection = "(DPhiJet1Jet2 < 2.5 || Jet2Pt < 60) && (HT > 200) && (Jet1Pt > 110)" + metSelection;
  std::string CCSR =    "(Njet < 3) && (DPhiJet1Jet2 < 2.5) && (Met > 300)";
  std::string CCSR1 =   CCSR +   " && (HT>400) && (Jet1Pt>100) && (NbLoose == 0) && (LepEta < 1.5 ) && (LepEta > -1.5)";
  std::string CCSR1a =  CCSR1 + " && (mt<60) && (LepChg == -1) && (LepPt<30)";
  std::string CCSR1b =  CCSR1 + " && (mt>60) && (mt<95) && (LepChg == -1) && (LepPt<30)";
  std::string CCSR1ab = "("+CCSR1a+")||("+CCSR1b+")";
  std::cout << CCSR1ab << std::endl;

  std::string wjetsEnrich = "(NbLoose == 0)";
  std::string ttbarEnrich = "(NbTight > 0)";
  std::string controlRegion = "(BDT < 0.2)";
  if(special)
  {
    std::stringstream converter;
    converter << "(BDT < " << SRCut << ")";
    controlRegion = converter.str();
  }
  std::string signalRegion = "";
  {
    std::stringstream converter;
    converter << "(BDT > " << SRCut << ")";
    signalRegion = converter.str();
  }
  if(isHighDM)
  {
  }
  else
  {
    if(!invertLepPt)
      baseSelection += " && (LepPt < 30.)";
    else
      baseSelection += " && (LepPt > 30.)";
  }
  if(isSwap)
  {
    if(isHighDM)
      baseSelection += " && (LepPt < 280.)";
    baseSelection += " && (HLT_Mu == 1)";
  }
  else
  {
  }

  std::string wjetsControlRegion = controlRegion + " && " + wjetsEnrich;
  std::string ttbarControlRegion = controlRegion + " && " + ttbarEnrich;
  std::string wjetsSignalRegionClosure = signalRegion + " && " + wjetsEnrich;
  std::string ttbarSignalRegionClosure = signalRegion + " && " + ttbarEnrich;

  if(doMCFakeClosure)
  {
   // Signal Region = Preselection
   controlRegion = "";
   signalRegion = baseSelection;
   wjetsControlRegion = wjetsEnrich;
   ttbarControlRegion = ttbarEnrich;
   wjetsSignalRegionClosure = wjetsEnrich;
   ttbarSignalRegionClosure = ttbarEnrich;
  }

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

    if(MC.process(i).tag().find("ttbar") != std::string::npos)
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
    //luminosity = Data.getLumi();
    luminosity = 38666;

  if(verbose)
    std::cout << "Computing correct MC weight" << std::endl;
  std::string mcWeight;
  {
    std::stringstream converter;
    if(isPseudoData)
      //converter << "XS*(genWeight/sumGenWeight)"; // The scale factors are not considered here
      converter << "XS*(genWeight/sumGenWeight)*puWeight"; // The scale factors are not considered here
      //converter << "splitFactor*XS*filterEfficiency*(genWeight/sumGenWeight)*puWeight"; // The scale factors are not considered here
    else
      converter << "splitFactor*weight";
    converter << "*" << luminosity;
    converter >> mcWeight;
  }
  ValueWithSystematics<std::string> mcWeightSys(mcWeight);
  if(!isPseudoData)
  {
    std::vector<std::string> systVars;
    systVars.push_back("ISRweight_Bin1_Up");
    systVars.push_back("ISRweight_Bin1_Down");
    systVars.push_back("ISRweight_Bin2_Up");
    systVars.push_back("ISRweight_Bin2_Down");
    systVars.push_back("ISRweight_Bin3_Up");
    systVars.push_back("ISRweight_Bin3_Down");
    systVars.push_back("ISRweight_Bin4_Up");
    systVars.push_back("ISRweight_Bin4_Down");
    systVars.push_back("ISRweight_Bin5_Up");
    systVars.push_back("ISRweight_Bin5_Down");
    systVars.push_back("ISRweight_Bin6_Up");
    systVars.push_back("ISRweight_Bin6_Down");
    systVars.push_back("EWKISRweight_Bin1_Up");
    systVars.push_back("EWKISRweight_Bin1_Down");
    systVars.push_back("EWKISRweight_Bin2_Up");
    systVars.push_back("EWKISRweight_Bin2_Down");
    systVars.push_back("EWKISRweight_Bin3_Up");
    systVars.push_back("EWKISRweight_Bin3_Down");
    systVars.push_back("EWKISRweight_Bin4_Up");
    systVars.push_back("EWKISRweight_Bin4_Down");
    systVars.push_back("EWKISRweight_Bin5_Up");
    systVars.push_back("EWKISRweight_Bin5_Down");
    systVars.push_back("EWKISRweight_Bin6_Up");
    systVars.push_back("EWKISRweight_Bin6_Down");
    systVars.push_back("EWKISRweight_Bin7_Up");
    systVars.push_back("EWKISRweight_Bin7_Down");
    systVars.push_back("TightLoose_NU_Bin1_Up");
    systVars.push_back("TightLoose_NU_Bin1_Down");
    systVars.push_back("TightLoose_NU_Bin2_Up");
    systVars.push_back("TightLoose_NU_Bin2_Down");
    systVars.push_back("TightLoose_NU_Bin3_Up");
    systVars.push_back("TightLoose_NU_Bin3_Down");
    systVars.push_back("TightLoose_NU_Bin4_Up");
    systVars.push_back("TightLoose_NU_Bin4_Down");
    systVars.push_back("TightLoose_NU_Bin5_Up");
    systVars.push_back("TightLoose_NU_Bin5_Down");
    systVars.push_back("HF_Up");
    systVars.push_back("HF_Down");
    systVars.push_back("LF_Up");
    systVars.push_back("LF_Down");
    systVars.push_back("HFStats1_Up");
    systVars.push_back("HFStats1_Down");
    systVars.push_back("LFStats1_Up");
    systVars.push_back("LFStats1_Down");
    systVars.push_back("HFStats2_Up");
    systVars.push_back("HFStats2_Down");
    systVars.push_back("LFStats2_Up");
    systVars.push_back("LFStats2_Down");
    systVars.push_back("CFErr1_Up");
    systVars.push_back("CFErr1_Down");
    systVars.push_back("CFErr2_Up");
    systVars.push_back("CFErr2_Down");

    for(auto& syst: systVars)
    {
      std::stringstream converter;
      converter << "splitFactor*weight_" << syst << "*" << luminosity;
      mcWeightSys.Systematic(syst) = converter.str();
    }
  }
  std::cout << "Using mcWeight: " << mcWeight << std::endl;
  if(mcWeightSys.Systematics().size() > 0)
  {
    std::cout << "And variations:" << std::endl;
    for(auto& syst: mcWeightSys.Systematics())
      std::cout << "  " << syst << ": " << mcWeightSys.Systematic(syst) << std::endl;
  }
  std::cout << std::endl;

  if(verbose)
    std::cout << "Filling table" << std::endl;
  std::ofstream outputTable(outputDirectory+"/table.tex");
  outputTable << "\\begin{tabular}{r|ccccc}\n";
  outputTable << " & SR & CR & Data in CR & other MC in CR & Estimate\\\\\n\\hline\n";

  if(verbose)
    std::cout << "Doing Fake rate prediction" << std::endl;
  outputTable << "\\hline\n";
  //fakeDD(outputTable, Data, MC, looseSelection + " && " + baseSelection, mcWeight);
  //fakeDD_MCClosure(outputTable, MC, looseSelection + " && " + baseSelection, mcWeight);
  for(auto & variable : variables)
  {
    MCClosure(MC, variable, baseSelection, looseSelection, leptonChannel, analysisLeptonFinalState, mcWeight, outputDirectory);
    // Make a plot of the BDToutput, just for control reasons
    if(verbose && doPlots)
    {
      std::cout << "Building control plots witn LepPt" << std::endl;
      TCanvas c2("LepPt_canv", "", 800, 800);
      c2.cd();

      std::string SR = looseSelection + " && " + leptonChannel + " && " + baseSelection;
      std::string AN = analysisLeptonFinalState + " && " + baseSelection;

      std::cout << "L!T Total MC" << std::endl;
      auto mcS = MC.getStack(variable.name(), variable, mcWeight+"*("+SR+")");
      mcS->Draw("hist");
      c2.SaveAs((outputDirectory+"/LepPt_total_LooseNotTight.png").c_str());
      delete mcS;

      std::cout << "L!T Non-prompt" << std::endl;
      auto mcS_nonPrompt  = MC.getStack(variable.name(), variable, mcWeight+"*("+SR+"&&(isPrompt==0)"+")");
      mcS_nonPrompt->Draw("hist");
      c2.SaveAs((outputDirectory+"/LepPt_nonPrompt_LooseNotTight.png").c_str());
      delete mcS_nonPrompt;

      std::cout << "L!T Prompt" << std::endl;
      auto mcS_prompt = MC.getStack(variable.name(), variable, mcWeight+"*("+SR+"&&(isPrompt==1)"+")");
      mcS_prompt->Draw("hist");
      c2.SaveAs((outputDirectory+"/LepPt_prompt_LooseNotTight.png").c_str());
      delete mcS_prompt;

      std::cout << "Weighted" << std::endl;
      auto LNTinSR = MC.getStack(variable.name(), variable, mcWeight+"*looseNotTightWeight2017MCClosure*("+SR+")");
      LNTinSR->Draw("hist");
      c2.SaveAs((outputDirectory+"/LepPt_weighted_LooseNotTight.png").c_str());
      delete LNTinSR;

      std::cout << "Weighted Prompt" << std::endl;
      auto LNTMCinSR = MC.getStack(variable.name(), variable, mcWeight+"*looseNotTightWeight2017MCClosure*("+SR+" && (isPrompt==1)"+")");
      LNTMCinSR->Draw("hist");
      c2.SaveAs((outputDirectory+"/LepPt_weighted_prompt_LooseNotTight.png").c_str());
      delete LNTMCinSR;

      std::cout << "Full MC, tight selection with non-Prompt" << std::endl;
      auto fullMC = MC.getStack(variable.name(), variable, mcWeight+"*("+AN+" && !(isPrompt==1)"+")");
      fullMC->Draw("hist");
      c2.SaveAs((outputDirectory+"/LepPt_full_tight_nonPrompt.png").c_str());
      delete fullMC;
    }
  }

  return 0;
}

doubleUnc naiveDD(std::ostream &outputTable, ProcessInfo &toEstimate, SampleReader &Data, SampleReader &MC, std::string signalRegion, std::string controlRegion, std::string mcWeight)
{
  doubleUnc NinSR = toEstimate.getYield(signalRegion, mcWeight);
  doubleUnc NinCR = toEstimate.getYield(controlRegion, mcWeight);
  doubleUnc DatainCR = Data.getYield(controlRegion, "1.0");
  doubleUnc otherMC (0,0);
  if(static_cast<double>(NinSR) == 0)
    NinSR = doubleUnc(4,2);
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

  return estimate;
}

doubleUnc fakeDD(std::ostream &outputTable, SampleReader &LNTData, SampleReader &LNTMC, std::string signalRegion, std::string mcWeight)
{
  doubleUnc LNTinSR = LNTData.getYield(signalRegion, "weight");
  doubleUnc LNTMCinSR = LNTMC.getYield(signalRegion + " && isPrompt == 1", mcWeight);

  doubleUnc NinCR = LNTMC.getYield(signalRegion + " && isPrompt == 1", mcWeight+"/looseNotTightWeight");
  doubleUnc DatainCR = LNTData.getYield(signalRegion, "1.0");

  doubleUnc estimate = LNTinSR - LNTMCinSR;

  outputTable << "Fakes & - & $";
  outputTable << NinCR << "$ ($" << LNTMCinSR << "$) & $";
  outputTable << DatainCR << "$ ($" << LNTinSR << "$) & - & $";
  outputTable << estimate << "$\\\\\n" << std::flush;

  return estimate;
}

doubleUnc fakeDD_MCClosure(std::ostream &outputTable, SampleReader &LNTMC, std::string signalRegion, std::string mcWeight)
{
  //doubleUnc LNTinSR = LNTMC.getYield(signalRegion, mcWeight);
  //doubleUnc LNTMCinSR = LNTMC.getYield(signalRegion + " && isPrompt == 1", mcWeight);
  doubleUnc LNTinSR = LNTMC.getYield(signalRegion, mcWeight+"*looseNotTightWeight2017MCClosure");
  doubleUnc LNTMCinSR = LNTMC.getYield(signalRegion + " && isPrompt == 1", mcWeight+"*looseNotTightWeight2017MCClosure");
  doubleUnc estimate = LNTinSR - LNTMCinSR;
  std::cout << "N estimated: "<< estimate << std::endl;

  doubleUnc NinCR = LNTMC.getYield(signalRegion + " && isPrompt == 1", mcWeight+"/looseNotTightWeight2017MCClosure");
  doubleUnc NinCR_np = LNTMC.getYield(signalRegion + " && isPrompt == 0", mcWeight+"/looseNotTightWeight2017MCClosure");

  outputTable << "Fakes & - & $";
  outputTable << NinCR << "$ ($" << LNTMCinSR << "$) & $";
  outputTable << NinCR_np << "$ ($" << LNTinSR << "$) & - & $";
  outputTable << estimate << "$\\\\\n" << std::flush;

  return estimate;
}

TH1* histsSumFromStack(THStack* stack)
{
  //TH1D* mcH = nullptr;
  const Int_t NBINS = 8;
  Double_t edges[NBINS+1] = {3.5,5,12,20,30,50,80,200,210};
  TH1D* mcH = new TH1D("mcH", "lepPt", NBINS, edges);
  //TH1D* mcH = new TH1D("mcH", "lepPt", 20,0,200.0);
  TList* hists = stack->GetHists();
  TIter next(hists);
  TObject* obj = nullptr;
  while ((obj = next()))
  {
    TH1* thisHist = static_cast<TH1*>(obj);
    mcH->Add(thisHist);
  }
  return mcH;
}

doubleUnc MCClosure(SampleReader &MC, VariableInfo& variable, std::string preSelection, std::string looseSelection, std::string leptonLoose ,std::string leptonTight,std::string mcWeight, std::string outputDirectory)
{
  std::string SR = looseSelection + " && " + leptonLoose + " && " + preSelection;
  std::cout << SR << std::endl;
  std::string AN = leptonTight + " && " + preSelection;
  std::cout << AN << std::endl;
  auto LNTinSR = MC.getStack(variable.name(), variable, mcWeight+"*looseNotTightWeight2017MCClosure*("+SR+")");
  auto sumInSR = histsSumFromStack(LNTinSR);
  auto LNTMCinSR = MC.getStack(variable.name(), variable, mcWeight+"*looseNotTightWeight2017MCClosure*("+SR+" && (isPrompt==1)"+")");
  auto sumMCinSR = histsSumFromStack(LNTMCinSR);
  auto fullMC = MC.getStack(variable.name(), variable, mcWeight+"*("+AN+" && !(isPrompt==1)"+")");
  auto sumFullMC = histsSumFromStack(fullMC);

  doubleUnc N = MC.getYield(AN+" && !(isPrompt==1)", mcWeight);
  std::cout << "Yield in Full MC-fakes: "<< N << std::endl;

  const Int_t NBINS = 8;
  Double_t edges[NBINS+1] = {3.5,5,12,20,30,50,80,200,210};
  TH1D* estimatedHist = new TH1D("estimatedHist", "lepPt", NBINS, edges);
  doubleUnc sumEstimated(0,0);
  for(int xbin=1; xbin <= sumFullMC->GetXaxis()->GetNbins(); xbin++)
  {
    doubleUnc binInSR(sumInSR->GetBinContent(xbin),sumInSR->GetBinError(xbin));
    doubleUnc binMCinSR(sumMCinSR->GetBinContent(xbin),sumMCinSR->GetBinError(xbin));
    auto estimate = binInSR - binMCinSR;

    sumEstimated += estimate;

    estimatedHist->SetBinContent(xbin, estimate.value());
    estimatedHist->SetBinError(xbin, estimate.uncertainty());
    //std::cout << estimate.value() << std::endl;
  }
  std::cout << "Total predicted: "<< sumEstimated << std::endl;

  auto ratio = static_cast<TH1D*>(estimatedHist->Clone("Ratio"));
  ratio->SetTitle("predicted/MC");
  ratio->Divide(sumFullMC);

  TCanvas c1("Closure_canv", "", 800, 800);
  gStyle->SetOptStat(0);

  TPad* t1 = new TPad("t1","t1", 0.0, 0.20, 1.0, 1.0);
  t1->Draw();
  t1->cd();
  //t1->SetLogy(true);
  fullMC->Draw("hist");
  estimatedHist->SetTitle("Prediction");
  estimatedHist->SetLineColor(1);
  estimatedHist->SetLineWidth(2.0);
  estimatedHist->Draw("same");

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
  sprintf(Buffer, "CMS preliminary, #sqrt{s}=%.1f TeV, #scale[0.5]{#int} L=%.1f fb^{-1}", 13.0, 38666.0/1000);
  T->AddText(Buffer);
  T->Draw("same");
  T->SetBorderSize(0);

  TPad* t2 = new TPad("t2","t2", 0.0, 0.0, 1.0, 0.2);
  c1.cd();
  t2->Draw();
  t2->cd();
  t2->SetGridy(true);
  t2->SetPad(0,0.0,1.0,0.2);
  t2->SetTopMargin(0);
  t2->SetBottomMargin(0.5);

  double yscale = (1.0-0.2)/(0.18-0);
  ratio->GetYaxis()->SetTitle("Pred/#Sigma MC");
  ratio->GetXaxis()->SetTitle("");
  ratio->GetXaxis()->SetTitleOffset(1.3);
  ratio->GetXaxis()->SetLabelSize(0.033*yscale);
  ratio->GetXaxis()->SetTitleSize(0.036*yscale);
  ratio->GetXaxis()->SetTickLength(0.03*yscale);
  ratio->GetYaxis()->SetTitleOffset(0.3);
  ratio->GetYaxis()->SetNdivisions(5);
  ratio->GetYaxis()->SetLabelSize(0.033*yscale);
  ratio->GetYaxis()->SetTitleSize(0.036*yscale);
  ratio->SetMinimum(0.5);
  ratio->SetMaximum(1.5);
  ratio->Draw("same");

  c1.SaveAs((outputDirectory+"/closure.png").c_str());
  doubleUnc placeholder = MC.getYield(AN+"&& !(isPrompt==1)", mcWeight);
  return placeholder;
  delete LNTinSR;
  delete LNTMCinSR;
  delete fullMC;
}

void printHelp()
{
  std::cout << "The program has the following parameters:" << std::endl;
  std::cout << "\t--help\t- Print this message" << std::endl;

  return;
}
