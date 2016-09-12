#include "Viewer.h"

namespace LineSLAM
{
    Viewer::Viewer(int i)
    {}

    void run()
    {
	cv::namedWindow("LineSLAM: Current Frame");
	while(1)
	{
	    cv::Mat im = vDrawer->Draw();
	    cv::imshow("LineSLAM: Current Frame",im);
	    cv::waitKey();
	}
    }
}

