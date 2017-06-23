// <author>Cristóvão B da Cruz e Silva</author>
// <email>c.beirao@cern.ch</email>
// <date>2017-06-19</date>
// <summary>Header file for the ValueWithSystematics class</summary>
//
// <description>
//  Header file with the declarations of the ValueWithSystematics class and derivates.
//  This class is made to function exactly like the builtin types (and other types when the operators are defined),
// except the types now have associated named systematic uncertainties, which are treated idependently. The systematic
// uncertainties are correctly handled when performing computations and many other tasks, only requiring to be
// handled by hand in specific circumstances.
//  Several methods have been implemented to allow to handle the uncertainties by hand when needed. Also, some other
// methods have been implemented to allow functionality that is normally available to specific types or builtin functions.
// </description>

#ifndef VALUE_WITH_SYSTEMATICS_H
#define VALUE_WITH_SYSTEMATICS_H

#include <map>
#include <string>
#include <vector>

class ValueException: public std::exception
{
  public:
    ValueException(std::string mess): message(mess)
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

template<class T>
class ValueWithSystematicsInternal
{
  public:
    ValueWithSystematicsInternal(T val = T(0));
    ValueWithSystematicsInternal(const ValueWithSystematicsInternal<T>& val); // Copy constructor
    ValueWithSystematicsInternal(ValueWithSystematicsInternal<T>&& val) noexcept; // Move constructor

    virtual void Reset();
    inline void Lock() { isLocked = true; };
    #ifdef WITH_UNLOCK
    inline void Unlock() { isLocked = false; };
    #endif

    inline T& Value() { return value; };
    inline const T& Value() const { return value; };
    inline T& DefaultValue() { return defaultValue; };
    T& Systematic(const std::string& name);
    const T& Systematic(const std::string& name) const;
    T& GetSystematicOrValue(const std::string& name);
    const T& GetSystematicOrValue(const std::string& name) const;

    std::vector<std::string> Systematics() const;

    bool AddMetadata(const std::string& key, const std::string& value);
    std::string GetMetadata(std::string& key);
    std::string GetMetadata(const std::string& key) const;
    std::string& Metadata(const std::string& key);
    const std::string& Metadata(const std::string& key) const;

    // These next operators are where the magic happens
    // ---------  Function  operator  ---------
    T& operator()(const std::string& name);
    // ---------  Casting   operator  ---------
    explicit operator T () const; // Can only be explicitly called, ie. it disables implicit calling
    // --------- Assignment operators ---------
    ValueWithSystematicsInternal<T>& operator= (const T& val);
    ValueWithSystematicsInternal<T>& operator= (T&& val) noexcept;
    ValueWithSystematicsInternal<T>& operator= (const ValueWithSystematicsInternal<T>& val);
    ValueWithSystematicsInternal<T>& operator= (ValueWithSystematicsInternal<T>&& val) noexcept;
    //       Compound Assignment Operators
    ValueWithSystematicsInternal<T>& operator+=(const T& val);
    ValueWithSystematicsInternal<T>& operator+=(T&& val) noexcept;
    ValueWithSystematicsInternal<T>& operator+=(const ValueWithSystematicsInternal<T>& val);
    ValueWithSystematicsInternal<T>& operator+=(ValueWithSystematicsInternal<T>&& val) noexcept;
    ValueWithSystematicsInternal<T>& operator-=(const T& val);
    ValueWithSystematicsInternal<T>& operator-=(T&& val) noexcept;
    ValueWithSystematicsInternal<T>& operator-=(const ValueWithSystematicsInternal<T>& val);
    ValueWithSystematicsInternal<T>& operator-=(ValueWithSystematicsInternal<T>&& val) noexcept;
    ValueWithSystematicsInternal<T>& operator*=(const T& val);
    ValueWithSystematicsInternal<T>& operator*=(T&& val) noexcept;
    ValueWithSystematicsInternal<T>& operator*=(const ValueWithSystematicsInternal<T>& val);
    ValueWithSystematicsInternal<T>& operator*=(ValueWithSystematicsInternal<T>&& val) noexcept;
    ValueWithSystematicsInternal<T>& operator/=(const T& val);
    ValueWithSystematicsInternal<T>& operator/=(T&& val) noexcept;
    ValueWithSystematicsInternal<T>& operator/=(const ValueWithSystematicsInternal<T>& val);
    ValueWithSystematicsInternal<T>& operator/=(ValueWithSystematicsInternal<T>&& val) noexcept;
    // --------- Arithmetic operators ---------
    const ValueWithSystematicsInternal<T> operator+(const T& val) const;
    const ValueWithSystematicsInternal<T> operator+(const ValueWithSystematicsInternal<T>& val) const;
    const ValueWithSystematicsInternal<T> operator-(const T& val) const;
    const ValueWithSystematicsInternal<T> operator-(const ValueWithSystematicsInternal<T>& val) const;
    const ValueWithSystematicsInternal<T> operator*(const T& val) const;
    const ValueWithSystematicsInternal<T> operator*(const ValueWithSystematicsInternal<T>& val) const;
    const ValueWithSystematicsInternal<T> operator/(const T& val) const;
    const ValueWithSystematicsInternal<T> operator/(const ValueWithSystematicsInternal<T>& val) const;
    // --------- Comparison operators ---------
    const ValueWithSystematicsInternal<bool> operator==(const T& val) const;
    const ValueWithSystematicsInternal<bool> operator==(const ValueWithSystematicsInternal<T>& val) const;
    const ValueWithSystematicsInternal<bool> operator!=(const T& val) const;
    const ValueWithSystematicsInternal<bool> operator!=(const ValueWithSystematicsInternal<T>& val) const;
    const ValueWithSystematicsInternal<bool> operator> (const T& val) const;
    const ValueWithSystematicsInternal<bool> operator> (const ValueWithSystematicsInternal<T>& val) const;
    const ValueWithSystematicsInternal<bool> operator< (const T& val) const;
    const ValueWithSystematicsInternal<bool> operator< (const ValueWithSystematicsInternal<T>& val) const;
    const ValueWithSystematicsInternal<bool> operator>=(const T& val) const;
    const ValueWithSystematicsInternal<bool> operator>=(const ValueWithSystematicsInternal<T>& val) const;
    const ValueWithSystematicsInternal<bool> operator<=(const T& val) const;
    const ValueWithSystematicsInternal<bool> operator<=(const ValueWithSystematicsInternal<T>& val) const;
    // ---------  Logical  operators  ---------
    const ValueWithSystematicsInternal<T> operator! () const;
    const ValueWithSystematicsInternal<T> operator&&(const ValueWithSystematicsInternal<T>& val) const;
    const ValueWithSystematicsInternal<T> operator||(const ValueWithSystematicsInternal<T>& val) const;
    // ---------   Unary  operators   ---------
    const ValueWithSystematicsInternal<T> operator-() const;
    ValueWithSystematicsInternal<T>& operator++();
    ValueWithSystematicsInternal<T> operator++(int);
    ValueWithSystematicsInternal<T>& operator--();
    ValueWithSystematicsInternal<T> operator--(int);

    // Iteration
    typedef typename std::map<std::string, T>::iterator iterator;
    typedef typename std::map<std::string, T>::const_iterator const_iterator;

    iterator& begin() {return systematics.begin();}
    const_iterator begin() const {return systematics.begin();}
    const_iterator cbegin() const {return systematics.cbegin();}
    iterator& end() {return systematics.end();}
    const_iterator end() const {return systematics.end();}
    const_iterator cend() const {return systematics.cend();}

  private:
  protected:
    bool isLocked;
    T defaultValue;
    T value;
    mutable std::map<std::string, T> systematics;
    mutable std::map<std::string,std::string> metadata;
};

// External classes (to be used)
template<class T, typename = void>
class ValueWithSystematics: public ValueWithSystematicsInternal<T>
{
  public:
    ValueWithSystematics(T val = T(0)): ValueWithSystematicsInternal<T>(val) {};
    ValueWithSystematics(const ValueWithSystematics<T>& val): ValueWithSystematicsInternal<T>(val) {}; // Copy constructor
    ValueWithSystematics(const ValueWithSystematicsInternal<T>& val): ValueWithSystematicsInternal<T>(val) {}; // Copy constructor
    ValueWithSystematics(ValueWithSystematics<T>&& val) noexcept: ValueWithSystematicsInternal<T>(std::move(val)) {}; // Move constructor
    ValueWithSystematics(ValueWithSystematicsInternal<T>&& val) noexcept: ValueWithSystematicsInternal<T>(std::move(val)) {}; // Move constructor

    // --------- Assignment operators ---------
    ValueWithSystematics<T>& operator= (const T& val) {ValueWithSystematicsInternal<T>::operator=(val); return *this;};
    ValueWithSystematics<T>& operator= (T&& val) noexcept {ValueWithSystematicsInternal<T>::operator=(std::move(val)); return *this;};
    ValueWithSystematics<T>& operator= (const ValueWithSystematics<T>& val) {ValueWithSystematicsInternal<T>::operator=(val); return *this;};
    ValueWithSystematics<T>& operator= (ValueWithSystematics<T>&& val) noexcept {ValueWithSystematicsInternal<T>::operator=(std::move(val)); return *this;};
    ValueWithSystematics<T>& operator= (const ValueWithSystematicsInternal<T>& val) {ValueWithSystematicsInternal<T>::operator=(val); return *this;};
    ValueWithSystematics<T>& operator= (ValueWithSystematicsInternal<T>&& val) noexcept {ValueWithSystematicsInternal<T>::operator=(std::move(val)); return *this;};

  private:
  protected:
    using ValueWithSystematicsInternal<T>::isLocked;
    using ValueWithSystematicsInternal<T>::defaultValue;
    using ValueWithSystematicsInternal<T>::value;
    using ValueWithSystematicsInternal<T>::systematics;
    using ValueWithSystematicsInternal<T>::metadata;
};

// forward declaration of double specialization
template<>
class ValueWithSystematics<double>;

// vector specialization
template<class T>
class ValueWithSystematics<std::vector<T>>: public ValueWithSystematicsInternal<std::vector<T>>
{
  public:
    ValueWithSystematics(): ValueWithSystematicsInternal<std::vector<T>>(std::vector<T>(0)) {};
    ValueWithSystematics(std::vector<T> val): ValueWithSystematicsInternal<std::vector<T>>(val) {};
    ValueWithSystematics(const ValueWithSystematics<std::vector<T>>& val): ValueWithSystematicsInternal<std::vector<T>>(val) {}; // Copy constructor
    ValueWithSystematics(const ValueWithSystematicsInternal<std::vector<T>>& val): ValueWithSystematicsInternal<std::vector<T>>(val) {}; // Copy constructor
    ValueWithSystematics(ValueWithSystematics<std::vector<T>>&& val) noexcept: ValueWithSystematicsInternal<std::vector<T>>(std::move(val)) {}; // Move constructor
    ValueWithSystematics(ValueWithSystematicsInternal<std::vector<T>>&& val) noexcept: ValueWithSystematicsInternal<std::vector<T>>(std::move(val)) {}; // Move constructor

    // --------- Assignment operators ---------
    ValueWithSystematics<std::vector<T>>& operator= (const std::vector<T>& val) {ValueWithSystematicsInternal<std::vector<T>>::operator=(val); return *this;};
    ValueWithSystematics<std::vector<T>>& operator= (std::vector<T>&& val) noexcept {ValueWithSystematicsInternal<std::vector<T>>::operator=(std::move(val)); return *this;};
    ValueWithSystematics<std::vector<T>>& operator= (const ValueWithSystematics<std::vector<T>>& val) {ValueWithSystematicsInternal<std::vector<T>>::operator=(val); return *this;};
    ValueWithSystematics<std::vector<T>>& operator= (ValueWithSystematics<std::vector<T>>&& val) noexcept {ValueWithSystematicsInternal<std::vector<T>>::operator=(std::move(val)); return *this;};
    ValueWithSystematics<std::vector<T>>& operator= (const ValueWithSystematicsInternal<std::vector<T>>& val) {ValueWithSystematicsInternal<std::vector<T>>::operator=(val); return *this;};
    ValueWithSystematics<std::vector<T>>& operator= (ValueWithSystematicsInternal<std::vector<T>>&& val) noexcept {ValueWithSystematicsInternal<std::vector<T>>::operator=(std::move(val)); return *this;};

    ValueWithSystematics<int> size() const;
    ValueWithSystematics<T> first(T defaultValue) const;

  private:
  protected:
    using ValueWithSystematicsInternal<std::vector<T>>::isLocked;
    using ValueWithSystematicsInternal<std::vector<T>>::defaultValue;
    using ValueWithSystematicsInternal<std::vector<T>>::value;
    using ValueWithSystematicsInternal<std::vector<T>>::systematics;
    using ValueWithSystematicsInternal<std::vector<T>>::metadata;
};

// float specialization
template<>
class ValueWithSystematics<float>: public ValueWithSystematicsInternal<float>
{
  public:
  //  using ValueWithSystematicsInternal<T>::ValueWithSystematicsInternal; //Why doesn't this one work?
    ValueWithSystematics(float val = 0): ValueWithSystematicsInternal<float>(val) {};
    ValueWithSystematics(const ValueWithSystematics<float>& val): ValueWithSystematicsInternal<float>(val) {}; // Copy constructor
    ValueWithSystematics(const ValueWithSystematicsInternal<float>& val): ValueWithSystematicsInternal<float>(val) {}; // Copy constructor
    ValueWithSystematics(ValueWithSystematics<float>&& val) noexcept: ValueWithSystematicsInternal<float>(std::move(val)) {}; // Move constructor
    ValueWithSystematics(ValueWithSystematicsInternal<float>&& val) noexcept: ValueWithSystematicsInternal<float>(std::move(val)) {}; // Move constructor

    // --------- Assignment operators ---------
    ValueWithSystematics<float>& operator= (const float& val);
    ValueWithSystematics<float>& operator= (float&& val) noexcept;
    ValueWithSystematics<float>& operator= (const ValueWithSystematics<float>& val);
    ValueWithSystematics<float>& operator= (ValueWithSystematics<float>&& val) noexcept;
    ValueWithSystematics<float>& operator= (const ValueWithSystematicsInternal<float>& val);
    ValueWithSystematics<float>& operator= (ValueWithSystematicsInternal<float>&& val) noexcept;
    ValueWithSystematics<float>& operator= (const ValueWithSystematics<double>& val);
    ValueWithSystematics<float>& operator= (ValueWithSystematics<double>&& val) noexcept;
    ValueWithSystematics<float>& operator= (const ValueWithSystematicsInternal<double>& val);
    ValueWithSystematics<float>& operator= (ValueWithSystematicsInternal<double>&& val) noexcept;

  private:
  protected:
    using ValueWithSystematicsInternal<float>::isLocked;
    using ValueWithSystematicsInternal<float>::defaultValue;
    using ValueWithSystematicsInternal<float>::value;
    using ValueWithSystematicsInternal<float>::systematics;
    using ValueWithSystematicsInternal<float>::metadata;
};

// double specialization
template<>
class ValueWithSystematics<double>: public ValueWithSystematicsInternal<double>
{
  public:
  //  using ValueWithSystematicsInternal<T>::ValueWithSystematicsInternal; //Why doesn't this one work?
    ValueWithSystematics(double val = 0): ValueWithSystematicsInternal<double>(val) {};
    ValueWithSystematics(const ValueWithSystematics<double>& val): ValueWithSystematicsInternal<double>(val) {}; // Copy constructor
    ValueWithSystematics(const ValueWithSystematicsInternal<double>& val): ValueWithSystematicsInternal<double>(val) {}; // Copy constructor
    ValueWithSystematics(ValueWithSystematics<double>&& val): ValueWithSystematicsInternal<double>(std::move(val)) {}; // Move constructor
    ValueWithSystematics(ValueWithSystematicsInternal<double>&& val): ValueWithSystematicsInternal<double>(std::move(val)) {}; // Move constructor

    // --------- Assignment operators ---------
    ValueWithSystematics<double>& operator= (const double& val);
    ValueWithSystematics<double>& operator= (double&& val) noexcept;
    ValueWithSystematics<double>& operator= (const ValueWithSystematics<double>& val);
    ValueWithSystematics<double>& operator= (ValueWithSystematics<double>&& val) noexcept;
    ValueWithSystematics<double>& operator= (const ValueWithSystematicsInternal<double>& val);
    ValueWithSystematics<double>& operator= (ValueWithSystematicsInternal<double>&& val) noexcept;

    ValueWithSystematics<double> Cos() const;
    ValueWithSystematics<double> Sqrt() const;
    ValueWithSystematics<double> abs() const;
    ValueWithSystematics<double> pow(double exp) const;

    //template<class U>
    //friend ValueWithSystematics<U>& ValueWithSystematics<U, typename std::enable_if<std::is_base_of<TLorentzVector, U>::value>::type>::operator*=(const ValueWithSystematics<double>& val);

  private:
  protected:
    using ValueWithSystematicsInternal<double>::isLocked;
    using ValueWithSystematicsInternal<double>::defaultValue;
    using ValueWithSystematicsInternal<double>::value;
    using ValueWithSystematicsInternal<double>::systematics;
    using ValueWithSystematicsInternal<double>::metadata;
};

template<class T>
void loadSystematics(std::vector<std::string>& list, const ValueWithSystematics<T>& variable);

// Templated classes must have their code in the header :(
#include "UserCode/Stop4Body/interface/ValueWithSystematics.hpp"

#endif
