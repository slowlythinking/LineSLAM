#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include "LineDetector.h"

using namespace std;

LS *DetectLinesByED(unsigned char *srcImg, int width, int height, int *pNoLines);

namespace LineSLAM
{

EDLineDetect::EDLineDetect(int i)
{
}

int EDLineDetect::ReadImagePGM(char *filename, char **pBuffer, int *pWidth, int *pHeight){
   FILE *fp;
   char buf[71];
   int width, height;

   if ((fp = fopen(filename, "rb")) == NULL){
     fprintf(stderr, "Error reading the file %s in ReadImagePGM().\n", filename);
     return(0);
   } //end-if

   /***************************************************************************
   * Verify that the image is in PGM format, read in the number of columns
   * and rows in the image and scan past all of the header information.
   ***************************************************************************/
   fgets(buf, 70, fp);
   bool P2 = false;
   bool P5 = false;

   if      (strncmp(buf, "P2", 2) == 0) P2 = true;
   else if (strncmp(buf, "P5", 2) == 0) P5 = true;

   if (P2 == false && P5 == false){
      fprintf(stderr, "The file %s is not in PGM format in ", filename);
      fprintf(stderr, "ReadImagePGM().\n");
      fclose(fp);
      return 0;
   } //end-if

   do {fgets(buf, 70, fp);} while (buf[0] == '#');  /* skip all comment lines */
   sscanf(buf, "%d %d", &width, &height);
   fgets(buf, 70, fp);  // Skip max value (255)

   *pWidth = width;
   *pHeight = height;

   /***************************************************************************
   * Allocate memory to store the image then read the image from the file.
   ***************************************************************************/
   if (((*pBuffer) = (char *) malloc((*pWidth)*(*pHeight))) == NULL){
      fprintf(stderr, "Memory allocation failure in ReadImagePGM().\n");
      fclose(fp);
      return(0);
   } //end-if  

   if (P2){
      int index=0;
      char *p = *pBuffer;
      int col = 0;
      int read = 0;

      while (1){
        int c;
        if (fscanf(fp, "%d", &c) < 1) break;
        read++;

        if (col < *pWidth) p[index++] = (unsigned char)c;

        col++;
        if (col == width) col = 0;
      } //end-while

      if (read != width*height){
        fprintf(stderr, "Error reading the image data in ReadImagePGM().\n");
        fclose(fp);
        free((*pBuffer));
        return(0);
      } //end-if

   } else if (P5){
      int index=0;
      char *p = *pBuffer;
      int col = 0;
      int read = 0;

      while (1){
        unsigned char c;
        if (fread(&c, 1, 1, fp) < 1) break;
        read++;

        if (col < *pWidth) p[index++] = c;

        col++;
        if (col == width) col = 0;
      } //end-while

     if (read != width*height){
        fprintf(stderr, "Error reading the image data in ReadImagePGM().\n");
        fclose(fp);
        free((*pBuffer));
        return(0);
     } //end-if
   } //end-else

   fclose(fp);
   return 1;
} //end-ReadPGMImage

///---------------------------------------------------------------------------------
/// Save a buffer as a .pgm image
///
void EDLineDetect::SaveImagePGM(char *filename, char *buffer, int width, int height){
  FILE *fp = fopen(filename, "wb");

  // .PGM header
  fprintf(fp, "P5\n");
  fprintf(fp, "# Some comment here!\n");
  fprintf(fp, "%d %d\n", width, height);
  fprintf(fp, "255\n");

  // Grayscale image
  fwrite(buffer, 1, width*height, fp);

  fclose( fp );
} //end-SaveImagePGM

LS *EDLineDetect::DetectEDLines(string filename, int *pNoLines)
  {
  int width, height;
  unsigned char *srcImg; 
  char *str = (char*)filename.c_str();

  if (ReadImagePGM(str, (char **)&srcImg, &width, &height) == 0){
    cout << "Failed opening " + filename << endl;
    return 0; 
  } //end-if

  printf("Working on %dx%d image\n", width, height);

  // EDLines Test below
  Timer timer;
  timer.Start();

  int noLines;


  //srcImg:
  //width:
  //height:
  //&noLines:
  LS *lines = DetectLinesByED(srcImg, width, height, &noLines);

 *pNoLines = noLines;
  timer.Stop();

  printf("<%d> line segments detected in <%4.2lf> ms\n", noLines, timer.ElapsedTime());

  // Dump the line segments to a file
  //if (noLines > 0){
  //  FILE *fp = fopen("LineSegments.txt", "w");

  //  fprintf(fp, "#Line segments are of the form: (sx, sy) (ex, ey)\n");
  //  for (int i=0; i<noLines; i++){
  //    fprintf(fp, "(%6.2lf %6.2lf)  (%6.2lf %6.2lf)\n", lines[i].sx, lines[i].sy, lines[i].ex, lines[i].ey);
  //  } //end-for

  //  fclose(fp);
  //} //end-for

  return lines;
  delete srcImg;
  }
}
