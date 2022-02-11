/***************************************************************************

    file        : MyParam.h
    created     : 18 Mar 2019
    copyright   : (C) 2019 D.Schellhammer

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _MYPARAM_H_
#define _MYPARAM_H_

#include <string>

class MyParam
{
public:
  MyParam(void** carparmhandle, const std::string &datadir, const std::string &trackname, unsigned int weather);

  double getNum(const std::string& sect, const std::string& att) const;
  void   setNum(const std::string& sect, const std::string& att, double value) const;

private:
  void* mCarParmHandle;
};

#endif // _MYPARAM_H_
