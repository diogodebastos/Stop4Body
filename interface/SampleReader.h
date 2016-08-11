#ifndef MY_SAMPLE_READER_H
#define MY_SAMPLE_READER_H

#include <iostream>
#include <string>
#include <vector>
#include <exception>

#include "TChain.h"
#include "THStack.h"
#include "TH1D.h"

#include "UserCode/Stop4Body/interface/json.hpp"
#include "UserCode/Stop4Body/interface/doubleWithUncertainty.h"

using json = nlohmann::json;

class SampleReaderException: public std::runtime_error
{
public:
  enum class ExceptionType {Base, FileNotFound, MissingParamsInJSON};

  explicit SampleReaderException(const std::string message):
    std::runtime_error(""),
    msg_(message),
    type_(ExceptionType::Base)
  {
  }

  explicit SampleReaderException(const std::string message, ExceptionType type):
    std::runtime_error(""),
    msg_(message),
    type_(type)
  {
  }

  virtual ~SampleReaderException() throw() {}

  virtual const char* what() const throw()
  {
    return msg_.c_str();
  }

  ExceptionType type() {return type_;}

private:
protected:
  std::string msg_;
  ExceptionType type_;
};

class MissingJSONParam: public SampleReaderException
{
public:
  explicit MissingJSONParam(const std::string message):
    SampleReaderException(message, ExceptionType::MissingParamsInJSON)
  {
  }

private:
protected:
};

class SampleInfo
{
public:
  SampleInfo() = delete;
  SampleInfo(json);
  //~SampleInfo();

  std::vector<std::string> getAllFiles();

private:
protected:
  double crossSection_;
  double branchingRatio_;
  std::string tag_;

  std::vector<std::string> filePaths_;
};

class ProcessInfo
{
public:
  ProcessInfo() = delete;
  ProcessInfo(json);
  //~ProcessInfo();

  std::vector<std::string> getAllFiles();
  TH1D* getHist(std::string, std::string, std::string, int, double, double);
  doubleUnc getYield(std::string, std::string);

  std::string tag() {return tag_;};
  std::string label() {return label_;};
  bool isdata() {return isdata_;};
  bool issignal() {return issignal_;};
  bool isfastsim() {return isfastsim_;};
  bool spimpose() {return spimpose_;};
  int color() {return color_;};
  int lcolor() {return lcolor_;};
  int lwidth() {return lwidth_;};
  int lstyle() {return lstyle_;};
  int fill() {return fill_;};
  int mcolor() {return mcolor_;};
  int marker() {return marker_;};

private:
protected:
  std::string tag_;
  std::string label_;
  bool isdata_;
  bool issignal_;
  bool isfastsim_;
  bool spimpose_;
  int color_;
  int lcolor_;
  int lwidth_;
  int lstyle_;
  int fill_;
  int marker_;
  int mcolor_;

  std::vector<SampleInfo> samples_;
};

class SampleReader
{
public:
  SampleReader(std::string);
  //~SampleReader();

  void printErrors(std::ostream&);
  std::vector<std::string> getAllFiles();
  TChain* getChain();
  THStack* getStack(std::string, std::string, std::string, int, double, double);
  TH1D* getHist(std::string, std::string, std::string, std::string, int, double, double);
  //void draw(std::string, std::string, std::string);
  doubleUnc getYield(std::string, std::string);

  SampleReader getData();
  SampleReader getMCBkg();
  SampleReader getMCSig();

private:
  SampleReader(): inputFile_("") {};

protected:
  std::string inputFile_;
  std::vector<ProcessInfo> processes_;
};

#endif
