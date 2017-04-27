#ifndef MY_COMMON_FUNCTION
#define MY_COMMON_FUNCTION

#include "UserCode/Stop4Body/interface/doubleWithUncertainty.h"

bool fileExists(std::string);
float DeltaPhi(double, double);

doubleUnc stopCrossSection(double stopM, double lspM);

#endif
