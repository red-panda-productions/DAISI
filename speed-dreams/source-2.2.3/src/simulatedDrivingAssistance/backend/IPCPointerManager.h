#pragma once
#include "boost/interprocess/shared_memory_object.hpp"
#include "boost/interprocess/mapped_region.hpp"
#include "raceman.h"
#include "BlackBoxData.h"

#define LOOKAHEAD_SEGMENTS 10

/// @brief               Manages shared pointers so the program can use them
/// @tparam BlackBoxData The data type that is set on the data pointer
template <typename BlackBoxData>
class IPCPointerManager
{
public:
    IPCPointerManager();

    BlackBoxData* GetDataPointer() const;

    tTrackSeg* GetSegmentPointer() const;

private:
    boost::interprocess::shared_memory_object m_currentDataObject =
        boost::interprocess::shared_memory_object(boost::interprocess::open_or_create, "SDA_SHARED_MEMORY", boost::interprocess::read_write);

    boost::interprocess::shared_memory_object m_segmentDataObject =
        boost::interprocess::shared_memory_object(boost::interprocess::open_or_create, "SDA_SHARED_SEGMENT_MEMORY", boost::interprocess::read_write);

    boost::interprocess::mapped_region m_dataRegion;

    boost::interprocess::mapped_region m_segmentRegion;
};

#define SIPCPointerManager IPCPointerManager<BlackBoxData>
