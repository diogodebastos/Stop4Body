#ifndef MY_VARIABLE_READER_H
#define MY_VARIABLE_READER_H

#include <string>
#include <vector>

#include "UserCode/Stop4Body-nanoAOD//interface/json.hpp"

class VariableInfo
{
  public:
    VariableInfo(std::string exp, std::string name, std::string label, double min, double max, int bins, std::string legPos):
    expression_(exp),
    name_(name),
    label_(label),
    min_(min),
    max_(max),
    bins_(bins),
    legPos_(legPos),
    logx_(false),
    logy_(true)
    {}
    VariableInfo(nlohmann::json jsonInfo);

    std::string expression() const {return expression_;}
    std::string expression(std::string newExp) {expression_ = newExp; return expression_;}
    std::string name() const {return name_;}
    std::string label() const {return label_;}
    double min() const {return min_;}
    double max() const {return max_;}
    int bins() const {return bins_;}
    std::string legPos() const {return legPos_;}
    bool legLeft() const {return legPos_ == "left";}
    bool legRight() const {return legPos_ == "right";}
    bool legTop() const {return legPos_ == "top";}
    bool legBottom() const {return legPos_ == "bottom";}
    bool logx() const {return logx_;}
    bool logy() const {return logy_;}
    std::vector<double> varBins() const {return varBins_;}
    bool hasVarBins() const {return varBins_.size() > 0;}

  private:
  protected:
    std::string expression_;
    std::string name_;
    std::string label_;
    double min_;
    double max_;
    int bins_;
    std::string legPos_;
    bool logx_, logy_;
    std::vector<double> varBins_;

};

class VariableJsonLoader
{
  public:
    VariableJsonLoader(std::string);

    // Iteration
    typedef typename std::vector<VariableInfo>::iterator iterator;
    typedef typename std::vector<VariableInfo>::const_iterator const_iterator;

    size_t size() {return variables_.size();}
    VariableInfo& Get(size_t n) {return variables_[n];}

    iterator begin() {return variables_.begin();}
    const_iterator begin() const {return variables_.begin();}
    const_iterator cbegin() const {return variables_.cbegin();}
    iterator end() {return variables_.end();}
    const_iterator end() const {return variables_.end();}
    const_iterator cend() const {return variables_.cend();}

  private:
  protected:
    std::string inputFile_;
    std::vector<VariableInfo> variables_;
};

class TwoDVariableInfo
{
  public:
    TwoDVariableInfo(std::string name, nlohmann::json XjsonInfo, nlohmann::json YjsonInfo):
    name_(name),
    X_(XjsonInfo),
    Y_(YjsonInfo)
    {}

    std::string name() const {return name_;}
    VariableInfo X() const {return X_;}
    VariableInfo Y() const {return Y_;}

  private:
  protected:
    std::string name_;
    VariableInfo X_;
    VariableInfo Y_;

};

class TwoDVariableJsonLoader
{
  public:
    TwoDVariableJsonLoader(std::string);

    // Iteration
    typedef typename std::vector<TwoDVariableInfo>::iterator iterator;
    typedef typename std::vector<TwoDVariableInfo>::const_iterator const_iterator;

    iterator begin() {return variables_.begin();}
    const_iterator begin() const {return variables_.begin();}
    const_iterator cbegin() const {return variables_.cbegin();}
    iterator end() {return variables_.end();}
    const_iterator end() const {return variables_.end();}
    const_iterator cend() const {return variables_.cend();}

  private:
  protected:
    std::string inputFile_;
    std::vector<TwoDVariableInfo> variables_;

};

#endif
