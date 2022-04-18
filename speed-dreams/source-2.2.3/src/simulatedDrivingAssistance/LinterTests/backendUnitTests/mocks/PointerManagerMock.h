#pragma once
#include "BlackBoxDataMock.h"

class PointerManagerMock
{
public:
    PointerManagerMock()
        : m_mock(GetExampleBlackBoxDataMock()){

          };

    BlackBoxDataMock* GetDataPointer()
    {
        return &m_mock;
    }

    tTrackSeg* GetSegmentPointer()
    {
        return m_segments;
    }

private:
    BlackBoxDataMock m_mock;
    tTrackSeg m_segments[10];
};