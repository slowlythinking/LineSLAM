#include "LineTracking.h"


using namespace cv;
using namespace cv::line_descriptor;
using namespace std;



namespace LineSLAM
{

    LineTracking::LineTracking(int detectMethod,  Drawer *ttFrameDrawer):
	tFrameDrawer(ttFrameDrawer)

    {
	currentImg = Mat(480,640,CV_8UC3, cv::Scalar(0,0,0));
	currentLineNum = 0;
	currentLines = nullptr;
	methods = detectMethod;
    }


    void LineTracking::detectLSD(const cv::Mat &im)
    {
	/* create a random binary mask */
	cv::Mat mask = Mat::ones(im.size(), CV_8UC1 );

	/* create a pointer to a BinaryDescriptor object with deafult parameters */
//	Ptr<LSDDetector> lsd = LSDDetector::createLSDDetector();
	Ptr<BinaryDescriptor> bd = BinaryDescriptor::createBinaryDescriptor();

	/* extract lines */
	bd->detect( im, lines, mask );
	//lsd->detect(im, lines, 2, 1, mask );
	currentLineNum = lines.size();
	//lines.clear();

    }


    void LineTracking::detectEDL(const cv::Mat& im)
    {
	if (im.type() != CV_8UC1) 
	{
	    cerr<<"error: callEDLines requires uchar image type\n";
	    exit(0);
	}

	// image data copy 
	uchar* p = new uchar[im.rows*im.cols];	
	uchar* srcImg = p;	
	for(int i=0; i<im.rows; ++i) {
	    const uchar* pr = im.ptr<uchar>(i);
	    for(int j=0; j<im.cols; ++j) {
		*(p++) =  pr[j];
	    }
	}

	int temp;
	currentLines = DetectLinesByED(srcImg, im.cols, im.rows,&temp);
	currentLineNum = temp;
	delete[] srcImg;

    }

    void LineTracking::matchingLine() 
    {
	/* create a pointer to a BinaryDescriptor object with default parameters */
	Ptr<BinaryDescriptor> bd = BinaryDescriptor::createBinaryDescriptor();

	/* compute descriptors */
	bd->compute(currentImg, lines, currentDescrib);



    }



    int LineTracking::Tracking(const cv::Mat &im, const double &timestamp)
    {
	if( im.data == NULL )
	{
	    std::cout << "Error, image could not be loaded. Please, check its path" << std::endl;
	    return 1;
	}
	currentImg = im.clone();
//	methods = detectMethod;

	//detect line
	Timer timer;
	timer.Start();
	if(methods == 0)
	    detectLSD(currentImg);
	else if(methods == 1)
	    detectEDL(currentImg);
	else
	{
	    cout << "wrong method parameter!" << endl;
	    return 1;
	}
	timer.Stop();
	cout << "detected " << currentLineNum << "line segments in " << timer.ElapsedTime() << " ms" << endl;

	//update framedrawer data
	tFrameDrawer->updateDrawer(this);
//	matchingLine();



	lines.clear();


	return currentLineNum;
    }








}
