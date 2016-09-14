#include "LineSLAMSystem.h"

using namespace cv;
using namespace cv::line_descriptor;
using namespace std;



namespace LineSLAM
{

    LineSLAMSystem::LineSLAMSystem(int i)

    {
    
	SyDrawer = new Drawer(1);
	
	SyLineTracker = new LineTracking(1,SyDrawer);
	
	SyViewer = new Viewer(1,SyDrawer);
	mptViewer = new thread(&Viewer::Run, SyViewer); 
	

	
    }


    int LineSLAMSystem::InputImageLSD(const cv::Mat &im, const double &timestamp)
    {

	return SyLineTracker->TrackInputImageLSD(im, timestamp);

    }

    int LineSLAMSystem::InputImageEDL(const cv:: Mat &im, const double &timestamp)
    {
	return SyLineTracker->TrackInputImageEDL(im, timestamp);
    }

//    void LineSLAMSystem::InputImageEDL(string file)
//    {
//	SyLineTracker->TrackInputImageEDL(file);
//    }
















}
