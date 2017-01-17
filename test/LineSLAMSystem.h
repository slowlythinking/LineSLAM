#ifndef _LINESLAMSYSTEM_H_
#define _LINESLAMSYSTEM_H_

#include <opencv2/opencv.hpp>
#include <thread>
#include "Viewer.h"
#include "Drawer.h"
#include "LineTracking.h"
//#include "LS.h"
#include "Timer.h"
#include "Map.h"
#include "MapDrawer.h"

#include <mutex>


namespace LineSLAM
{

class LineSLAMSystem
{
    public:
	LineSLAMSystem(int LineDetectMethod, string settingPath);

	int InputImage(const cv::Mat &im, const double &timestamp, int &stateInitial);

	int getSystemState();

	enum eTrackingState{
		SYSTEM_NOT_READY=-1,
		NO_IMAGES_YET=-2,
		NOT_INITIALIZED=-3,
		INITIALIZED=-4,
		OK=-5,
		LOST=-6
	    };

	int SyState = NO_IMAGES_YET;

	private:
	LineTracking* SyLineTracker;
	Drawer* SyDrawer;
	Viewer* SyViewer;
	std::thread* mptViewer;
	Map* mpMap;
	MapDrawer* mpMapDrawer;
	int linenum;//当前帧中的线特征数目
	std::mutex mSysState;

};


}

#endif
