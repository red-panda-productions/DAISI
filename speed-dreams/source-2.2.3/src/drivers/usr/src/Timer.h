/***************************************************************************

    file        : Timer.cpp
    created     : 24 Mar 2019
    copyright   : (C) 2019 D. Schellhammer

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _TIMER_H_
#define _TIMER_H_

#include <chrono>

class Timer
{
public:
    Timer() : mStart(clock_t::now())
    {
    }

    void reset()
    {
        mStart = clock_t::now();
    }

    double elapsed() const
    {
        return std::chrono::duration_cast<second_t>(clock_t::now() - mStart).count();
    }

private:
    using clock_t = std::chrono::high_resolution_clock;
    using second_t = std::chrono::duration<double, std::ratio<1>>;
    std::chrono::time_point<clock_t> mStart;
};

#endif // _TIMER_H_
