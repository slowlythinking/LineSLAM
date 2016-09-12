#ifndef _DRAWER_H_
#define _DRAWER_H

#include "LineSLAMSystem.h"
#include "LS.h"

namespace LineSLAM
{
    class Drawer
    {
	public:
	    Drawer(int i);

	    cv::Mat Draw();
	    void updateDrawer(LineSLAMSystem *tLineSLAMSystem);

	protected:
	    cv::Mat drawMat;
	    LS *tCurrentLine;
	    int tlineNum;
	    vector<LS> drawLine;
    };

}
#endif
