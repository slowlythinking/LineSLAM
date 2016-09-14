#ifndef _DRAWER_H_
#define _DRAWER_H_

#include "LineTracking.h"
#include "LS.h"
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
	    LS *tCurrentLine;
	    int tlineNum;
	    std::vector<LS> drawLine;
	    int MethodForLineDetect;
	    std::vector<cv::line_descriptor::KeyLine> dlines; 
	    std::mutex mMutex;
    };

}
#endif
