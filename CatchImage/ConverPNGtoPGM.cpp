#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>

using namespace cv;
using namespace std;

const int ProcessingImagesNum = 1000;

void LoadImages(const string &strFile, vector<string> &vstrImageFilenames,vector<double> &vTimestamps);

int main()
{
    vector<string> vstrImageFilenames;
    vector<double> vTimestamps;
//  string strFile = "/home/weixinyu/LearnAndWork/gaoyanyuan/ORB_SLAM/MySLAM/CatchTime.txt";
    string strFile = "/home/weixinyu/LearnAndWork/gaoyanyuan/ORB_SLAM/ORB_SLAM2/Examples/Monocular/rgbd_dataset_freiburg2_large_with_loop/rgb.txt";
    cout << "the String strFile is: " << strFile << endl << "Begin to loadImages." << endl;
    LoadImages(strFile, vstrImageFilenames, vTimestamps);
    cout << "LoadImages done." << endl;

    int nImages = vstrImageFilenames.size();


    Mat inputimg, outputimg;

    for(int ni=0; ni<nImages && ni < ProcessingImagesNum; ni++)
    {
	// Load the source image into our simple image class.
	inputimg = imread("../mydata/"+vstrImageFilenames[ni],CV_LOAD_IMAGE_COLOR);

	if(inputimg.empty())
	{
	    cout<<"empty frame skipped"<<endl;
	    continue;
	}

	// name each frame as frame_numOframe

	stringstream strs;
	strs << ni;
	string temp_str = strs.str(); 
	temp_str="../mydataPGM/corridor-"+temp_str+".png.pgm";//assign coords into one string

	//parameters for saving as .pgm

	vector<int> compression_params;
	compression_params.push_back(CV_IMWRITE_PXM_BINARY);
	compression_params.push_back(0);

	cvtColor(inputimg,outputimg, CV_BGR2GRAY);//convert to grayscale
	imwrite(temp_str,outputimg);//save image
    }
    return 0;
}

void LoadImages(const string &strFile, vector<string> &vstrImageFilenames, vector<double> &vTimestamps)
{
    ofstream CatchTimePGM;
//  CatchTimePGM.open("../CatchTimePGM.txt");
    CatchTimePGM.open("../TUMPGM.txt");
    ifstream f;
    f.open(strFile.c_str());
    
    string s0;
    getline(f,s0);
    getline(f,s0);
    getline(f,s0);
    
    int i = 0;
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
	    CatchTimePGM << sRGB + ".pgm " << fixed << t << endl;
               }
    }
    CatchTimePGM.close();
    f.close();
}


