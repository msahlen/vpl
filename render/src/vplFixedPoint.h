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

#ifndef VPL_FIXEDPOINT_H_INCLUDED_
#define VPL_FIXEDPOINT_H_INCLUDED_

#include "vplConfig.h"

namespace vpl
{
    class FixedPoint
    {
    public:

        FixedPoint():fixed_(0) {}
        FixedPoint(float a):fixed_(static_cast<int>(a/step())){}
        FixedPoint(int a):fixed_(a << cLowBits){}

        inline FixedPoint operator +(const FixedPoint& a) const
        {
            return FixedPoint(cFixedRaw,fixed_ + a.fixed_);
        }
        
		inline FixedPoint operator -(const FixedPoint& a) const
        {
            return FixedPoint(cFixedRaw,fixed_ - a.fixed_);
        }
        
		inline FixedPoint& operator +=(FixedPoint a) 
		{ 
			return *this = *this + a;
		}
        
		inline bool operator ==(const FixedPoint& a) const 
		{ 
			return fixed_ == a.fixed_; 
		}
        
		inline bool operator !=(const FixedPoint& a) const 
		{ 
			return fixed_ != a.fixed_; 
		}
        
		inline bool operator <=(const FixedPoint& a) const 
		{ 
			return fixed_ <= a.fixed_; 
		}
        
		inline bool operator >=(const FixedPoint& a) const 
		{ 
			return fixed_ >= a.fixed_; 
		}
        
		inline bool operator  <(const FixedPoint& a) const 
		{ 
			return fixed_< a.fixed_; 
		}
        
		inline bool operator  >(const FixedPoint& a) const 
		{ 
			return fixed_  > a.fixed_; 
		}

        inline float toFloat() const 
		{
			return fixed_ * step();
		}
        
		inline int toInt()     const 
		{
			return fixed_ >> cLowBits;
		}

    private:

        vplInt32 fixed_;

        // how many low bits there are to the right of the binary point
        const static int cLowBits = 16;

        // lowest possible number representable
        static float step() {return 1.0f/(1 << cLowBits);}

        // Private constructor
        enum FixedPointRaw { cFixedRaw };
        FixedPoint(FixedPointRaw, int fixed) : fixed_(fixed) {}
    };
}

#endif
