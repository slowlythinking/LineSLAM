#include "Frame.h"
#include <thread>


namespace LineSLAM 
{
    bool Frame::mbInitialComputations=true;
    float Frame::cx, Frame::cy, Frame::fx, Frame::fy, Frame::invfx, Frame::invfy;
    float Frame::mnMinX, Frame::mnMinY, Frame::mnMaxX, Frame::mnMaxY;

    Frame::Frame()
    {}

    Frame::Frame(const Frame &frame)
	:img(frame.img.clone()), mTimeStamp(frame.mTimeStamp), lines(frame.lines), goodlines(frame.goodlines), lineDescrib(frame.lineDescrib.clone()), goodLineDescrib(frame.goodLineDescrib.clone()), lineNum(frame.lineNum), goodLineNum(frame.goodLineNum), mTcw(frame.mTcw.clone()), mK(frame.mK.clone()), mDistCoef(frame.mDistCoef.clone()) 
    {
	if(!frame.mTcw.empty())
	    SetPose(frame.mTcw);
    }

    Frame::Frame(const cv::Mat &imGray, const double &timeStamp, cv::Mat &K, cv::Mat &distCoef, int methods, int &trackLineNum)
	:mTimeStamp(timeStamp), mK(K.clone()), mDistCoef(distCoef.clone()) 
    {
	/* create a pointer to a BinaryDescriptor object with default parameters */
	/* compute descriptors */

	//detect line
	img = imGray.clone();
	Timer timer;
	timer.Start();
	if(methods == 0)
	    detectLSD(imGray);
	else 
	    detectEDL(imGray);
	
	timer.Stop();
	trackLineNum = lineNum;
	cout << "detected " << lineNum << "line segments in " << timer.ElapsedTime() << " ms" << endl;

	bd->compute(imGray, lines, lineDescrib);
	for ( int i = 0; i < (int) lines.size(); i++ )
	{
	    if( lines[i].octave == 0 )
	    {
		goodlines.push_back( lines[i] );
		goodLineDescrib.push_back( lineDescrib.row(i) );
	    }
	}
	goodLineNum = goodlines.size();
	if(mbInitialComputations)
	{
	    ComputeImageBounds(imGray);

	    fx = K.at<float>(0,0);
	    fy = K.at<float>(1,1);
	    cx = K.at<float>(0,2);
	    cy = K.at<float>(1,2);
	    invfx = 1.0f/fx;
	    invfy = 1.0f/fy;

	    mbInitialComputations=false;
	}


    }

    void Frame::detectLSD(const cv::Mat &im)
    {
	/* create a random binary mask */
	cv::Mat mask = Mat::ones(im.size(), CV_8UC1 );

	/* create a pointer to a BinaryDescriptor object with deafult parameters */
	Ptr<LSDDetector> lsd = LSDDetector::createLSDDetector();

	/* extract lines */
	lsd->detect(im, lines, 2, 1, mask );
	lineNum = lines.size();

    }

    void Frame::detectEDL(const cv::Mat &im)
    {
	/* create a random binary mask */
	cv::Mat mask = Mat::ones(im.size(), CV_8UC1 );

	/* create a pointer to a BinaryDescriptor object with deafult parameters */
	//Ptr<LSDDetector> lsd = LSDDetector::createLSDDetector();
	Ptr<BinaryDescriptor> bd = BinaryDescriptor::createBinaryDescriptor();

	/* extract lines */
	bd->detect( im, lines, mask );

	//lsd->detect(im, lines, 2, 1, mask );
	lineNum = lines.size();

    }

    cv::Mat Frame::GetK()
    {
	return mK.clone();
    }

    cv::Mat Frame::GetPose()
    {
	return mTcw.clone();
    }
//    void Frame::detectEDL(const cv::Mat& im)
//    {
//	if (im.type() != CV_8UC1) 
//	{
//	    cerr<<"error: callEDLines requires uchar image type\n";
//	    exit(0);
//	}
//
//	// image data copy 
//	uchar* p = new uchar[im.rows*im.cols];	
//	uchar* srcImg = p;	
//	for(int i=0; i<im.rows; ++i) {
//	    const uchar* pr = im.ptr<uchar>(i);
//	    for(int j=0; j<im.cols; ++j) {
//		*(p++) =  pr[j];
//	    }
//	}
//
//	int temp;
//	currentLines = DetectLinesByED(srcImg, im.cols, im.rows,&temp);
//	lineNum = temp;
//	delete[] srcImg;
//    }


    void Frame::ComputeImageBounds(const cv::Mat &imLeft)
    {
	if(mDistCoef.at<float>(0)!=0.0)
	{
	    // 矫正前四个边界点：(0,0) (cols,0) (0,rows) (cols,rows)
	    cv::Mat mat(4,2,CV_32F);
	    mat.at<float>(0,0)=0.0;         //左上
	    mat.at<float>(0,1)=0.0;
	    mat.at<float>(1,0)=imLeft.cols; //右上
	    mat.at<float>(1,1)=0.0;
	    mat.at<float>(2,0)=0.0;         //左下
	    mat.at<float>(2,1)=imLeft.rows;
	    mat.at<float>(3,0)=imLeft.cols; //右下
	    mat.at<float>(3,1)=imLeft.rows;

	    // Undistort corners
	    mat=mat.reshape(2);
	    cv::undistortPoints(mat,mat,mK,mDistCoef,cv::Mat(),mK);
	    mat=mat.reshape(1);

	    mnMinX = min(mat.at<float>(0,0),mat.at<float>(2,0));//左上和左下横坐标最小的
	    mnMaxX = max(mat.at<float>(1,0),mat.at<float>(3,0));//右上和右下横坐标最大的
	    mnMinY = min(mat.at<float>(0,1),mat.at<float>(1,1));//左上和右上纵坐标最小的
	    mnMaxY = max(mat.at<float>(2,1),mat.at<float>(3,1));//左下和右下纵坐标最小的
	}
	else
	{
	    mnMinX = 0.0f;
	    mnMaxX = imLeft.cols;
	    mnMinY = 0.0f;
	    mnMaxY = imLeft.rows;
	}
    }


    /**
     * @brief Set the camera pose.
     * 
     * 设置相机姿态，随后会调用 UpdatePoseMatrices() 来改变mRcw,mRwc等变量的值
     * @param Tcw Transformation from world to camera
     */
    void Frame::SetPose(cv::Mat Tcw)
    {
	mTcw = Tcw.clone();
	UpdatePoseMatrices();
    }

    /**
     * @brief Computes rotation, translation and camera center matrices from the camera pose.
     *
     * 根据Tcw计算mRcw、mtcw和mRwc、mOw
     */
    void Frame::UpdatePoseMatrices()
    {
	// [x_camera 1] = [R|t]*[x_world 1]，坐标为齐次形式
	// x_camera = R*x_world + t
	mRcw = mTcw.rowRange(0,3).colRange(0,3);
	mRwc = mRcw.t();
	mtcw = mTcw.rowRange(0,3).col(3);
	// mtcw, 即相机坐标系下相机坐标系到世界坐标系间的向量, 向量方向由相机坐标系指向世界坐标系
	// mOw, 即世界坐标系下世界坐标系到相机坐标系间的向量, 向量方向由世界坐标系指向相机坐标系
	mOw = -mRcw.t()*mtcw;
    }





}

