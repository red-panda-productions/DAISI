#pragma once
#include "BlackBoxDataMock.h"

class PointerManagerMock
{
public:
    PointerManagerMock() : mock(GetExampleBlackBoxDataMock())
    {
        
    };

    BlackBoxDataMock* GetDataPointer()
    {
        return &mock;
    }

    tTrackSeg* GetSegmentPointer()
    {
        return segments;
    }

private:

    BlackBoxDataMock mock;

    tTrackSeg segments[10];
};