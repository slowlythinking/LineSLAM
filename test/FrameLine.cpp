#include "FrameLine.h"

#include<mutex>

namespace LineSLAM 
{

long unsigned int MapLine::nNextId=0;
mutex MapLine::mGlobalMutex;

/**
 * @brief 给定坐标与keyframe构造MapPoint
 *
 * 双目：StereoInitialization()，CreateNewKeyFrame()，LocalMapping::CreateNewMapPoints()
 * 单目：CreateInitialMapMonocular()，LocalMapping::CreateNewMapPoints()
 * @param Pos    MapPoint的坐标（wrt世界坐标系）
 * @param pRefKF KeyFrame
 * @param pMap   Map
 */
MapLine::MapLine(const cv::Mat &Pos1, const cv::Mat &Pos2, Map* pMap):
    mpMap(pMap)
{
    Pos1.copyTo(mWorldPos1);
    Pos2.copyTo(mWorldPos2);
    mNormalVector = cv::Mat::zeros(3,1,CV_32F);

    // MapPoints can be created from Tracking and Local Mapping. This mutex avoid conflicts with id.
    unique_lock<mutex> lock(mpMap->mMutexLineCreation);
    mnId=nNextId++;
}

/**
 * @brief 给定坐标与frame构造MapPoint
 *
 * 双目：UpdateLastFrame()
 * @param Pos    MapPoint的坐标（wrt世界坐标系）
 * @param pMap   Map
 * @param pFrame Frame
 * @param idxF   MapPoint在Frame中的索引，即对应的特征点的编号
 */
void MapLine::SetWorldPos(const cv::Mat &Pos1,const cv::Mat &Pos2)
{
    unique_lock<mutex> lock2(mGlobalMutex);
    unique_lock<mutex> lock(mMutexPos);
    Pos1.copyTo(mWorldPos1);
    Pos2.copyTo(mWorldPos2);
}

cv::Mat MapLine::GetWorldPos1()
{
    unique_lock<mutex> lock(mMutexPos);
    return mWorldPos1.clone();
}

cv::Mat MapLine::GetWorldPos2()
{
    unique_lock<mutex> lock(mMutexPos);
    return mWorldPos2.clone();
}

cv::Mat MapLine::GetNormal()
{
    unique_lock<mutex> lock(mMutexPos);
    return mNormalVector.clone();
}


} //namespace LineSLAM 
