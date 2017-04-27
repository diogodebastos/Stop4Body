
#include "UserCode/Stop4Body/interface/commonFunctions.h"
#define M_PIl          3.141592653589793238462643383279502884L /* pi */


bool fileExists(std::string fileName)
{
  std::ifstream infile(fileName);
  return infile.good();
}

float DeltaPhi(double p1, double p2)
{
  double x = p1 - p2;

  while(x >= M_PIl)
    x -= (2.*M_PIl);

  while(x < -M_PIl)
    x += (2.*M_PIl);

  if(x < 0)
    return -x;
  return x;
  //return std::abs(x);
}
