#include "LineSLAMSystem.h"
#include <opencv2/line_descriptor.hpp>
#include "opencv2/core/utility.hpp"
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace cv::line_descriptor;
using namespace std;



namespace LineSLAM
{

    LineSLAMSystem::LineSLAMSystem(int i)
    {

    }


    void LineSLAMSystem::InputImage(const cv::Mat &im, const double &timestamp)
    {

	//	cv::imshow("images",im);
	//	cv::waitKey(1);
	if( im.data == NULL )
	{
	    std::cout << "Error, image could not be loaded. Please, check its path" << std::endl;
	    return;
	}

	/* create a random binary mask */
	cv::Mat mask = Mat::ones( im.size(), CV_8UC1 );

	/* create a pointer to a BinaryDescriptor object with deafult parameters */
	Ptr<LSDDetector> bd = LSDDetector::createLSDDetector();

	/* create a structure to store extracted lines */
	vector<KeyLine> lines;

	/* extract lines */
		cv::Mat output = im.clone();
	bd->detect( im, lines, 2, 1, mask );
	cout << "detected lines number is: " << lines.size() << endl;

	/* draw lines extracted from octave 0 */

	for ( size_t i = 0; i < lines.size(); i++ )
	{
	    KeyLine kl = lines[i];
	    /* get extremes of line */
	    Point pt1 = Point2f( kl.startPointX, kl.startPointY );
	    Point pt2 = Point2f( kl.endPointX, kl.endPointY );

	    /* draw line */
	    //	line( output, pt1, pt2, Scalar( B, G, R ), 3 );
	    line( output, pt1, pt2, Scalar( 255, 0, 0 ), 1 );

	}

	/* show lines on image */
	imshow( "LSD lines", output );
	waitKey();


    }


















}
