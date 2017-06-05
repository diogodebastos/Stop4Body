#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <list>
#include <iterator>
#include <algorithm>
#include <iomanip>
#include <math.h>
#include <memory>
#include <cmath>
#include <cstdlib>

#include "UserCode/Stop4Body/interface/json.hpp"
#include "UserCode/Stop4Body/interface/SampleReader.h"

using json = nlohmann::json;

int main(int argc, char** argv)
{
  std::string jsonSampleFileName = "";
  std::string jsonTemplateFileName = "";
  std::string templateFileName = "";
  std::string outputDirectory = "./OUT/";

  if(argc < 2)
  {
    std::cout << "You did not pass enough parameters" << std::endl;
    return 0;
  }

  for(int i = 1; i < argc; ++i)
  {
    std::string argument = argv[i];

    if(argument == "--samples")
      jsonSampleFileName = argv[++i];

    if(argument == "--template")
      templateFileName = argv[++i];

    if(argument == "--outDir")
      outputDirectory = argv[++i];

    if(argument == "--jsonTemplate")
      jsonTemplateFileName = argv[++i];
  }

  if(jsonSampleFileName == "")
  {
    std::cout << "You must define a json file with the samples" << std::endl;
    return 1;
  }

  if(templateFileName == "")
  {
    std::cout << "You must define a template file for the jobs" << std::endl;
    return 1;
  }

  if(jsonTemplateFileName == "")
  {
    std::cout << "You must define a template file for the json for each job" << std::endl;
    return 1;
  }

  std::cout << "Reading JSON file" << std::endl;
  SampleReader samples(jsonSampleFileName);

  for(auto &process : samples)
  {
    std::cout << "Building jobs for process: " << process.tag() << std::endl;

    for(auto &sample : process)
    {
      std::cout << "\tBuilding jobs for sample: " << sample.tag() << std::endl;

      std::string sampleDirectory = outputDirectory + "/" + sample.tag();
      int retVal = system(("mkdir -p \"" + sampleDirectory + "\"").c_str());
      if(retVal != 0)
      {
        std::cout << "Unable to create the driectory: " << sampleDirectory << std::endl;
        std::cout << "Continuing to next sample..." << std::endl;
        continue;
      }

      for(int thisPart = 0; thisPart < sample.nParts(); ++thisPart)
      {
        std::stringstream converter;
        std::string thisPartStr;
        converter << thisPart;
        converter >> thisPartStr;

        json thisPartJson;
        std::ifstream jsonTemplateFile(jsonTemplateFileName);
        jsonTemplateFile >> thisPartJson;

        auto &thisPartProcess = thisPartJson["lines"][0];

        thisPartProcess["tag"] = process.jsonObj()["tag"];
        thisPartProcess["label"] = process.jsonObj()["label"];
        thisPartProcess["color"] = process.jsonObj()["color"];

        std::vector<std::string> processTags = {"isdata",
                                                "issignal",
                                                "isfastsim",
                                                "spimpose",
                                                "lcolor",
                                                "lwidth",
                                                "lstyle",
                                                "fill",
                                                "mcolor",
                                                "marker",
                                                "selection"};
        for(auto &tag : processTags)
        {
          if(process.jsonObj().count(tag) > 0)
            thisPartProcess[tag] = process.jsonObj()[tag];
        }
        thisPartProcess["files"][0] = sample.jsonObj();

        thisPartProcess["files"][0]["runPart"] = thisPart;

        std::ofstream thisPartJsonFile((sampleDirectory + "/" + sample.tag() + "_Part" + thisPartStr), std::ios_base::binary | std::ios_base::trunc);
        thisPartJsonFile << thisPartJson;
      }
    }
  }

  return 0;
}
