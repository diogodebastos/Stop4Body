
#include "UserCode/Stop4Body/interface/json.hpp"
#include "UserCode/Stop4Body/interface/SampleReader.h"
#include "UserCode/Stop4Body/interface/doubleWithUncertainty.h"

#define GENPART_LIMIT  40
#define JETCOLL_LIMIT  40
#define LEPCOLL_LIMIT  40
#define TAUCOLL_LIMIT  40
#define TRACOLL_LIMIT 200

using json = nlohmann::json;

int main(int argc, char** argv)
{
  std::string jsonFileName = "";
  std::string inputDirectory = "";
  std::string outputDirectory = "./plots/";
  std::string suffix = "";
  std::string variablesJson = "";
  bool normalize = false;

  if(argc < 2)
  {
    std::cout << "You did not pass enough parameters" << std::endl;
    printHelp();
    return 0;
  }

  for(int i = 1; i < argc; ++i)
  {
    std::string argument = argv[i];

    if(argument == "--help")
    {
      printHelp();
      return 0;
    }

    if(argument == "--json")
      jsonFileName = argv[++i];

    if(argument == "--outDir")
      outputDirectory = argv[++i];

    if(argument == "--inDir")
      inputDirectory = argv[++i];

    if(argument == "--suffix")
      suffix = argv[++i];

    if(argument == "--variables")
      variablesJson = argv[++i];

    if(argument == "--normalize")
    {
      normalize = true;
    }
  }
}

void printHelp()
{
  std::cout << "The program has the following parameters:" << std::endl;
  std::cout << "\t--help\t- Print this message" << std::endl;
  std::cout << "\t--json\t- Used to specify the json file which describes the samples, this option is obligatory. The parameter immediately after this one should be the desired json file" << std::endl;
  std::cout << "\t--outDir\t- Used to specify the output directory. The parameter immediately after this one should be the desired directory. By default the directory './OUT/' is used" << std::endl;
  std::cout << "\t--suffix\t- Suffix to add at the end of the name of the output files." << std::endl;

  return;
}
