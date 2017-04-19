#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "UserCode/Stop4Body/interface/json.hpp"
using json = nlohmann::json;

bool fileExists(std::string);

int main(int argc, char** argv)
{
  std::string outputJSON = "";
  std::vector<std::string> inputJSONs;

  if(argc < 4)
  {
    std::cout << "You must supply at least 2 files to be merged" << std::endl;
    //printHelp();
    return 0;
  }

  outputJSON = argv[1];
  for(int i = 2; i < argc; ++i)
  {
    std::string argument = argv[i];
    if(fileExists(argument))
      inputJSONs.push_back(argument);
    else
      std::cout << "The file " << argument << " does not exist" << std::endl;
  }

  if(inputJSONs.size() < 2)
  {
    std::cout << "Less than 2 files were found" << std::endl;
    return 0;
  }

  json outJsonFile;
  for(auto &file: inputJSONs)
  {
    json jsonFile;
    std::ifstream inputFile(file);
    inputFile >> jsonFile;

    if(jsonFile.count("lines") == 0)
      continue;

    if(outJsonFile.count("lines") == 0) // Copy the first file directly to output
    {
      outJsonFile = jsonFile;
      continue;
    }

    for(auto& process: jsonFile["lines"])
    {
      bool found = false;

      for(auto& otherProcess: outJsonFile["lines"])
      {
        if(process["tag"] == otherProcess["tag"])
        {
          found = true;
          break;
        }
      }

      if(found)
      {
        for(auto& outProcess: outJsonFile["lines"])
        {
          if(process["tag"] != outProcess["tag"])
            continue;

          for(auto& sample: process["files"])
          {
            bool foundSample = false;
            for(auto& outSample: outProcess["files"])
            {
              if(sample["tag"] == outSample["tag"])
              {
                foundSample = true;
                break;
              }
            }

            if(foundSample)
            {
              std::cout << "The sample \"" << sample["tag"] << "\" from process \"" << process["tag"] << "\" was already in the output json." << std::endl;
              std::cout << "I do not know how to handle it, so ignoring for now" << std::endl;
              continue;
            }

            outProcess["files"].push_back(sample);
          }

          break;
        }
      }
      else
        outJsonFile["lines"].push_back(process);
    }
  }

  std::ofstream outputFile(outputJSON);
  outputFile << outJsonFile.dump(2);

  return 0;
}

bool fileExists(std::string fileName)
{
  std::ifstream infile(fileName);
  return infile.good();
}
