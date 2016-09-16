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


LS *DetectLinesByED(unsigned char *srcImg, int width, int height, int *pNoLines);

namespace LineSLAM
{
    class Drawer;

    class LineTracking
    {
	private:

	public:
	    int methods;//which methods do we use to detect line.
	    cv::Mat currentImg;
	    LS *currentLines;
	    int currentLineNum;

	    std::vector<cv::line_descriptor::KeyLine> lines;

	    LineTracking(int i, Drawer *ttFrameDrawer);

	    void TrackInputImageLSD(const cv::Mat &im);
	    void TrackInputImageEDL(const cv::Mat &im);
	    int Tracking(const cv::Mat &im, const double &timestamp, int detectMethod);
	protected:
	    Drawer* tFrameDrawer;

    };


}

#endif
