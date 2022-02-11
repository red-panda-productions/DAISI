/***************************************************************************

    file                 : datalog.h
    created              : 2017-07-20 06:35:00 UTC
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

#ifndef _DATALOG_H_
#define _DATALOG_H_

#include "dataloghead.h"

#include <string>
#include <vector>

class DataLog
{
public:
    DataLog();
    void init(const std::string &dir, const std::string &carname);
    void add(const std::string &name, double* data, double scale);
    void update();
    void write();

private:
    std::string mDir;
    std::string mFile;
    std::vector<DataLogHead> mHead;
    std::vector<double> mData;

    unsigned mLogLine;
    unsigned mMaxLines;
};

#endif // _DATALOG_H_
