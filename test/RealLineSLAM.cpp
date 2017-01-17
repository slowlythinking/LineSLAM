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

// Set the processing image number when input dataset.
const int ProcessingImagesNum = 1000;

void LoadImages(const string &strFile, vector<string> &vstrImageFilenames,
	vector<double> &vTimestamps, const int num);


int main(int argc, char **argv)
{
    if(argc != 3)
    {
	cout << "please choose a method to input image and detect line!" << endl;
	cout << "0: Dataset" << endl << "1:RealData" << endl;
	cout << "0: LSD" << endl << "1:EDLine" << endl;
	return 1;
    }
    int InputData = stoi(string(argv[1]));
    int LineDetectMethod = stoi(string(argv[2]));
    string settingPath = "/home/weixinyu/LearnAndWork/gaoyanyuan/ORB_SLAM/ORB_SLAM2/Examples/Monocular/TUM2.yaml";
 
    LineSLAM::LineSLAMSystem SLAM(LineDetectMethod, settingPath);
    int stateForInitial = 0;
    int ImageNumInTrue;

    // Vector for tracking time statistics
    vector<double> vTimesTrack;
    // Store lines number of each frame.
    vector<int> vLinesNum;

    if(InputData == 1)
    {
	ImageNumInTrue = 0;
	VideoCapture cap;
	cap.open(0);

	if(!cap.isOpened())
	{
	    cout << "can not open camera!" << endl;
	    return -1;
	}

	//    double rate = cap.get(CV_CAP_PROP_FPS);
	bool stop(false);

	// Main loop
	cv::Mat im;
	cv::Mat frame;
	double tframe = 1.1;
	while(!stop)
	{
	    if(stateForInitial != -4)
		//	if(1)
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
		frame.convertTo(im, CV_8UC3);
		chrono::steady_clock::time_point t1 = chrono::steady_clock::now();

		// Pass the image to the SLAM system
		cout << "now I'm pretended to process the " << ImageNumInTrue << "th image:)" << endl;
		if(LineDetectMethod == 0 || LineDetectMethod == 1)
		{
		    vLinesNum.push_back(SLAM.InputImage(im,tframe,stateForInitial));
		    cout << "in LineDetectAndMatch.cpp detect line number is: " << vLinesNum.back() << endl;
		}
		else
		{
		    cout << "please choose a correct method to detect line!" << endl;
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
	    else
	    {
		while(1)
		{
		    im = cv::Mat(480,640,CV_8UC3, cv::Scalar(0,0,0));
		    SLAM.InputImage(im,1.0,stateForInitial);
		}
	    }

	}

    }
    else if(InputData == 0)
    {
	// Store the time catching images.
	vector<double> vTimestamps;
	// Store image names.
	vector<string> vstrImageFilenames;
	cv::Mat im;
	string strFile = "/home/weixinyu/LearnAndWork/gaoyanyuan/ORB_SLAM/MySLAM/rgb.txt"; 
	//string strFile = "/home/weixinyu/LearnAndWork/gaoyanyuan/ORB_SLAM/ORB_SLAM2/Examples/Monocular/rgbd_dataset_freiburg2_large_with_loop/LineRGB.txt"; 

	LoadImages(strFile, vstrImageFilenames, vTimestamps, ProcessingImagesNum);

	int nImages = vstrImageFilenames.size();
	ProcessingImagesNum >= nImages ? ImageNumInTrue = nImages : ImageNumInTrue = ProcessingImagesNum;
	cout << "ImageNumInTrue: " << ImageNumInTrue << endl;
	cout << "vTimestamps.size: " << vTimestamps.size() << endl;
	cout << "vstrImageFilenames.size: " << vstrImageFilenames.size() << endl;


	for(int ni=0; ni<ImageNumInTrue; ni++)
	{
	    if(stateForInitial != -4)
	    {
		double tframe = vTimestamps[ni];
		string temp = "/home/weixinyu/LearnAndWork/gaoyanyuan/ORB_SLAM/MySLAM/"+vstrImageFilenames[ni]; 
		//string temp = "/home/weixinyu/LearnAndWork/gaoyanyuan/ORB_SLAM/ORB_SLAM2/Examples/Monocular/rgbd_dataset_freiburg2_large_with_loop/"+vstrImageFilenames[ni]; 
		im = imread(temp,1);

		if(im.empty())
		{
		    cerr << endl << "Failed to load image at: "
			<< "./mydataPGM/" << vstrImageFilenames[ni] << endl;
		    return 1;
		}
		chrono::steady_clock::time_point t1 = chrono::steady_clock::now();

		// Pass the image to the SLAM system
		cout << "now I'm pretended to process the " << ImageNumInTrue << "th image:)" << endl;
		if(LineDetectMethod == 0 || LineDetectMethod == 1)
		{
		    vLinesNum.push_back(SLAM.InputImage(im,tframe,stateForInitial));
		    cout << "in LineDetectAndMatch.cpp detect line number is: " << vLinesNum.back() << endl;
		}
		else
		{
		    cout << "please choose a correct method to detect line!" << endl;
		    cout << "0: LSD" << endl << "1:EDLine" << endl;
		    return 0;
		}


		chrono::steady_clock::time_point t2 = chrono::steady_clock::now();
		double ttrack= chrono::duration_cast<chrono::duration<double> >(t2 - t1).count();

		vTimesTrack.push_back(ttrack);

		// Wait to load the next frame
		double T=0;
		if(ni<ImageNumInTrue-1)
		    T = vTimestamps[ni+1]-tframe;
		else if(ni>0)
		    T = tframe-vTimestamps[ni-1];

		if(ttrack<T)
		    usleep((T-ttrack)*1e6);
		cout << "I pretended to be busy and I must sleep for " << (T-ttrack)*1e3 << " milliseconds for waiting next image. You know I've take " << ttrack*1e3 << " milliseconds for processing this image." << endl << endl;
	    }
	    else
	    {
		while(1)
		{
		    im = cv::Mat(480,640,CV_8UC3, cv::Scalar(0,0,0));
		    SLAM.InputImage(im,1.0,stateForInitial);
		}
	    }
	}
    }
    else
	cerr << endl << "Please choose right method to input data." << endl << "0: Dataset" << endl << "1:RealData" << endl;
	    

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

void LoadImages(const string &strFile, vector<string> &vstrImageFilenames, vector<double> &vTimestamps, const int num)
{
    ifstream f;
    f.open(strFile.c_str());

    string stemps;
    getline(f,stemps);
    getline(f,stemps);
    getline(f,stemps);
    int i = 0;

    while(!f.eof() && i < num)
    {
	string s;
	getline(f,s);
	if(!s.empty())
	{
	    stringstream ss;
	    ss << s;
	    double t;
	    string sRGB;
	    //	    cout << "sRGB is: " << sRGB << endl;
	    ss >> t;
	    vTimestamps.push_back(t);
	    ss >> sRGB;
	    vstrImageFilenames.push_back(sRGB);
	    //	    cout << "the double time is: " << t << endl << endl;
	}
	i++;
    }
    f.close();
}

