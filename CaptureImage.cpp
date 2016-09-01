#include <iostream>
#include <unistd.h>
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

int main()
{
    VideoCapture cap(0);
    usleep(2000);
    if(!cap.isOpened())
    {
	cout <<"can not open the camera!" << endl;
	return -1;
    }

    Mat frame;
    namedWindow("video");
    double rate = cap.get(CV_CAP_PROP_FPS);
    bool stop(false);
    int delay = 1000/rate;

    while(!stop)
    {
	if(!cap.read(frame))
	    break;

	imshow("video", frame);
    }
    return 0;
}
