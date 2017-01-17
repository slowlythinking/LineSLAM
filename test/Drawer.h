#ifndef _DRAWER_H_
#define _DRAWER_H_

#include "LineTracking.h"
//#include "LS.h"
#include "Frame.h"
#include <mutex>

namespace LineSLAM
{
class LineTracking;//如果没有这个，会报错not name the type

    class Drawer
    {
	public:
	    Drawer(int i);

	    cv::Mat Draw();
	    void updateDrawer(LineTracking *tLineTracking);

	protected:
	    cv::Mat drawMat;
	    cv::Mat currentImg;
	    cv::Mat lastImg;
//	    LS *tCurrentLine;
	    int tlineNum;
	    int frameNum;
//	    std::vector<LS> drawLine;
	    int MethodForLineDetect;
	    Frame mCurrentFrame;
	    Frame initialFrame;
	    std::vector<cv::DMatch> good_matches;
	    std::mutex mMutex;
	    int mState;
    };

}
#endif
