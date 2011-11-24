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

#ifndef VPL_COLORHELPER_H_
#define VPL_COLORHELPER_H_

#include "vplConfig.h"

namespace vpl
{
	inline vplUchar getAlphaChannelFromRGBA(vplUint32 color)
	{
		return ((color >> 24) & 0xFF);
	}
	
	inline vplUchar getBlueChannelFromRGBA(vplUint32 color)
	{
		return ((color >> 16) & 0xFF);
	}
	
	inline vplUchar getGreenChannelFromRGBA(vplUint32 color)
	{
		return ((color >> 8) & 0xFF);
	}
	
	inline vplUchar getRedChannelFromRGBA(vplUint32 color)
	{
		return (color & 0xFF);
	}

	inline vplUint32 multiplyPixel(vplUint32 pixel,vplUint32 alpha,vplUint subShift)
    {
        vplUint32 c1 = (pixel & 0xff00ff) * alpha;
        vplUint32 c2 = ((pixel >> 8) & 0xff00ff) * alpha;

        c1 = (c1 >> subShift) & 0xff00ff;
        c2 = (c2 << (8 - subShift)) & 0xff00ff00;

        return c1 + c2;
    }

    inline vplUint32 multiplyPixel(vplUint32 pixel, vplUint32 alpha)
    {
        if(alpha != 255)
		{
			vplUint t = (pixel & 0xff00ff) * alpha;
			t = (t + ((t >> 8) & 0xff00ff) + 0x800080) >> 8;
			t &= 0xff00ff;
			pixel = ((pixel >> 8) & 0xff00ff) * alpha;
			pixel = (pixel + ((pixel >> 8) & 0xff00ff) + 0x800080);
			pixel &= 0xff00ff00;
			pixel |= t;
		}

        return pixel;
    }

    inline vplUint32 interpolatePixel(vplUint32 color1, vplUint32 alpha1,
                                      vplUint32 color2, vplUint32 alpha2)
    {
        vplUint t = (color1 & 0xff00ff) * alpha1 + (color2 & 0xff00ff) * alpha2;
        t = (t + ((t >> 8) & 0xff00ff) + 0x800080) >> 8;
        t &= 0xff00ff;

        color1 = ((color1 >> 8) & 0xff00ff) * alpha1 + ((color2 >> 8) & 0xff00ff) * alpha2;
        color1 = (color1 + ((color1 >> 8) & 0xff00ff) + 0x800080);
        color1 &= 0xff00ff00;
        color1 |= t;
        return color1;
    }

    inline vplUint32 preMultiplyColorRGBA(const vplUint32 color)
    {
        vplUint alpha = getAlphaChannelFromRGBA(color);
        vplUint red   = getRedChannelFromRGBA(color)  * alpha + 128;
        vplUint green = getGreenChannelFromRGBA(color)* alpha + 128;
        vplUint blue  = getBlueChannelFromRGBA(color) * alpha + 128;

        red   = (red   + (red   >> 8)) >> 8;
        green = (green + (green >> 8)) >> 8;
        blue  = (blue  + (blue  >> 8)) >> 8;

        return ((alpha << 24)|(blue << 16)|(green << 8)|red);
    }

    inline vplUint32 unPreMultiplyColorRGBA(vplUint32 color)
    {
        vplUint alpha = getAlphaChannelFromRGBA(color);

        if(alpha == 0)
            return 0;

        vplUint red   = getRedChannelFromRGBA(color)  * 255 / alpha;
        vplUint green = getGreenChannelFromRGBA(color)* 255 / alpha;
        vplUint blue  = getBlueChannelFromRGBA(color) * 255 / alpha;

        return ((alpha << 24)|(blue << 16)|(green << 8)|red);
    }
}
#endif