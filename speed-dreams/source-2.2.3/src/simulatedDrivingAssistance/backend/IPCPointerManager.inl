#pragma once
#include "IPCPointerManager.h"

#define CREATE_IPC_POINTER_MANAGER_IMPLEMENTATION(p_type) \
    template IPCPointerManager<p_type>::IPCPointerManager(); \
    template p_type* IPCPointerManager<p_type>::GetDataPointer() const; \
    template tTrackSeg* IPCPointerManager<p_type>::GetSegmentPointer() const;


template <typename BlackBoxData>
IPCPointerManager<BlackBoxData>::IPCPointerManager()
{
    m_currentDataObject.truncate(sizeof(BlackBoxData));
    m_segmentDataObject.truncate(sizeof(tTrackSeg) * LOOKAHEAD_SEGMENTS);
    m_dataRegion = boost::interprocess::mapped_region(m_currentDataObject, boost::interprocess::read_write);
    m_segmentRegion = boost::interprocess::mapped_region(m_segmentDataObject, boost::interprocess::read_write);
}

template<typename BlackBoxData>
BlackBoxData* IPCPointerManager<BlackBoxData>::GetDataPointer() const
{
    return static_cast<BlackBoxData*>(m_dataRegion.get_address());
}

template <typename BlackBoxData>
tTrackSeg* IPCPointerManager<BlackBoxData>::GetSegmentPointer() const
{
    return static_cast<tTrackSeg*>(m_segmentRegion.get_address());
}
