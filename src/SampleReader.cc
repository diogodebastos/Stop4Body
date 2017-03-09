#include "UserCode/Stop4Body/interface/SampleReader.h"

#include <fstream>
#include <sstream>

#include "TDirectory.h"

using json = nlohmann::json;

SampleInfo::SampleInfo(json jsonInfo, std::string baseDir, std::string suffix):
  baseDir_(baseDir),
  suffix_(suffix),
  crossSection_(0.0),
  branchingRatio_(1.0),
  tag_(""),
  filterEfficiencyFile_(""),
  recordedLumi_(0)
{
  if(jsonInfo.count("xsec") == 0 || jsonInfo.count("tag") == 0 || jsonInfo.count("paths") == 0)
    throw MissingJSONParam("Not all parameters are defined for the sample");

  for(auto &path: jsonInfo["paths"])
  {
    if(path.count("path") == 0 || path.count("split") == 0)
      throw MissingJSONParam("Not all parameters are defined for the path of the sample");
  }

  crossSection_ = jsonInfo["xsec"];
  tag_ = jsonInfo["tag"];

  if(jsonInfo.count("br") > 0)
    branchingRatio_ = jsonInfo["br"];

  filterEfficiencyFile_ = "";
  if(jsonInfo.count("filterEfficiencyFile") > 0)
    filterEfficiencyFile_ = jsonInfo["filterEfficiencyFile"];

  if(jsonInfo.count("recordedLumi") > 0)
    recordedLumi_ = jsonInfo["recordedLumi"];

  if(baseDir_ != "")
  {
    std::string file = baseDir_ + "/" + tag_;
    if(suffix_ != "")
      file += "_" + suffix_;
    file += ".root";
    if(fileExists(file))
      filePaths_.push_back(file);
    else
      missingFiles_.push_back(file);
  }
  else
  {
    for(auto &path: jsonInfo["paths"])
    {
      std::string basePath = path["path"];
      int split = path["split"];

      for(int i = 1; i <= split; ++i)
      {
        std::stringstream converter;
        std::string file;
        converter << basePath << "/Chunk" << i << "/treeProducerStop4Body/tree.root";
        converter >> file;
        if(fileExists(file))
          filePaths_.push_back(file);
        else
          missingFiles_.push_back(file);
      }
    }
  }

  if(filePaths_.size() == 0)
    throw EmptySampleInfo(missingFiles_);
}

bool SampleInfo::fileExists(std::string fileName)
{
  std::ifstream infile(fileName);
  return infile.good();
}

doubleUnc SampleInfo::getYield(std::string cut, std::string weight)
{
  TH1D tmpHist("tmpHist", "tmpHist", 1, 0.0, 20.0);
  tmpHist.Sumw2();

  auto cwd = gDirectory;

  TChain* chain = new TChain("bdttree");
  for(auto& file : filePaths_)
  {
    chain->Add(file.c_str());
  }

  chain->Draw((weight+">>tmpHist").c_str(), (weight+"*("+cut+")").c_str(), "goff");

  doubleUnc retVal(tmpHist.GetBinContent(1),tmpHist.GetBinError(1));

  retVal += doubleUnc(tmpHist.GetBinContent(0),tmpHist.GetBinError(0));
  retVal += doubleUnc(tmpHist.GetBinContent(2),tmpHist.GetBinError(2));

  cwd->cd();
  delete chain;

  return retVal;
}

ProcessInfo::ProcessInfo(json jsonInfo, std::string baseDir, std::string suffix):
  baseDir_(baseDir),
  suffix_(suffix),
  tag_(""),
  label_(""),
  isdata_(false),
  issignal_(false),
  isfastsim_(false),
  spimpose_(false),
  color_(1),
  lcolor_(1),
  lwidth_(3),
  lstyle_(3),
  fill_(0),
  marker_(1),
  mcolor_(1)
{
  if(jsonInfo.count("tag") == 0 || jsonInfo.count("color") == 0 || jsonInfo.count("label") == 0 || jsonInfo.count("files") == 0)
    throw MissingJSONParam("Not all parameters are defined for the sample");

  tag_ = jsonInfo["tag"];
  label_ = jsonInfo["label"];
  color_ = jsonInfo["color"];

  if(jsonInfo.count("isdata") > 0)
    isdata_ = jsonInfo["isdata"];
  if(jsonInfo.count("issignal") > 0)
    issignal_ = jsonInfo["issignal"];
  if(jsonInfo.count("isfastsim") > 0)
    isfastsim_ = jsonInfo["isfastsim"];
  if(jsonInfo.count("spimpose") > 0)
    spimpose_ = jsonInfo["spimpose"];
  if(jsonInfo.count("lcolor") > 0)
    lcolor_ = jsonInfo["lcolor"];
  else
    lcolor_ = color_;
  if(jsonInfo.count("lwidth") > 0)
    lwidth_ = jsonInfo["lwidth"];
  if(jsonInfo.count("lstyle") > 0)
    lstyle_ = jsonInfo["lstyle"];
  if(jsonInfo.count("fill") > 0)
    fill_ = jsonInfo["fill"];
  else
    fill_ = color_;
  if(jsonInfo.count("mcolor") > 0)
    mcolor_ = jsonInfo["mcolor"];
  else
    mcolor_ = color_;
  if(jsonInfo.count("marker") > 0)
    marker_ = jsonInfo["marker"];
  if(jsonInfo.count("selection") > 0)
    selection_ = jsonInfo["selection"];

  for(auto& sample : jsonInfo["files"])
  {
    try
    {
      SampleInfo thisSample(sample, baseDir_, suffix_);
      samples_.push_back(thisSample);
    }
    catch(SampleReaderException& exception)
    {
      switch(exception.type())
      {
      case SampleReaderException::ExceptionType::MissingParamsInJSON:
        std::cout << "Unable to read '" << sample["tag"] << "', there are missing fields in the JSON file." << std::endl;
        break;
      case SampleReaderException::ExceptionType::EmptySampleInfo:
        std::cout << "Unable to read any file for the sample '" << sample["tag"] << "'" << std::endl;
        std::cout << "The expected files were: " << exception.what() << std::endl;
        //std::cout << "The pointer: " << exception.extra_info() << std::endl;
        //std::cout << "The size of missing files: " << static_cast<std::vector<std::string>*>(exception.extra_info())->size() << std::endl;
        for(auto file : *static_cast<std::vector<std::string>*>(exception.extra_info()))
        {
          missingFiles_.push_back(file);
        }
        break;
      default:
        throw; //Re-throw the exception
      }
    }
  }

  if(samples_.size() == 0)
    throw EmptyProcessInfo(missingFiles_);
}

std::vector<std::string> ProcessInfo::getAllFiles()
{
  std::vector<std::string> retVal;
  for(auto& sample : samples_)
  {
    auto vec = sample.getAllFiles();
    for(auto& entry : vec)
      retVal.push_back(entry);
  }
  return retVal;
}

TH1D* ProcessInfo::getHist(std::string variable, std::string axis, std::string weight, int bins, double xmin, double xmax)
{
  TH1D* retVal = new TH1D((variable+"_"+tag_).c_str(), (label_+";"+axis).c_str(), bins, xmin, xmax);
  retVal->Sumw2();

  auto cwd = gDirectory;

  TChain* chain = new TChain("bdttree");
  auto files = getAllFiles();
  for(auto& file : files)
  {
    //std::cout << "Adding file '" << file << "' to the chain" << std::endl;
    chain->Add(file.c_str());
  }

  chain->Draw((variable+">>"+variable+"_"+tag_).c_str(), (weight).c_str(), "goff");

  retVal->SetLineColor(color_);
  retVal->SetFillColor(fill_);
  retVal->SetMarkerColor(mcolor_);
  retVal->SetLineColor(lcolor_);
  retVal->SetLineWidth(lwidth_);
  retVal->SetLineStyle(lstyle_);
  retVal->SetMarkerStyle(marker_);

  cwd->cd();
  delete chain;

  return retVal;
}

doubleUnc ProcessInfo::getYield(std::string cut, std::string weight)
{
  TH1D tmpHist("tmpHist", "tmpHist", 1, 0.0, 20.0);
  tmpHist.Sumw2();

  auto cwd = gDirectory;

  TChain* chain = new TChain("bdttree");
  auto files = getAllFiles();
  for(auto& file : files)
  {
    chain->Add(file.c_str());
  }

  chain->Draw((weight+">>tmpHist").c_str(), (weight+"*("+cut+")").c_str(), "goff");

  doubleUnc retVal(tmpHist.GetBinContent(1),tmpHist.GetBinError(1));

  retVal += doubleUnc(tmpHist.GetBinContent(0),tmpHist.GetBinError(0));
  retVal += doubleUnc(tmpHist.GetBinContent(2),tmpHist.GetBinError(2));

  cwd->cd();
  delete chain;

  return retVal;
}

double ProcessInfo::getLumi() const
{
  double lumi = 0;

  for(auto& sample: samples_)
    lumi += sample.recordedLumi();

  return lumi;
}

SampleReader::SampleReader(std::string fileName, std::string baseDir, std::string suffix):
  inputFile_(fileName),
  baseDir_(baseDir),
  suffix_(suffix)
{
  json jsonFile;
  std::ifstream inputFile(inputFile_);
  inputFile >> jsonFile;

  if(jsonFile.count("lines") == 0)
    throw MissingJSONParam("The JSON file does not contain the 'lines' entry. It is not a valid file.");

  for(auto& process : jsonFile["lines"])
  {
    try
    {
      ProcessInfo thisProcess(process, baseDir_, suffix_);
      processes_.push_back(thisProcess);
    }
    catch(SampleReaderException& exception)
    {
      switch(exception.type())
      {
      case SampleReaderException::ExceptionType::MissingParamsInJSON:
        std::cout << "Unable to read '" << process["tag"] << "', there are missing fields in the JSON file." << std::endl;
        break;
      case SampleReaderException::ExceptionType::EmptyProcessInfo:
        std::cout << "Unable to read any file for the process '" << process["tag"] << "'" << std::endl;
        std::cout << "The expected files were: " << exception.what() << std::endl;
        for(auto file : *static_cast<std::vector<std::string>*>(exception.extra_info()))
        {
          missingFiles_.push_back(file);
        }
        break;
      default:
        throw; //Re-throw the exception
      }
    }
  }

  if(processes_.size() == 0)
    std::cout << "No files were found for any of the processes." << std::endl;
}

std::vector<std::string> SampleReader::getAllFiles()
{
  std::vector<std::string> retVal;
  for(auto& process : processes_)
  {
    auto vec = process.getAllFiles();
    for(auto& entry : vec)
      retVal.push_back(entry);
  }
  return retVal;
}

void SampleReader::printErrors(std::ostream& stream)
{
  stream << "The following errors were encountered when reading the \"" << inputFile_ << "\":" <<std::endl;
}

SampleReader SampleReader::getData()
{
  SampleReader retVal;

  retVal.inputFile_ = inputFile_;
  retVal.baseDir_ = baseDir_;
  retVal.suffix_ = suffix_;
  for(auto& process : processes_)
  {
    if(process.isdata())
      retVal.processes_.push_back(process);
  }

  return retVal;
}

SampleReader SampleReader::getMCBkg()
{
  SampleReader retVal;

  retVal.inputFile_ = inputFile_;
  retVal.baseDir_ = baseDir_;
  retVal.suffix_ = suffix_;
  for(auto& process : processes_)
  {
    if(!process.isdata() && !process.issignal())
      retVal.processes_.push_back(process);
  }

  return retVal;
}

SampleReader SampleReader::getMCSig()
{
  SampleReader retVal;

  retVal.inputFile_ = inputFile_;
  retVal.baseDir_ = baseDir_;
  retVal.suffix_ = suffix_;
  for(auto& process : processes_)
  {
    if(!process.isdata() && process.issignal())
      retVal.processes_.push_back(process);
  }

  return retVal;
}

SampleReader SampleReader::getMC()
{
  SampleReader retVal;

  retVal.inputFile_ = inputFile_;
  retVal.baseDir_ = baseDir_;
  retVal.suffix_ = suffix_;
  for(auto& process : processes_)
  {
    if(!process.isdata())
      retVal.processes_.push_back(process);
  }

  return retVal;
}

TChain* SampleReader::getChain()
{
  TChain* retVal = new TChain("bdttree");

  auto files = getAllFiles();
  for(auto& file : files)
    retVal->Add(file.c_str());

  return retVal;
}

THStack* SampleReader::getStack(std::string variable, std::string axis, std::string weight, int bins, double xmin, double xmax)
{
  THStack* retVal = new THStack(variable.c_str(), (variable + ";" + axis).c_str());

  for(auto& process : processes_)
  {
    TH1D* tmp = process.getHist(variable, axis, weight, bins, xmin, xmax);
    retVal->Add(tmp);
    //delete tmp;
  }

  return retVal;
}

TH1D* SampleReader::getHist(std::string name, std::string variable, std::string axis, std::string weight, int bins, double xmin, double xmax)
{
  TH1D* retVal = new TH1D((name).c_str(), (name+";"+axis).c_str(), bins, xmin, xmax);
  retVal->Sumw2();

  for(auto& process : processes_)
  {
    TH1D* tmp = process.getHist(variable, axis, weight, bins, xmin, xmax);
    if(retVal == nullptr)
      retVal = tmp;
    else
    {
      retVal->Add(tmp);
      delete tmp;
    }
  }

  return retVal;
}

doubleUnc SampleReader::getYield(std::string cut, std::string weight)
{
  doubleUnc retVal(0,0);

  for(auto& process : processes_)
    retVal += process.getYield(cut, weight);

  return retVal;
}

double SampleReader::getLumi() const
{
  double lumi = 0;

  for(auto& process : processes_)
    if(process.isdata())
      lumi += process.getLumi();

  return lumi;
}
