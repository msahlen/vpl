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

#ifndef VPL_STROKE_H_INCLUDED_
#define VPL_STROKE_H_INCLUDED_

#include "vplConfig.h"
#include "vplPath.h"

namespace vpl
{
    class VPL_API Stroker
    {
        public:

            // The different end types
            enum EndType
            {
                cButtEnd = 0,
                cRoundEnd,
                cSquareEnd
            };

            // The different join types
            enum JoinType
            {
                cBevelJoin = 0,
                cRoundJoin,
                cMiterJoin
            };

            Stroker(JoinType joinType,EndType endType,float size,float miterlimit);

            ~Stroker(){}

            void generateStrokeOutlines(SubPath* subPath,
                                        const AffineMatrix& transform);

            inline DynamicArray<float>& getOuterArray() { return outerPoints_;}
            inline DynamicArray<float>& getInnerArray() { return innerPoints_;}
            inline float* getOuterPoints()              { return outerPoints_.getContents();}
            inline float* getInnerPoints()              { return innerPoints_.getContents();}
            inline vplUint getNumberOfOuterPoints()     { return outerPoints_.getItemCount()/2;}
            inline vplUint getNumberOfInnerPoints()     { return innerPoints_.getItemCount()/2;}

        private:

            enum Winding
            {
                cUndefined,
                cClockwise,
                cCounterclockwise
            };

            typedef void (*StrokeJoiner)(Stroker* stroker,
                                         const Vector& point,
                                         const Vector& beforeNormal,
                                         const Vector& afterNormal);

            void handleLineSegment(Segment* segment);
            void handleArcSegment(Segment* segment);
            void handleBezierSegment(Segment* segment);
            void endStroke();
            void processSegment(Path::Segment& segment,Vector offset);
            void updateCurrentPoint(const Path::Segment* segment);

            void generateArc(const Vector& center,const Vector& start,
                             const Vector& end,Winding winding,
                             DynamicArray<float>* points);

            static void miterJoiner(Stroker* stroker,
                                    const Vector& point,
                                    const Vector& beforeNormal,
                                    const Vector& afterNormal);

            static void bevelJoiner(Stroker* stroker,
                                    const Vector& point,
                                    const Vector& beforeNormal,
                                    const Vector& afterNormal);

            static  void roundJoiner(Stroker* stroker,
                                     const Vector& point,
                                     const Vector& beforeNormal,
                                     const Vector& afterNormal);

            inline void addPoint(DynamicArray<float>* dynArray,Vector point)
            {
                dynArray->add(point.x_);
                dynArray->add(point.y_);
            }

            inline void calculateStrokeNormal(const float* points,Vector& normal)
            {
                normal.x_ = *(points + 2) - *(points);
                normal.y_ = *(points + 3) - *(points + 1);

                inverseScale_.transform(normal);

                normal.normalize();
                normal.perpendiculate();
                normal *= size_ /2;

                scale_.transform(normal);
            }
            inline void calculateStrokeNormal(Vector& p1,Vector& p2,Vector& normal)
            {
                normal = p2 - p1;

                inverseScale_.transform(normal);

                normal.normalize();
                normal.perpendiculate();
                normal *= size_ /2;

                scale_.transform(normal);
            }

            inline Winding determineWinding(const Vector& prev,const Vector& next)
            {
                if(computePerpDotProduct(prev,next) >= 0)
                    return cClockwise;
                else
                    return cCounterclockwise;
            }

            inline void innerJoiner(DynamicArray<float>* inner,
                                    const Vector& point,
                                    const Vector& normal)
            {
                addPoint(inner,point);
                addPoint(inner,point - normal);
            }

            JoinType joinType_;
            EndType endType_;
            float size_;
            float miterLimit_;
            vplUint numSegments_;
            Vector currentPoint_;
            Vector prevPoint_;
            Vector normal_;
            Vector prevNormal_;
            Vector firstPoint_;
            Vector firstNormal_;
            DynamicArray<float> outerPoints_;
            DynamicArray<float> innerPoints_;
            StrokeJoiner joiner_;
            AffineMatrix pathTransform_;
            AffineMatrix inversePathTransform_;
            AffineMatrix scale_;
            AffineMatrix inverseScale_;
    };
}
#endif