/*
 * This program has been developed by students from the bachelor Computer Science at
 * Utrecht University within the Software Project course.
 * © Copyright Utrecht University (Department of Information and Computing Sciences)
 */

#pragma once
#include "IPCPointerManager.h"

#define CREATE_IPC_POINTER_MANAGER_IMPLEMENTATION(p_type)               \
    template IPCPointerManager<p_type>::IPCPointerManager();            \
    template p_type* IPCPointerManager<p_type>::GetDataPointer() const; \
    template tTrackSeg* IPCPointerManager<p_type>::GetSegmentPointer() const;

/// @brief               Creates/opens the data pointers and sets their size
template <typename BlackBoxData>
IPCPointerManager<BlackBoxData>::IPCPointerManager()
{
    m_currentDataObject.truncate(sizeof(BlackBoxData));
    m_segmentDataObject.truncate(sizeof(tTrackSeg) * LOOKAHEAD_SEGMENTS);
    m_dataRegion = boost::interprocess::mapped_region(m_currentDataObject, boost::interprocess::read_write);
    m_segmentRegion = boost::interprocess::mapped_region(m_segmentDataObject, boost::interprocess::read_write);
}

/// @brief  Gets the data pointer from the shared memory
/// @return The data pointer
template <typename BlackBoxData>
BlackBoxData* IPCPointerManager<BlackBoxData>::GetDataPointer() const
{
    return static_cast<BlackBoxData*>(m_dataRegion.get_address());
}

/// @brief  Gets the segment pointer from the shared memory
/// @return The segment pointer
template <typename BlackBoxData>
tTrackSeg* IPCPointerManager<BlackBoxData>::GetSegmentPointer() const
{
    return static_cast<tTrackSeg*>(m_segmentRegion.get_address());
}
