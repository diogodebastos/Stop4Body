#include <iostream>
#include <vector>

#include "TROOT.h"
#include "TFile.h"
#include "TH1.h"

int main(int argc, char** argv)
{
  std::string file1;
  std::string file2;

  if(argc != 3)
  {
    std::cout << "You did not pass the right number of parameters" << std::endl;
    printHelp();
    return 0;
  }

  file1 = argv[1];
  file2 = argv[2];

  std::cout << "File1: " << file1 << std::endl;
  std::cout << "File2: " << file2 << std::endl;

  return 0;
}
