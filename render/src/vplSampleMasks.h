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

#ifndef VPL_SUPER_SAMPLE_MASKS_H_INCLUDED_
#define VPL_SUPER_SAMPLE_MASKS_H_INCLUDED_

namespace vpl
{
    // Supersampling masks
    static const FixedPoint SAMPLEMASK8x8[] =  
	{ 
		FixedPoint(0.625f),FixedPoint(0.0f),
        FixedPoint(0.375f),FixedPoint(0.75f),
        FixedPoint(0.125f),FixedPoint(0.5f),
        FixedPoint(0.875f),FixedPoint(0.25f) 
	};

    static const FixedPoint SAMPLEMASK16x16[] = 
	{
		FixedPoint(0.0625f),FixedPoint(0.5f),
        FixedPoint(0.25f),  FixedPoint(0.9375f),
        FixedPoint(0.6875f),FixedPoint(0.125f),
        FixedPoint(0.375f), FixedPoint(0.875f),
        FixedPoint(0.625f), FixedPoint(0.1875f),
        FixedPoint(0.4375f),FixedPoint(0.75f),
        FixedPoint(0.0f),   FixedPoint(0.5625f),
        FixedPoint(0.3125f),FixedPoint(0.8125f) 
	};

    static const FixedPoint SAMPLEMASK32x32[] = 
	{
		FixedPoint(0.875f), FixedPoint(0.40625f),
        FixedPoint(0.1875f),FixedPoint(0.71875f),
        FixedPoint(0.0f),   FixedPoint(0.53125f),
        FixedPoint(0.3125f),FixedPoint(0.84375f),
        FixedPoint(0.125f), FixedPoint(0.65625f),
        FixedPoint(0.4375f),FixedPoint(0.96875f),
        FixedPoint(0.25f),  FixedPoint(0.78125f),
        FixedPoint(0.5625f),FixedPoint(0.09375f),
        FixedPoint(0.375f), FixedPoint(0.90625f),
        FixedPoint(0.6875f),FixedPoint(0.21875f),
        FixedPoint(0.5f),   FixedPoint(0.03125f),
        FixedPoint(0.8125f),FixedPoint(0.34375f),
        FixedPoint(0.625f), FixedPoint(0.15625f),
        FixedPoint(0.9375f),FixedPoint(0.46875f),
        FixedPoint(0.75f),  FixedPoint(0.28125f),
        FixedPoint(0.0625f),FixedPoint(0.59375f)
	};
}

#endif