#ifndef _LINESLAMSYSTEM_H_
#define _LINESLAMSYSTEM_H_

#include <opencv2/opencv.hpp>
#include <thread>
#include "Viewer.h"
#include "Drawer.h"
#include "LineTracking.h"
#include "LS.h"
#include "Timer.h"



namespace LineSLAM
{

class LineSLAMSystem
{
    public:
	LineSLAMSystem(int LineDetectMethod);

	int InputImageLSD(const cv::Mat &im, const double &timestamp);
	int InputImageEDL(const cv::Mat &im, const double &timestamp);
	
	private:
	LineTracking* SyLineTracker;
	Drawer* SyDrawer;
	Viewer* SyViewer;
	std::thread* mptViewer;

};


}

#endif
