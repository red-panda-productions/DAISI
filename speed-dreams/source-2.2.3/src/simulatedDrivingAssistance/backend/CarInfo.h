#pragma once

class CarInfo
{
private:
    TrackPosition m_trackPosition;
public:
    float Speed(){return 0;};
	float TopSpeed(){return 0;};
    int Gear(){return 0;};
    bool Headlights(){return false;};

    TrackPosition* TrackLocalPosition()
    {
        return &m_trackPosition;
    }
};