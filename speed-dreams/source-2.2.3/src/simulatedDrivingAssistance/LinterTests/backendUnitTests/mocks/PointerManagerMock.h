#pragma once
#include "BlackBoxDataMock.h"
#define SEGMENT_AMOUNT 10

class PointerManagerMock
{
public:
    PointerManagerMock()
        : m_mock(GetExampleBlackBoxDataMock())
    {
        for (int i = 0; i < SEGMENT_AMOUNT; i++)
        {
            m_segments[i] = {};
        }
    }

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
    tTrackSeg m_segments[SEGMENT_AMOUNT];
};