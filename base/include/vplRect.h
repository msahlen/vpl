/* VPL - Vector path library
 * Copyright (C) 2009 - 2011 Mattias Sahlén <mattias.sahlen@gmail.com>
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General General Public License for more details.
 *
 * You should have received a copy of the GNU General General Public
 * License along with this library;
 * if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef VPL_RECT_H_INCLUDED_
#define VPL_RECT_H_INCLUDED_

#include "vplConfig.h"

namespace vpl
{
    struct VPL_API Rect
    {
        Rect():left_(0),right_(0),top_(0),bottom_(0){}

        Rect(float l,float r,float t,float b):left_(l),right_(r),
                                              top_(t),bottom_(b){}

        float left_;
        float right_;
        float top_;
        float bottom_;
    };
}
#endif