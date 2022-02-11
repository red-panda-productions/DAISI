/***************************************************************************

    file                     : OsgNodeMask.h
    created                  : Wen Nov 13 15:52:56 UTC 2019
    .........                : ... 2019 by Ivan Mahonin
    email                    : bh@icystar.com
    version                  : $Id$

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _OSGNODEMASK_H
#define _OSGNODEMASK_H 1

enum NodeMask
{
    NODE_MASK_NONE               = 0,
    NODE_MASK_ALL                = 0xffffffff,
    NODE_MASK_COMMON             = 1 <<  0,
    NODE_MASK_COMMON_CURCAR      = 1 <<  1,
    NODE_MASK_COMMON_CURDRV      = 1 <<  2,
    NODE_MASK_SKY_BACKGROUND     = 1 <<  3,
    NODE_MASK_SKY_MODEL          = 1 <<  4,
    NODE_MASK_SHADOW_RECV_COMMON = 1 <<  5,
    NODE_MASK_SHADOW_CAST_COMMON = 1 <<  6,
    NODE_MASK_SHADOW_RECV_CURCAR = 1 <<  7,
    NODE_MASK_SHADOW_CAST_CURCAR = 1 <<  8,
    NODE_MASK_SHADOW_RECV_CURDRV = 1 <<  9,
    NODE_MASK_SHADOW_CAST_CURDRV = 1 << 10,

    NODE_MASK_SHADOW_RECV        = NODE_MASK_SHADOW_RECV_COMMON
                                 | NODE_MASK_SHADOW_RECV_CURCAR
                                 | NODE_MASK_SHADOW_RECV_CURDRV,

    NODE_MASK_SHADOW_CAST        = NODE_MASK_SHADOW_CAST_COMMON
                                 | NODE_MASK_SHADOW_CAST_CURCAR
                                 | NODE_MASK_SHADOW_CAST_CURDRV,

    NODE_MASK_SHADOW             = NODE_MASK_SHADOW_RECV
                                 | NODE_MASK_SHADOW_CAST,

    NODE_MASK_CURCAR             = NODE_MASK_COMMON_CURCAR
                                 | NODE_MASK_SHADOW_RECV_CURCAR
                                 | NODE_MASK_SHADOW_CAST_CURCAR,

    NODE_MASK_CURDRV             = NODE_MASK_COMMON_CURDRV
                                 | NODE_MASK_SHADOW_RECV_CURDRV
                                 | NODE_MASK_SHADOW_CAST_CURDRV,
};

#endif // _OSGNODEMASK_H
