#include <opencv2/line_descriptor.hpp>

#include "opencv2/core/utility.hpp"
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/highgui.hpp>
#include <vector>
//#include <opencv2/opencv.hpp>

#include <iostream>

#define MATCHES_DIST_THRESHOLD 20 

using namespace cv;
using namespace cv::line_descriptor;

static const char* keys =
{ "{@image_path1 | | Image path 1 }"
    "{@image_path2 | | Image path 2 }" };

static void help()
{
    std::cout << "\nThis example shows the functionalities of lines extraction " << "and descriptors computation furnished by BinaryDescriptor class\n"
	<< "Please, run this sample using a command in the form\n" << "./example_line_descriptor_compute_descriptors <path_to_input_image 1>"
	<< "<path_to_input_image 2>" << std::endl;

}

int main( int argc, char** argv )
{
    /* get parameters from command line */
    //      CommandLineParser parser( argc, argv, keys );
    //      String image_path1 = parser.get<String>( 0 );
    //      String image_path2 = parser.get<String>( 1 );
    String image_path1 = "/home/weixinyu/LearnAndWork/gaoyanyuan/ORB_SLAM/MySLAM/test/mydata/corridor-0.png";
    String image_path2 = "/home/weixinyu/LearnAndWork/gaoyanyuan/ORB_SLAM/MySLAM/test/mydata/corridor-1.png";

    if( image_path1.empty() || image_path2.empty() )
    {
	help();
	return -1;
    }

    /* load image */
    cv::Mat imageMat1 = imread( image_path1, 1 );
    cv::Mat imageMat2 = imread( image_path2, 1 );

    if( imageMat1.data == NULL || imageMat2.data == NULL )
    {
	std::cout << "Error, images could not be loaded. Please, check their path" << std::endl;
    }

    /* create binary masks */
    cv::Mat mask1 = Mat::ones( imageMat1.size(), CV_8UC1 );
    cv::Mat mask2 = Mat::ones( imageMat2.size(), CV_8UC1 );

    /* create a pointer to a BinaryDescriptor object with default parameters */
    Ptr<BinaryDescriptor> bd = BinaryDescriptor::createBinaryDescriptor(  );

    /* compute lines and descriptors */
    std::vector<KeyLine> keylines1, keylines2;
    cv::Mat descr1, descr2;

    ( *bd )( imageMat1, mask1, keylines1, descr1, false, false );
    ( *bd )( imageMat2, mask2, keylines2, descr2, false, false );
    

    //test keypoint
    cv::KeyPoint kp1(keylines1[2].startPointX,keylines1[2].startPointY,0.3);
    cv::KeyPoint kp2(keylines1[2].endPointX,keylines1[2].endPointY,0.3);
    const float u1 = kp1.pt.x;
    const float v1 = kp1.pt.y;
    const float u2 = kp2.pt.x;
    const float v2 = kp2.pt.y;
    std::cout << "test converting keyline to keypoint: " << std::endl;
    std::cout << "u1：" << u1 <<"----v1: " << v1 << "----u2: " << u2 << "----v2: " << v2 << std::endl;




    /* select keylines from first octave and their descriptors */
    std::vector<KeyLine> lbd_octave1, lbd_octave2;
    Mat left_lbd, right_lbd;
    for ( int i = 0; i < (int) keylines1.size(); i++ )
    {
	if( keylines1[i].octave == 0 )
	{
	    lbd_octave1.push_back( keylines1[i] );
	    left_lbd.push_back( descr1.row( i ) );
	}
    }

    for ( int j = 0; j < (int) keylines2.size(); j++ )
    {
	if( keylines2[j].octave == 0 )
	{
	    lbd_octave2.push_back( keylines2[j] );
	    right_lbd.push_back( descr2.row( j ) );
	}
    }

    /* create a BinaryDescriptorMatcher object */
    Ptr<BinaryDescriptorMatcher> bdm = BinaryDescriptorMatcher::createBinaryDescriptorMatcher();

    /* require match */
    std::vector<DMatch> matches;
    bdm->match( left_lbd, right_lbd, matches );

    /* select best matches */
    std::vector<DMatch> good_matches;
    for ( int i = 0; i < (int) matches.size(); i++ )
    {
	if( matches[i].distance < MATCHES_DIST_THRESHOLD )
	    good_matches.push_back( matches[i] );
    }
    std::cout << "BDmatches number is: " << matches.size() << std::endl;
    std::cout << "BDgoodMatches number is: " << good_matches.size() << std::endl;

    std::vector<cv::KeyPoint> mvKeys1; ///< 存储Current Frame中的特征点
    int pointNum1 = keylines1.size();
    mvKeys1.clear();
    mvKeys1.reserve(2*pointNum1);
    for(int i = 0; i < pointNum1; i++)
    {
	cv::KeyPoint linekp1(keylines1[i].startPointX,keylines1[i].startPointY,0.3);
	cv::KeyPoint linekp2(keylines1[i].endPointX,keylines1[i].endPointY,0.3);
	mvKeys1.push_back(linekp1);
	mvKeys1.push_back(linekp2);
    }
    
    std::vector<cv::KeyPoint> mvKeys2; ///< 存储Current Frame中的特征点
    int pointNum2 = keylines2.size();
    mvKeys2.clear();
    mvKeys2.reserve(2*pointNum2);
    for(int i = 0; i < pointNum2; i++)
    {
	cv::KeyPoint linekp1(keylines2[i].startPointX,keylines2[i].startPointY,0.3);
	cv::KeyPoint linekp2(keylines2[i].endPointX,keylines2[i].endPointY,0.3);
	mvKeys2.push_back(linekp1);
	mvKeys2.push_back(linekp2);
    }


    for(int i = 0; i < good_matches.size(); i++)
    {
	std::cout << "pre frame and cur frame, and i is:  " << i  << std::endl;
	std::cout << "x y: " << mvKeys1[2*good_matches[i].queryIdx].pt << std::endl;
	std::cout << "x y: " << mvKeys2[2*good_matches[i].trainIdx].pt << std::endl;
	std::cout << "x y: " << mvKeys1[2*good_matches[i].queryIdx + 1].pt << std::endl;
	std::cout << "x y: " << mvKeys2[2*good_matches[i].trainIdx + 1].pt << std::endl;
    }	

    /* plot matches */
    cv::Mat outImg;
    cv::Mat scaled1, scaled2;
    std::vector<char> mask( matches.size(), 1 );
    drawLineMatches( imageMat1, lbd_octave1, imageMat2, lbd_octave2, good_matches, outImg, Scalar::all( -1 ), Scalar::all( -1 ), mask,
	    DrawLinesMatchesFlags::DEFAULT );

    imshow( "Matches", outImg );
    waitKey(2);
    imwrite("/home/weixinyu/LearnAndWork/gaoyanyuan/ORB_SLAM/MySLAM/test/match/matches.jpg", outImg);
    /* create an LSD detector */
    Ptr<LSDDetector> lsd = LSDDetector::createLSDDetector();

    /* detect lines */
    std::vector<KeyLine> klsd1, klsd2;
    Mat lsd_descr1, lsd_descr2;
    lsd->detect( imageMat1, klsd1, 2, 2, mask1 );
    lsd->detect( imageMat2, klsd2, 2, 2, mask2 );

    /* compute descriptors for lines from first octave */
    bd->compute( imageMat1, klsd1, lsd_descr1 );
    bd->compute( imageMat2, klsd2, lsd_descr2 );

    /* select lines and descriptors from first octave */
    std::vector<KeyLine> octave0_1, octave0_2;
    Mat leftDEscr, rightDescr;
    for ( int i = 0; i < (int) klsd1.size(); i++ )
    {
	if( klsd1[i].octave == 0 )
	{
	    octave0_1.push_back( klsd1[i] );
	    leftDEscr.push_back( lsd_descr1.row( i ) );
	}
    }

    for ( int j = 0; j < (int) klsd2.size(); j++ )
    {
	if( klsd2[j].octave == 0 )
	{
	    octave0_2.push_back( klsd2[j] );
	    rightDescr.push_back( lsd_descr2.row( j ) );
	}
    }

    /* compute matches */
    std::vector<DMatch> lsd_matches;
    bdm->match( leftDEscr, rightDescr, lsd_matches );

    /* select best matches */
    good_matches.clear();
    for ( int i = 0; i < (int) lsd_matches.size(); i++ )
    {
	if( lsd_matches[i].distance < MATCHES_DIST_THRESHOLD )
	    good_matches.push_back( lsd_matches[i] );
    }
    std::cout << "LSDmatches number is: " << lsd_matches.size() << std::endl;
    std::cout << "LSDgoodMatches number is: " << good_matches.size() << std::endl;

    /* plot matches */
    cv::Mat lsd_outImg;
    //if the octave is 1,then we should use codes below.
//    resize( imageMat1, imageMat1, Size( imageMat1.cols / 2, imageMat1.rows / 2 ) );
//    resize( imageMat2, imageMat2, Size( imageMat2.cols / 2, imageMat2.rows / 2 ) );
    std::vector<char> lsd_mask( matches.size(), 1 );
    drawLineMatches( imageMat1, octave0_1, imageMat2, octave0_2, good_matches, lsd_outImg, Scalar::all( -1 ), Scalar::all( -1 ), lsd_mask,
	    DrawLinesMatchesFlags::DEFAULT );

    imshow( "LSD matches", lsd_outImg );
    imwrite("/home/weixinyu/LearnAndWork/gaoyanyuan/ORB_SLAM/MySLAM/test/match/lsd_matches.jpg", lsd_outImg);
    waitKey();


}
