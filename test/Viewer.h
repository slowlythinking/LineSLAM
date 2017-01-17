#ifndef _VIEWER_H_
#define _VIEWER_H_

#include "Drawer.h"
#include "MapDrawer.h"
#include <opencv2/opencv.hpp>

namespace LineSLAM
{
    class Drawer;

    class Viewer
    {
	public:
	    Viewer(int i, string strSettingPath, Drawer *vFrameDrawer, MapDrawer *pMapDrawer);
	    void Run();

	    void RequestFinish();

	    void RequestStop();

	    bool isFinished();

	    bool isStopped();

	    void Release();


	private:
	    bool Stop();
	    // 1/fps in ms
	    double mT;
	    float mImageWidth, mImageHeight;

	    float mViewpointX, mViewpointY, mViewpointZ, mViewpointF;
	    bool CheckFinish();
	    void SetFinish();

	    bool mbFinishRequested;
	    bool mbFinished;
	    std::mutex mMutexFinish;

	    bool mbStopped;
	    bool mbStopRequested;
	    std::mutex mMutexStop;

	    cv::Mat im;
	    Drawer* vDrawer;
	    MapDrawer* mpMapDrawer;
    };
}

#endif
