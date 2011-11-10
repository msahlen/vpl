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

#include "vplVector.h"

namespace vpl
{
    // Create a perpendicular vector
    Vector perpendiculate(const Vector& v)
    {
            Vector r(v.y_,-v.x_);
            return r;
    }

    // Create an inverted vector
    Vector invert(const Vector& v)
    {
            Vector i(-v.x_,-v.y_);
            return i;
    }

    // Create a normalized vector
    Vector normalize(const Vector& v)
     {
            float len  = v.computeLength();
            Vector ret = v / len;
            return ret;
    }

    // Compute orientation
    float computeOrientation(const Vector& v)
    {
            float norm = v.computeLength();
            float cosa = v.x_/norm;
            float sina = v.y_/norm;
            
			if(sina >= 0) 
				return acos(cosa);
            else          
				return 2*cPi - acos(cosa);
    }

    // Compute angle between vectors
    float computeAngle(const Vector& v1,const Vector& v2)
    {
        float dot = computeDotProduct(v1,v2);
        float norm = v1.computeLength()*v2.computeLength();
        float quotient = dot/norm;

        if(quotient < -1.0f)
            quotient = -1.0f;
        else if(quotient > 1.0f)
            quotient = 1.0f;
        
		return acosf(quotient);
    }
   
     // Compute the dot product
    float computeDotProduct(const Vector& v1,const Vector& v2)
    {
            float res = v1.x_*v2.x_ + v1.y_*v2.y_;

            return res;
    }

	// Compute  the perp dot product
    float computePerpDotProduct(const Vector& v1,const Vector& v2)
    {
            return (v1.x_*v2.y_ - v1.y_*v2.x_);
    }

    // Compute intersection of p1 and p2 with direction v1 and v2
    bool computeIntersection(const Vector& p1,
                             const Vector& p2,
                             const Vector& dir1,
                             const Vector& dir2,
                             Vector& res)

    {
            Vector tmp  = dir1;
            Vector diff = p2 - p1;
            float d     = dir1.x_ *(-dir2.y_) - dir1.y_ * (-dir2.x_);
            float dx    = diff.x_ *(-dir2.y_) - diff.y_ * (-dir2.x_);
            
			if(d == 0.0f)
            {
                res = p1;
                return false;
            }

            tmp *= dx/d;
            res  = p1 + tmp;
            return true;
    }

}
