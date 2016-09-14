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

const int ProcessingImagesNum = 1000;

void LoadImages(const string &strFile, vector<string> &vstrImageFilenames,
                vector<double> &vTimestamps);

int main(int argc, char **argv)
{
    if(argc != 2)
    {
	cout << "please choose a method to detect line!" << endl;
	cout << "0: LSD" << endl << "1:EDLine" << endl;
	return 1;
    }
    int LineDetectMethod = stoi(string(argv[1]));

    vector<string> vstrImageFilenames;
    vector<double> vTimestamps;
//  string strFile = "/home/weixinyu/LearnAndWork/gaoyanyuan/ORB_SLAM/MySLAM/CatchTimePGM.txt"; 
    string strFile = "/home/weixinyu/LearnAndWork/gaoyanyuan/ORB_SLAM/ORB_SLAM2/Examples/Monocular/rgbd_dataset_freiburg2_large_with_loop/LineRGB.txt"; 
    cout << "the String strFile is: " << strFile << endl << "Begin to loadImages." << endl;
    LoadImages(strFile, vstrImageFilenames, vTimestamps);
    cout << "LoadImages done." << endl;

    int nImages = vstrImageFilenames.size();
    int ImageNumInTrue;
    ProcessingImagesNum >= nImages ? ImageNumInTrue = nImages : ImageNumInTrue = ProcessingImagesNum;

    LineSLAM::LineSLAMSystem SLAM(1);

    // Vector for tracking time statistics
    vector<double> vTimesTrack;
    vector<int> vLinesNum;
    vTimesTrack.resize(ImageNumInTrue);
    vLinesNum.resize(ImageNumInTrue);

    cout << endl << "-------" << endl;
    cout << "Start processing sequence ..." << endl;
    cout << "Images in the sequence: " << nImages << endl << endl;



    // Main loop
    cv::Mat im;
    for(int ni=0; ni<ImageNumInTrue; ni++)
    {
	// Read image from file

	double tframe = vTimestamps[ni];
	//	im = imread(string(argv[3])+"/"+vstrImageFilenames[ni],CV_LOAD_IMAGE_UNCHANGED);

	//read png
//	string temp = "./mydataPGM/"+vstrImageFilenames[ni]; 
	string temp = "/home/weixinyu/LearnAndWork/gaoyanyuan/ORB_SLAM/ORB_SLAM2/Examples/Monocular/rgbd_dataset_freiburg2_large_with_loop/"+vstrImageFilenames[ni]; 

	//read pgm
//	im = imread("./mydataPGM/"+vstrImageFilenames[ni],IMREAD_GRAYSCALE);
//	cout << "vstrImageFilenames[ni]: "  << vstrImageFilenames[ni] << endl;

//	im = imread(temp,CV_LOAD_IMAGE_COLOR);
	im = imread(temp,CV_8UC1);
	if(im.empty())
	{
	    cerr << endl << "Failed to load image at: "
		<< "./mydataPGM/" << vstrImageFilenames[ni] << endl;
	    return 1;
	}

	chrono::steady_clock::time_point t1 = chrono::steady_clock::now();

	// Pass the image to the SLAM system
	cout << "now I'm pretended to process the " << ni << "th image:)" << endl;
	if(LineDetectMethod == 0)
	    vLinesNum[ni] = SLAM.InputImageLSD(im,tframe);
	else if(LineDetectMethod == 1)
	    vLinesNum[ni] = SLAM.InputImageEDL(im,tframe);
	else
	{
	    cout << "please choose a method to detect line!" << endl;
	    cout << "0: LSD" << endl << "1:EDLine" << endl;
	    return 0;
	}



	chrono::steady_clock::time_point t2 = chrono::steady_clock::now();

	double ttrack= chrono::duration_cast<chrono::duration<double> >(t2 - t1).count();

	vTimesTrack[ni]=ttrack;

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

void LoadImages(const string &strFile, vector<string> &vstrImageFilenames, vector<double> &vTimestamps)
{
    ifstream f;
    f.open(strFile.c_str());
    while(!f.eof())
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
    }
    f.close();
}


