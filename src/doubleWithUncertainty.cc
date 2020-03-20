// <author>Cristóvão B da Cruz e Silva</author>
// <email>c.beirao@cern.ch</email>
// <date>2014-11-24</date>
// <summary>Definition of a simple class that defines a double with an associated uncertainty</summary>

#include "UserCode/Stop4Body-nanoAOD//interface/doubleWithUncertainty.h"

std::ostream& operator << (std::ostream &o, const doubleUnc& val)
{
  return o << val.value_ << " \\pm " << std::sqrt(val.uncertainty2_);
}

doubleUnc& doubleUnc::operator= (const doubleUnc& val)
{
  value_ = val.value_;
  uncertainty2_ = val.uncertainty2_;

  return *this;
}

doubleUnc& doubleUnc::operator+=(const doubleUnc& val)
{
  value_ += val.value_;
  uncertainty2_ += val.uncertainty2_;

  return *this;
}

doubleUnc& doubleUnc::operator-=(const doubleUnc& val)
{
  value_ -= val.value_;
  uncertainty2_ += val.uncertainty2_;

  return *this;
}

doubleUnc& doubleUnc::operator*=(const doubleUnc& val)
{
  uncertainty2_ = val.value_*val.value_*uncertainty2_ + value_*value_*val.uncertainty2_;
  value_ *= val.value_;

  return *this;
}

doubleUnc& doubleUnc::operator/=(const doubleUnc& val)
{
  uncertainty2_ = uncertainty2_/(val.value_*val.value_) + (val.uncertainty2_/(val.value_*val.value_)) * ((value_*value_)/(val.value_*val.value_));
  value_ /= val.value_;

  return *this;
}

doubleUnc  doubleUnc::operator+ (const doubleUnc& val) const
{
  doubleUnc retVal(*this);

  return retVal += val;
}

doubleUnc  doubleUnc::operator- (const doubleUnc& val) const
{
  doubleUnc retVal(*this);

  return retVal -= val;
}

doubleUnc  doubleUnc::operator* (const doubleUnc& val) const
{
  doubleUnc retVal(*this);

  return retVal *= val;
}

doubleUnc  doubleUnc::operator/ (const doubleUnc& val) const
{
  doubleUnc retVal(*this);

  return retVal /= val;
}

doubleUnc& doubleUnc::operator= (const double& val)
{
  value_ = val;
  uncertainty2_ = defaultUnc(val);

  return *this;
}

doubleUnc& doubleUnc::operator+=(const double& val)
{
  doubleUnc tempVal(val, defaultUnc(val));
  return (*this) += tempVal;
}

doubleUnc& doubleUnc::operator-=(const double& val)
{
  doubleUnc tempVal(val, defaultUnc(val));
  return (*this) -= tempVal;
}

doubleUnc& doubleUnc::operator*=(const double& val)
{
  doubleUnc tempVal(val, defaultUnc(val));
  return (*this) *= tempVal;
}

doubleUnc& doubleUnc::operator/=(const double& val)
{
  doubleUnc tempVal(val, defaultUnc(val));
  return (*this) /= tempVal;
}

doubleUnc  doubleUnc::operator+ (const double& val) const
{
  doubleUnc retVal(val, defaultUnc(val));

  return retVal += *this;
}

doubleUnc  doubleUnc::operator- (const double& val) const
{
  doubleUnc tempVal(val, defaultUnc(val));
  doubleUnc retVal = *this;
  return retVal -= tempVal;
}

doubleUnc  doubleUnc::operator* (const double& val) const
{
  doubleUnc retVal(val, defaultUnc(val));

  return retVal *= *this;
}

doubleUnc  doubleUnc::operator/ (const double& val) const
{
  doubleUnc tempVal(val, defaultUnc(val));
  doubleUnc retVal = *this;

  return retVal /= tempVal;
}

doubleUnc doubleUnc::sqrt() const
{
  doubleUnc tempVal;

  tempVal.value_ = std::sqrt(value_);
  tempVal.uncertainty2_ = uncertainty2_/(4*value_);

  return tempVal;
}

double doubleUnc::defaultUnc(double currentValue) const
{
  switch(defaultUncValue)
  {
  case 0:
    return 0;
    break;
  case 1:
    return 1;
    break;
  case 2:
    return std::sqrt(currentValue);
    break;
  }

  return 0;
}
