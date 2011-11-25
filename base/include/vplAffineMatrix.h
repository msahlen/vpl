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

#ifndef VPL_AFFINEMATRIX_H_INCLUDED_
#define VPL_AFFINEMATRIX_H_INCLUDED_

#include <cmath>
#include "vplConfig.h"
#include "vplArray.h"
#include "vplVector.h"

namespace vpl
{
    // An affine matrix
    // SX and SY are scaling values in x and y resp.
    // SHX and SHY are shearing values in x and y resp.
    // TX and TY are translation values in x and y resp.
    //      SX  SHY 0
    // M =  SHX SY  0
    //      TX  TY  1
    // Matrix data is stored as:
    // SX  = m_[0]
    // SHX = m_[1]
    // TX  = m_[2]
    // SHY = m_[3]
    // SY  = m_[4]
    // TY  = m_[5]

    class VPL_API AffineMatrix
    {
    public:

        AffineMatrix();
        virtual ~AffineMatrix(){}

        //Getters and setters
        inline float* getMatrix()             {return m_;}
        inline const float* getMatrix() const {return m_;}
        void setMatrix(float sx,float shx,float tx,float shy, float sy, float ty);

        //Functions
        //Apply transformations to matrix
        void translate(float x,float y);
        void scale(float x,float y);
        void scale(float s);
        void rotate(float rads);
        void shear(float radX,float radY);
        void makeIdentity();

        void getTranslation(float& x,float& y) const;
        void getScale(float& x,float& y) const;
        void getShear(float& x,float& y) const;

        //Invert matrix
        void invert();

        //Compute the determinant
        float computeDeterminant();

        //Transform vector(s)
        void transform(Vector& vector) const;
        void transform(const Vector& vector,Vector& result) const;
        void transform(float& x,float& y) const;
        void transform(float* vectors,vplUint numVectors) const;
        void transform(DynamicArray<float>& vectors) const;

        //Operators
         const AffineMatrix& operator *= (const AffineMatrix& m);
         AffineMatrix        operator *  (const AffineMatrix& m);

    protected:

       MEM_ALIGN(float m_[9],16);
    };

    //Affine transformation matrices
    class VPL_API TranslationMatrix:public AffineMatrix
    {
    public:
        TranslationMatrix(float x,float y);
        ~TranslationMatrix(){}
    };
    // Rotation matrix
    class VPL_API RotationMatrix:public AffineMatrix
    {
    public:
        RotationMatrix(float rads);
        ~RotationMatrix(){}
    };
    // Scaling matrix
    class VPL_API ScalingMatrix:public AffineMatrix
    {
    public:

        ScalingMatrix(float s);
        ScalingMatrix(float x,float y);
         ~ScalingMatrix(){}
    };
    //Shear matrix
    class VPL_API ShearMatrix:public AffineMatrix
    {
    public:
        ShearMatrix(float rads);
        ShearMatrix(float radX,float radY);
        ~ShearMatrix(){}
    };

}
#endif
