#include "Drawer.h"
#include <iostream>

using namespace cv;
using namespace cv::line_descriptor;
using namespace std;

namespace LineSLAM
{
    Drawer::Drawer(int i)
    {
    drawMat = Mat(480,640,CV_8UC3, cv::Scalar(0,0,0));
    tlineNum = 0;
    MethodForLineDetect = 0;
    }

    Mat Drawer::Draw()
    {
	unique_lock<mutex> lock(mMutex);

	//use EDLine
	if(MethodForLineDetect == 1)
	{
	    for (int i=0; i<tlineNum; i++)
	    {
		Point pt1 = Point2f(tCurrentLine[i].sx, tCurrentLine[i].sy);
		Point pt2 = Point2f(tCurrentLine[i].ex, tCurrentLine[i].ey);
		/* draw line */
		//	line( output, pt1, pt2, Scalar( B, G, R ), 3 );
		cv::line( drawMat, pt1, pt2, Scalar( 255, 0, 0 ), 1 );
		//	cout << "what's the problem!" << endl;
	    } //end-for
	}

	//use LSD 
	else if(MethodForLineDetect == 0)
	{
	    for ( int i = 0; i < tlineNum; i++)
	    {
	        KeyLine kl = dlines[i];
	        /* get extremes of line */
              Point pt1 = Point2f( kl.startPointX, kl.startPointY );
              Point pt2 = Point2f( kl.endPointX, kl.endPointY );

	        /* draw line */
	        //	line( output, pt1, pt2, Scalar( B, G, R ), 3 );
		cv::line( drawMat, pt1, pt2, Scalar( 255, 0, 0 ), 1 );

	    }

	}
	else
	    cout << "don't choose right line detector!" << endl;
	return drawMat;
    }

    void Drawer::updateDrawer(LineTracking *tLineTracking)
    {
	unique_lock<mutex> lock(mMutex);

	MethodForLineDetect = tLineTracking->methods;
	int tLineNum = tLineTracking->currentLineNum;
	cout << "更新Drawer！！！！！！！！！线段数目为：" << tLineNum << endl;
	if(tLineNum > 0)
	{
	    if(MethodForLineDetect == 1)
	    {
		drawMat = tLineTracking->currentImg;
		tlineNum = tLineTracking->currentLineNum;
		tCurrentLine = tLineTracking->currentLines;
	    }
	    else if(MethodForLineDetect == 0)
	    {
		drawMat = tLineTracking->currentImg;
		tlineNum = tLineTracking->currentLineNum;
		dlines = tLineTracking->lines;
	    }
	    else
		cout << "don't choose right line detector!" << endl;
	}
    }
}
