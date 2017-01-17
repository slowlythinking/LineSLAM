#ifndef _INITIALIZATIONANALYSIS_H_
#define _INITIALIZATIONANALYSIS_H_

#include "Map.h"
#include "Frame.h"
#include <opencv2/opencv.hpp>
#include <cmath>
#include <string>
#include <sstream>
#include <fstream>
#include <mutex>

namespace LineSLAM
{
class LineTracking;
    class InitializationAnalysis 
    {
	public:
	    Map* mpMap;

    InitializationAnalysis(Map* pMap, Frame CurFrame, Frame RefFrame);
	    int Run();
	    cv::Mat mergeRows(cv::Mat A, cv::Mat B);
	    cv::Mat mergeCols(cv::Mat A, cv::Mat B);

	protected:
	    cv::Mat currentImg;
	    cv::Mat refImg;

	    cv::Mat curTCW;
	    cv::Mat curmRcw;
	    cv::Mat curmTcw;
	    cv::Mat refTCW;

	    //     |fx  0   cx|
	    // K = |0   fy  cy|
	    //     |0   0   1 |
	    //本质矩阵
	    cv::Mat mK;
	    float fx, fy, cx, cy;

	    int SpatialLineNum;
	    //距离
	    double refTotalDistance;
	    double refMeanDistance;
	    double curTotalDistance;
	    double curMeanDistance;

	    Frame refFrame;
	    Frame curFrame;
	    std::mutex mMutex;
	    int mState;

	    std::vector<Point> matchPoints;
    };

}
#endif
