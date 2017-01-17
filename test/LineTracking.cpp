#include "LineTracking.h"


using namespace cv;
using namespace cv::line_descriptor;
using namespace std;



namespace LineSLAM
{

    LineTracking::LineTracking(int detectMethod,  Drawer *ttFrameDrawer,Map *pMap, MapDrawer *pMapDrawer, string strSettingPath):
	tFrameDrawer(ttFrameDrawer), mpMap(pMap), mpMapDrawer(pMapDrawer)

    {
	//currentImg = Mat(480,640,CV_8UC3, cv::Scalar(0,0,0));
	//lastImg = Mat(480,640,CV_8UC3, cv::Scalar(0,0,0));
	currentLineNum = 0;
	frameNum = 0;
	lastLineNum = 0;
//	currentLines = nullptr;
//	lastLines = nullptr;
	methods = detectMethod;

	cv::FileStorage fSettings(strSettingPath, cv::FileStorage::READ);
	float fx = fSettings["Camera.fx"];
	float fy = fSettings["Camera.fy"];
	float cx = fSettings["Camera.cx"];
	float cy = fSettings["Camera.cy"];

	//     |fx  0   cx|
	// K = |0   fy  cy|
	//     |0   0   1 |
	//本质矩阵
	cv::Mat K = cv::Mat::eye(3,3,CV_32F);
	K.at<float>(0,0) = fx;
	K.at<float>(1,1) = fy;
	K.at<float>(0,2) = cx;
	K.at<float>(1,2) = cy;
	K.copyTo(mK);

	// 图像矫正系数
	// [k1 k2 p1 p2 k3]
	cv::Mat DistCoef(4,1,CV_32F);
	DistCoef.at<float>(0) = fSettings["Camera.k1"];
	DistCoef.at<float>(1) = fSettings["Camera.k2"];
	DistCoef.at<float>(2) = fSettings["Camera.p1"];
	DistCoef.at<float>(3) = fSettings["Camera.p2"];
	const float k3 = fSettings["Camera.k3"];
	if(k3!=0)
	{
	    DistCoef.resize(5);
	    DistCoef.at<float>(4) = k3;
	}
	DistCoef.copyTo(mDistCoef);
    }

    void LineTracking::monoInitialization()
    {
	// 单目初始化器还没有被初始化（当前帧的特征线段小于100）
	if(!mpInitializer)
	{
	    // Set Reference Frame
	    // 选择单目初始化的第一帧
	    if(mCurrentFrame.goodlines.size()>30)
	    {
		// 当前帧作为初始帧，初始化需要两帧，这里选择了一帧
		mInitialFrame = Frame(mCurrentFrame);
		// 记录最近的一帧
		mLastFrame = Frame(mCurrentFrame);
		//将线段转化为点
		int pointNum1 = mCurrentFrame.goodlines.size();
		mvKeys1.clear();
		mvKeys1.reserve(2*pointNum1);
		for(int i = 0; i < pointNum1; i++)
		{
		    cv::KeyPoint linekp1(mCurrentFrame.goodlines[i].startPointX,mCurrentFrame.goodlines[i].startPointY,0.3);
		    cv::KeyPoint linekp2(mCurrentFrame.goodlines[i].endPointX,mCurrentFrame.goodlines[i].endPointY,0.3);
		    mvKeys1.push_back(linekp1);
		    mvKeys1.push_back(linekp2);
		}


		// 由当前帧构造初始器 sigma:1.0 iterations:200
		if(mpInitializer)
		    delete mpInitializer;
		std::cout << "初始化第一帧选定！" << std::endl;

		mpInitializer =  new Initializer(mCurrentFrame,mvKeys1,1.0,200);
		fill(mvIniMatches.begin(),mvIniMatches.end(),-1);

		return;
	    }
	}
	else
	{
	    // Try to initialize
	    // 选择单目初始化的第二帧
	    // 如果当前帧特征点太少，重新构造初始器
	    // 最终的表现是：只有用于初始化的两帧的特征点个数都大于100时，才能继续进行初始化过程
	    if((int)mCurrentFrame.goodlines.size()<=30)
	    {
		std::cout << "当前帧特征为：" << mCurrentFrame.goodlines.size() << "，太少，重新初始化" << std::endl;
		delete mpInitializer;
		mpInitializer = static_cast<Initializer*>(NULL);
//		fill(mvIniMatches.begin(),mvIniMatches.end(),-1);
		return;
	    }

	    // Find correspondences
	    // 在mInitialFrame与mCurrentFrame中找匹配的特征点对
	    // mvbPrevMatched为前一帧匹配上的特征点，存储了mInitialFrame中哪些点将进行接下来的匹配
	    // mvIniMatches存储mInitialFrame,mCurrentFrame之间匹配的特征点

	    //-----------------------------------------------------------
	    Ptr<BinaryDescriptorMatcher> bdm = BinaryDescriptorMatcher::createBinaryDescriptorMatcher();
	    matches.clear();
	    good_matches.clear();
	    // cout << matches.size() << " sdfskldfjlskdjfskljfksd" << endl;

	    //lastFrame:queryDescriptor
	    //CurrentFrame:trainDescriptor
	    //matches:queryldx(int) trainLdx(int)
	    bdm->match(mInitialFrame.goodLineDescrib, mCurrentFrame.goodLineDescrib, matches);

	    /* select best matches */
	    for ( int i = 0; i < (int) matches.size(); i++ )
	    {
		if( matches[i].distance < MATCHES_DIST_THRESHOLD )
		    good_matches.push_back(matches[i]);
	    }
	    std::cout << "------BDmatches number is: " << matches.size() << std::endl;
	    std::cout << "---------------------BDgoodMatches number is: " << good_matches.size() << std::endl;
	    //---------------------------------------------------------

	    // Check if there are enough correspondences
	    // 如果初始化的两帧之间的匹配点太少，重新初始化
	    int nmatches = good_matches.size();
    cout << "nmatches：" << nmatches << endl;
	    mvIniMatches.reserve(nmatches);
	    fill(mvIniMatches.begin(),mvIniMatches.end(),1);
    cout << "chushihuashi匹配点对数目：" << mvIniMatches.size() << endl;
	    if(nmatches < 25)
	    {
		std::cout << "两帧之间的匹配对为：" << nmatches << "，太少,重新初始化" << std::endl;
		delete mpInitializer;
		mpInitializer = static_cast<Initializer*>(NULL);
		return;
	    }

	    cv::Mat Rcw; // Current Camera Rotation
	    cv::Mat tcw; // Current Camera Translation
	    //把线段端点转化为点vector
	    int pointNum2 = mCurrentFrame.goodlines.size();
	    mvKeys2.clear();
	    mvKeys2.reserve(2*pointNum2);
	    for(int i = 0; i < pointNum2; i++)
	    {
		cv::KeyPoint linekp1(mCurrentFrame.goodlines[i].startPointX,mCurrentFrame.goodlines[i].startPointY,0.3);
		cv::KeyPoint linekp2(mCurrentFrame.goodlines[i].endPointX,mCurrentFrame.goodlines[i].endPointY,0.3);
		mvKeys2.push_back(linekp1);
		mvKeys2.push_back(linekp2);
	    }


	    if(mpInitializer->Initialize(mCurrentFrame, mvKeys2, good_matches, Rcw, tcw, mvIniP3D, vbTriangulated))
	    {
		cout << "初始化成功！" << endl;
		cout << "R:" << endl;
		cout << Rcw << endl;
		cout << "T:" << endl;
		cout << tcw << endl;
		for(size_t i=0, iend=mvIniMatches.size(); i<iend;i++)
		{
		    // 如果初始化成功，删除那些无法进行三角化的匹配点
		    if(mvIniMatches[i]>=0 && !vbTriangulated[i])
		    {
			mvIniMatches[i]=-1;
			nmatches--;
		    }
		}

		// Set Frame Poses
		// 将初始化的第一帧作为世界坐标系，对应的变换矩阵为单位矩阵
		mInitialFrame.SetPose(cv::Mat::eye(4,4,CV_32F));
		// 由Rcw和tcw构造Tcw,并赋值给mTcw，mTcw为世界坐标系到该帧的变换矩阵
		cv::Mat Tcw = cv::Mat::eye(4,4,CV_32F);
		Rcw.copyTo(Tcw.rowRange(0,3).colRange(0,3));
		tcw.copyTo(Tcw.rowRange(0,3).col(3));
		mCurrentFrame.SetPose(Tcw);

		mState = OK;

		// Initialize函数会得到mvIniP3D，
		// mvIniP3D是cv::Point3f类型的一个容器，是个存放3D点的临时变量，
		// CreateInitialMapMonocular将3D点包装成MapPoint类型存入KeyFrame和Map中
		CreateInitialMap();
	    }
	}
	cout << "初始化过程结束" << endl;
    }

void LineTracking::CreateInitialMap()
{
    // Create KeyFrames
    //    KeyFrame* pKFini = new KeyFrame(mInitialFrame,mpMap,mpKeyFrameDB);
    //    KeyFrame* pKFcur = new KeyFrame(mCurrentFrame,mpMap,mpKeyFrameDB);

    // 将初始关键帧的描述子转为BoW
    //    pKFini->ComputeBoW();
    // 将当前关键帧的描述子转为BoW
    //    pKFcur->ComputeBoW();

    // Insert KFs in the map
    // 凡是关键帧，都要插入地图
    //    mpMap->AddKeyFrame(pKFini);
    //    mpMap->AddKeyFrame(pKFcur);

    // Create MapPoints and asscoiate to keyframes
    // 这部分和SteroInitialization()相似
    cout << "开始处理地图数据" << endl;
    cout << "计算出的可能三维点数目：" << mvIniP3D.size() << endl;
    cout << "vbTriangulated：" << vbTriangulated.size() << endl;
    cout << "good_matches.size:"<< good_matches.size() << std::endl;
    cout << "refFrame.goodlines:"<< mInitialFrame.goodlines.size() << std::endl;
    cout << "curFrame.goodlines:"<< mCurrentFrame.goodlines.size() << std::endl;
    for(size_t i=0,j=0; i<mvIniP3D.size();i++)
    {
	//        if(mvIniMatches[i]<0)
	//            continue;
	if(!vbTriangulated[i])
	    continue;

	//Create MapPoint.
	cv::Mat worldPos(mvIniP3D[i]);

	MapPoint* pMP = new MapPoint(worldPos, mpMap);

	//        pKFini->AddMapPoint(pMP,i);
	//        pKFcur->AddMapPoint(pMP,mvIniMatches[i]);

	//        pMP->AddObservation(pKFini,i);
	//        pMP->AddObservation(pKFcur,mvIniMatches[i]);
	//
	//        pMP->ComputeDistinctiveDescriptors();
	//        pMP->UpdateNormalAndDepth();

	//	  Fill Current Frame structure
	//        mCurrentFrame.mvpMapPoints[mvIniMatches[i]] = pMP;
	//        mCurrentFrame.mvbOutlier[mvIniMatches[i]] = false;

	//Add to Map
	mpMap->AddMapPoint(pMP);
	if (i == j+1 && j%2 == 0)
	{
	    cout << "地图加线,index,i: " << i << "j: " << j << endl;
	    cv::Mat worldPos1(mvIniP3D[j]);
	    cv::Mat worldPos2(mvIniP3D[i]);
	    int lineMatchIndex = j/2;
//	    int refIndex = good_matches[lineMatchIndex].queryIdx;
//	    int curIndex = good_matches[lineMatchIndex].trainIdx;
	    int refIndex = matches[lineMatchIndex].queryIdx;
	    int curIndex = matches[lineMatchIndex].trainIdx;
//	    std::cout << "refIndex:"<< refIndex << std::endl;
//	    std::cout << "curIndex:"<< curIndex << std::endl;

	    MapLine* pML = new MapLine(worldPos1, worldPos2, refIndex, curIndex, mpMap);
	    mpMap->AddMapLine(pML);
	}
	j = i;
	cout << "地图加点,index: " << i << endl;

    }
    IniAnalysis = new InitializationAnalysis(mpMap, mCurrentFrame, mInitialFrame);
    IniAnalysis->Run();
    //		InitialAyalyThread = new thread(&InitializationAnalysis::Run, IniAnalysis); 

    // Update Connections
    // 在3D点和关键帧之间建立边，每个边有一个权重，边的权重是该关键帧与当前帧公共3D点的个数
    //    pKFini->UpdateConnections();
    //    pKFcur->UpdateConnections();
    //
    //    // Bundle Adjustment
    //    cout << "New Map created with " << mpMap->MapPointsInMap() << " points" << endl;
    //
    //    Optimizer::GlobalBundleAdjustemnt(mpMap,20);
    //
    //    // Set median depth to 1
    //    // 评估关键帧场景深度，q=2表示中值
    //    float medianDepth = pKFini->ComputeSceneMedianDepth(2);
    //    float invMedianDepth = 1.0f/medianDepth;
    //
    //    if(medianDepth<0 || pKFcur->TrackedMapPoints(1)<100)
    //    {
    //        cout << "Wrong initialization, reseting..." << endl;
    //        Reset();
    //        return;
    //    }
    //
    //    // Scale initial baseline
    //    cv::Mat Tc2w = pKFcur->GetPose();
    //    // x/z y/z 将z归一化到1 
    //    Tc2w.col(3).rowRange(0,3) = Tc2w.col(3).rowRange(0,3)*invMedianDepth;
    //    pKFcur->SetPose(Tc2w);
    //
    //    // Scale points
    //    // 把3D点的尺度也归一化到1
    //    vector<MapPoint*> vpAllMapPoints = pKFini->GetMapPointMatches();
    //    for(size_t iMP=0; iMP<vpAllMapPoints.size(); iMP++)
    //    {
    //        if(vpAllMapPoints[iMP])
    //        {
    //            MapPoint* pMP = vpAllMapPoints[iMP];
    //            pMP->SetWorldPos(pMP->GetWorldPos()*invMedianDepth);
    //        }
    //    }
    //
    //    // 这部分和SteroInitialization()相似
    //    mpLocalMapper->InsertKeyFrame(pKFini);
    //    mpLocalMapper->InsertKeyFrame(pKFcur);
    //
    //    mCurrentFrame.SetPose(pKFcur->GetPose());
    //    mnLastKeyFrameId=mCurrentFrame.mnId;
    //    mpLastKeyFrame = pKFcur;
    //
    //    mvpLocalKeyFrames.push_back(pKFcur);
    //    mvpLocalKeyFrames.push_back(pKFini);
    //    mvpLocalMapPoints=mpMap->GetAllMapPoints();
    //    mpReferenceKF = pKFcur;
    //    mCurrentFrame.mpReferenceKF = pKFcur;
    //
    //    mLastFrame = Frame(mCurrentFrame);
    //
    //    mpMap->SetReferenceMapPoints(mvpLocalMapPoints);
    //

    //下面的不对，原因是mCurrentFrame不是指针，是类，需要用点来访问
    //如果是Frame *mCurrentFrame,就需要用->来访问
    //      mpMapDrawer->SetCurrentCameraPose(mCurrentFrame->GetPose());
    mpMapDrawer->SetCurrentCameraPose(mCurrentFrame.GetPose());
    //
    //    mpMap->mvpKeyFrameOrigins.push_back(pKFini);

    mState=INITIALIZED;//初始化成功，至此，初始化过程完成
}

    int LineTracking::Tracking(const cv::Mat &im, const double &timestamp, int &LineNum)
    {
	if( im.data == NULL )
	{
	    std::cout << "Error, image could not be loaded. Please, check its path" << std::endl;
	    return 1;
	}
//	mCurrentFrame = Frame(im, timestamp, mK, mDistCoef, methods, LineNum); 
	frameNum++;
	//methods = detectMethod;

	if(mState==NO_IMAGES_YET)
	{
	    mState = NOT_INITIALIZED;
	}

	//update framedrawer data
	if(mState == NOT_INITIALIZED)
	{
	    mCurrentFrame = Frame(im, timestamp, mK, mDistCoef, methods, LineNum); 
	    monoInitialization();
	    tFrameDrawer->updateDrawer(this);
	}
	if(mState == INITIALIZED)
	    return mState;
	return mState;
    }





}
