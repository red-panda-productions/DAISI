/***************************************************************************

    file                 : message.h
    created              : 2018-12-30 06:35:00 UTC
    copyright            : (C) Daniel Schellhammer

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include <string>
#include <vector>

#include "MyCar.h"

class Message
{
public:
    Message();
    void init(std::string dir, MyCar* car);
    void clearPanel() const;
    void displayOnPanel(const std::string& line1, const std::string& line2) const;
    void print(const double time, const std::string& flags, const std::string& desc, const double value);
    void write();

private:
    std::string mDir;
    std::string mFile;
    MyCar* mCar;
    std::vector<std::string> mMsgLog;
    unsigned mLogLine {0};

    const unsigned mMaxLines {200};
};

#endif // _MESSAGE_H_
