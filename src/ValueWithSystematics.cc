
#include "UserCode/Stop4Body/interface/ValueWithSystematics.h"
#include <algorithm>

// Specialized method for the bool type, where the return value is the logical or of all systematics
template<>
ValueWithSystematicsInternal<bool>::operator bool () const
{
  bool retVal = value;
  for(auto& kv: systematics)
    retVal = retVal || kv.second;
  return retVal;
}

//****************** float ************************************************************************
ValueWithSystematics<float>& ValueWithSystematics<float>::operator=(const float& val)
{
  value = val;

  if(isLocked)
  {
    for(auto& kv: systematics)
    {
      kv.second = val;
    }
  }
  else
    systematics.clear();

  return *this;
}

ValueWithSystematics<float>& ValueWithSystematics<float>::operator=(const ValueWithSystematics<float>& val)
{
  value = val.Value();

  if(isLocked)
  {
    auto tmp = val.Systematics();
    for(auto& kv: systematics)
    {
      if(std::count(tmp.begin(), tmp.end(), kv.first) == 0)
        kv.second = val.Value();
    }
  }
  else
    systematics.clear();

  for(auto& kv: val.Systematics())
    Systematic(kv) = val.Systematic(kv);

  return *this;
}

ValueWithSystematics<float>& ValueWithSystematics<float>::operator=(const ValueWithSystematics<double>& val)
{
  value = val.Value();

  if(isLocked)
  {
    auto tmp = val.Systematics();
    for(auto& kv: systematics)
    {
      if(std::count(tmp.begin(), tmp.end(), kv.first) == 0)
        kv.second = val.Value();
    }
  }
  else
    systematics.clear();

  for(auto& kv: val.Systematics())
    Systematic(kv) = val.Systematic(kv);

  return *this;
}

//****************** double ************************************************************************
ValueWithSystematics<double> ValueWithSystematics<double>::Cos() const
{
  ValueWithSystematics<double> retVal = cos(value);

  for(auto& kv: systematics)
    retVal(kv.first) = cos(kv.second);

  return retVal;
}

ValueWithSystematics<double> ValueWithSystematics<double>::Sqrt() const
{
  ValueWithSystematics<double> retVal = sqrt(value);

  for(auto& kv: systematics)
    retVal(kv.first) = sqrt(kv.second);

  return retVal;
}

ValueWithSystematics<double> ValueWithSystematics<double>::abs() const
{
  ValueWithSystematics<double> retVal = std::abs(value);

  for(auto& kv: systematics)
    retVal(kv.first) = std::abs(kv.second);

  return retVal;
}

