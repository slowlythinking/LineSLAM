#include <iostream>
#include <fstream>
#include <opencv2/core/core.hpp>

using namespace std;
using namespace cv;


void LoadImages(const string &strFile, vector<string> &vstrImageFilenames,
                vector<double> &vTimestamps);

int main(int argc, char **argv)
{
//    if(argc != 1)
//    {
//	cout << "please input the path to sequence" << endl;
//	return 1;
//    }

    vector<string> vstrImageFilenames;
    vector<double> vTimestamps;
//    string strFile = string(argv[1]);
    string strFile = "/home/weixinyu/LearnAndWork/gaoyanyuan/ORB_SLAM/ORB_SLAM2/Examples/Monocular/rgbd_dataset_freiburg2_large_with_loop/rgb.txt"; 
    cout << "the String strFile is: " << strFile << endl << "Begin to loadImages.";
    LoadImages(strFile, vstrImageFilenames, vTimestamps);
    cout << "LoadImages done." << endl;


//    int nImages = vstrImageFilenames.size();
//    // Main loop
//    cv::Mat im;
//    for(int ni=0; ni<nImages; ni++)
//    {
//	// Read image from file
//	im = cv::imread(string(argv[3])+"/"+vstrImageFilenames[ni],CV_LOAD_IMAGE_UNCHANGED);
//	double tframe = vTimestamps[ni];
//
//	if(im.empty())
//	{
//	    cerr << endl << "Failed to load image at: "
//		<< string(argv[3]) << "/" << vstrImageFilenames[ni] << endl;
//	    return 1;
//	}
//
//
//
//
//    }
    return 0;
}

void LoadImages(const string &strFile, vector<string> &vstrImageFilenames, vector<double> &vTimestamps)
{
    ifstream f;
    f.open(strFile.c_str());

    // skip first three lines
    string s0;
    getline(f,s0);
    cout << "first line is: " << s0 << endl;
    getline(f,s0);
    getline(f,s0);

    while(!f.eof())
    {
        string s;
        getline(f,s);
//	cout << "s is: " << s << endl;
        if(!s.empty())
        {
            stringstream ss;
            ss << s;
            double t;
            string sRGB;
            ss >> t;
           // vTimestamps.push_back(t);
            ss >> sRGB;
	cout << "sRGB is: " << sRGB << endl;
            vstrImageFilenames.push_back(sRGB);
        }
    }
}


