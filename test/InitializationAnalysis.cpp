#include "InitializationAnalysis.h"


namespace LineSLAM
{
    InitializationAnalysis::InitializationAnalysis(Map* pMap, Frame CurFrame, Frame RefFrame):mpMap(pMap)
    {
	curFrame = CurFrame;
	refFrame = RefFrame;
	currentImg = CurFrame.img;
	refImg = refFrame.img;
	curTCW = curFrame.GetPose();
	curmRcw = curTCW.rowRange(0,3).colRange(0,3);
	curmTcw = curTCW.rowRange(0,3).col(3);

	refTCW = refFrame.GetPose();
	mK = curFrame.GetK();
	fx = mK.at<float>(0,0);
	fy = mK.at<float>(1,1);
	cx = mK.at<float>(0,2);
	cy = mK.at<float>(1,2);
	refTotalDistance = 0.0;
	refMeanDistance = 0.0;
	curTotalDistance = 0.0;
	curMeanDistance = 0.0;
	//	imshow( "ReferenceImage", refImg);
	//	waitKey();
	//	imshow( "CurrentImage", currentImg);
	//	waitKey(2);
	//std::cout << "初始化分析类构造完成" << std::endl;
    }


    int InitializationAnalysis::Run()
    {
	std::cout << "初始化分析开始" << std::endl;
	vector<MapLine*> vpMLs = mpMap->GetAllMapLines();
	std::cout << "从地图获取数据" << std::endl;
	std::cout << "vpMLs.size:"<< vpMLs.size() << std::endl;
	std::cout << "refFrame.goodlines:"<< refFrame.goodlines.size() << std::endl;
	std::cout << "curFrame.goodlines:"<< curFrame.goodlines.size() << std::endl;
	SpatialLineNum = vpMLs.size();
	//写入文件
	time_t nowtime = time(NULL);
	tm* local;
	char buf[128]={0};
	local = localtime(&nowtime); //转为本地时间
	strftime(buf, 64, "%Y-%m-%d %H:%M:%S", local);
	string timestring = buf;
	ofstream outputfile;
	outputfile.open("../match/RealDATA/RealAnalysisResult.txt",ios::app);
	if(!outputfile)
	{
	    std::cout << "can't open file!" << std::endl;
	    return 1;
	}
	outputfile << "初始化实验分析，实验时间: " << ctime(&nowtime) << std::endl;
	outputfile << "对应图像: " << timestring << ".png" << std::endl;
	outputfile << "三维线段数目：" << SpatialLineNum << std::endl;
	for(size_t i=0, iend=vpMLs.size(); i<iend;i++)
	{
	    std::cout << "i:"<< i << std::endl;
	    outputfile << "三维线段" << i << ": ";

	    int refIndex = vpMLs[i]->refFrameLineIndex;
	    int curIndex = vpMLs[i]->curFrameLineIndex;
	    cv::Mat point3d1 = vpMLs[i]->GetWorldPos1();
	    cv::Mat point3d2 = vpMLs[i]->GetWorldPos2();

	    //	    std::cout << "refIndex:"<< refIndex << std::endl;
	    //参考帧中对应的直线端点
	    KeyLine refLine = refFrame.goodlines[refIndex];
	    Point refpt1 = Point2f(refLine.startPointX, refLine.startPointY);
	    Point refpt2 = Point2f(refLine.endPointX, refLine.endPointY );
	    cv::line( refImg, refpt1, refpt2, Scalar( 250, 0, 0 ), 1 );


	    //	    std::cout << "curIndex:"<< curIndex << std::endl;
	    //当前帧中对应的直线端点
	    KeyLine curLine = curFrame.goodlines[curIndex];
	    Point curpt1 = Point2f(curLine.startPointX, curLine.startPointY);
	    Point curpt2 = Point2f(curLine.endPointX, curLine.endPointY );
	    Point curpt1c = Point2f(curLine.startPointX+refImg.cols, curLine.startPointY);
	    Point curpt2c = Point2f(curLine.endPointX+refImg.cols, curLine.endPointY);
	    cv::line( currentImg, curpt1, curpt2, Scalar( 250, 0, 0 ), 1 );

	    //两帧对应直线端点之间的连线
	    matchPoints.push_back(refpt1);
	    matchPoints.push_back(curpt1c);
	    matchPoints.push_back(refpt2);
	    matchPoints.push_back(curpt2c);

	    //三维直线在当前帧和参考帧中的投影
	    //orb-SLAM frame.cpp 306l
	    //当前帧
	    cv::Mat curPoint3d1 = curmRcw*point3d1+curmTcw;
	    cv::Mat curPoint3d2 = curmRcw*point3d2+curmTcw;
	    float PcX1 = curPoint3d1.at<float>(0);
	    float PcY1 = curPoint3d1.at<float>(1);
	    float PcZ1 = curPoint3d1.at<float>(2);
	    float PcX2 = curPoint3d2.at<float>(0);
	    float PcY2 = curPoint3d2.at<float>(1);
	    float PcZ2 = curPoint3d2.at<float>(2);
	    //Check positive depth
	    if(PcZ1<0.0f || PcZ2<0.0f)
		std::cout << "error: point depth is incorrect." << std::endl;
	    //将MapPoint投影到当前帧, 
	    float invz1 = 1.0f/PcZ1;
	    float u1=fx*PcX1*invz1+cx;
	    float v1=fy*PcY1*invz1+cy;
	    float invz2 = 1.0f/PcZ2;
	    float u2=fx*PcX2*invz2+cx;
	    float v2=fy*PcY2*invz2+cy;
	    Point curptTCW1 = Point2f(u1, v1);
	    Point curptTCW2 = Point2f(u2, v2);
	    cv::line( currentImg, curptTCW1, curptTCW2, Scalar(0, 0, 250), 1);

	    //计算当前帧投影线段与原线段的距离
	    // 点(x0,y0)到直线Ax+By+C=0的距离为d = (A*x0+B*y0+C)/sqrt(A^2+B^2)
	    // 化简两点式为一般式
	    // 两点式公式为(y - y1)/(x - x1) = (y2 - y1)/ (x2 - x1)
	    // 化简为一般式为(y2 - y1)x + (x1 - x2)y + (x2y1 - x1y2) = 0
	    // A = y2 - y1
	    // B = x1 - x2
	    // C = x2y1 - x1y2
	    //原线段所在的直线
	    double A,B,C,dis;
	    A = curpt2.y - curpt1.y;
	    B = curpt1.x - curpt2.x;
	    C = curpt2.x * curpt1.y - curpt1.x * curpt2.y;
	    //距离公式为d = |A*x0 + B*y0 + C|/√(A^2 + B^2)
	    dis = abs(A * curptTCW1.x + B * curptTCW1.y + C) / sqrt(A * A + B * B);
	    dis += abs(A * curptTCW2.x + B * curptTCW2.y + C) / sqrt(A * A + B * B);
	    curTotalDistance += dis;
	    std::cout << "Distance in current frame: " << dis << std::endl;
	    outputfile << "当前帧中误差距离:" << dis << ",";


	    //参考帧
	    PcX1 = point3d1.at<float>(0);
	    PcY1 = point3d1.at<float>(1);
	    PcZ1 = point3d1.at<float>(2);
	    PcX2 = point3d2.at<float>(0);
	    PcY2 = point3d2.at<float>(1);
	    PcZ2 = point3d2.at<float>(2);
	    //将MapPoint投影到参考帧, 
	    invz1 = 1.0f/PcZ1;
	    u1=fx*PcX1*invz1+cx;
	    v1=fy*PcY1*invz1+cy;
	    invz2 = 1.0f/PcZ2;
	    u2=fx*PcX2*invz2+cx;
	    v2=fy*PcY2*invz2+cy;
	    Point refptTCW1 = Point2f(u1, v1);
	    Point refptTCW2 = Point2f(u2, v2);
	    cv::line( refImg, refptTCW1, refptTCW2, Scalar(0, 0, 250), 1);

	    //计算参考帧投影线段与原线段的距离
	    A = refpt2.y - refpt1.y;
	    B = refpt1.x - refpt2.x;
	    C = refpt2.x * refpt1.y - refpt1.x * refpt2.y;
	    //距离公式为d = |A*x0 + B*y0 + C|/√(A^2 + B^2)
	    dis = abs(A * refptTCW1.x + B * refptTCW1.y + C) / sqrt(A * A + B * B);
	    dis += abs(A * refptTCW2.x + B * refptTCW2.y + C) / sqrt(A * A + B * B);
	    refTotalDistance += dis;
	    std::cout << "Distance in reference frame: " << dis << std::endl;
	    outputfile << "参考帧中误差距离" << dis << ";" << std::endl;
	}
	curMeanDistance = curTotalDistance / SpatialLineNum;
	refMeanDistance = refTotalDistance / SpatialLineNum;
	std::cout << "Distance in current frame is: " << curTotalDistance << std::endl;
	std::cout << "Mean Distance in current frame is: " << curMeanDistance << std::endl;
	std::cout << "Distance in reference frame is: " << refTotalDistance << std::endl;
	std::cout << "Mean Distance in reference frame is: " << refMeanDistance << std::endl;

	outputfile << std::endl << "在当前帧中总的误差距离: " << curTotalDistance << ",平均误差距离: " << curMeanDistance << std::endl;
	outputfile << "在参考帧中总的误差距离: " << refTotalDistance << ",平均误差距离: " << refMeanDistance << std::endl << std::endl;
	outputfile.close();
	std::cout << "Finish writing txt file!" << std::endl;

	cv::Mat mergeImg = mergeCols(refImg, currentImg);
	for(size_t i=0, iend=matchPoints.size(); i<iend;i+=2)
	{
	    cv::line( mergeImg, matchPoints[i], matchPoints[i+1], Scalar(0,100, 0), 1 );

	}

	imwrite("/home/weixinyu/LearnAndWork/gaoyanyuan/ORB_SLAM/MySLAM/test/match/RealDATA/InitializationAnalysis1-" + timestring + ".jpg", mergeImg);
	imwrite("/home/weixinyu/LearnAndWork/gaoyanyuan/ORB_SLAM/MySLAM/test/match/RealDATA/RefenrenceImage1-" + timestring + ".jpg", refImg);
	imwrite("/home/weixinyu/LearnAndWork/gaoyanyuan/ORB_SLAM/MySLAM/test/match/RealDATA/CurrentImage1-" + timestring + ".jpg", currentImg);
	//	if(!refImg.empty() || !currentImg.empty())
	//	{
	//	    imshow( "MergeImage", mergeImg);
	//	       waitKey();
	//	    imshow( "ReferenceImage", refImg);
	//	       waitKey();
	//	        imshow( "CurrentImage", currentImg);
	//	       waitKey(40);
	//	}
	std::cout << "Finish writing image file!" << std::endl;
	return 0;
    }


    cv::Mat InitializationAnalysis::mergeRows(cv::Mat A, cv::Mat B)
    {
	// cv::CV_ASSERT(A.cols == B.cols&&A.type() == B.type());
	int totalRows = A.rows + B.rows;
	cv::Mat mergedDescriptors(totalRows, A.cols, A.type());
	cv::Mat submat = mergedDescriptors.rowRange(0, A.rows);
	A.copyTo(submat);
	submat = mergedDescriptors.rowRange(A.rows, totalRows);
	B.copyTo(submat);
	return mergedDescriptors;
    }
    cv::Mat InitializationAnalysis::mergeCols(cv::Mat A, cv::Mat B)
    {
	// cv::CV_ASSERT(A.cols == B.cols&&A.type() == B.type());
	int totalCols = A.cols + B.cols;
	cv::Mat mergedDescriptors(A.rows,totalCols, A.type());
	cv::Mat submat = mergedDescriptors.colRange(0, A.cols);
	A.copyTo(submat);
	submat = mergedDescriptors.colRange(A.cols, totalCols);
	B.copyTo(submat);
	return mergedDescriptors;
    }




}
