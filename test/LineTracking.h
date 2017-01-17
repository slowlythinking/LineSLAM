#ifndef _LINETRACKING_H_
#define _LINETRACKING_H_

#include <opencv2/opencv.hpp>
#include <opencv2/line_descriptor.hpp>
#include "opencv2/core/utility.hpp"
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/highgui.hpp>
#include <thread>
#include <cmath>
#include <iostream>
#include "Timer.h"
#include "Drawer.h"
#include "Map.h"
#include "MapDrawer.h"
//#include "LS.h"
#include "Initializer.h"
#include "Frame.h"
#include "InitializationAnalysis.h"

#define MATCHES_DIST_THRESHOLD 20


namespace LineSLAM
{
    class Drawer;

    class LineTracking
    {
	private:

	public:
	    LineTracking(int detectMethod, Drawer *ttFrameDrawer, Map *pMap, MapDrawer *pMapDrawer, string strSettingPath);

	    //Calibration matrix
	    cv::Mat mK;
	    cv::Mat mDistCoef;

	    //state
	    enum eTrackingState{
		SYSTEM_NOT_READY=-1,
		NO_IMAGES_YET=-2,
		NOT_INITIALIZED=-3,
		INITIALIZED=-4,
		OK=-5,
		LOST=-6
	    };
	    int mState = NO_IMAGES_YET;

	    //for initialization
	    vector<bool> vbTriangulated; // Triangulated Correspondences (mvIniMatches)
	    vector<cv::KeyPoint> mvKeys1; ///< 存储Reference Frame中的特征点
	    vector<cv::KeyPoint> mvKeys2; ///< 存储Current Frame中的特征点
	    vector<cv::Point3f> mvIniP3D;
	    vector<int> mvIniMatches;// 跟踪初始化时前两帧之间的匹配
	    int mLastProcessedState = NO_IMAGES_YET;
	    bool isRefFrame = true;
	    Frame mInitialFrame;
	    Frame mLastFrame;
	    Initializer* mpInitializer;
	    InitializationAnalysis* IniAnalysis;
	    std::vector<cv::DMatch> matches;
	    std::vector<cv::DMatch> good_matches;

	    //for all methods
	    Frame mCurrentFrame;
	    Frame lastFrame;
	    int methods;//which methods do we use to detect line.
	    cv::Mat currentImg;
	    cv::Mat lastImg;

	    int frameNum;
	    int currentLineNum;
	    int lastLineNum;
	    
	    //for EDLine method
//	    LS *currentLines;
//	    LS *lastLines;

	    //matching line segements
	    void matchingLine(); 

	    //Initialization
	    void monoInitialization();

	    //add initial point line and frame into map
	    void CreateInitialMap();

	    int Tracking(const cv::Mat &im, const double &timestamp, int &LineNum);
	protected:
	    Drawer* tFrameDrawer;
	    Map* mpMap;
	    MapDrawer* mpMapDrawer;
	    std::thread* InitialAyalyThread;

    };


}

#endif
