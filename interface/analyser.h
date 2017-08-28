// <author>Cristóvão B da Cruz e Silva</author>
// <email>c.beirao@cern.ch</email>
// <date>2017-04-10</date>
// <summary>Header file for the Analyser class</summary>
//
// <description>
//  Header file with the declarations of the Analyser class and related classes.
//  The base analyser from which other analyses can be derived from is declared within.
// </description>

#ifndef MY_ANALYSER_H
#define MY_ANALYSER_H

//#define MY_ANALYSER_WITH_DEBUG

#include <map>
#include <vector>
#include <string>
#include <exception>
#include <iostream>
#include <fstream>
#include <sstream>

#include "TFile.h"
#include "TTree.h"
//#include "TLorentzVector.h"

//#include "CondFormats/JetMETObjects/interface/JetResolution.h"
//#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"
//#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"

#include "FWCore/PythonParameterSet/interface/MakeParameterSets.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

//#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"
//#include "DataFormats/FWLite/interface/Handle.h"

//#include "PhysicsTools/Utilities/interface/LumiReWeighting.h"

//#include "UserCode/llvv_fwk/interface/PDFInfo.h"
//#include "UserCode/llvv_fwk/interface/MacroUtils.h"
//#include "UserCode/llvv_fwk/interface/llvvObjects.h"
//#include "UserCode/llvv_fwk/interface/SmartSelectionMonitor.h"
//#include "UserCode/llvv_fwk/interface/MuScleFitCorrector.h"
//#include "UserCode/llvv_fwk/interface/ValueWithSystematics.h"

class AnalyserException: public std::exception
{
  public:
    AnalyserException(std::string mess): message(mess)
    {
    }

  private:
    std::string message;

    virtual const char* what() const throw()
    {
      return message.c_str();
    }

  protected:
};

class EventInfo
{
  public:
    EventInfo();
    EventInfo(const EventInfo&) = delete; // Delete copy constructor
    EventInfo(EventInfo&&) = delete;      // Delete move constructor
    void Reset();
    inline void Lock()
    {
      isLocked = true;
      for(auto & kv : eventDoubles)
        kv.second.Lock();
      for(auto & kv : eventInts)
        kv.second.Lock();
      for(auto & kv : eventBools)
        kv.second.Lock();
    };
    #ifdef WITH_UNLOCK
    inline void Unlock()
    {
      isLocked = false;
      for(auto & kv : eventDoubles)
        kv.second.Unlock();
      for(auto & kv : eventInts)
        kv.second.Unlock();
      for(auto & kv : eventBools)
        kv.second.Unlock();
    };
    #endif

    ValueWithSystematics<double>& AddDouble(std::string name, double defaultVal);
    ValueWithSystematics<double>& GetDouble(std::string name);
    ValueWithSystematics<float>&  AddFloat (std::string name, float defaultVal);
    ValueWithSystematics<float>&  GetFloat (std::string name);
    ValueWithSystematics<int>&    AddInt   (std::string name, int defaultVal);
    ValueWithSystematics<int>&    GetInt   (std::string name);
    ValueWithSystematics<bool>&   AddBool  (std::string name, bool defaultVal);
    ValueWithSystematics<bool>&   GetBool  (std::string name);

    void OutputEventListHeader(ofstream& file, const std::vector<std::string>& priority = std::vector<std::string>(0)) const;
    void OutputEventList(ofstream& file, const std::vector<std::string>& priority = std::vector<std::string>(0)) const;
    void SetSummaryTreeBranches(TTree* const tree);

  private:
  protected:
    bool isLocked;
    std::map<std::string,ValueWithSystematics<double>> eventDoubles;
    std::map<std::string,ValueWithSystematics<float>>  eventFloats;
    std::map<std::string,ValueWithSystematics<int>>    eventInts;
    std::map<std::string,ValueWithSystematics<bool>>   eventBools;

    template<class T>
    void OutputValueListHeader(ofstream& file, const ValueWithSystematics<T>& val, const std::string& name) const;

    template<class T>
    void OutputValueList(ofstream& file, const ValueWithSystematics<T>& val) const;

    template<class T>
    void AddBranch(TTree* const tree, ValueWithSystematics<T>& val, std::string name);
};

class Analyser
{
public:
  Analyser(std::string cfgFile);
  virtual ~Analyser();

  virtual void Setup();
  virtual void LoopOverEvents();

  inline void SetEventLimit(size_t val)      { limitEvents = val; };
  inline void SetDebugEvent(bool val)        { debugEvent = val; };
  inline void SetSkipEvents(int val)         { skipEvents = val; };

  inline void RedirectCout() { saveRedirect = true; };
  inline void DiscardCout() { saveRedirect = false; };

  inline void KeepAllEvents() { keepAllEvents = true; };
  inline void KeepTriggeredEvents() { keepAllEvents = false; };

private:

protected:
  size_t limitEvents; // If only a limited number of events should be processed, for example 300 for synchronization purposes
  bool debugEvent; // Whether information about the event should be printed out to help debugging the event info
  int skipEvents; // How many events should be skipped at the beginning (useful for debugging sometimes)
  bool saveRedirect; // Whatever outputs to the regular buffers (cout and cerr) is intercepted and placed in a buffer. If this option is true, the buffer is printed out at the end of the job
  bool keepAllEvents; // If all events should be kept in output, independent of whether they pass selection criteria or not

  bool isSetup; // Whether the Setup step has been ran or not
  std::ostream analyserCout; // The output method to be used by the analyser, this circumvents the buffer mentioned above

  EventInfo eventContent; // The information about the event, as a general rule, this information is either saved to the summary output (ttree) or to the event list (synchronization)

  virtual void LoadCfgOptions(); // Method that loads options from the cfg file
  virtual void InitHistograms(); // Method that initializes the histograms in the output
  virtual void EventContentSetup(); // Method the initializes the event content
  virtual void ProcessEvent(size_t iev); // Method that processes each event
  virtual void FillHistograms(); // Method that fills the histograms

  // User functions associated to each of the above functions.
  // Normally, the general functions should not be overloaded and the user functions should, providing additional details to those set by the common functions
  virtual void UserLoadCfgOptions() = 0;
  virtual void UserInitHistograms() = 0;
  virtual void UserEventContentSetup() = 0;
  virtual void UserProcessEvent(size_t iev) = 0;
  virtual void UserFillHistograms() = 0;
  virtual void UserSetup() = 0;

  template<class T>
  void loadSystematics(std::vector<std::string>& list, ValueWithSystematics<T> variable);

  std::stringstream analyserBuffer; // The buffer for the stream redirection

  edm::ParameterSet cfgOptions; // The options from the CFG file passed from the user
  std::string sampleJSON; // JSON file describing the samples and where to find them
  std::string outputDirectory; // Directory where to place the output, the file names are generated automatically
  bool saveSummaryTree; // Whether the summary output file should be filled (normally yes because it gives us the ttree with which to further process the sample)
  bool outputEventList; // Whether an event list should be produced

  #ifdef MY_ANALYSER_WITH_DEBUG
  bool debug; // Whether information about the code being run should be printed out to help debugging the code itself
  #endif

  std::string outFile; // The file name of the output file (histograms and such)
  std::string summaryOutFile; // The file name of the summary output file (ttree)
  TFile* summaryOutTFile;
  TTree* summaryTree;
  std::string eventlistOutFile;
  ofstream eventListFile;
  SmartSelectionMonitor histMonitor;



  bool isMC;
  double crossSection;
  std::vector<std::string> fileList;
  std::string baseDir;
  std::string outDir;
  std::string jecDir;
  std::string pdfDir;
  bool runSystematics;
  bool applyScaleFactors;
  bool doDDBkg;
  bool isV0JetsMC;
  std::vector<double> pileupDistribution;
  int mctruthmode;

  llvvGenEvent genEv;
  fwlite::Handle<LHEEventProduct> LHEHandle;
  std::vector<bool> triggerBits;
  llvvGenParticleCollection gen;
  llvvLeptonCollection leptons;
  llvvTauCollection taus;
  llvvTauCollection boostedTaus;
  llvvJetCollection jets_;
  llvvJetExtCollection jets;
  llvvMet metVec;
  std::vector<int> triggerPrescales;

  ValueWithSystematics<double> xsecWeight;
  ValueWithSystematics<double> crossSection_;
  double PUNorm[3];
  edm::LumiReWeighting* LumiWeights;
  utils::cmssw::PuShifter_t PuShifters;
  MuScleFitCorrector* muCor;
  FactorizedJetCorrector* jesCor;
  JetCorrectionUncertainty* totalJESUnc;

  std::vector<TString> chTags;

  ValueWithSystematics<TLorentzVector> getMETvariations();
  ValueWithSystematics<TLorentzVector> MET;

  std::vector<PDFInfo*> pdfVariations;

};

#include "UserCode/Stop4Body/interface/analyser.hpp"

#endif

