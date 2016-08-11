#include "UserCode/Stop4Body/interface/SampleReader.h"

#include <fstream>

#include "TDirectory.h"

SampleInfo::SampleInfo(json jsonInfo): crossSection_(0.0), branchingRatio_(1.0), tag_("")
{
  if(jsonInfo.count("xsec") == 0 || jsonInfo.count("tag") == 0 || jsonInfo.count("path") == 0)
  {
    throw MissingJSONParam("Not all parameters are defined for the sample");
  }

  crossSection_ = jsonInfo["xsec"];
  tag_ = jsonInfo["tag"];

  if(jsonInfo.count("br") > 0)
    branchingRatio_ = jsonInfo["br"];

  filePaths_.push_back(jsonInfo["path"]);
}

std::vector<std::string> SampleInfo::getAllFiles()
{
  return filePaths_;
}

ProcessInfo::ProcessInfo(json jsonInfo):
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

  for(auto& sample : jsonInfo["files"])
  {
    try
    {
      SampleInfo thisSample(sample);
      samples_.push_back(thisSample);
    }
    catch(SampleReaderException& exception)
    {
      if(exception.type() == SampleReaderException::ExceptionType::MissingParamsInJSON)
        std::cout << "Unable to read '" << sample["tag"] << "'" << std::endl;
      else
        throw; //Re-throw the exception
    }
  }
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

  return retVal;
}

doubleUnc ProcessInfo::getYield(std::string cut, std::string weight)
{
  TH1D tmpHist("tmpHist", "tmpHist", 1, 0.0, 20.0);
  tmpHist.Sumw2();

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

  return retVal;
}

SampleReader::SampleReader(std::string fileName): inputFile_(fileName)
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
      ProcessInfo thisProcess(process);
      processes_.push_back(process);
    }
    catch(SampleReaderException& exception)
    {
      if(exception.type() == SampleReaderException::ExceptionType::MissingParamsInJSON)
        std::cout << "Unable to read '" << process["tag"] << "'" << std::endl;
      else
        throw; //Re-throw the exception
    }
  }
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
  for(auto& process : processes_)
  {
    if(!process.isdata() && process.issignal())
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
    retVal->Add(process.getHist(variable, axis, weight, bins, xmin, xmax));
  }

  return retVal;
}

TH1D* SampleReader::getHist(std::string name, std::string variable, std::string axis, std::string weight, int bins, double xmin, double xmax)
{
  TH1D* retVal = new TH1D((name).c_str(), (name+";"+axis).c_str(), bins, xmin, xmax);
  retVal->Sumw2();

  for(auto& process : processes_)
    retVal->Add(process.getHist(variable, axis, weight, bins, xmin, xmax));

  return retVal;
}

doubleUnc SampleReader::getYield(std::string cut, std::string weight)
{
  doubleUnc retVal(0,0);

  for(auto& process : processes_)
    retVal += process.getYield(cut, weight);

  return retVal;
}
