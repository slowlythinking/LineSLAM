#ifndef _VIEWER_H_
#define _VIEWER_H_

#include "LineSLAMSystem.h"

namespace LineSLAM
{
    class Viewer
    {
	public:
	    Viewer(int i);
	    void run();
	private:
	    Drawer* vDrawer;
    };
}

#endif
