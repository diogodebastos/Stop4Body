// Compile with:
// clang++ -std=c++11 -stdlib=libc++ main.cc ValueWithSystematics.cc
#include <iostream>
#include <string>

#include "ValueWithSystematics.h"

int main(int argc, char** argv)
{
  auto print = [&] (const ValueWithSystematics<float>& val, std::string name="test") -> bool
  {
    std::cout << name << " summary:" << std::endl;
    std::cout << "  Central value: " << val.Value() << std::endl;
    for(auto &syst: val.Systematics())
      std::cout << "  " << syst << ": " << val.Systematic(syst) << std::endl;
    return true;
  };


  ValueWithSystematics<float> test = 1;
  test.Systematic("test") = 1.1;
  print(test);

  ValueWithSystematics<float> test1 = test;
  test1.Systematic("down") = 0.9;
  print(test1, "test1");

  ValueWithSystematics<float> test2 = std::move(test1);
  test2 += 1.0;
  test2 += test;
  print(test2, "test2");

  ValueWithSystematics<float> test3(test + test2);
  print(test3, "test3");

  test1 = 2;
  print(test1, "test1");

  test1 = test2 - 3;
  print(test1, "test1");

  test1 += test;
  print(test1, "test1");

  ValueWithSystematics<bool> boolTest = true;
  std::cout << ((static_cast<bool>(boolTest))?("True"):("False")) << std::endl;


  ValueWithSystematics<float> liveTest = 1;
  test.Systematic("PU_Up") = 1.1;
  test.Systematic("PU_Down") = 0.9;
  ValueWithSystematics<float> liveTest2 = 2;
  test.Systematic("PU_Up") = 2.1;
  test.Systematic("PU_Down") = 1.9;
  ValueWithSystematics<float> liveTest3;
  liveTest3 = liveTest2 * liveTest;


  std::cout << std::endl << "Testing assignment" << std::endl;
  ValueWithSystematics<float> assignmentVariable = 0.8;
  assignmentVariable.Systematic("Whatever_Up") = 0.9;
  assignmentVariable.Systematic("Whatever_Down") = 0.7;
  assignmentVariable.Lock();
  print(assignmentVariable, "assignmentVariable");
  assignmentVariable = 1;
  print(assignmentVariable, "assignmentVariable");


  return 0;
}
