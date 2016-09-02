#include <iostream>
#include <string>
#include <sstream>
#include <ctime>
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
    time_t begin = time(0);
    ostringstream ss;
    ss << num ;
    string imageName = StorePath + ss.str() + ".png";
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
    time_t now = time(0);
	ss << num ;
	imageName = StorePath + ss.str() + ".png";
	ss.str("");
	ss.clear();
	cout << imageName << endl;

	waitKey(20);

    }
    cap.release();
    return 0;
}

