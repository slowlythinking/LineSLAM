#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include "Timer.h"
#include "LS.h"

using namespace std;

namespace LineSLAM
{
class EDLineDetect {
public:
  EDLineDetect(int i);

  int ReadImagePGM(char *filename, char **pBuffer, int *pWidth, int *pHeight);
  
  void SaveImagePGM(char *filename, char *buffer, int width, int height);

  //the true function for line detect
  LS *DetectEDLines(string filename, int *pNoLines);
};
}
