#include "Viewer.h"
#include <pangolin/pangolin.h>

namespace LineSLAM
{
    Viewer::Viewer(int i, string strSettingPath, Drawer *vFrameDrawer, MapDrawer *pMapDrawer):
	vDrawer(vFrameDrawer), mpMapDrawer(pMapDrawer), mbFinishRequested(false), mbFinished(true), mbStopped(false), mbStopRequested(false)

    {    
	im = cv::Mat(480,640,CV_8UC3, cv::Scalar(0,0,0));
	cv::FileStorage fSettings(strSettingPath, cv::FileStorage::READ);

	float fps = fSettings["Camera.fps"];
	if(fps<1)
	    fps=30;
	mT = 1e3/fps;

	mImageWidth = fSettings["Camera.width"];
	mImageHeight = fSettings["Camera.height"];
	if(mImageWidth<1 || mImageHeight<1)
	{
	    mImageWidth = 640;
	    mImageHeight = 480;
	}

	mViewpointX = fSettings["Viewer.ViewpointX"];
	mViewpointY = fSettings["Viewer.ViewpointY"];
	mViewpointZ = fSettings["Viewer.ViewpointZ"];
	mViewpointF = fSettings["Viewer.ViewpointF"];

    }

    void Viewer::Run()
    {
	cv::namedWindow("LineSLAM: Current Frame");
	std::cout << "start view thread!" << std::endl;


	mbFinished = false;

	pangolin::CreateWindowAndBind("LineSLAM: Map Viewer",1024,768);

	// 3D Mouse handler requires depth testing to be enabled
	glEnable(GL_DEPTH_TEST);

	// Issue specific OpenGl we might need
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	pangolin::CreatePanel("menu").SetBounds(0.0,1.0,0.0,pangolin::Attach::Pix(175));
//	pangolin::Var<bool> menuFollowCamera("menu.Follow Camera",true,true);
	pangolin::Var<bool> menuShowPoints("menu.Show Points",true,true);
	pangolin::Var<bool> menuShowLines("menu.Show Lines",true,true);
	pangolin::Var<bool> menuSaveImage("menu.SaveImage",false,false);
	//	pangolin::Var<bool> menuShowKeyFrames("menu.Show KeyFrames",true,true);
//	pangolin::Var<bool> menuShowGraph("menu.Show Graph",true,true);
//	pangolin::Var<bool> menuLocalizationMode("menu.Localization Mode",false,true);
//	pangolin::Var<bool> menuReset("menu.Reset",false,false);

	// Define Camera Render Object (for view / scene browsing)
	pangolin::OpenGlRenderState s_cam(
		pangolin::ProjectionMatrix(1024,768,mViewpointF,mViewpointF,512,389,0.1,1000),
		pangolin::ModelViewLookAt(mViewpointX,mViewpointY,mViewpointZ, 0,0,0,0.0,-1.0, 0.0)
		);

	// Add named OpenGL viewport to window and provide 3D Handler
	pangolin::View& d_cam = pangolin::CreateDisplay()
	    .SetBounds(0.0, 1.0, pangolin::Attach::Pix(175), 1.0, -1024.0f/768.0f)
	    .SetHandler(new pangolin::Handler3D(s_cam));

	pangolin::OpenGlMatrix Twc, initialTwc;
	Twc.SetIdentity();
	initialTwc.SetIdentity();

	bool bFollow = true;
	bool bLocalizationMode = false;


	while(1)
	{
	    cv::Mat im = vDrawer->Draw();
	    //	    if(im.empty())
	    //		std::cout << "显示空空空空空空空空空空空空空空空空空空空空空空空空!" << std::endl;

	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	    mpMapDrawer->GetCurrentOpenGLCameraMatrix(Twc);
//
//	    //跟踪相机
//	    if(menuFollowCamera && bFollow)
//	    {
//		s_cam.Follow(Twc);
//	    }
//	    else if(menuFollowCamera && !bFollow)
//	    {
//		s_cam.SetModelViewMatrix(pangolin::ModelViewLookAt(mViewpointX,mViewpointY,mViewpointZ, 0,0,0,0.0,-1.0, 0.0));
//		s_cam.Follow(Twc);
//		bFollow = true;
//	    }
//	    else if(!menuFollowCamera && bFollow)
//	    {
//		bFollow = false;
//	    }
//
//	    //重定位
//	    if(menuLocalizationMode && !bLocalizationMode)
//	    {
////		mpSystem->ActivateLocalizationMode();
//		bLocalizationMode = true;
//	    }
//	    else if(!menuLocalizationMode && bLocalizationMode)
//	    {
////		mpSystem->DeactivateLocalizationMode();
//		bLocalizationMode = false;
//	    }
//
	    d_cam.Activate(s_cam);
	    glClearColor(1.0f,1.0f,1.0f,1.0f);
	    //当前帧
	    mpMapDrawer->DrawCurrentCamera(Twc);
	    mpMapDrawer->DrawInitialCamera(initialTwc);
	    //	    //关键帧
//	    //	    if(menuShowKeyFrames || menuShowGraph)
//	    //		mpMapDrawer->DrawKeyFrames(menuShowKeyFrames,menuShowGraph);
	    //地图点
	    if(menuShowPoints)
		mpMapDrawer->DrawMapPoints();
	    if(menuShowLines)
		mpMapDrawer->DrawMapLines();
//
//	    //开始显示地图信息
	    pangolin::FinishFrame();
//
//	    //reset
//	    if(menuReset)
//	    {
//		menuShowGraph = true;
//		//		menuShowKeyFrames = true;
//		menuShowPoints = true;
//		menuLocalizationMode = false;
//
////		if(bLocalizationMode)
////		    mpSystem->DeactivateLocalizationMode();
//
//		bLocalizationMode = false;
//		bFollow = true;
//		menuFollowCamera = true;
//
////		mpSystem->Reset();
//
//		menuReset = false;
//	    }
//
//	    if(Stop())
//	    {
//		while(isStopped())
//		{
//		    //usleep(3000);
//		    std::this_thread::sleep_for(std::chrono::milliseconds(3));
//
//		}
//	    }
//
//	    if(CheckFinish())
//		break;




	    if(!im.empty())
	    {
		cv::imshow("LineSLAM: Current Frame",im);
		cv::waitKey(40);
	    }
	}
	SetFinish();


    }

    void Viewer::RequestFinish()
    {
	unique_lock<mutex> lock(mMutexFinish);
	mbFinishRequested = true;
    }

    bool Viewer::CheckFinish()
    {
	unique_lock<mutex> lock(mMutexFinish);
	return mbFinishRequested;
    }

    void Viewer::SetFinish()
    {
	unique_lock<mutex> lock(mMutexFinish);
	mbFinished = true;
    }

    bool Viewer::isFinished()
    {
	unique_lock<mutex> lock(mMutexFinish);
	return mbFinished;
    }

    void Viewer::RequestStop()
    {
	unique_lock<mutex> lock(mMutexStop);
	if(!mbStopped)
	    mbStopRequested = true;
    }

    bool Viewer::isStopped()
    {
	unique_lock<mutex> lock(mMutexStop);
	return mbStopped;
    }

    bool Viewer::Stop()
    {
	unique_lock<mutex> lock(mMutexStop);
	unique_lock<mutex> lock2(mMutexFinish);

	if(mbFinishRequested)
	    return false;
	else if(mbStopRequested)
	{
	    mbStopped = true;
	    mbStopRequested = false;
	    return true;
	}

	return false;

    }

    void Viewer::Release()
    {
	unique_lock<mutex> lock(mMutexStop);
	mbStopped = false;
    }



}

