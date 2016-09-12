#include "Drawer.h"

namespace LineSLAM
{
    Drawer::Drawer(int i)
    {}

    cv::Mat Draw()
    {
	for (int i=0; i<tlineNum; i++)
	{
	    Point pt1 = Point2f(tCurrentLine[i].sx, tCurrentLine[i].sy);
	    Point pt2 = Point2f(tCurrentLine[i].ex, tCurrentLine[i].ey);
	    /* draw line */
	    //	line( output, pt1, pt2, Scalar( B, G, R ), 3 );
	    cv::line( drawMat, pt1, pt2, Scalar( 255, 0, 0 ), 1 );
	} //end-for
	return drawMat;
    }

    void updateDrawer(LineSLAMSystem *tLineSLAMSystem)
    {
//	{
//      unique_lock<mutex> lock(mMutex);
	int tLineNum = tLineSLAMSystem::currentLineNum;
	drawMat = tLineSLAMSystem::currentImg;
	tlineNum = tLineSLAMSystem::currentLineNum;
//	}
    }
}
