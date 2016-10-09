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
	lastImg = Mat(480,640,CV_8UC3, cv::Scalar(0,0,0));
	currentLineNum = 0;
	frameNum = 0;
	lastLineNum = 0;
	currentLines = nullptr;
	lastLines = nullptr;
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
	
//	lsd->detect(im, lines, 2, 1, mask );
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
	Ptr<BinaryDescriptorMatcher> bdm = BinaryDescriptorMatcher::createBinaryDescriptorMatcher();
	/* compute descriptors */
	bd->compute(currentImg, lines, currentDescrib);
	for ( int i = 0; i < (int) lines.size(); i++ )
	{
	    if( lines[i].octave == 0 )
	    {
		goodlines.push_back( lines[i] );
		goodcurrentDescrib.push_back( currentDescrib.row( i ) );
	    }
	}
	cout << "matching !!!!!!!!!!" << endl;
	if (frameNum > 1)
	{
	    good_matches.clear();
	    std::vector<DMatch> matches;
	   // cout << matches.size() << " sdfskldfjlskdjfskljfksd" << endl;
	    bdm->match(lastDescrib, goodcurrentDescrib, matches );

	    /* select best matches */
	    for ( int i = 0; i < (int) matches.size(); i++ )
	    {
		if( matches[i].distance < MATCHES_DIST_THRESHOLD )
		    good_matches.push_back( matches[i] );
	    }
	    std::cout << "------BDmatches number is: " << matches.size() << std::endl;
	    std::cout << "---------------------BDgoodMatches number is: " << good_matches.size() << std::endl;

	}

//	lastDesrcrib.swap(goodcurrentDescrib);
//	lastlines.swap(goodlines);
//	cout << "current mat rows: " << goodcurrentDescrib.rows << endl;
//	cout << "current mat cols: " << goodcurrentDescrib.cols << endl;
	lastDescrib.release();
	lastDescrib = goodcurrentDescrib;
	goodcurrentDescrib.release();
	lastlines = goodlines;
    }



    int LineTracking::Tracking(const cv::Mat &im, const double &timestamp)
    {
	if( im.data == NULL )
	{
	    std::cout << "Error, image could not be loaded. Please, check its path" << std::endl;
	    return 1;
	}
	frameNum++;
	currentImg = im.clone();
	//methods = detectMethod;

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
	if(methods == 0)
	    matchingLine();
	tFrameDrawer->updateDrawer(this);



	lines.clear();
	goodlines.clear();
	lastImg = currentImg;


	return currentLineNum;
    }








}
