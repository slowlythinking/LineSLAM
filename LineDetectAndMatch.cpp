#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <unistd.h>
#include<algorithm>
#include "LineSLAMSystem.h"
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

const int ProcessingImagesNum = 500;

void LoadImages(const string &strFile, vector<string> &vstrImageFilenames,
                vector<double> &vTimestamps);

int main(int argc, char **argv)
{
    //    if(argc != 1)
    //    {
    //	cout << "please input the path to sequence" << endl;
    //	return 1;
    //    }

    //    string strFile = string(argv[1]);
    //    string strFile = "/home/weixinyu/LearnAndWork/gaoyanyuan/ORB_SLAM/ORB_SLAM2/Examples/Monocular/rgbd_dataset_freiburg2_large_with_loop/rgb.txt"; 

    vector<string> vstrImageFilenames;
    vector<double> vTimestamps;
    string strFile = "/home/weixinyu/LearnAndWork/gaoyanyuan/ORB_SLAM/MySLAM/CatchTimePGM.txt"; 
    cout << "the String strFile is: " << strFile << endl << "Begin to loadImages." << endl;
    LoadImages(strFile, vstrImageFilenames, vTimestamps);
    cout << "LoadImages done." << endl;

    int nImages = vstrImageFilenames.size();
    int ImageNumInTrue;
    ProcessingImagesNum >= nImages ? ImageNumInTrue = nImages : ImageNumInTrue = ProcessingImagesNum;

    LineSLAM::LineSLAMSystem SLAM(1);

    // Vector for tracking time statistics
    vector<double> vTimesTrack;
    vTimesTrack.resize(nImages);

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
	string temp = "./mydataPGM/"+vstrImageFilenames[ni]; 
//	//read pgm
////	im = imread("./mydataPGM/"+vstrImageFilenames[ni],IMREAD_GRAYSCALE);
////	cout << "vstrImageFilenames[ni]: "  << vstrImageFilenames[ni] << endl;
//
//	im = imread(temp,CV_LOAD_IMAGE_COLOR);
//	if(im.empty())
//	{
//	    cerr << endl << "Failed to load image at: "
//		<< "./mydataPGM/" << vstrImageFilenames[ni] << endl;
//	    return 1;
//	}

	chrono::steady_clock::time_point t1 = chrono::steady_clock::now();

	// Pass the image to the SLAM system
	cout << "now I'm pretended to process the " << ni << "th image:)" << endl;
//	SLAM.InputImageLSD(im,tframe);

	SLAM.InputImageEDL(temp);



	chrono::steady_clock::time_point t2 = chrono::steady_clock::now();

	double ttrack= chrono::duration_cast<chrono::duration<double> >(t2 - t1).count();

	vTimesTrack[ni]=ttrack;

	// Wait to load the next frame
	double T=0;
	if(ni<nImages-1)
	    T = vTimestamps[ni+1]-tframe;
	else if(ni>0)
	    T = tframe-vTimestamps[ni-1];

	if(ttrack<T)
	    usleep((T-ttrack)*1e6);
	cout << "I pretended to be busy and I must sleep for " << (T-ttrack)*1e3 << " milliseconds for waiting next image. You know I've take " << ttrack*1e3 << " milliseconds for processing this image." << endl << endl;


    }
    // Tracking time statistics
    sort(vTimesTrack.begin(),vTimesTrack.end());
    double totaltime = 0;
    for(int ni=0; ni<nImages; ni++)
    {
	totaltime+=vTimesTrack[ni];
    }
    cout << "-------" << endl << endl;
    cout << "median tracking time(milliseconds): " << vTimesTrack[ImageNumInTrue/2]*1e3 << endl;
    cout << "mean tracking time(milliseconds): " << (totaltime/ImageNumInTrue)*1e3 << endl;


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
	    ss >> sRGB;
//	    cout << "sRGB is: " << sRGB << endl;
	    vstrImageFilenames.push_back(sRGB);
	    ss >> t;
//	    cout << "the double time is: " << t << endl << endl;
            vTimestamps.push_back(t);
               }
    }
    f.close();
}


