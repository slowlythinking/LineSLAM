#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <string>
#include <unistd.h>
#include<algorithm>
#include "LineSLAMSystem.h"
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;




int main(int argc, char **argv)
{
    if(argc != 2)
    {
	cout << "please choose a method to detect line!" << endl;
	cout << "0: LSD" << endl << "1:EDLine" << endl;
	return 1;
    }
    int LineDetectMethod = stoi(string(argv[1]));
 
    int ImageNumInTrue = 0;
    LineSLAM::LineSLAMSystem SLAM(1);

    // Vector for tracking time statistics
    vector<double> vTimesTrack;
    vector<int> vLinesNum;

    VideoCapture cap;
    cap.open(1);

    if(!cap.isOpened())
    {
	cout << "can not open camera!" << endl;
	return -1;
    }

    double rate = cap.get(CV_CAP_PROP_FPS);
    bool stop(false);
    
    // Main loop
    cv::Mat im;
    cv::Mat frame;
    double tframe = 1.1;

    
    while(!stop)
    {

	if (!cap.read(frame))
	    break;
	if(frame.empty())
	{
	    cerr << endl << "Failed to load image from camera" << endl;
	    break;
	}
	//cvtColor(frame, im, CV_8UC1);

	cout << "cv::type: " << im.type() << endl;
	frame.convertTo(im, CV_8UC1);
	chrono::steady_clock::time_point t1 = chrono::steady_clock::now();

	// Pass the image to the SLAM system
	cout << "now I'm pretended to process the " << ImageNumInTrue << "th image:)" << endl;
	if(LineDetectMethod == 0)
	    vLinesNum.push_back(SLAM.InputImageLSD(im,tframe));
	else if(LineDetectMethod == 1)
	    vLinesNum.push_back(SLAM.InputImageEDL(im,tframe));
	else
	{
	    cout << "please choose a method to detect line!" << endl;
	    cout << "0: LSD" << endl << "1:EDLine" << endl;
	    return 0;
	}



	chrono::steady_clock::time_point t2 = chrono::steady_clock::now();

	double ttrack= chrono::duration_cast<chrono::duration<double> >(t2 - t1).count();
	vTimesTrack.push_back(ttrack);


	// Wait to load the next frame
	ImageNumInTrue++;

	cout << "You know I've take " << ttrack*1e3 << " milliseconds for processing this image." << endl << endl;

    }
    

    // Tracking time statistics
    sort(vTimesTrack.begin(),vTimesTrack.end());
    sort(vLinesNum.begin(),vLinesNum.end());
    double totaltime = 0;
    unsigned long long totalline = 0;
    for(int ni=0; ni<ImageNumInTrue; ni++)
    {
	totaltime += vTimesTrack[ni];
	totalline += vLinesNum[ni];
    }
    cout << "-------" << endl << endl;
    cout << "total processing time is: " << totaltime*1e3 << endl;
    cout << "median tracking time(milliseconds): " << vTimesTrack[ImageNumInTrue/2]*1e3 << endl;
    cout << "mean tracking time(milliseconds): " << (totaltime/ImageNumInTrue)*1e3 << endl << endl;
    cout << "total line num is: " << totalline << endl;
    cout << "median detecting line number: " << vLinesNum[ImageNumInTrue/2] << endl;
    cout << "mean detecting line number: " << (totalline/ImageNumInTrue) << endl;


    return 0;
}



