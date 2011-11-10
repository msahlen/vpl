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

#ifndef VPL_VECTOR_H_INCLUDED_
#define VPL_VECTOR_H_INCLUDED_

#include <limits>
#include <cmath>
#include "vplConfig.h"

namespace vpl
{
	// Constants
    const float cPi      = 3.14159265359f;
    const float cDegree  = 0.0174532925199f;
    const float cEpsilon = 0.0001f;

    class VPL_API Vector
    {
    public:

        Vector():x_(0.0f),y_(0.0f){}
        Vector(float x,float y):x_(x),y_(y){}

        ~Vector(){}

        // Functions
        inline float computeLength() const
        {
            return sqrtf(x_*x_ + y_*y_);
        }
        inline void perpendiculate()
        {
            float tmp = -x_;
            x_        = y_;
            y_        = tmp;
        }
        inline void invert()
        {
            x_ = -x_;
            y_ = -y_;
        }
        inline void normalize()
        {
            float len = computeLength();

            if(len > std::numeric_limits<float>::epsilon())
            {
                x_ /= len;
                y_ /= len;
            }
            else
            {
                x_ = 1.0f;
                y_ = 1.0f;
            }
        }
        // Operators
        inline Vector& operator -= (const Vector& rhs)
        {
            x_ -= rhs.x_;
            y_ -= rhs.y_;

            return *this;
        }
        inline Vector& operator += (const Vector& rhs)
        {
            x_ += rhs.x_;
            y_ += rhs.y_;

            return *this;
        }
        inline Vector& operator *= (float f)
        {
            x_ *= f;
            y_ *= f;

            return *this;
        }
        inline Vector& operator /= (float f)
        {
            x_ /= f;
            y_ /= f;

            return *this;
        }
        inline Vector operator / (float f) const
        {
            return Vector(x_/f,y_/f);
        }
        inline Vector operator * (float f) const
        {
            return Vector(x_*f,y_*f);
        }
        inline Vector operator + (const Vector& rhs) const
        {
            return Vector(x_ + rhs.x_,y_ + rhs.y_);
        }
        inline Vector operator - (const Vector& rhs) const
        {
            return Vector(x_ - rhs.x_,y_ - rhs.y_);
        }
        inline bool operator == (const Vector &rhs) const
        {
            return (isEqual(x_,rhs.x_ ) && isEqual(y_, rhs.y_));
        }
        inline bool operator != (const Vector &rhs) const
        {
            return !(Vector(x_,y_) == rhs);
        }

        // Values
        float x_;
        float y_;

    private:

        inline bool isEqual(float x, float y) const
        {
            return std::fabs(x - y) < cEpsilon;
        }
    };

	// Perpendiculate
    VPL_API Vector perpendiculate(const Vector& v);

    // Invert
    VPL_API Vector invert(const Vector& v);

    // Normalize
    VPL_API Vector normalize(const Vector& v);

    // Compute vector orientation
    VPL_API float computeOrientation(const Vector& v);

    // Compute angle between vectors
    VPL_API float computeAngle(const Vector& v1,const Vector& v2);

    // Compute dot product
    VPL_API float computeDotProduct(const Vector& v1,const Vector& v2);

	 // Compute perp dot product
    VPL_API float computePerpDotProduct(const Vector& v1,const Vector& v2);

    // Compute intersection of p1 and p2 with direction v1 and v2
    VPL_API bool computeIntersection(const Vector& p1,const Vector& p2,
                                     const Vector& dir1,const Vector& dir2,
                                     Vector& res);
}
#endif
