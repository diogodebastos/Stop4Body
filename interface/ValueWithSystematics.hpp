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

#ifndef VALUE_WITH_SYSTEMATICS_HPP
#define VALUE_WITH_SYSTEMATICS_HPP

//#define DO_TRACE

#include <iostream>

template<class T>
ValueWithSystematics<T> operator* (const double& builtin, const ValueWithSystematics<T>& myClass)
{
  return myClass*builtin;
}

template<class T>
ValueWithSystematics<T> operator/ (const double& builtin, const ValueWithSystematics<T>& myClass)
{
  return ValueWithSystematics<T>(builtin)/myClass;
}

template<class T>
ValueWithSystematics<T> operator+ (const double& builtin, const ValueWithSystematics<T>& myClass)
{
  return myClass+builtin;
}

template<class T>
ValueWithSystematics<T> operator- (const int& builtin, const ValueWithSystematics<T>& myClass)
{
  return (-myClass) + builtin;
}

template<class T>
ValueWithSystematicsInternal<T>::ValueWithSystematicsInternal(T val): isLocked(false), defaultValue(val), value(val)
{
}

template<class T>
ValueWithSystematicsInternal<T>::ValueWithSystematicsInternal(const ValueWithSystematicsInternal<T>& val):
            isLocked(false),
            defaultValue(val.defaultValue),
            value(val.value),
            systematics(val.systematics),
            metadata(val.metadata)
{
  #ifdef DO_TRACE
  std::cout << "Copy constructor" << std::endl;
  #endif
}

template<class T>
ValueWithSystematicsInternal<T>::ValueWithSystematicsInternal(ValueWithSystematicsInternal<T>&& val) noexcept:
            isLocked(std::move(val.isLocked)),
            defaultValue(std::move(val.defaultValue)),
            value(std::move(val.value)),
            systematics(std::move(val.systematics)),
            metadata(std::move(val.metadata))
{
  #ifdef DO_TRACE
  std::cout << "Move constructor" << std::endl;
  #endif
}

template<class T>
void ValueWithSystematicsInternal<T>::Reset()
{
  value = defaultValue;
  if(isLocked)
  {
    for(auto& kv : systematics)
      kv.second = defaultValue;
  }
  else
  {
    systematics.clear();
  }
}

template<class T>
T& ValueWithSystematicsInternal<T>::Systematic(const std::string& name)
{
  if(systematics.count(name) == 0)
  {
    if(isLocked)
      throw ValueException("Unable to add systematic \""+name+"\" after locking.");
    systematics[name] = value;
  }

  return systematics[name];
}

template<class T>
const T& ValueWithSystematicsInternal<T>::Systematic(const std::string& name) const
{
  if(systematics.count(name) == 0)
  {
    if(isLocked)
      throw ValueException("Unable to add systematic \""+name+"\" after locking.");
    systematics[name] = value;
  }

  return systematics[name];
}

template<class T>
T& ValueWithSystematicsInternal<T>::GetSystematicOrValue(const std::string& name)
{
  if(systematics.count(name) != 0)
    return systematics[name];
  return value;
}

template<class T>
const T& ValueWithSystematicsInternal<T>::GetSystematicOrValue(const std::string& name) const
{
  if(systematics.count(name) != 0)
    return systematics[name];
  return value;
}

#ifdef _USE_CERN_ROOT
#include "TDirectory.h"

template<class T>
void ValueWithSystematicsInternal<T>::SaveTTree(std::string& name, TFile* file, std::string& title) const
{
  TDirectory* cwd = gDirectory;
  file->cd();

  TTree* myTree = new TTree(name.c_str(), ((title == "")?(name.c_str()):(title.c_str())));

  std::string varName = "central";
  T varValue = value;

  myTree->Branch("variation", &varName);
  myTree->Branch("value", &varValue);

  myTree->Fill(); // Save the central value
  // Then loop over the variations and save their values
  for(auto& syst : systematics)
  {
    varName = syst;
    varValue = systematics[syst];
    myTree->Fill();
  }

  myTree->Write("",TObject::kOverwrite);
  cwd->cd()
  return;
}

template<class T>
void ValueWithSystematicsInternal<T>::LoadTTree(std::string& name, TFile* file)
{
  TDirectory* cwd = gDirectory;
  file->cd();

  TTree* myTree = static_cast<TTree*>(file->Get(name.c_str()));

  std::string varName = "";
  T varValue;

  myTree->SetBranchAddress("variation", &varName);
  myTree->SetBranchAddress("value", &varValue);

  size_t nentries = static_cast<size_t>(myTree->GetEntries());

  for(size_t i = 0; i < nentries; i++)
  {
    myTree->GetEntry(i);

    if(i == 0)
    {
      if(varName != "central")
        throw ValueException("The chosen ttree (" + name + ") does not have the central value as the first entry.\nIt is likely the ttree was not a ValueWithSystematics saved as a ttree.");
      value = varValue;
    }
    else
    {
      systematics[varName] = varValue;
    }
  }

  cwd->cd()
  return;
}
#endif

template<class T>
std::vector<std::string> ValueWithSystematicsInternal<T>::Systematics() const
{
  std::vector<std::string> retVal;

  for(auto& kv: systematics)
    retVal.push_back(kv.first);

  return retVal;
}

template<class T>
bool ValueWithSystematicsInternal<T>::AddMetadata(const std::string& key, const std::string& value)
{
  std::cout << "Deprecated AddMetadata used" << std::endl;

  if(metadata.count(key) != 0)
    std::cout << "Metadata already exists with that key, it will be overwritten. Old value: \"" << metadata[key] << "\"" << std::endl;
  else
    if(isLocked)
      throw ValueException("Unable to add metadata \""+key+":"+value+"\" after locking.");

  metadata[key] = value;
  return true;
}

template<class T>
std::string ValueWithSystematicsInternal<T>::GetMetadata(std::string& key)
{
  if(metadata.count(key) == 0)
    return "";
  return metadata.at(key);
}

template<class T>
std::string ValueWithSystematicsInternal<T>::GetMetadata(const std::string& key) const
{
  if(metadata.count(key) == 0)
    return "";
  return metadata.at(key);
}

template<class T>
std::string& ValueWithSystematicsInternal<T>::Metadata(const std::string& key)
{
  if(metadata.count(key) == 0)
  {
    if(isLocked)
      throw ValueException("Unable to add metadata \""+key+"\" after locking.");
    metadata[key] = "";
  }
  return metadata[key];
}

template<class T>
const std::string& ValueWithSystematicsInternal<T>::Metadata(const std::string& key) const
{
  if(metadata.count(key) == 0)
  {
    if(isLocked)
      throw ValueException("Unable to add metadata \""+key+"\" after locking.");
    metadata[key] = "";
  }
  return metadata[key];
}

template<class T>
T& ValueWithSystematicsInternal<T>::operator()(const std::string& name)
{
  return Systematic(name);
}

template<class T>
ValueWithSystematicsInternal<T>::operator T () const
{
  return value;
}

// Specialized method for the bool type, where the return value is the logical or of all systematics
template<>
ValueWithSystematicsInternal<bool>::operator bool () const;
// Defined in cpp file

template<class T>
ValueWithSystematicsInternal<T>& ValueWithSystematicsInternal<T>::operator=(const T& val)
{
  value = val;

  for(auto& kv: systematics)
  {
    kv.second = value;
  }

  return *this;
}

template<class T>
ValueWithSystematicsInternal<T>& ValueWithSystematicsInternal<T>::operator=(T&& val) noexcept
{
  value = std::move(val);

  for(auto& kv: systematics)
  {
    kv.second = value;
  }

  return *this;
}

template<class T>
ValueWithSystematicsInternal<T>& ValueWithSystematicsInternal<T>::operator=(const ValueWithSystematicsInternal<T>& val)
{
  if(this == &val) // Check for self assignment
    return *this;

  value = val.value;

  if(isLocked)
  {
    for(auto& kv: systematics)
      if(val.systematics.count(kv.first) == 0)
        kv.second = value;
  }
  else
    systematics.clear();

  for(auto& kv: val.systematics)
    Systematic(kv.first) = kv.second;

  return *this;
}

template<class T>
ValueWithSystematicsInternal<T>& ValueWithSystematicsInternal<T>::operator=(ValueWithSystematicsInternal<T>&& val) noexcept
{
  if(this == &val) // Check for self assignment (This check is probably not needed, but better safe than sorry)
    return *this;

  value = std::move(val.value);

  if(isLocked)
  {
    for(auto& kv: systematics)
      if(val.systematics.count(kv.first) == 0)
        kv.second = value;
  }
  else
    systematics.clear();

  for(auto& kv: val.systematics)
    Systematic(std::move(kv.first)) = std::move(kv.second);

  return *this;
}

template<class T>
ValueWithSystematicsInternal<T>& ValueWithSystematicsInternal<T>::operator+=(const T& val)
{
  static_assert(!(std::is_same<T, bool>::value), "You can not use addition operators with booleans");

  for(auto& kv: systematics)
  {
    kv.second += val;
  }

  value += val;

  return *this;
}

template<class T>
ValueWithSystematicsInternal<T>& ValueWithSystematicsInternal<T>::operator+=(T&& val) noexcept
{
  static_assert(!(std::is_same<T, bool>::value), "You can not use addition operators with booleans");

  for(auto& kv: systematics)
  {
    kv.second += val;
  }

  value += std::move(val);

  return *this;
}

template<class T>
ValueWithSystematicsInternal<T>& ValueWithSystematicsInternal<T>::operator+=(const ValueWithSystematicsInternal<T>& val)
{
  static_assert(!(std::is_same<T, bool>::value), "You can not use addition operators with booleans");

  for(auto& kv: systematics)
    if(val.systematics.count(kv.first) == 0)
      kv.second += val.value;

  for(auto& kv: val.systematics)
    Systematic(kv.first) += kv.second;

  value += val.value;

  return *this;
}

template<class T>
ValueWithSystematicsInternal<T>& ValueWithSystematicsInternal<T>::operator+=(ValueWithSystematicsInternal<T>&& val) noexcept
{
  static_assert(!(std::is_same<T, bool>::value), "You can not use addition operators with booleans");

  for(auto& kv: systematics)
    if(val.systematics.count(kv.first) == 0)
      kv.second += val.value;

  for(auto& kv: val.systematics)
    Systematic(std::move(kv.first)) += std::move(kv.second);

  value += std::move(val.value);

  return *this;
}

template<class T>
ValueWithSystematicsInternal<T>& ValueWithSystematicsInternal<T>::operator-=(const T& val)
{
  static_assert(!(std::is_same<T, bool>::value), "You can not use subtraction operators with booleans");

  for(auto& kv: systematics)
  {
    kv.second -= val;
  }

  value -= val;

  return *this;
}

template<class T>
ValueWithSystematicsInternal<T>& ValueWithSystematicsInternal<T>::operator-=(T&& val) noexcept
{
  static_assert(!(std::is_same<T, bool>::value), "You can not use subtraction operators with booleans");

  for(auto& kv: systematics)
  {
    kv.second -= val;
  }

  value -= std::move(val);

  return *this;
}

template<class T>
ValueWithSystematicsInternal<T>& ValueWithSystematicsInternal<T>::operator-=(const ValueWithSystematicsInternal<T>& val)
{
  static_assert(!(std::is_same<T, bool>::value), "You can not use subtraction operators with booleans");

  for(auto& kv: systematics)
    if(val.systematics.count(kv.first) == 0)
      kv.second -= val.value;

  for(auto& kv: val.systematics)
    Systematic(kv.first) -= kv.second;

  value -= val.value;

  return *this;
}

template<class T>
ValueWithSystematicsInternal<T>& ValueWithSystematicsInternal<T>::operator-=(ValueWithSystematicsInternal<T>&& val) noexcept
{
  static_assert(!(std::is_same<T, bool>::value), "You can not use subtraction operators with booleans");

  for(auto& kv: systematics)
    if(val.systematics.count(kv.first) == 0)
      kv.second -= val.value;

  for(auto& kv: val.systematics)
    Systematic(std::move(kv.first)) -= std::move(kv.second);

  value -= std::move(val.value);

  return *this;
}

template<class T>
ValueWithSystematicsInternal<T>& ValueWithSystematicsInternal<T>::operator*=(const T& val)
{
  static_assert(!(std::is_same<T, bool>::value), "You can not use multiplication operators with booleans");

  for(auto& kv: systematics)
  {
    kv.second *= val;
  }

  value *= val;

  return *this;
}

template<class T>
ValueWithSystematicsInternal<T>& ValueWithSystematicsInternal<T>::operator*=(T&& val) noexcept
{
  static_assert(!(std::is_same<T, bool>::value), "You can not use multiplication operators with booleans");

  for(auto& kv: systematics)
  {
    kv.second *= val;
  }

  value *= std::move(val);

  return *this;
}

template<class T>
ValueWithSystematicsInternal<T>& ValueWithSystematicsInternal<T>::operator*=(const ValueWithSystematicsInternal<T>& val)
{
  static_assert(!(std::is_same<T, bool>::value), "You can not use multiplication operators with booleans");

  for(auto& kv: systematics)
    if(val.systematics.count(kv.first) == 0)
      kv.second *= val.value;

  for(auto& kv: val.systematics)
    Systematic(kv.first) *= kv.second;

  value *= val.value;

  return *this;
}

template<class T>
ValueWithSystematicsInternal<T>& ValueWithSystematicsInternal<T>::operator*=(ValueWithSystematicsInternal<T>&& val) noexcept
{
  static_assert(!(std::is_same<T, bool>::value), "You can not use multiplication operators with booleans");

  for(auto& kv: systematics)
    if(val.systematics.count(kv.first) == 0)
      kv.second *= val.value;

  for(auto& kv: val.systematics)
    Systematic(std::move(kv.first)) *= std::move(kv.second);

  value *= std::move(val.value);

  return *this;
}

template<class T>
ValueWithSystematicsInternal<T>& ValueWithSystematicsInternal<T>::operator/=(const T& val)
{
  static_assert(!(std::is_same<T, bool>::value), "You can not use division operators with booleans");

  for(auto& kv: systematics)
  {
    kv.second /= val;
  }

  value /= val;

  return *this;
}

template<class T>
ValueWithSystematicsInternal<T>& ValueWithSystematicsInternal<T>::operator/=(T&& val) noexcept
{
  static_assert(!(std::is_same<T, bool>::value), "You can not use division operators with booleans");

  for(auto& kv: systematics)
  {
    kv.second /= val;
  }

  value /= std::move(val);

  return *this;
}

template<class T>
ValueWithSystematicsInternal<T>& ValueWithSystematicsInternal<T>::operator/=(const ValueWithSystematicsInternal<T>& val)
{
  static_assert(!(std::is_same<T, bool>::value), "You can not use division operators with booleans");

  for(auto& kv: systematics)
    if(val.systematics.count(kv.first) == 0)
      kv.second /= val.value;

  for(auto& kv: val.systematics)
    Systematic(kv.first) /= kv.second;

  value /= val.value;

  return *this;
}

template<class T>
ValueWithSystematicsInternal<T>& ValueWithSystematicsInternal<T>::operator/=(ValueWithSystematicsInternal<T>&& val) noexcept
{
  static_assert(!(std::is_same<T, bool>::value), "You can not use division operators with booleans");

  for(auto& kv: systematics)
    if(val.systematics.count(kv.first) == 0)
      kv.second /= val.value;

  for(auto& kv: val.systematics)
    Systematic(std::move(kv.first)) /= std::move(kv.second);

  value /= std::move(val.value);

  return *this;
}

template<class T>
const ValueWithSystematicsInternal<T> ValueWithSystematicsInternal<T>::operator+(const T& val) const
{
  return ValueWithSystematicsInternal<T>(*this) += val;
}

template<class T>
const ValueWithSystematicsInternal<T> ValueWithSystematicsInternal<T>::operator+(const ValueWithSystematicsInternal<T>& val) const
{
  return ValueWithSystematicsInternal<T>(*this) += val;
}

template<class T>
const ValueWithSystematicsInternal<T> ValueWithSystematicsInternal<T>::operator-(const T& val) const
{
  return ValueWithSystematicsInternal<T>(*this) -= val;
}

template<class T>
const ValueWithSystematicsInternal<T> ValueWithSystematicsInternal<T>::operator-(const ValueWithSystematicsInternal<T>& val) const
{
  return ValueWithSystematicsInternal<T>(*this) -= val;
}

template<class T>
const ValueWithSystematicsInternal<T> ValueWithSystematicsInternal<T>::operator*(const T& val) const
{
  return ValueWithSystematicsInternal<T>(*this) *= val;
}

template<class T>
const ValueWithSystematicsInternal<T> ValueWithSystematicsInternal<T>::operator*(const ValueWithSystematicsInternal<T>& val) const
{
  return ValueWithSystematicsInternal<T>(*this) *= val;
}

template<class T>
const ValueWithSystematicsInternal<T> ValueWithSystematicsInternal<T>::operator/(const T& val) const
{
  return ValueWithSystematicsInternal<T>(*this) /= val;
}

template<class T>
const ValueWithSystematicsInternal<T> ValueWithSystematicsInternal<T>::operator/(const ValueWithSystematicsInternal<T>& val) const
{
  return ValueWithSystematicsInternal<T>(*this) /= val;
}

template<class T>
const ValueWithSystematicsInternal<T> ValueWithSystematicsInternal<T>::operator-() const
{
  ValueWithSystematicsInternal<T> retVal(*this);

  retVal.value = -retVal.value;
  for(auto& kv: retVal.systematics)
    kv.second = -kv.second;

  return retVal;
}

template<class T>
ValueWithSystematicsInternal<T>& ValueWithSystematicsInternal<T>::operator++()
{
  static_assert(!(std::is_same<T, bool>::value), "You can not use increment operators with booleans");

  ++value;
  for(auto& kv: systematics)
    ++kv.second;

  return *this;
}

template<class T>
ValueWithSystematicsInternal<T> ValueWithSystematicsInternal<T>::operator++(int)
{
  static_assert(!(std::is_same<T, bool>::value), "You can not use increment operators with booleans");

  ValueWithSystematicsInternal<T> retVal(*this);

  ++value;
  for(auto& kv: systematics)
    ++kv.second;

  return retVal;
}

template<class T>
ValueWithSystematicsInternal<T>& ValueWithSystematicsInternal<T>::operator--()
{
  static_assert(!(std::is_same<T, bool>::value), "You can not use increment operators with booleans");

  --value;
  for(auto& kv: systematics)
    --kv.second;

  return *this;
}

template<class T>
ValueWithSystematicsInternal<T> ValueWithSystematicsInternal<T>::operator--(int)
{
  static_assert(!(std::is_same<T, bool>::value), "You can not use increment operators with booleans");

  ValueWithSystematicsInternal<T> retVal(*this);

  --value;
  for(auto& kv: systematics)
    --kv.second;

  return retVal;
}

template<class T>
const ValueWithSystematicsInternal<bool> ValueWithSystematicsInternal<T>::operator==(const T& val) const
{
  ValueWithSystematicsInternal<bool> retVal(value == val);

  for(auto& kv: systematics)
    retVal(kv.first) = (kv.second == val);

  return retVal;
}

template<class T>
const ValueWithSystematicsInternal<bool> ValueWithSystematicsInternal<T>::operator==(const ValueWithSystematicsInternal<T>& val) const
{
  ValueWithSystematicsInternal<bool> retVal(value == val.value);

  for(auto& kv: systematics)
  {
    if(val.systematics.count(kv.first) == 0)
      retVal(kv.first) = (kv.second == val.value);
  }

  for(auto& kv: val.systematics)
  {
    if(systematics.count(kv.first) == 0)
      retVal(kv.first) = (value == kv.second);
    else
      retVal(kv.first) = (systematics.at(kv.first) == kv.second);
  }

  return retVal;
}

template<class T>
const ValueWithSystematicsInternal<bool> ValueWithSystematicsInternal<T>::operator!=(const T& val) const
{
  ValueWithSystematicsInternal<bool> retVal(value != val);

  for(auto& kv: systematics)
    retVal(kv.first) = (kv.second != val);

  return retVal;
}

template<class T>
const ValueWithSystematicsInternal<bool> ValueWithSystematicsInternal<T>::operator!=(const ValueWithSystematicsInternal<T>& val) const
{
  ValueWithSystematicsInternal<bool> retVal(value != val.value);

  for(auto& kv: systematics)
  {
    if(val.systematics.count(kv.first) == 0)
      retVal(kv.first) = (kv.second != val.value);
  }

  for(auto& kv: val.systematics)
  {
    if(systematics.count(kv.first) == 0)
      retVal(kv.first) = (value != kv.second);
    else
      retVal(kv.first) = (systematics.at(kv.first) != kv.second);
  }

  return retVal;
}

template<class T>
const ValueWithSystematicsInternal<bool> ValueWithSystematicsInternal<T>::operator> (const T& val) const
{
  static_assert(!(std::is_same<T, bool>::value), "You can not test > with booleans");
  ValueWithSystematicsInternal<bool> retVal(value > val);

  for(auto& kv: systematics)
    retVal(kv.first) = (kv.second > val);

  return retVal;
}

template<class T>
const ValueWithSystematicsInternal<bool> ValueWithSystematicsInternal<T>::operator> (const ValueWithSystematicsInternal<T>& val) const
{
  static_assert(!(std::is_same<T, bool>::value), "You can not test > with booleans");
  ValueWithSystematicsInternal<bool> retVal(value > val.value);

  for(auto& kv: systematics)
  {
    if(val.systematics.count(kv.first) == 0)
      retVal(kv.first) = (kv.second > val.value);
  }

  for(auto& kv: val.systematics)
  {
    if(systematics.count(kv.first) == 0)
      retVal(kv.first) = (value > kv.second);
    else
      retVal(kv.first) = (systematics.at(kv.first) > kv.second);
  }

  return retVal;
}

template<class T>
const ValueWithSystematicsInternal<bool> ValueWithSystematicsInternal<T>::operator< (const T& val) const
{
  static_assert(!(std::is_same<T, bool>::value), "You can not test < with booleans");
  ValueWithSystematicsInternal<bool> retVal(value < val);

  for(auto& kv: systematics)
    retVal(kv.first) = (kv.second < val);

  return retVal;
}

template<class T>
const ValueWithSystematicsInternal<bool> ValueWithSystematicsInternal<T>::operator< (const ValueWithSystematicsInternal<T>& val) const
{
  static_assert(!(std::is_same<T, bool>::value), "You can not test < with booleans");
  ValueWithSystematicsInternal<bool> retVal(value < val.value);

  for(auto& kv: systematics)
  {
    if(val.systematics.count(kv.first) == 0)
      retVal(kv.first) = (kv.second < val.value);
  }

  for(auto& kv: val.systematics)
  {
    if(systematics.count(kv.first) == 0)
      retVal(kv.first) = (value < kv.second);
    else
      retVal(kv.first) = (systematics.at(kv.first) < kv.second);
  }

  return retVal;
}

template<class T>
const ValueWithSystematicsInternal<bool> ValueWithSystematicsInternal<T>::operator>=(const T& val) const
{
  static_assert(!(std::is_same<T, bool>::value), "You can not test >= with booleans");
  ValueWithSystematicsInternal<bool> retVal(value >= val);

  for(auto& kv: systematics)
    retVal(kv.first) = (kv.second >= val);

  return retVal;
}

template<class T>
const ValueWithSystematicsInternal<bool> ValueWithSystematicsInternal<T>::operator>=(const ValueWithSystematicsInternal<T>& val) const
{
  static_assert(!(std::is_same<T, bool>::value), "You can not test >= with booleans");
  ValueWithSystematicsInternal<bool> retVal(value >= val.value);

  for(auto& kv: systematics)
  {
    if(val.systematics.count(kv.first) == 0)
      retVal(kv.first) = (kv.second >= val.value);
  }

  for(auto& kv: val.systematics)
  {
    if(systematics.count(kv.first) == 0)
      retVal(kv.first) = (value >= kv.second);
    else
      retVal(kv.first) = (systematics.at(kv.first) >= kv.second);
  }

  return retVal;
}

template<class T>
const ValueWithSystematicsInternal<bool> ValueWithSystematicsInternal<T>::operator<=(const T& val) const
{
  static_assert(!(std::is_same<T, bool>::value), "You can not test <= with booleans");
  ValueWithSystematicsInternal<bool> retVal(value <= val);

  for(auto& kv: systematics)
    retVal(kv.first) = (kv.second <= val);

  return retVal;
}

template<class T>
const ValueWithSystematicsInternal<bool> ValueWithSystematicsInternal<T>::operator<=(const ValueWithSystematicsInternal<T>& val) const
{
  static_assert(!(std::is_same<T, bool>::value), "You can not test <= with booleans");
  ValueWithSystematicsInternal<bool> retVal(value <= val.value);

  for(auto& kv: systematics)
  {
    if(val.systematics.count(kv.first) == 0)
      retVal(kv.first) = (kv.second <= val.value);
  }

  for(auto& kv: val.systematics)
  {
    if(systematics.count(kv.first) == 0)
      retVal(kv.first) = (value <= kv.second);
    else
      retVal(kv.first) = (systematics.at(kv.first) <= kv.second);
  }

  return retVal;
}

template<class T>
const ValueWithSystematicsInternal<T> ValueWithSystematicsInternal<T>::operator! () const
{
  static_assert(std::is_same<T, bool>::value, "You can only negate with booleans");

  ValueWithSystematicsInternal<T> retVal(!value);

  for(auto& kv: systematics)
    retVal(kv.first) = !kv.second;

  return retVal;
}

template<class T>
const ValueWithSystematicsInternal<T> ValueWithSystematicsInternal<T>::operator&&(const ValueWithSystematicsInternal<T>& val) const
{
  static_assert(std::is_same<T, bool>::value, "You can only perform a logical AND with booleans");

  ValueWithSystematicsInternal<T> retVal(value && val.value);

  for(auto& kv: systematics)
    if(val.systematics.count(kv.first) == 0)
      retVal(kv.first) = kv.second && val.value;

  for(auto& kv: val.systematics)
  {
    if(systematics.count(kv.first) == 0)
      retVal(kv.first) = value && kv.second;
    else
      retVal(kv.first) = systematics.at(kv.first) && kv.second;
  }

  return retVal;
}

template<class T>
const ValueWithSystematicsInternal<T> ValueWithSystematicsInternal<T>::operator||(const ValueWithSystematicsInternal<T>& val) const
{
  static_assert(std::is_same<T, bool>::value, "You can only perform a logical OR with booleans");

  ValueWithSystematicsInternal<T> retVal(value || val.value);

  for(auto& kv: systematics)
    if(val.systematics.count(kv.first) == 0)
      retVal(kv.first) = kv.second || val.value;

  for(auto& kv: val.systematics)
  {
    if(systematics.count(kv.first) == 0)
      retVal(kv.first) = value || kv.second;
    else
      retVal(kv.first) = systematics.at(kv.first) || kv.second;
  }

  return retVal;
}


// Specializations --------------------------------------------------------------------------------
//****************** std::vector *******************************************************************
template<class T>
ValueWithSystematics<int> ValueWithSystematics<std::vector<T>>::size() const
{
  ValueWithSystematics<int> retVal(value.size());

  for(auto& kv: systematics)
    retVal(kv.first) = kv.second.size();

  return retVal;
}

template<class T>
ValueWithSystematics<T> ValueWithSystematics<std::vector<T>>::first(T defaultValue) const
{
  ValueWithSystematics<T> retVal(defaultValue);

  for(auto& kv: systematics)
  {
    retVal(kv.first); // Create the variation
    if(kv.second.size() > 0) // Only assign a value if it makes sense (the default value will be used instead)
      retVal(kv.first) = kv.second.at(0);
  }

  if(value.size() > 0)
    retVal.Value() = value.at(0);

  return retVal;
}

template<class T>
void loadSystematics(std::vector<std::string>& list, const ValueWithSystematics<T>& variable)
{
  for(auto& syst: variable.Systematics())
  {
    if(std::find(list.begin(), list.end(), syst) == list.end())
    {
      list.push_back(syst);
    }
  }

  return;
}

#endif
