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

#include "vplAffineMatrix.h"

#define SX  m_[0]
#define SHX m_[1]
#define TX  m_[2]
#define SHY m_[3]
#define SY  m_[4]
#define TY  m_[5]

#ifdef USE_SSE_

extern "C" void PRE_CDECL_ batchTransform(const float* transform,
                                          float* vectors,
                                          vplUint numVectors) POST_CDECL_;
#else

inline void batchTransform(const float* transform,
                           float* vectors,
                           vplUint numVectors)
{
    for (vplUint i = 0;i < numVectors;i+=2)
    {
        float tmpX  = vectors[2*i1;

        vectors[2*i1 = tmpX * transform[0]  + vectors[2*i+1] * transform[1] + transform[2];
        vectors[2*i+1] = tmpX * transform[3] + vectors[2*i+1] * transform[4]  + transform[5];
    }
}

#endif // USE_SSE__

namespace vpl
{
    // Constructor
    AffineMatrix::AffineMatrix()
    {
        SX  = SY = m_[8] = 1.0f;
        SHX = TX = SHY = TY = m_[6] = m_[7] = 0.0f;
    }

    void AffineMatrix::setMatrix(float sx,float shx,float tx,
                                 float shy,float sy, float ty)
    {
        SX  = sx;
        SHX = shx;
        TX  = tx;
        SHY = shy;
        SY  = sy;
        TY  = ty;
    }
    // Transform matrix
    void AffineMatrix::translate(float x,float y)
    {
        TX += x;
        TY += y;
    }

    void AffineMatrix::scale(float x,float y)
    {
        SX *= x;
        SHX*= x;
        TX *= x;
        SHY*= y;
        SY *= y;
        TY *= y;
    }

    void AffineMatrix::scale(float s)
    {
        SX *= s;
        SHX*= s;
        TX *= s;
        SHY*= s;
        SY *= s;
        TY *= s;
    }

    void AffineMatrix::rotate(float rads)
    {
        float cosRads = cos(rads);
        float sinRads = sin(rads);
        float t0      = SX  * cosRads - SHY * sinRads;
        float t1      = SHX * cosRads - SY  * sinRads;
        float t2      = TX  * cosRads - TY  * sinRads;

        SHY = SX  * sinRads + SHY * cosRads;
        SY  = SHX * sinRads + SY  * cosRads;
        TY  = TX  * sinRads + TY  * cosRads;
        SX  = t0;
        SHX = t1;
        TX  = t2;
    }

    void AffineMatrix::shear(float radX,float radY)
    {
        float tanX = tan(radX);
        float tanY = tan(radY);

        float t0 = SX  * tanY + SHY;
        float t1 = SHX * tanY + SY;
        float t2 = TX  * tanY + TY;

        SX  = SX  + SHY * tanX;
        SHX = SHX + SY  * tanX;
        TX  = TX  + TY  * tanX;
        SHY = t0;
        SY  = t1;
        TY  = t2;
    }

    void AffineMatrix::getTranslation(float& x,float& y) const
    {
        x = TX;
        y = TY;
    }

    void AffineMatrix::getScale(float& x,float& y) const
    {
        x = SX;
        y = SY;
    }

    void AffineMatrix::getShear(float& x,float& y) const
    {
        x = SHX;
        y = SHY;
    }

    void AffineMatrix::makeIdentity()
    {
        SX  = SY = 1.0f;
        SHX = TX = SHY = TY = 0.0f;
    }
    // Compute determinant
    float AffineMatrix::computeDeterminant()
    {
        return SX * SY - SHX * SHY;
    }

    // Transform vector
    void AffineMatrix::transform(Vector& vector) const
    {
        float tmpX  = vector.x_;

        vector.x_ = tmpX * SX  + vector.y_ * SHX + TX;
        vector.y_ = tmpX * SHY + vector.y_ * SY  + TY;
    }

    void AffineMatrix::transform(const Vector& vector,Vector& result) const
    {
        result.x_ = vector.x_ * SX  + vector.y_ * SHX + TX;
        result.y_ = vector.x_ * SHY + vector.y_ * SY  + TY;
    }

    void AffineMatrix::transform(float& x,float& y) const
    {
        float tmpX = x;
        float tmpY = y;

        x = tmpX * SX  + tmpY * SHX + TX;
        y = tmpX * SHY + tmpY * SY  + TY;
    }

    void AffineMatrix::transform(float* vectors,vplUint numVectors) const
    {
        batchTransform(m_,vectors,numVectors);
    }

    void AffineMatrix::transform(DynamicArray<float>& vectors) const
    {
        batchTransform(m_,vectors.getContents(),vectors.getItemCount()/2);
    }
    // Invert the matrix
    void AffineMatrix::invert()
    {
        float d  = 1.0f / computeDeterminant();
        float t0 =  SY * d;

        SY  =  SX  * d;
        SHY = -SHY * d;
        SHX = -SHX * d;

        float t4 = -TX * t0  - TY * SHX;

        TY  = -TX * SHY - TY * SY;
        SX  = t0;
        TX  = t4;
    }
    // Operators
    // Matrix multiplication
    const AffineMatrix& AffineMatrix::operator *= (const AffineMatrix& m)
    {
        const float* mPtr = m.getMatrix();

        float t1 = SX  * mPtr[0] + SHY * mPtr[1];
        float t2 = SHX * mPtr[0] + SY  * mPtr[1];
        float t3 = TX  * mPtr[0] + TY  * mPtr[1] + mPtr[2];
        SHY      = SX  * mPtr[3] + SHY * mPtr[4];
        SY       = SHX * mPtr[3] + SY  * mPtr[4];
        TY       = TX  * mPtr[3] + TY  * mPtr[4] + mPtr[5];
        SX       = t1;
        SHX      = t2;
        TX       = t3;
        return *this;
    }

    AffineMatrix AffineMatrix::operator * (const AffineMatrix& m)
    {
        return AffineMatrix(*this) *= m;
    }
    // Transformation matrices
    // Translation
    TranslationMatrix::TranslationMatrix(float x,float y)
    {
        setMatrix(1.0f,0.0f,x,0.0f,1.0f,y);
    }
    // Rotation
    RotationMatrix::RotationMatrix(float rads)
    {
        setMatrix(cos(rads),-sin(rads),0.0f,sin(rads),cos(rads),0.0f);
    }
    // Scaling
    ScalingMatrix::ScalingMatrix(float s)
    {
        setMatrix(s,0.0f,0.0f,0.0f,s,0.0f);
    }
    ScalingMatrix::ScalingMatrix(float x,float y)
    {
        setMatrix(x,0.0f,0.0f,0.0f,y,0.0f);
    }
    // Shearing
    ShearMatrix::ShearMatrix(float rads)
    {
        setMatrix(1.0f,tan(rads),0.0f,tan(rads),1.0f,0.0f);
    }
    ShearMatrix::ShearMatrix(float radX,float radY)
    {
        setMatrix(1.0f,tan(radY),0.0f,tan(radX),1.0f,0.0f);
    }

}

#undef SX
#undef SHX
#undef TX
#undef SHY
#undef SY
#undef TY
