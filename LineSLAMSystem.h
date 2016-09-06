#include <opencv2/opencv.hpp>
#include "LineDetector.h"

namespace LineSLAM
{

class LineSLAMSystem
{
    private:
	EDLineDetect* EDLine;
    public:

	LineSLAMSystem(int i);

	void InputImageLSD(const cv::Mat &im, const double &timestamp);
	void InputImageEDL(std::string file);

};











}
