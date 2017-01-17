#ifndef MAP_H
#define MAP_H

#include "MapPoint.h"
#include "MapLine.h"
#include <set>

#include <mutex>



namespace LineSLAM 
{

class MapPoint;
class MapLine;

class Map
{
public:
    Map();

//    void AddKeyFrame(KeyFrame* pKF);
    void AddMapPoint(MapPoint* pMP);
    void AddMapLine(MapLine* pML);
    void EraseMapPoint(MapPoint* pMP);
    void EraseMapLine(MapLine* pML);
 //   void EraseKeyFrame(KeyFrame* pKF);
    void SetReferenceMapPoints(const std::vector<MapPoint*> &vpMPs);

//    std::vector<KeyFrame*> GetAllKeyFrames();
    std::vector<MapPoint*> GetAllMapPoints();
    std::vector<MapLine*> GetAllMapLines();
    std::vector<MapPoint*> GetReferenceMapPoints();

    long unsigned int MapPointsInMap();
//    long unsigned  KeyFramesInMap();

//    long unsigned int GetMaxKFid();

    void clear();

//    vector<KeyFrame*> mvpKeyFrameOrigins;

    std::mutex mMutexMapUpdate;

    // This avoid that two points are created simultaneously in separate threads (id conflict)
    std::mutex mMutexPointCreation;


    std::mutex mMutexLineCreation;

protected:
    std::set<MapPoint*> mspMapPoints; ///< MapPoints
 //   std::set<KeyFrame*> mspKeyFrames; ///< Keyframs
    std::set<MapLine*> mspMapLines; ///< MapPoints

    std::vector<MapPoint*> mvpReferenceMapPoints;

    long unsigned int mnMaxKFid;

    std::mutex mMutexMap;
    std::mutex mMutexMapGetLine;
};

} //namespace LineSLAM 

#endif // MAP_H
