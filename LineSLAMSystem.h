#include <opencv2/opencv.hpp>


namespace LineSLAM
{


class LineSLAMSystem
{
    public:

	LineSLAMSystem(int i);

	void InputImage(const cv::Mat &im, const double &timestamp);

};











}
