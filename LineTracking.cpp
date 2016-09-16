#include "LineTracking.h"


using namespace cv;
using namespace cv::line_descriptor;
using namespace std;



namespace LineSLAM
{

    LineTracking::LineTracking(int i,  Drawer *ttFrameDrawer):
	tFrameDrawer(ttFrameDrawer)

    {
	currentImg = Mat(480,640,CV_8UC3, cv::Scalar(0,0,0));
	currentLineNum = 0;
	currentLines = nullptr;
	methods = 0;
    }


    void LineTracking::TrackInputImageLSD(const cv::Mat &im)
    {
	/* create a random binary mask */
	cv::Mat mask = Mat::ones(im.size(), CV_8UC1 );

	/* create a pointer to a BinaryDescriptor object with deafult parameters */
	Ptr<LSDDetector> bd = LSDDetector::createLSDDetector();

	/* extract lines */
	bd->detect(im, lines, 2, 1, mask );
	currentLineNum = lines.size();
//	lines.clear();

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


    void LineTracking::TrackInputImageEDL(const cv::Mat& im_uchar)
    {
	if (im_uchar.type() != CV_8UC1) 
	{
	    cerr<<"error: callEDLines requires uchar image type\n";
	    exit(0);
	}

	// image data copy 
	uchar* p = new uchar[im_uchar.rows*im_uchar.cols];	
	uchar* srcImg = p;	
	for(int i=0; i<im_uchar.rows; ++i) {
	    const uchar* pr = im_uchar.ptr<uchar>(i);
	    for(int j=0; j<im_uchar.cols; ++j) {
		*(p++) =  pr[j];
	    }
	}

	int temp;
	currentLines = DetectLinesByED(srcImg, im_uchar.cols, im_uchar.rows,&temp);
	currentLineNum = temp;
	delete[] srcImg;

    }

//    void LineTracking::TrackInputImageEDL(const cv::Mat &im, const double &timestamp)
//    {
//	currentLineNum = 0;
//	currentLines = EDLine->DetectEDLines(file,&currentLineNum);
//      cout << "detected lines number is: " << currentLineNum << endl;
//      currentImg = imread(file,CV_LOAD_IMAGE_COLOR);
//      tFrameDrawer->updateDrawer(this);
//
////	for (int i=0; i<lineNum; i++)
////	{
////	    Point pt1 = Point2f(lines[i].sx, lines[i].sy);
////	    Point pt2 = Point2f(lines[i].ex, lines[i].ey);
////	    /* draw line */
////	    //	line( output, pt1, pt2, Scalar( B, G, R ), 3 );
////	    line( output, pt1, pt2, Scalar( 255, 0, 0 ), 1 );
////
////
////	} //end-for
////	imshow( "LSD lines", output );
////	waitKey();
//    }

    int LineTracking::Tracking(const cv::Mat &im, const double &timestamp, int detectMethod)
    {
	if( im.data == NULL )
	{
	    std::cout << "Error, image could not be loaded. Please, check its path" << std::endl;
	    return 1;
	}
	currentImg = im.clone();
	methods = detectMethod;

	//detect line
	Timer timer;
	timer.Start();
	if(detectMethod == 0)
	    TrackInputImageLSD(currentImg);
	else if(detectMethod == 1)
	    TrackInputImageEDL(currentImg);
	else
	{
	    cout << "wrong method parameter!" << endl;
	    return 1;
	}
	timer.Stop();
	cout << "detected " << currentLineNum << "line segments in " << timer.ElapsedTime() << " ms" << endl;

	//update framedrawer data
	tFrameDrawer->updateDrawer(this);
	lines.clear();

	return currentLineNum;
    }








}
