#include <iostream>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;


int main()
{
    VideoCapture cap;
    cap.open(1);

    if(!cap.isOpened())
    {
	cout << "can not open camera!" << endl;
	return -1;
    }

    double rate = cap.get(CV_CAP_PROP_FPS);
    bool stop(false);
    Mat frame;
    namedWindow("images");

    int delay = 1000/rate;
    while(!stop)
    {
	if (!cap.read(frame))
	    break;
	imshow("images",frame);
	waitKey(20);
    }
    cap.release();
    return 0;
}
    
