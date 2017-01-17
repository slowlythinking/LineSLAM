#ifndef FRAMELINE_H 
#define FRAMELINE_H 

#include"Frame.h"
#include"Map.h"

#include<opencv2/core/core.hpp>
#include<mutex>

namespace LineSLAM 
{

class Map;
class Frame;

/**
 * @brief MapLine是一条地图线段
 */
class MapLine
{
public:
    MapLine(const cv::Mat &Pos1, const cv::Mat &Pos2, Map* pMap);

    void SetWorldPos(const cv::Mat &Pos1, const cv::Mat &Pos2);
    cv::Mat GetWorldPos1();
    cv::Mat GetWorldPos2();

    cv::Mat GetNormal();
//    KeyFrame* GetReferenceKeyFrame();

//    std::map<KeyFrame*,size_t> GetObservations();
    
public:
    long unsigned int mnId; ///< Global ID for MapPoint
    static long unsigned int nNextId;

    // Variables used by the tracking

    static std::mutex mGlobalMutex;

protected:    

    // Position in absolute coordinates
    cv::Mat mWorldPos1; ///< MapLine在世界坐标系下的坐标1
    cv::Mat mWorldPos2; ///< MapLine在世界坐标系下的坐标2

    // Mean viewing direction
    cv::Mat mNormalVector;

    // Best descriptor to fast matching
    // 每个3D点也有一个descriptor
    // 如果MapPoint与很多帧图像特征点对应（由keyframe来构造时），那么距离其它描述子的平均距离最小的描述子是最佳描述子
    // MapPoint只与一帧的图像特征点对应（由frame来构造时），那么这个特征点的描述子就是该3D点的描述子
    cv::Mat mDescriptor; ///< 通过 ComputeDistinctiveDescriptors() 得到的最优描述子


    // Tracking counters
    int mnVisible;
    int mnFound;

    // Bad flag (we do not currently erase MapPoint from memory)
    bool mbBad;
    Map* mpMap;

    std::mutex mMutexPos;
    std::mutex mMutexFeatures;
};

} //namespace LineSLAM 

#endif // FRAMELINE_H 
