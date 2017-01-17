#include "LineSLAMSystem.h"

using namespace cv;
using namespace cv::line_descriptor;
using namespace std;



namespace LineSLAM
{

    LineSLAMSystem::LineSLAMSystem(int LineDetectMethod, string settingPath)

    {
	linenum = 0;


	mpMap = new Map();
	SyDrawer = new Drawer(1);
	mpMapDrawer = new MapDrawer(mpMap, settingPath);
	SyLineTracker = new LineTracking(LineDetectMethod,SyDrawer, mpMap, mpMapDrawer, settingPath);
	SyViewer = new Viewer(1, settingPath, SyDrawer, mpMapDrawer);
	mptViewer = new thread(&Viewer::Run, SyViewer); 
    }


    int LineSLAMSystem::InputImage(const cv::Mat &im, const double &timestamp, int &stateInitial)
    {

	SyState = SyLineTracker->Tracking(im, timestamp, linenum);
	stateInitial = SyState;
	return linenum;

    }

    int LineSLAMSystem::getSystemState()
    {
	unique_lock<mutex> lock(mSysState);
	return SyState;
    }


//    void LineSLAMSystem::InputImageEDL(string file)
//    {
//	SyLineTracker->TrackInputImageEDL(file);
//    }
















}
