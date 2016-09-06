#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <ctime>
#include <sys/time.h>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

const string StorePath= "./mydata/corridor-";
const int NumofImages = 1000;

int main()
{
    VideoCapture cap;
    cap.open(0);

    if(!cap.isOpened())
    {
	cout << "can not open camera!" << endl;
	return -1;
    }

    double rate = cap.get(CV_CAP_PROP_FPS);
    bool stop(false);
    Mat frame;
    int num = 0;
    timeval begin;
    gettimeofday(&begin, 0);
    ofstream CatchTime;
    CatchTime.open("CatchTime.txt");
    ostringstream ss;

    ss << num;
    string imageName = StorePath + ss.str() + ".png";
    CatchTime <<  "corridor-" << ss.str() << ".png" <<  " " << begin.tv_sec << "." << begin.tv_usec << endl;

    ss.str("");
    ss.clear();
    namedWindow("images");

    int delay = 1000/rate;
    while(!stop)
    {

	if (!cap.read(frame))
	    break;
	imshow("images",frame);
	imwrite(imageName, frame);

	num++;
	if(num > NumofImages)
	    break;
	timeval now;
	gettimeofday(&now, 0);

	ss << num ;
	imageName = StorePath + ss.str() + ".png";
	CatchTime << "corridor-" << ss.str() << ".png"  << " " << now.tv_sec << "." << now.tv_usec << endl;

	ss.str("");
	ss.clear();
	cout << "the " << num << " image" << endl;

	waitKey();

    }
    cap.release();
    CatchTime.close();
    
    return 0;
}

