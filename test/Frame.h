#ifndef FRAME_H
#define FRAME_H

#include <opencv2/line_descriptor.hpp>
#include "opencv2/core/utility.hpp"
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/highgui.hpp>
#include <vector>
#include <opencv2/opencv.hpp>
#include "Timer.h"
//#include "LS.h"

using namespace cv;
using namespace cv::line_descriptor;
using namespace std;

//LS *DetectLinesByED(unsigned char *srcImg, int width, int height, int *pNoLines);

namespace LineSLAM 
{

    class Frame
    {
	public:
	    Frame();
	    // Copy constructor.
	    Frame(const Frame &frame);
	    Frame(const cv::Mat &imGray, const double &timeStamp, cv::Mat &K, cv::Mat &distCoef, int methods, int &trackLineNum);

	    // Set the camera pose.
	    // 用Tcw更新mTcw
	    void SetPose(cv::Mat Tcw);
	    cv::Mat GetPose();
	    cv::Mat GetK();

	    // Computes rotation, translation and camera center matrices from the camera pose.
	    void UpdatePoseMatrices();


	    // Frame timestamp.
	    double mTimeStamp;

	    // Calibration matrix and OpenCV distortion parameters.
	    static bool mbInitialComputations;
	    cv::Mat mK;
	    static float fx;
	    static float fy;
	    static float cx;
	    static float cy;
	    static float invfx;
	    static float invfy;
	    cv::Mat mDistCoef;
	    // Undistorted Image Bounds (computed once).
	    // 用于确定画格子时的边界
	    static float mnMinX;
	    static float mnMaxX;
	    static float mnMinY;
	    static float mnMaxY;


	    // Far points are inserted as in the monocular case from 2 views.
	    cv::Mat img;

	    // Number of KeyPoints.
	    int Num; ///< KeyLines数量

	    // MSLD descriptor, each row associated to a keyline.
	    // 线特征对应的描述子
	    cv::Mat lineDescrib;
	    cv::Mat goodLineDescrib;

	    //detect line segements
	    void detectLSD(const cv::Mat &im);
	    void detectEDL(const cv::Mat &im);

	    // 帧内提取出的线段
	    Ptr<BinaryDescriptor> bd = BinaryDescriptor::createBinaryDescriptor();
	    std::vector<cv::line_descriptor::KeyLine> lines;
	    std::vector<cv::line_descriptor::KeyLine> goodlines;
//	    LS *currentLines;
	    int lineNum;
	    int goodLineNum;

	    //线段对应的点
	    vector<cv::Point2f> points;

	    // Flag to identify outlier associations.
	    // 观测不到Map中的3D点
	    std::vector<bool> mvbOutlier;

	    // Camera pose.
	    cv::Mat mTcw; ///< 相机姿态 世界坐标系到相机坐标坐标系的变换矩阵

	    // Current and Next Frame id.
	    static long unsigned int nNextId; ///< Next Frame id.
	    long unsigned int mnId; ///< Current Frame id.

	    // Reference Keyframe.
//	    KeyFrame* mpReferenceKF;//指针，指向参考关键帧

	private:
	    // Rotation, translation and camera center
	    cv::Mat mRcw; ///< Rotation from world to camera
	    cv::Mat mtcw; ///< Translation from world to camera
	    cv::Mat mRwc; ///< Rotation from camera to world
	    cv::Mat mOw;  ///< mtwc,Translation from camera to world

	    // Computes image bounds for the undistorted image (called in the constructor).
	    void ComputeImageBounds(const cv::Mat &imLeft);

    };

}



#endif // FRAME_H
