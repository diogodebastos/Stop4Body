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
#include <TList.h>
#include <TCollection.h>
#include <TObject.h>
#include <TLatex.h>

#include <iostream>
#include <vector>
#include <sstream>
#include <map>
#include <fstream>
#include <stdexcept>
#include <regex>

#include "UserCode/Stop4Body/interface/json.hpp"
#include "UserCode/Stop4Body/interface/SampleReader.h"
#include "UserCode/Stop4Body/interface/VariableReader.h"
#define _USE_CERN_ROOT
#include "UserCode/Stop4Body/interface/ValueWithSystematics.h"

using json = nlohmann::json;

void printHelp();

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

std::string Sanitize(std::string inStr)
{
  std::regex e1 ("[^\\w]+");
  auto tmpStr = std::regex_replace(inStr, e1, "_");
  std::regex e2 ("_$");
  return std::regex_replace(tmpStr, e2, "");
}

std::string cleanString(std::string inputStr)
{
  std::regex nonASCII("[^a-zA-Z0-9 _]");
  std::regex multipleSpace(" +");
  std::regex multipleUnderscore("_+");

  std::string replaced = std::regex_replace(inputStr, nonASCII, "_");
  replaced = std::regex_replace(replaced, multipleSpace, "_");
  replaced = std::regex_replace(replaced, multipleUnderscore, "_");

  return replaced;
}

int main(int argc, char** argv)
{
  std::string jsonFileName = "";
  std::string inputDirectory = "";
  std::string outputDirectory = "./plots/";
  std::string suffix = "";
  std::string variablesJson = "";
  std::string cutsJson = "";
  double luminosity = -1.0;
  bool noPUweight = false;
  bool puTest = false;
  bool verbose = false;
  bool cumulativeCuts = false;
  bool rawEvents = false;
  bool noSF = false;
  bool unblind = false;
  bool dofakeclosure = false;
  bool doSummary = false;
  bool final = false;

  double constantUncertainties = 0;
  constantUncertainties += 0.025*0.025; // Luminosity uncertainty
  constantUncertainties +=  0.01* 0.01; // Trigger efficiency
  constantUncertainties +=  0.01* 0.01; // Pile Up
  constantUncertainties +=  0.16* 0.16; // JES
  constantUncertainties +=  0.06* 0.06; // JER
  constantUncertainties +=  0.08* 0.08; // b-tag
  constantUncertainties +=  0.01* 0.01; // Lep ID
  constantUncertainties +=  0.01* 0.01; // Lep Iso


  // Build selection strings, with the systematic variations
  ValueWithSystematics<std::string> Met          = std::string("Met");
  ValueWithSystematics<std::string> DPhiJet1Jet2 = std::string("DPhiJet1Jet2");
  ValueWithSystematics<std::string> Jet1Pt       = std::string("Jet1Pt");
  ValueWithSystematics<std::string> Jet2Pt       = std::string("Jet2Pt");
  ValueWithSystematics<std::string> HT           = std::string("HT");
  ValueWithSystematics<std::string> NbLoose      = std::string("NbLoose");
  ValueWithSystematics<std::string> NbTight      = std::string("NbTight");
  ValueWithSystematics<std::string> BDT          = std::string("BDT");
  ValueWithSystematics<std::string> LepPt        = std::string("LepPt");
  { // Loading the systematic variations of the quantities above
    std::vector<std::string> variations = {"JES_Up", "JES_Down", "JER_Up", "JER_Down"};
    for(auto& syst : variations)
    {
      Met.Systematic(syst)          = Met.Value() + "_" + syst;
      DPhiJet1Jet2.Systematic(syst) = DPhiJet1Jet2.Value() + "_" + syst;
      Jet1Pt.Systematic(syst)       = Jet1Pt.Value() + "_" + syst;
      Jet2Pt.Systematic(syst)       = Jet2Pt.Value() + "_" + syst;
      HT.Systematic(syst)           = HT.Value() + "_" + syst;
      NbLoose.Systematic(syst)      = NbLoose.Value() + "_" + syst;
      NbTight.Systematic(syst)      = NbTight.Value() + "_" + syst;
      BDT.Systematic(syst)          = BDT.Value() + "_" + syst;
    }
  }

  std::vector<std::string> systematics = {"JES", "JER"};
  {
    auto loadSystName = [&systematics](std::string baseName, int bins, int min = 1) -> void
    {
      for(int i = min; i <= bins; ++i)
      {
        std::stringstream converter;
        converter << baseName << i;
        systematics.push_back(converter.str());
      }
      return;
    };

    systematics.push_back("PU");

    systematics.push_back("Q2");
    //loadSystName("Q2_", 8);
    //systematics.push_back("Q2_1");
    //systematics.push_back("Q2_2");
    //systematics.push_back("Q2_3");
    //systematics.push_back("Q2_4");
    ////systematics.push_back("Q2_5");
    //systematics.push_back("Q2_6");
    ////systematics.push_back("Q2_7");
    //systematics.push_back("Q2_8");

    systematics.push_back("CFErr1");
    systematics.push_back("CFErr2");
    systematics.push_back("HF");
    systematics.push_back("HFStats1");
    systematics.push_back("HFStats2");
    systematics.push_back("LF");
    systematics.push_back("LFStats1");
    systematics.push_back("LFStats2");

    //systematics.push_back("FullFast");
    systematics.push_back("FullFast_HIIP_AltCorr");
    systematics.push_back("FullFast_ID_AltCorr");
    //loadSystName("FullFast_HIIP_Electron_Bin", 40);
    //loadSystName("FullFast_HIIP_Muon_Bin", 48);
    //loadSystName("FullFast_ID_Electron_Bin", 35);
    //loadSystName("FullFast_ID_Muon_Bin", 42);

    systematics.push_back("LeptonIDSF_AltCorr");
    systematics.push_back("LeptonISOSF_AltCorr");
    //loadSystName("LeptonIDSF_Electron_Bin", 98);
    //loadSystName("LeptonIDSF_Muon_Bin", 56);
    //loadSystName("LeptonISOSF_Electron_Bin", 12);
    //loadSystName("LeptonISOSF_Muon_Bin", 6);

    //systematics.push_back("ISRweight_AltCorr");
    loadSystName("ISRweight_Bin", 6);

    //systematics.push_back("EWKISRweight_AltCorr");
    loadSystName("EWKISRweight_Bin", 7);

    //systematics.push_back("TightLoose_AltCorr");
    loadSystName("TightLoose_Electron_Bin", 16);
    loadSystName("TightLoose_Muon_Bin", 18);

    //systematics.push_back("TightLoose_NU_AltCorr");
    loadSystName("TightLoose_NU_Bin", 5);

    systematics.push_back("triggerEfficiency");

    //systematics.push_back("triggerEfficiency_stat");
  }
  std::vector<std::string> variations;
  {
    for(auto& syst : systematics)
    {
      if(syst == "Q2")
      {
        variations.push_back("Q2_1");
        variations.push_back("Q2_2");
        variations.push_back("Q2_3");
        variations.push_back("Q2_4");
        //variations.push_back("Q2_5");
        variations.push_back("Q2_6");
        //variations.push_back("Q2_7");
        variations.push_back("Q2_8");
        continue;
      }
      variations.push_back(syst+"_Up");
      variations.push_back(syst+"_Down");
    }
  }
  ValueWithSystematics<std::string> weight = std::string("weight");
  for(auto& syst : variations)
  {
    weight.Systematic(syst) = weight.Value() + "_" + syst;
  }

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

    if(argument == "--cuts")
      cutsJson = argv[++i];

    if(argument == "--lumi")
    {
      std::stringstream convert;
      convert << argv[++i];
      convert >> luminosity;
    }

    if(argument == "--noPUweight")
    {
      noPUweight = true;
    }

    if(argument == "--puTest") // This option normalizes data and MC so we compare shapes
    {
      puTest = true;
    }

    if(argument == "--verbose")
    {
      verbose = true;
    }

    if(argument == "--final")
    {
      final = true;
    }

    if(argument == "--cumulative")
    {
      cumulativeCuts = true;
    }

    if(argument == "--rawEvents") // Change the MC weight to 1 to get the raw number of events (does not affect if puTest is set)
    {
      rawEvents = true;
    }

    if(argument == "--noSF")
    {
      noSF = true;
    }

    if(argument == "--unblind")
    {
      unblind = true;
    }

    if(argument == "--dofakeclosure")
    {
      dofakeclosure = true;
      unblind = true;
    }

    if(argument == "--doSummary")
      doSummary = true;
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

  gStyle->SetOptStat(000000);
  gStyle->SetOptTitle(0);
  gStyle->SetPadTickY(1);

  std::cout << "Reading json files" << std::endl;
  VariableJsonLoader variables(variablesJson);
  TwoDVariableJsonLoader twoDvariables(variablesJson);
  SampleReader samples(jsonFileName, inputDirectory, suffix);

  auto MC = samples.getMCBkg();
  auto Sig = samples.getMCSig();
  auto Data = samples.getData();

  if(luminosity <= 0)
    luminosity = Data.getLumi();

  {
    json jsonFile;
    std::ifstream inputFile(variablesJson);
    inputFile >> jsonFile;

    if(jsonFile.count("prefilter") != 0)
    {
      std::string prefilter = jsonFile["prefilter"];
      std::cout << "Filtering the trees with: " << prefilter << std::endl;
      if(prefilter == "")
      {
        MC.filter("(weight_Q2_1 < 2) && (weight_Q2_2 < 2) && (weight_Q2_3 < 2) && (weight_Q2_4 < 2) && (weight_Q2_6 < 2) && (weight_Q2_8 < 2)");
        Sig.filter("(weight_Q2_1 < 2) && (weight_Q2_2 < 2) && (weight_Q2_3 < 2) && (weight_Q2_4 < 2) && (weight_Q2_6 < 2) && (weight_Q2_8 < 2)");
      }
      else
      {
        MC.filter(prefilter + " && (weight_Q2_1 < 2) && (weight_Q2_2 < 2) && (weight_Q2_3 < 2) && (weight_Q2_4 < 2) && (weight_Q2_6 < 2) && (weight_Q2_8 < 2)");
        Sig.filter(prefilter + " && (weight_Q2_1 < 2) && (weight_Q2_2 < 2) && (weight_Q2_3 < 2) && (weight_Q2_4 < 2) && (weight_Q2_6 < 2) && (weight_Q2_8 < 2)");
        Data.filter(prefilter);
      }
      std::cout << "Done filtering the trees" << std::endl;
    }
  }

  std::string mcWeight;
  {
    std::stringstream converter;
    if(!noSF)
    {
      converter << "splitFactor*weight"; // Full
      //converter << "weight/(triggerEfficiency*WISRSF*ISRweight)"; // Incrementally adding new tests
      //converter << "weight/puWeight"; // Full no PU
      //converter << "XS*filterEfficiency*puWeight*genWeight/sumGenWeight";
      if(noPUweight)
        converter << "/puWeight";
    }
    else
    {
      converter << "XS*filterEfficiency*(genWeight/sumGenWeight)";
    }
    converter << "*" << luminosity;
    converter >> mcWeight;
  }
  if(rawEvents)
    mcWeight = "1.0";
  std::cout << "Using mcWeight: " << mcWeight << std::endl;

  std::vector<CutInfo> cutFlow;
  if(cutsJson == "")
  {
    cutFlow.push_back(CutInfo("Preselection", "HT > 200", "$H_T > 200$"));
    //cutFlow.push_back(CutInfo("JetPt110", "Jet1Pt > 110", "$p_T\\left(j_1\\right) > 110$"));
    //cutFlow.push_back(CutInfo("MET300", "Met > 300", "$MET > 300$"));
    //cutFlow.push_back(CutInfo("LepPt30", "LepPt < 30", "$p_T\\left(l\\right) < 30$"));
    cutFlow.push_back(CutInfo("Selection", "(DPhiJet1Jet2 < 2.5 || Jet2Pt < 60) && (HT > 200) && (Met > 280) && (Jet1Pt > 110) && (nGoodEl+nGoodMu <= 2)", "Selection"));
    cutFlow.push_back(CutInfo("Test", "(DPhiJet1Jet2 < 2.5 || Jet2Pt < 60) && (HT > 200) && (Met > 280) && (Jet1Pt > 110) && (nGoodEl+nGoodMu <= 2) && (Njet60 < 3)", "$N(jet_{60}) < 3$"));
  }
  else
  {
    json jsonFile;
    std::ifstream inputFile(cutsJson);
    inputFile >> jsonFile;

    if(jsonFile.count("cuts") == 0)
      throw MissingJSONParam("The JSON file does not contain the 'cuts' entry. It is not a valid file.");

    for(auto& cut : jsonFile["cuts"])
    {
      try
      {
        std::string cutName, cutString, cutLatex;

        if(cut.count("name") == 0)
          throw MissingJSONParam("The cut does not have a name.");
        cutName = cut["name"];

        if(cut.count("expression") == 0)
          throw MissingJSONParam("The cut does not have an expression.");
        cutString = cut["expression"];

        if(cut.count("latex") == 0)
        {
          cutLatex = cutName;
        }
        else
        {
          cutLatex = cut["latex"];
        }

        cutFlow.push_back(CutInfo(cutName, cutString, cutLatex));
      }
      catch(MissingJSONParam& exception)
      {
        std::cout << "Incomplete cut found, skipping it." << std::endl;
        std::cout << "The message was: " << exception.what() << std::endl;
      }
    }
  }

  std::ofstream cutFlowTable(outputDirectory+"/cutFlow.tex");

  cutFlowTable << "\\begin{tabular}{";
  cutFlowTable << "c|";
  for(auto& process : MC)
  {
    cutFlowTable << "c";
    process.label();
  }
  cutFlowTable << "|c";
  for(auto& process : Data)
  {
    cutFlowTable << "c";
    process.label();
  }
  cutFlowTable << "|";
  for(auto& process : Sig)
  {
    cutFlowTable << "c";
    process.label();
  }
  cutFlowTable << "}\n";

  //cutFlowTable << " & ";
  for(auto& process : MC)
  {
    cutFlowTable << " & $" << process.label() << "$";
  }
  cutFlowTable << " & Sum";
  for(auto& process : Data)
  {
    cutFlowTable << " & $" << process.label() << "$";
  }
  for(auto& process : Sig)
  {
    cutFlowTable << " & $" << process.label() << "$";
  }
  cutFlowTable << "\\\\\n";

  std::string selection = "";
  std::string blindSel = "";
  if(verbose)
    std::cout << "Data has BDT: " << ((Data.hasBDT())?("True"):("False")) << std::endl;
  if(!unblind && Data.hasBDT())
  {
    if(verbose)
      std::cout << "Blinding!!!!!!!!!!!!!!" << std::endl;
    blindSel = "(BDT < 0.3) && ";
  }
  std::ofstream outSummary(outputDirectory+"/summary.txt", std::ios_base::binary | std::ios_base::trunc);
  for(auto& cut : cutFlow)
  {
    if(cut.cut() != "")
    {
      if(cumulativeCuts)
      {
        if(selection == "")
          selection  = "(" + cut.cut() + ")";
        else
          selection += " && (" + cut.cut() + ")";
      }
      else
      {
        selection = cut.cut();
      }
    }

    std::cout << "Getting variables and yields with selection (" << selection << ") and weight (" << mcWeight << ")" << std::endl;

    for(auto & variable : variables)
    {
      std::string plotBaseName = cut.name()+"_"+variable.name();
      std::cout << "  Variable: " << variable.name() << std::endl;

      if(doSummary)
      {
        outSummary << "Cut: " << cut.name() << std::endl;
        outSummary << "Variable: " << variable.name() << std::endl;
        outSummary << "Bins:" << std::endl;
      }

      std::string dataSel;
      std::string mcSel;

      if(dofakeclosure)
      {
        mcSel = mcWeight+"*( ((isPrompt == 0) && (isTight == 1)) && "+selection+")";
        dataSel = "weight * ( ((isPrompt == 0) && (isLoose == 1) && (isTight == 0)) && " + selection + ")";
      }
      else
      {
        mcSel = mcWeight+"*("+selection+")";
        if(variable.expression() == "BDT")
          dataSel = blindSel+selection;
        else
          dataSel = selection;
      }

      if(rawEvents)
      {
        mcSel = selection;
      }

      //auto dataH = Data.getHist(cut.name()+"_"+variable.name()+"_Data",   variable.expression(), variable.label()+";Evt.", dataSel    , variable.bins(), variable.min(), variable.max());
      auto dataH = Data.process(0).getHist(cut.name(), variable, dataSel);
      TH1D* sigH = nullptr; // TODO: stack different signal points

      if(dofakeclosure)
        dataH->SetTitle(("DD" + std::string(dataH->GetTitle())).c_str());

      std::string histName = cleanString(cut.name()+"_"+variable.expression());
      THStack* mcS = new THStack((cut.name() + "_" + variable.name()).c_str(), (variable.expression() + ";" + variable.label() + ";Events").c_str());
      TH1D* mcH = nullptr;
      if(variable.hasVarBins())
        mcH = new TH1D(histName.c_str(), (variable.expression() + ";" + variable.label() + ";Events").c_str(), variable.varBins().size() - 1, variable.varBins().data());
      else
        mcH = new TH1D(histName.c_str(), (variable.expression() + ";" + variable.label() + ";Events").c_str(), variable.bins(), variable.min(), variable.max());
      mcH->Sumw2();

      auto cwd = gDirectory;
      TFile syncPlot((outputDirectory+"/"+plotBaseName+"_syncPlot.root").c_str(), "RECREATE");

      dataH->Write("Data");

      for(auto& process : Sig)
      {
        cwd->cd();
        auto tmpHist = process.getHist(cut.name(), variable, mcSel);
        syncPlot.cd();

        tmpHist->Write(Sanitize(process.label()).c_str());
        if(sigH == nullptr)
          sigH = tmpHist;
        else
          delete tmpHist;
      }

      for(auto& process : MC)
      {
        cwd->cd();
        auto tmpHist = process.getHist(cut.name(), variable, mcSel);
        syncPlot.cd();

        tmpHist->Write(process.tag().c_str());
        mcS->Add(tmpHist);
        mcH->Add(tmpHist);
      }

      mcH->Write("mcSum");
      mcS->Write("mcStack");

      TH1D* systUncEnv = static_cast<TH1D*>(mcH->Clone("relativeSystematicUncertaintiesEnvelope"));
      TH1D* systUnc    = static_cast<TH1D*>(mcH->Clone("relativeSystematicUncertainties"));
      systUnc->Sumw2();
      for(int xbin=0; xbin <= systUnc->GetXaxis()->GetNbins(); xbin++)
      {
        double unc = 0;
        double binContent = mcH->GetBinContent(xbin);
        if(binContent == 0)
          continue;

        TList* hists = mcS->GetHists();
        TIter next(hists);
        TObject* obj = nullptr;
        while ((obj = next()))
        {
          TH1* thisHist = static_cast<TH1*>(obj);
          std::string histName = thisHist->GetName();
          double thisUnc = 0;
          double thisBinContent=thisHist->GetBinContent(xbin);
          if(histName.find("WJets") != std::string::npos || histName.find("ttbar") != std::string::npos) // ttbar and WJets
          {
            thisUnc = thisBinContent*0.2;
          }
          else
          {
            thisUnc = thisBinContent*0.5;
          }

          thisUnc = thisUnc/binContent;
          unc += thisUnc*thisUnc;
        }

        systUncEnv->SetBinContent(xbin, 0);
        systUnc->SetBinContent(xbin, 0);

        systUncEnv->SetBinError(xbin, std::sqrt(unc + constantUncertainties));
        systUnc->SetBinError(xbin, std::sqrt(unc + 0.025*0.025)); //Only add lumi
      }

      TDirectory *systDir = syncPlot.mkdir("SystematicVariations");
      auto replaceSyst = [&](std::string orig, std::string target, std::string with) -> std::string
      {
        std::string retVal;
        retVal = std::regex_replace(orig, std::regex(target), with);
        return retVal;
      };
      auto getVarHist = [&](std::string name, VariableInfo variable, std::string selection, std::string systematic) -> TH1D*
      {
        std::string innerName = name + "_" + systematic;

        selection = replaceSyst(selection, Met         .Value(), Met         .GetSystematicOrValue(systematic));
        selection = replaceSyst(selection, DPhiJet1Jet2.Value(), DPhiJet1Jet2.GetSystematicOrValue(systematic));
        selection = replaceSyst(selection, Jet1Pt      .Value(), Jet1Pt      .GetSystematicOrValue(systematic));
        selection = replaceSyst(selection, Jet2Pt      .Value(), Jet2Pt      .GetSystematicOrValue(systematic));
        selection = replaceSyst(selection, HT          .Value(), HT          .GetSystematicOrValue(systematic));
        selection = replaceSyst(selection, NbLoose     .Value(), NbLoose     .GetSystematicOrValue(systematic));
        selection = replaceSyst(selection, NbTight     .Value(), NbTight     .GetSystematicOrValue(systematic));
        selection = replaceSyst(selection, BDT         .Value(), BDT         .GetSystematicOrValue(systematic));
        selection = replaceSyst(selection, LepPt       .Value(), LepPt       .GetSystematicOrValue(systematic));
        selection = replaceSyst(selection, weight      .Value(), weight      .GetSystematicOrValue(systematic));

        variable.expression(replaceSyst(variable.expression(), Met         .Value(), Met         .GetSystematicOrValue(systematic)));
        variable.expression(replaceSyst(variable.expression(), DPhiJet1Jet2.Value(), DPhiJet1Jet2.GetSystematicOrValue(systematic)));
        variable.expression(replaceSyst(variable.expression(), Jet1Pt      .Value(), Jet1Pt      .GetSystematicOrValue(systematic)));
        variable.expression(replaceSyst(variable.expression(), Jet2Pt      .Value(), Jet2Pt      .GetSystematicOrValue(systematic)));
        variable.expression(replaceSyst(variable.expression(), HT          .Value(), HT          .GetSystematicOrValue(systematic)));
        variable.expression(replaceSyst(variable.expression(), NbLoose     .Value(), NbLoose     .GetSystematicOrValue(systematic)));
        variable.expression(replaceSyst(variable.expression(), NbTight     .Value(), NbTight     .GetSystematicOrValue(systematic)));
        variable.expression(replaceSyst(variable.expression(), BDT         .Value(), BDT         .GetSystematicOrValue(systematic)));
        variable.expression(replaceSyst(variable.expression(), LepPt       .Value(), LepPt       .GetSystematicOrValue(systematic)));
        variable.expression(replaceSyst(variable.expression(), weight      .Value(), weight      .GetSystematicOrValue(systematic)));

        auto retVal = MC.getHist(innerName, variable, selection);

        systDir->cd();
        retVal->Write(systematic.c_str());
        syncPlot.cd();
        return retVal;
      };
      auto getSyst = [&](std::string systematic) -> TH1D*
      {
        TH1D* retVal = nullptr;

        std::vector<std::string> myVariations;
        if(systematic == "Q2")
        {
          myVariations.push_back(systematic + "_1");
          myVariations.push_back(systematic + "_2");
          myVariations.push_back(systematic + "_3");
          myVariations.push_back(systematic + "_4");
          //myVariations.push_back(systematic + "_5");
          myVariations.push_back(systematic + "_6");
          //myVariations.push_back(systematic + "_7");
          myVariations.push_back(systematic + "_8");
        }
        else
        {
          myVariations.push_back(systematic + "_Up");
          myVariations.push_back(systematic + "_Down");
        }

        for(auto& variation : myVariations)
        {
          auto varHist = getVarHist(cut.name(), variable, mcSel, variation);
          varHist->Add(mcH, -1);

          if(retVal == nullptr)
          {
            retVal = varHist;

            for(int xbin=0; xbin <= retVal->GetXaxis()->GetNbins(); xbin++)
            {
              double unc = std::abs(retVal->GetBinContent(xbin));
              double binValue = mcH->GetBinContent(xbin);

              if(binValue == 0)
                unc = 0;
              else
                unc = unc/binValue;

              retVal->SetBinContent(xbin, 0);
              retVal->SetBinError(xbin, unc);
            }
          }
          else
          {
            for(int xbin=0; xbin <= varHist->GetXaxis()->GetNbins(); xbin++)
            {
              double prevUnc = retVal->GetBinError(xbin);

              double newUnc = std::abs(varHist->GetBinContent(xbin));
              double binValue = mcH->GetBinContent(xbin);

              if(binValue == 0)
                newUnc = 0;
              else
                newUnc = newUnc/binValue;

              if(newUnc > prevUnc)
                retVal->SetBinError(xbin, newUnc);
            }

            delete varHist;
          }
        }

        systDir->cd();
        retVal->Write(systematic.c_str());
        syncPlot.cd();
        return retVal;
      };
      auto getTotalSyst = [&]() -> TH1D*
      {
        TH1D* retVal = nullptr;

        for(auto & systematic : systematics)
        {
          std::cout << "    " << systematic << std::endl;
          auto tmpHist = getSyst(systematic);
          if(retVal == nullptr)
          {
            retVal = tmpHist;
            retVal->Sumw2();
          }
          else
          {
            retVal->Add(tmpHist);
            delete tmpHist;
          }
        }

        return retVal;
      };

      auto otherSyst = getTotalSyst();
      systUnc->Add(otherSyst);
      delete otherSyst;

      systUncEnv->Write();
      systUnc->Write();

      delete systUncEnv;
      delete systUnc;
      cwd->cd();

      auto ratio = static_cast<TH1D*>(dataH->Clone((cut.name()+"_"+variable.name()+"_Ratio").c_str()));
      ratio->SetTitle((";" + variable.label() + ";Data/#Sigma MC").c_str());
      ratio->Divide(mcH);

      for(int xbin=1; xbin <= dataH->GetXaxis()->GetNbins(); xbin++)
      {
        doubleUnc data  (dataH->GetBinContent(xbin), dataH->GetBinError(xbin));
        doubleUnc mcSum (  mcH->GetBinContent(xbin),   mcH->GetBinError(xbin));
        if(doSummary)
        {
          outSummary << "  " << xbin << ": Data = " << data;
          outSummary << "; MC = " << mcSum;
          outSummary << "; Diff = " << (mcSum-data)*((dofakeclosure)?(-1.0):(1.0));
          outSummary << "; Relative = " << (mcSum-data)/((dofakeclosure)?(data*(-1.0)):(mcSum)) << std::endl;
        }
      }
      if(doSummary)
        outSummary << std::endl;

      TCanvas c1((cut.name()+"_"+variable.name()).c_str(), "", 1200, 1350); // 800x900 in original, then scaled by 1.5
      gStyle->SetOptStat(0);

      double ratioPadFraction = 0.20;
      double legPadFraction = 0.2;

      TPad* t1 = new TPad("t1","t1", 0.0, ratioPadFraction, 1.0, 1.0);
      TPad* t2 = new TPad("t2","t2", 0.0, 0.0, 1.0, ratioPadFraction);

      t1->Draw();
      t1->cd();
      if(variable.logx())
        t1->SetLogx();
      if(variable.logy())
        t1->SetLogy();
      t1->SetTopMargin(0.07);
      //t1->SetBottomMargin(0.10);
      //t1->SetRightMargin(0.20);
      mcS->Draw("hist");
      if(variable.legTop())
      {
        double maxVal = mcS->GetYaxis()->GetXmax();
        double minVal = mcS->GetYaxis()->GetXmin();

        maxVal = std::max(mcS->GetMaximum(), dataH->GetMaximum());
        minVal = std::min(mcS->GetMinimum(), dataH->GetMinimum());
        minVal = std::min(0.2, maxVal/1000);

        if(t1->GetLogy() == 1)
        {
          if(minVal == 0)
            minVal = 0.5;

          maxVal = std::pow(maxVal/std::pow(minVal, legPadFraction), 1/(1 - legPadFraction));
        }
        else
        {
          maxVal = maxVal + legPadFraction*(maxVal - minVal) / (1 - legPadFraction);
        }

        mcS->SetMinimum(minVal);
        mcS->SetMaximum(maxVal);
        //mcS->Draw("hist");
      }
      if(!puTest)
      {
        dataH->Draw("same");
        sigH->Draw("hist same");
      }
      else
      {
        TH1D* data2 = static_cast<TH1D*>(dataH->Clone("dataToDraw"));
        data2->Scale(mcH->Integral()/data2->Integral());
        data2->Draw("same");
      }

      TLegend *legA;
      //std::cout << "  The legTop value is: " << variable.legTop() << " (" << (variable.legTop()?"true":"false") << ")" << std::endl;
      //std::cout << "  The legLeft value is: " << variable.legLeft() << " (" << (variable.legLeft()?"true":"false") << ")" << std::endl;
      if(variable.legTop())
      {
        //legA = gPad->BuildLegend(0.155, 1, 0.845, 1-legPadFraction, "NDC"); // The current version does not allow options... what?
        legA = gPad->BuildLegend(0.155, 0.95, 0.845, 0.95*(1-legPadFraction), "");
        legA->SetNColumns(3);
      }
      else
      {
        if(variable.legLeft())
          //legA = gPad->BuildLegend(0.155,0.39,0.35,0.89,"", "NDC"); // The current version does not allow options... what?
          legA = gPad->BuildLegend(0.155,0.39,0.35,0.89,"");
        else
          //legA = gPad->BuildLegend(0.845,0.39,0.65,0.89,"", "NDC"); // The current version does not allow options... what?
          legA = gPad->BuildLegend(0.845,0.39,0.65,0.89,"");
      }

      legA->SetFillColor(0);
      legA->SetFillStyle(0);
      legA->SetLineColor(0);
      legA->SetHeader("");
      legA->SetTextFont(42);

      /*TPaveText* T = new TPaveText(0.1,0.995,0.84,0.95, "NDC");
      T->SetFillColor(0);
      T->SetFillStyle(0);
      T->SetLineColor(0);
      T->SetTextAlign(11);
      char Buffer[1024];
      //CMS Preliminary   35.9 fb^{-1} (13 TeV)
      sprintf(Buffer, "CMS preliminary   #sqrt{s}=%.1f TeV, #scale[0.5]{#int} L=%.1f fb^{-1}", 13.0, luminosity/1000);
      T->AddText(Buffer);
      T->Draw("same");
      T->SetBorderSize(0);// */
      char Buffer[1024];
      sprintf(Buffer, "%.1f fb^{-1} (%.1f TeV)", luminosity/1000, 13.0);
      TLatex *   tex  = new TLatex(0.805,0.975,Buffer);
      tex->SetNDC();
      tex->SetTextAlign(33);
      tex->SetTextFont(42);
      tex->SetTextSize(0.038);
      tex->SetLineWidth(2);
      tex->Draw();
      TLatex *   tex2;
      if(final)
        tex2 = new TLatex(0.15,0.965,"#font[61]{CMS}");
      else
        tex2 = new TLatex(0.15,0.965,"#font[61]{CMS} #font[52]{Preliminary}");
      tex2->SetNDC();
      tex2->SetTextAlign(13);
      tex2->SetTextFont(42);
      tex2->SetTextSize(0.038);
      tex2->SetLineWidth(2);
      tex2->Draw();


      c1.cd();
      t2->Draw();
      t2->cd();
      if(variable.logx())
        t2->SetLogx();
      t2->SetGridy(true);
      t2->SetPad(0,0.0,1.0,ratioPadFraction);
      t2->SetTopMargin(0);
      t2->SetBottomMargin(0.25);

      TH1D *bgUncH = static_cast<TH1D*>(mcH->Clone((cut.name()+"_"+variable.name()+"_bgUncH").c_str()));
      for(int xbin=1; xbin <= bgUncH->GetXaxis()->GetNbins(); xbin++)
      {
        if(bgUncH->GetBinContent(xbin)==0)
          continue;

        double unc = bgUncH->GetBinError(xbin) / bgUncH->GetBinContent(xbin);
        double binContent = bgUncH->GetBinContent(xbin);

        // Add systematic uncertainties
        unc = unc*unc;
        unc += 0.025*0.025; // Luminosity uncertainty
        unc += 0.01*0.01; // Efficiency
        unc += 0.01*0.01; // Pile Up
        unc += 0.04*0.04; // JES
        unc += 0.04*0.04; // Lep ID

        TList* hists = mcS->GetHists();
        TIter next(hists);
        TObject* obj = nullptr;
        while ((obj = next()))
        {
          TH1* thisHist = static_cast<TH1*>(obj);
          std::string histName = thisHist->GetName();
          double thisUnc = 0;
          double thisBinContent=thisHist->GetBinContent(xbin);
          if(histName.find("WJets") != std::string::npos || histName.find("ttbar") != std::string::npos) // ttbar and WJets
          {
            thisUnc = thisBinContent*0.2;
          }
          else
          {
            thisUnc = thisBinContent*0.5;
          }

          thisUnc = thisUnc/binContent;
          unc += thisUnc*thisUnc;
        }

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
      double minErr = 0.5;
      double maxErr = 1.5;
      double yscale = (1.0-0.2)/(0.18-0);
      bgUncH->GetYaxis()->SetTitle("Data/#Sigma MC");
      bgUncH->SetMinimum(minErr);
      bgUncH->SetMaximum(maxErr);
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

      c1.SaveAs((outputDirectory+"/"+plotBaseName+".png").c_str());
      c1.SaveAs((outputDirectory+"/"+plotBaseName+".C").c_str());

      delete dataH;
      delete mcH;
      delete sigH;
      // Delete individual hists in the stack
      TList* histList = mcS->GetHists();
      if(histList != nullptr)
        histList->Delete();
      delete mcS;
      delete ratio;
      delete legA;
      //delete T;
      delete tex;
      delete tex2;
      delete bgUncH;
      delete bgUnc;

      if(doSummary)
        outSummary << std::endl;
    }

    for(auto & twoDvariable : twoDvariables)
    {
      continue;
      std::string dataSel;
      std::string mcSel;

      if(dofakeclosure)
      {
        mcSel = mcWeight+"*( (isPrompt == 0) && (isTight == 1) && "+selection+")";
        dataSel = "weight * ( ((isPrompt == 0) && (isLoose == 1) && (isTight == 0)) && " + selection + ")";
      }
      else
      {
        mcSel = mcWeight+"*("+selection+")";
        if(twoDvariable.X().expression() == "BDT" || twoDvariable.Y().expression() == "BDT")
          dataSel = blindSel+selection;
        else
          dataSel = selection;
      }

      if(rawEvents)
      {
        mcSel = selection;
      }

      int nPlots = 0;
      nPlots += Data.nProcesses();
      nPlots += MC.nProcesses();
      nPlots += Sig.nProcesses();

      int plotX = 400;
      int plotY = 400;
      int canvasX = 0;
      int canvasY = 0;
      int nPadsX = 0;
      int nPadsY = 0;
      switch(nPlots)
      {
        case 1:
          canvasY = plotY;
          canvasX = plotX;
          nPadsX = 1;
          nPadsY = 1;
          break;
        case 2:
          canvasY = plotY;
          canvasX = 2*plotX;
          nPadsX = 2;
          nPadsY = 1;
          break;
        case 3:
          canvasY = plotY;
          canvasX = 3*plotX;
          nPadsX = 3;
          nPadsY = 1;
          break;
        case 4:
          canvasY = 2*plotY;
          canvasX = 2*plotX;
          nPadsX = 2;
          nPadsY = 2;
          break;
        case 5:
        case 6:
          canvasY = 2*plotY;
          canvasX = 3*plotX;
          nPadsX = 3;
          nPadsY = 2;
          break;
        case 7:
        case 8:
          canvasY = 2*plotY;
          canvasX = 4*plotX;
          nPadsX = 4;
          nPadsY = 2;
          break;
        case 9:
          canvasY = 3*plotY;
          canvasX = 3*plotX;
          nPadsX = 3;
          nPadsY = 3;
          break;
        case 10:
        case 11:
        case 12:
          canvasY = 3*plotY;
          canvasX = 4*plotX;
          nPadsX = 4;
          nPadsY = 3;
          break;
        default:
          nPadsX = 4;
          nPadsY = std::ceil(static_cast<double>(nPlots)/nPadsX);
          canvasY = nPadsY*plotY;
          canvasX = nPadsX*plotX;
          //throw std::out_of_range("Calm down speedy, check the number of processes");
      }

      std::vector<TObject*> ObjectToDelete;
      TCanvas c1((cut.name()+"_"+twoDvariable.name()).c_str(), "", canvasX, canvasY);
      c1.SetLogz();
      c1.Divide(nPadsX,nPadsY,0,0);

      int pad = 0;
      for(auto & process : Data)
      {
        pad++;
        TVirtualPad* thisPad = c1.cd(pad);

        thisPad->SetLogz(true);
        thisPad->SetTopMargin(0.10);
        thisPad->SetBottomMargin(0.10);
        thisPad->SetRightMargin(0.20);

        auto hist = process.get2DHist(twoDvariable.X().expression(),
                                      twoDvariable.Y().expression(),
                                      twoDvariable.X().label()+";"+twoDvariable.Y().label()+";Evt.",
                                      dataSel,
                                      twoDvariable.X().bins(),
                                      twoDvariable.X().min(),
                                      twoDvariable.X().max(),
                                      twoDvariable.Y().bins(),
                                      twoDvariable.Y().min(),
                                      twoDvariable.Y().max());
        ObjectToDelete.push_back(hist);
        hist->SetTitle("");
        hist->SetStats(kFALSE);
        hist->Draw("COLZ");

        TPaveText* leg = new TPaveText(0.10,0.995,0.90,0.90, "NDC");
        leg->SetFillColor(0);
        leg->SetFillStyle(0);
        leg->SetLineColor(0);
        leg->SetTextAlign(12);
        leg->AddText(process.label().c_str());
        leg->Draw("same");
        ObjectToDelete.push_back(leg);
      }
      for(auto & process : MC)
      {
        pad++;
        TVirtualPad* thisPad = c1.cd(pad);

        thisPad->SetLogz(true);
        thisPad->SetTopMargin(0.10);
        thisPad->SetBottomMargin(0.10);
        thisPad->SetRightMargin(0.20);

        auto hist = process.get2DHist(twoDvariable.X().expression(),
                                      twoDvariable.Y().expression(),
                                      twoDvariable.X().label()+";"+twoDvariable.Y().label()+";Evt.",
                                      mcSel,
                                      twoDvariable.X().bins(),
                                      twoDvariable.X().min(),
                                      twoDvariable.X().max(),
                                      twoDvariable.Y().bins(),
                                      twoDvariable.Y().min(),
                                      twoDvariable.Y().max());
        ObjectToDelete.push_back(hist);
        hist->SetTitle("");
        hist->SetStats(kFALSE);
        hist->Draw("COLZ");

        TPaveText* leg = new TPaveText(0.10,0.995,0.90,0.90, "NDC");
        leg->SetFillColor(0);
        leg->SetFillStyle(0);
        leg->SetLineColor(0);
        leg->SetTextAlign(12);
        leg->AddText(process.label().c_str());
        leg->Draw("same");
        ObjectToDelete.push_back(leg);
      }
      for(auto & process : Sig)
      {
        pad++;
        TVirtualPad* thisPad = c1.cd(pad);

        thisPad->SetLogz(true);
        thisPad->SetTopMargin(0.10);
        thisPad->SetBottomMargin(0.10);
        thisPad->SetRightMargin(0.20);

        auto hist = process.get2DHist(twoDvariable.X().expression(),
                                      twoDvariable.Y().expression(),
                                      twoDvariable.X().label()+";"+twoDvariable.Y().label()+";Evt.",
                                      mcSel,
                                      twoDvariable.X().bins(),
                                      twoDvariable.X().min(),
                                      twoDvariable.X().max(),
                                      twoDvariable.Y().bins(),
                                      twoDvariable.Y().min(),
                                      twoDvariable.Y().max());
        ObjectToDelete.push_back(hist);
        hist->SetTitle("");
        hist->SetStats(kFALSE);
        hist->Draw("COLZ");

        TPaveText* leg = new TPaveText(0.10,0.995,0.90,0.90, "NDC");
        leg->SetFillColor(0);
        leg->SetFillStyle(0);
        leg->SetLineColor(0);
        leg->SetTextAlign(12);
        leg->AddText(process.label().c_str());
        leg->Draw("same");
        ObjectToDelete.push_back(leg);
      }
      c1.cd(0);

      c1.SaveAs((outputDirectory+"/"+cut.name()+"_"+twoDvariable.name()+".png").c_str());
      c1.SaveAs((outputDirectory+"/"+cut.name()+"_"+twoDvariable.name()+".C").c_str());

      for(unsigned int d = 0; d < ObjectToDelete.size(); d++)
        delete ObjectToDelete[d];
      ObjectToDelete.clear();
    }


    std::string dataSelToUse;
    std::string mcSelToUse;
    std::string dataWeightToUse;
    std::string mcWeightToUse;

    if(dofakeclosure)
    {
      mcSelToUse = "((isPrompt == 0) && (isTight == 1) && "+selection+")";
      mcWeightToUse = mcWeight;
      dataSelToUse = "(((isPrompt == 0) && (isLoose == 1) && (isTight == 0)) && " + selection + ")";
      dataWeightToUse = "weight";
    }
    else
    {
      mcSelToUse = selection;
      mcWeightToUse = mcWeight;
      dataWeightToUse = "1";
      dataSelToUse = blindSel+selection;
    }

    if(rawEvents)
    {
      mcSelToUse = selection;
      mcWeightToUse = "1";
    }

    cutFlowTable << cut.name();
    for(auto& process : MC)
    {
      auto yield = process.getYield(mcSelToUse, mcWeightToUse);
      cutFlowTable << " & $" << yield << "$";
      if(verbose)
      {
        std::cout << process.label() << ": " << yield << std::endl;
        for(auto& sample: process)
          std::cout << sample.tag() << ": " << sample.getYield(mcSelToUse, mcWeightToUse) << std::endl;
      }
    }
    cutFlowTable << " & $" << MC.getYield(mcSelToUse, mcWeightToUse) << "$";
    for(auto& process : Data)
    {
      auto yield = process.getYield(dataSelToUse, dataWeightToUse);
      cutFlowTable << " & $" << yield << "$";
      if(blindSel != "")
        cutFlowTable << " (SR blinded)";
      if(verbose)
        std::cout << process.label() << ": " << yield << std::endl;
    }
    for(auto& process : Sig)
    {
      auto yield = process.getYield(mcSelToUse, mcWeightToUse);
      cutFlowTable << " & $" << yield << "$";
      if(verbose)
        std::cout << process.label() << ": " << yield << std::endl;
    }
    cutFlowTable << "\\\\\n";
  }

  cutFlowTable << "\\end{tabular}";

  return 0;
}

void printHelp()
{
  std::cout << "The program has the following parameters:" << std::endl;
  std::cout << "\t--help\t- Print this message" << std::endl;

  return;
}
