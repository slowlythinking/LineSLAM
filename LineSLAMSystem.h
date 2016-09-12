#include <opencv2/opencv.hpp>
#include "LineDetector.h"
#include "LS.h"

namespace LineSLAM
{

class LineSLAMSystem
{
    private:
	EDLineDetect* EDLine;
    public:
	cv::Mat currentImg;
	LS *currentLines;
	int currentLineNum;

	LineSLAMSystem(int i, Drawer *ttFrameDrawer);

	void InputImageLSD(const cv::Mat &im, const double &timestamp);
	void InputImageEDL(std::string file);
    protected:
	Drawer* tFrameDrawer;

};











}
