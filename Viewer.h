#ifndef _VIEWER_H_
#define _VIEWER_H_

#include "Drawer.h"
#include <opencv2/opencv.hpp>

namespace LineSLAM
{
class Drawer;

    class Viewer
    {
	public:
	    Viewer(int i, Drawer *vFrameDrawer);
	    void Run();
	private:
	cv::Mat im;
	    Drawer* vDrawer;
    };
}

#endif
