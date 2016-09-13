#include "UserCode/Stop4Body/interface/VariableReader.h"

using json = nlohmann::json;

VariableJsonLoader::VariableJsonLoader(std::string file): inputFile_(file)
{}