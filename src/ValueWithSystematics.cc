// <author>Cristóvão B da Cruz e Silva</author>
// <email>c.beirao@cern.ch</email>
// <date>2017-06-19</date>
// <summary>Implementation file for the ValueWithSystematics class</summary>
//
// <description>
//  Implementation file with the definitions of the ValueWithSystematics class and derivates.
//  This class is made to function exactly like the builtin types (and other types when the operators are defined),
// except the types now have associated named systematic uncertainties, which are treated idependently. The systematic
// uncertainties are correctly handled when performing computations and many other tasks, only requiring to be
// handled by hand in specific circumstances.
//  Several methods have been implemented to allow to handle the uncertainties by hand when needed. Also, some other
// methods have been implemented to allow functionality that is normally available to specific types or builtin functions.
// </description>

#include "UserCode/Stop4Body/interface/ValueWithSystematics.h"

#include <cmath>
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
  ValueWithSystematicsInternal<float>::operator=(val);
  return *this;
}

ValueWithSystematics<float>& ValueWithSystematics<float>::operator=(float&& val) noexcept
{
  ValueWithSystematicsInternal<float>::operator=(std::move(val));
  return *this;
}

ValueWithSystematics<float>& ValueWithSystematics<float>::operator=(const ValueWithSystematics<float>& val)
{
  ValueWithSystematicsInternal<float>::operator=(val);
  return *this;
}

ValueWithSystematics<float>& ValueWithSystematics<float>::operator=(ValueWithSystematics<float>&& val) noexcept
{
  ValueWithSystematicsInternal<float>::operator=(std::move(val));
  return *this;
}

ValueWithSystematics<float>& ValueWithSystematics<float>::operator=(const ValueWithSystematicsInternal<float>& val)
{
  ValueWithSystematicsInternal<float>::operator=(val);
  return *this;
}

ValueWithSystematics<float>& ValueWithSystematics<float>::operator=(ValueWithSystematicsInternal<float>&& val) noexcept
{
  ValueWithSystematicsInternal<float>::operator=(std::move(val));
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
        kv.second = value;
    }
  }
  else
    systematics.clear();

  for(auto& kv: val.Systematics())
    Systematic(kv) = val.Systematic(kv);

  return *this;
}

ValueWithSystematics<float>& ValueWithSystematics<float>::operator=(ValueWithSystematics<double>&& val) noexcept
{
  value = std::move(val.Value());

  if(isLocked)
  {
    auto tmp = val.Systematics();
    for(auto& kv: systematics)
    {
      if(std::count(tmp.begin(), tmp.end(), kv.first) == 0)
        kv.second = value;
    }
  }
  else
    systematics.clear();

  for(auto& kv: val.Systematics())
    Systematic(kv) = std::move(val.Systematic(kv));

  return *this;
}

ValueWithSystematics<float>& ValueWithSystematics<float>::operator=(const ValueWithSystematicsInternal<double>& val)
{
  value = val.Value();

  if(isLocked)
  {
    auto tmp = val.Systematics();
    for(auto& kv: systematics)
    {
      if(std::count(tmp.begin(), tmp.end(), kv.first) == 0)
        kv.second = value;
    }
  }
  else
    systematics.clear();

  for(auto& kv: val.Systematics())
    Systematic(kv) = val.Systematic(kv);

  return *this;
}

ValueWithSystematics<float>& ValueWithSystematics<float>::operator=(ValueWithSystematicsInternal<double>&& val) noexcept
{
  value = std::move(val.Value());

  if(isLocked)
  {
    auto tmp = val.Systematics();
    for(auto& kv: systematics)
    {
      if(std::count(tmp.begin(), tmp.end(), kv.first) == 0)
        kv.second = value;
    }
  }
  else
    systematics.clear();

  for(auto& kv: val.Systematics())
    Systematic(kv) = std::move(val.Systematic(kv));

  return *this;
}

ValueWithSystematics<float>& ValueWithSystematics<float>::operator=(const ValueWithSystematics<int>& val)
{
  value = val.Value();

  if(isLocked)
  {
    auto tmp = val.Systematics();
    for(auto& kv: systematics)
    {
      if(std::count(tmp.begin(), tmp.end(), kv.first) == 0)
        kv.second = value;
    }
  }
  else
    systematics.clear();

  for(auto& kv: val.Systematics())
    Systematic(kv) = val.Systematic(kv);

  return *this;
}

ValueWithSystematics<float>& ValueWithSystematics<float>::operator=(ValueWithSystematics<int>&& val) noexcept
{
  value = std::move(val.Value());

  if(isLocked)
  {
    auto tmp = val.Systematics();
    for(auto& kv: systematics)
    {
      if(std::count(tmp.begin(), tmp.end(), kv.first) == 0)
        kv.second = value;
    }
  }
  else
    systematics.clear();

  for(auto& kv: val.Systematics())
    Systematic(kv) = std::move(val.Systematic(kv));

  return *this;
}

ValueWithSystematics<float>& ValueWithSystematics<float>::operator=(const ValueWithSystematicsInternal<int>& val)
{
  value = val.Value();

  if(isLocked)
  {
    auto tmp = val.Systematics();
    for(auto& kv: systematics)
    {
      if(std::count(tmp.begin(), tmp.end(), kv.first) == 0)
        kv.second = value;
    }
  }
  else
    systematics.clear();

  for(auto& kv: val.Systematics())
    Systematic(kv) = val.Systematic(kv);

  return *this;
}

ValueWithSystematics<float>& ValueWithSystematics<float>::operator=(ValueWithSystematicsInternal<int>&& val) noexcept
{
  value = std::move(val.Value());

  if(isLocked)
  {
    auto tmp = val.Systematics();
    for(auto& kv: systematics)
    {
      if(std::count(tmp.begin(), tmp.end(), kv.first) == 0)
        kv.second = value;
    }
  }
  else
    systematics.clear();

  for(auto& kv: val.Systematics())
    Systematic(kv) = std::move(val.Systematic(kv));

  return *this;
}

//****************** double ************************************************************************
ValueWithSystematics<double> ValueWithSystematics<double>::Cos() const
{
  ValueWithSystematics<double> retVal = std::cos(value);

  for(auto& kv: systematics)
    retVal(kv.first) = std::cos(kv.second);

  return retVal;
}

ValueWithSystematics<double> ValueWithSystematics<double>::Sin() const
{
  ValueWithSystematics<double> retVal = std::sin(value);

  for(auto& kv: systematics)
    retVal(kv.first) = std::sin(kv.second);

  return retVal;
}

ValueWithSystematics<double> ValueWithSystematics<double>::Sqrt() const
{
  ValueWithSystematics<double> retVal = std::sqrt(value);

  for(auto& kv: systematics)
    retVal(kv.first) = std::sqrt(kv.second);

  return retVal;
}

ValueWithSystematics<double> ValueWithSystematics<double>::abs() const
{
  ValueWithSystematics<double> retVal = std::abs(value);

  for(auto& kv: systematics)
    retVal(kv.first) = std::abs(kv.second);

  return retVal;
}

ValueWithSystematics<double> ValueWithSystematics<double>::pow(double exp) const
{
  ValueWithSystematics<double> retVal = std::pow(value, exp);

  for(auto& kv: systematics)
    retVal(kv.first) = std::pow(kv.second, exp);

  return retVal;
}

ValueWithSystematics<double>& ValueWithSystematics<double>::operator=(const double& val)
{
  ValueWithSystematicsInternal<double>::operator=(val);
  return *this;
}

ValueWithSystematics<double>& ValueWithSystematics<double>::operator=(double&& val) noexcept
{
  ValueWithSystematicsInternal<double>::operator=(std::move(val));
  return *this;
}

ValueWithSystematics<double>& ValueWithSystematics<double>::operator=(const ValueWithSystematics<double>& val)
{
  ValueWithSystematicsInternal<double>::operator=(val);
  return *this;
}

ValueWithSystematics<double>& ValueWithSystematics<double>::operator=(ValueWithSystematics<double>&& val) noexcept
{
  ValueWithSystematicsInternal<double>::operator=(std::move(val));
  return *this;
}

ValueWithSystematics<double>& ValueWithSystematics<double>::operator=(const ValueWithSystematicsInternal<double>& val)
{
  ValueWithSystematicsInternal<double>::operator=(val);
  return *this;
}

ValueWithSystematics<double>& ValueWithSystematics<double>::operator=(ValueWithSystematicsInternal<double>&& val) noexcept
{
  ValueWithSystematicsInternal<double>::operator=(std::move(val));
  return *this;
}

