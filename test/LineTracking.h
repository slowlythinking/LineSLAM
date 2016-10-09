#ifndef _LINETRACKING_H_
#define _LINETRACKING_H_

#include <opencv2/opencv.hpp>
#include <opencv2/line_descriptor.hpp>
#include "opencv2/core/utility.hpp"
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/highgui.hpp>
#include <thread>
#include <iostream>
#include "Timer.h"
#include "Drawer.h"
#include "LS.h"

#define MATCHES_DIST_THRESHOLD 25

LS *DetectLinesByED(unsigned char *srcImg, int width, int height, int *pNoLines);

namespace LineSLAM
{
    class Drawer;

    class LineTracking
    {
	private:

	public:
	    LineTracking(int detectMethod, Drawer *ttFrameDrawer);

	    //for all methods
	    int methods;//which methods do we use to detect line.
	    cv::Mat currentImg;
	    cv::Mat lastImg;

	    cv::Mat currentDescrib;
	    cv::Mat goodcurrentDescrib;
	    cv::Mat lastDescrib;

	    std::vector<cv::DMatch> good_matches;

	    int frameNum;
	    int currentLineNum;
	    int lastLineNum;
	    
	    //for EDLine method
	    LS *currentLines;
	    LS *lastLines;

	    //for LSD method
	    std::vector<cv::line_descriptor::KeyLine> lines;
	    std::vector<cv::line_descriptor::KeyLine> goodlines;
	    std::vector<cv::line_descriptor::KeyLine> lastlines;

	    //detect line segements
	    void detectLSD(const cv::Mat &im);
	    void detectEDL(const cv::Mat &im);

	    //matching line segements
	    void matchingLine(); 

	    int Tracking(const cv::Mat &im, const double &timestamp);
	protected:
	    Drawer* tFrameDrawer;

    };


}

#endif
