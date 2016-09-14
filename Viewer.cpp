#include "Viewer.h"

namespace LineSLAM
{
    Viewer::Viewer(int i, Drawer *vFrameDrawer):
	vDrawer(vFrameDrawer)
    {    
        im = cv::Mat(480,640,CV_8UC3, cv::Scalar(0,0,0));
    }

    void Viewer::Run()
    {
	cv::namedWindow("LineSLAM: Current Frame");
	while(1)
	{
	    cv::Mat im = vDrawer->Draw();
	    cv::imshow("LineSLAM: Current Frame",im);
	    cv::waitKey(10);
	}
    }
}

