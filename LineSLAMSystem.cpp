#include "LineSLAMSystem.h"
#include <opencv2/line_descriptor.hpp>
#include "opencv2/core/utility.hpp"
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/highgui.hpp>
#include <thread>
#include <iostream>
#include "Timer.h"

using namespace cv;
using namespace cv::line_descriptor;
using namespace std;



namespace LineSLAM
{

    LineSLAMSystem::LineSLAMSystem(int i,  Drawer *ttFrameDrawer)
    {
	EDLine = new EDLineDetect(1);
	tFrameDrawer(ttFrameDrawer);

    }


    void LineSLAMSystem::InputImageLSD(const cv::Mat &im, const double &timestamp)
    {

	//	cv::imshow("images",im);
	//	cv::waitKey(1);
	if( im.data == NULL )
	{
	    std::cout << "Error, image could not be loaded. Please, check its path" << std::endl;
	    return;
	}

	currentImg = im.clone();
	/* create a random binary mask */
	cv::Mat mask = Mat::ones( im.size(), CV_8UC1 );

	/* create a pointer to a BinaryDescriptor object with deafult parameters */
	Ptr<LSDDetector> bd = LSDDetector::createLSDDetector();

	/* create a structure to store extracted lines */
	vector<KeyLine> lines;
	Timer timer;
	timer.Start();

	/* extract lines */
	bd->detect( im, lines, 2, 1, mask );
	timer.Stop();
	cout << "detected " << lines.size() << "line segments in " << timer.ElapsedTime() << " ms" << endl;

	/* draw lines extracted from octave 0 */

//	cv::Mat output = im.clone();
//	for ( size_t i = 0; i < lines.size(); i++ )
//	{
//	    KeyLine kl = lines[i];
//	    /* get extremes of line */
//	    Point pt1 = Point2f( kl.startPointX, kl.startPointY );
//	    Point pt2 = Point2f( kl.endPointX, kl.endPointY );
//
//	    /* draw line */
//	    //	line( output, pt1, pt2, Scalar( B, G, R ), 3 );
//	    line( output, pt1, pt2, Scalar( 255, 0, 0 ), 1 );
//
//	}
//
//	/* show lines on image */
//	imshow( "LSD lines", output );
//	waitKey();


    }

    void LineSLAMSystem::InputImageEDL(string file)
    {
	currentLineNum = 0;
	currentLines = EDLine->DetectEDLines(file,&currentLineNum);
      cout << "detected lines number is: " << currentLineNum << endl;
      currentImg = imread(file,CV_LOAD_IMAGE_COLOR);
      tFrameDrawer->updateDrawer(this);
//	for (int i=0; i<lineNum; i++)
//	{
//	    Point pt1 = Point2f(lines[i].sx, lines[i].sy);
//	    Point pt2 = Point2f(lines[i].ex, lines[i].ey);
//	    /* draw line */
//	    //	line( output, pt1, pt2, Scalar( B, G, R ), 3 );
//	    line( output, pt1, pt2, Scalar( 255, 0, 0 ), 1 );
//
//
//	} //end-for
//	imshow( "LSD lines", output );
//	waitKey();
    }
















}
