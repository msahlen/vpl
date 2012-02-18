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

#include "vplStroke.h"

namespace vpl
{
    static inline void swap(DynamicArray<float>*& outer,
                            DynamicArray<float>*& inner)
    {
        DynamicArray<float>* temp = outer;
        outer = inner;
        inner = temp;
    }

    Stroker::Stroker(JoinType joinType,EndType endType,
                     float size,float miterlimit):joinType_(joinType),
                                                  endType_(endType),
                                                  size_(size),
                                                  miterLimit_(miterlimit),
                                                  numSegments_(0)
    {
        // Choose correct joiner
        switch(joinType_)
        {
            case cBevelJoin:
                joiner_ = bevelJoiner;
                break;

            case cRoundJoin:
                joiner_ = roundJoiner;
                break;

            case cMiterJoin:
                joiner_ = miterJoiner;
                break;
        }
    }

    void Stroker::generateStrokeOutlines(SubPath* subPath,
                                         const AffineMatrix& transform)
    {
        if(subPath->getNumberOfPoints() == 0)
            return;

        // Iterate over all segments in subpath
        Segment* segment = subPath->beginSegmentIteration();

        const Path::Segment* pathSegment = segment->getSegment();

        // Clear state varibles and helper paths for this subpath
        numSegments_ = 0;

        innerPoints_.clear();
        outerPoints_.clear();

        // Store transform
        pathTransform_ = transform;
        inversePathTransform_ = transform;
        inversePathTransform_.invert();

        float xScale;
        float yScale;

        pathTransform_.getScale(xScale,yScale);

        scale_.makeIdentity();
        scale_.scale(xScale,yScale);

        inverseScale_ = scale_;
        inverseScale_.invert();

        // Handle the case where we start with
        // a move to separetely for now
        if(pathSegment->command_ == Path::cMoveToAbs ||
            pathSegment->command_ == Path::cMoveToRel)
        {
            updateCurrentPoint(pathSegment);

            firstPoint_ = prevPoint_ = currentPoint_;

            segment = subPath->getNextSegment();
            pathSegment = segment->getSegment();
        }

        // Iterate over sub path and handle all segments
        const Path::Segment* lastSegment = pathSegment;

        while(subPath->hasMoreSegments())
        {

            if(pathSegment->command_ < Path::cQuadBezierToAbs)
                handleLineSegment(segment);

            else if(pathSegment->command_ > Path::cSmoothCubicBezierToRel)
                handleArcSegment(segment);
            else
                handleBezierSegment(segment);

            ++numSegments_;

            segment = subPath->getNextSegment();
            lastSegment = pathSegment;
            pathSegment = segment->getSegment();
        }

        // Join or add ends
        if(lastSegment->command_ == Path::cClosePath)
            joiner_(this,firstPoint_,prevNormal_,firstNormal_);

        else
            endStroke();

    }
    void Stroker::handleLineSegment(Segment* segment)
    {
        const Path::Segment* pathSegment = segment->getSegment();

        // Where are we going to?
        updateCurrentPoint(pathSegment);

        // Calculate stroke normal
        calculateStrokeNormal(prevPoint_,currentPoint_,normal_);

        Vector outer;
        Vector inner;

        // Join segments or begin the first segment
        if(numSegments_ == 0)
        {
            firstPoint_  = prevPoint_;
            firstNormal_ = normal_;

            outer = firstPoint_ + firstNormal_;
            inner = firstPoint_ - firstNormal_;

            addPoint(&outerPoints_,outer);
            addPoint(&innerPoints_,inner);
        }
        else
            joiner_(this,prevPoint_,prevNormal_,normal_);

        outer = currentPoint_ + normal_;
        inner = currentPoint_ - normal_;

        // Add point
        addPoint(&outerPoints_,outer);
        addPoint(&innerPoints_,inner);

        // Update state
        prevPoint_  = currentPoint_;
        prevNormal_ = normal_;
    }
    void Stroker::handleArcSegment(Segment* segment)
    {
        // Where are we going to?
        updateCurrentPoint(segment->getSegment());

        // First point is already added
        float* points = segment->beginPointIteration();

        points = segment->getNextPoint();

        // First normal using segment end points
        Vector firstNormal = prevPoint_ - currentPoint_;
        firstNormal.normalize();
        firstNormal *= size_ /2;

        scale_.transform(firstNormal);

        // Calculate first and second normal from points
        Vector secondNormal;
        calculateStrokeNormal(points,normal_);
        calculateStrokeNormal(points + 2,secondNormal);

        // Adjust first normal so it points
        // in the same direction as the second normal
        Winding firstWinding  = determineWinding(firstNormal,normal_);
        Winding secondWinding = determineWinding(normal_,secondNormal);

        if(firstWinding != secondWinding)
            firstNormal.invert();

        // Join segments or begin the first segment
        Vector outer;
        Vector inner;

        if(numSegments_ == 0)
        {
            firstPoint_  = prevPoint_;
            firstNormal_ = firstNormal;

            outer = firstPoint_ + firstNormal_;
            inner = firstPoint_ - firstNormal_;

            addPoint(&outerPoints_,outer);
            addPoint(&innerPoints_,inner);
        }
        else
            joiner_(this,prevPoint_,prevNormal_,firstNormal);


        // Add points
        while(segment->hasMorePoints())
        {
            Vector point(*points,*(points + 1));

            calculateStrokeNormal(points,normal_);

            outer = point + normal_;
            inner = point - normal_;

            addPoint(&outerPoints_,outer);
            addPoint(&innerPoints_,inner);

            points = segment->getNextPoint();
        }

        // Add last
        normal_ = invert(firstNormal);

        outer = currentPoint_ + normal_;
        inner = currentPoint_ - normal_;

        addPoint(&outerPoints_,outer);
        addPoint(&innerPoints_,inner);

        prevPoint_  = currentPoint_;
        prevNormal_ = normal_;
    }

    void Stroker::handleBezierSegment(Segment* segment)
    {
         // Where are we going to?
         updateCurrentPoint(segment->getSegment());

        // Calculate first normal
        float* points = segment->beginPointIteration();

        calculateStrokeNormal(points,normal_);

        // Join segments or begin the first segment
        Vector outer;
        Vector inner;

        if(numSegments_ == 0)
        {
            firstPoint_  = prevPoint_;
            firstNormal_ = normal_;

            outer = firstPoint_ + firstNormal_;
            inner = firstPoint_ - firstNormal_;

            addPoint(&outerPoints_,outer);
            addPoint(&innerPoints_,inner);
        }
        else
            joiner_(this,prevPoint_,prevNormal_,normal_);


        // Add points,ignoring the first
        // since it is already added
        points = segment->getNextPoint();

        Vector point;

        if(segment->getNumberOfPoints() > 2)
        {
            while(!segment->atSecondToLastPoint())
            {
                point.x_ = *points;
                point.y_ = *(points + 1);

                calculateStrokeNormal(points,normal_);

                outer = point + normal_;
                inner = point - normal_;

                addPoint(&outerPoints_,outer);
                addPoint(&innerPoints_,inner);

                points = segment->getNextPoint();
            }
        }

        // Calculate last normal
        calculateStrokeNormal(point,currentPoint_,normal_);

        // Add last point
        outer = currentPoint_ + normal_;
        inner = currentPoint_ - normal_;

        addPoint(&outerPoints_,outer);
        addPoint(&innerPoints_,inner);

        prevPoint_  = currentPoint_;
        prevNormal_ = normal_;
    }
    // Make one closed path by adding the points of the inner path
    void Stroker::endStroke()
    {
        switch(endType_)
        {
            // Simplest case, just add the inner points
            case cButtEnd:
            {
                for(vplUint i = innerPoints_.getItemCount(); i > 0;i-=2)
                {
                    outerPoints_.add(innerPoints_[i - 2]);
                    outerPoints_.add(innerPoints_[i - 1]);
                }

                // Close the path
                outerPoints_.add(outerPoints_[0]);
                outerPoints_.add(outerPoints_[1]);
            }
                break;

            // End outer path with an arc, then add the inner points.
            // Finalize with a closing arc to the first point
            case cRoundEnd:
            {
                Winding winding = determineWinding(normal_,invert(normal_));

                generateArc(currentPoint_,normal_,invert(normal_),
                             winding,&outerPoints_);

                for(vplUint i = innerPoints_.getItemCount(); i > 0;i-=2)
                {
                    outerPoints_.add(innerPoints_[i - 2]);
                    outerPoints_.add(innerPoints_[i - 1]);
                }

                winding = determineWinding(invert(firstNormal_),firstNormal_);

                generateArc(firstPoint_,invert(firstNormal_),firstNormal_,
                            winding,&outerPoints_);

                // Close the path
                outerPoints_.add(outerPoints_[0]);
                outerPoints_.add(outerPoints_[1]);
            }
                break;

            // Add a rectangle with sides stroke width/2 x stroke width
            case cSquareEnd:
            {
                // Use the normals for calculation of the ends
                Vector dir(-normal_.y_,normal_.x_);
                Vector firstDir(-firstNormal_.y_,firstNormal_.x_);

                dir.normalize();
                firstDir.normalize();

                scale_.transform(dir);
                scale_.transform(firstDir);

                dir *= size_ / 2;
                firstDir *= size_ / 2;

                Vector v1 = currentPoint_ + normal_ + dir;
                Vector v2 = currentPoint_ - normal_ + dir;

                addPoint(&outerPoints_,v1);
                addPoint(&outerPoints_,v2);

                for(vplUint i = innerPoints_.getItemCount(); i > 0;i-=2)
                {
                    outerPoints_.add(innerPoints_[i - 2]);
                    outerPoints_.add(innerPoints_[i - 1]);
                }

                v1 = firstPoint_ - firstNormal_ - firstDir;
                v2 = firstPoint_ + firstNormal_ - firstDir;

                addPoint(&outerPoints_,v1);
                addPoint(&outerPoints_,v2);

                // Close the path
                outerPoints_.add(outerPoints_[0]);
                outerPoints_.add(outerPoints_[1]);
            }
                break;
        }

        innerPoints_.clear();
    }

    void Stroker::updateCurrentPoint(const Path::Segment* segment)
    {
        float x;
        float y;
        float dummy;

        // Close path
        if(segment->command_ == Path::cClosePath)
        {
            x = firstPoint_.x_;
            y = firstPoint_.y_;
        }
        // Horizontal and vertical line tos
        else if(segment->command_ == Path::cVerticalLineToAbs)
        {
            y = segment->args_[0];
            x = currentPoint_.x_;

            pathTransform_.transform(dummy,y);
        }
        else if(segment->command_ == Path::cVerticalLineToRel)
        {
            x = currentPoint_.x_;
            y = currentPoint_.y_;

            inversePathTransform_.transform(x,y);

            y += segment->args_[0];

            pathTransform_.transform(x,y);
        }
        else if(segment->command_ == Path::cHorizontalLineToAbs)
        {
             x = segment->args_[0];
             y = currentPoint_.y_;

            pathTransform_.transform(x,dummy);
        }
        else if(segment->command_ == Path::cHorizontalLineToRel)
        {
            x = currentPoint_.x_;
            y = currentPoint_.y_;

            inversePathTransform_.transform(x,y);

            x += segment->args_[0];

            pathTransform_.transform(x,y);
        }
        // Relative commands
        else if(segment->command_ & 0x1)
        {
            x = currentPoint_.x_;
            y = currentPoint_.y_;

            inversePathTransform_.transform(x,y);

            x += segment->args_[0];
            y += segment->args_[1];

            pathTransform_.transform(x,y);
        }
        else
        {
            x = segment->args_[0];
            y = segment->args_[1];

            pathTransform_.transform(x,y);
        }

        currentPoint_.x_ = x;
        currentPoint_.y_ = y;
    }

    void Stroker::generateArc(const Vector& center,const Vector& start,
                              const Vector& end,Winding winding,
                              DynamicArray<float>* points)
    {

        Vector realStart = start;
        Vector realEnd = end;

        inversePathTransform_.transform(realStart);
        inversePathTransform_.transform(realEnd);

        float startAngle = computeOrientation(realStart);
        float diff = computeAngle(realStart,realEnd);
        float radius = size_ / 2;
        double ra = (std::fabs(radius) + std::fabs(radius)) / 2;
        double da = acos(ra/(ra + 0.125f)) * 2;

        vplUint numSteps = static_cast<vplUint>(diff/da);

        float angle = 0.0f;

        Vector point;

        for(vplUint i = 0;i < numSteps;i++)
        {
            if(winding == cClockwise)
                angle = startAngle + i / float(numSteps) * diff;
            else
                angle = startAngle - i / float(numSteps) * diff;

            Vector offset(cos(angle),sin(angle));

            offset *= radius;

            scale_.transform(offset);

            point = center + offset;

            addPoint(points,point);
        }
    }

       void Stroker::bevelJoiner(Stroker* stroker,
                                 const Vector& point,
                                 const Vector& beforeNormal,
                                 const Vector& afterNormal)
    {
        DynamicArray<float>* outer = &stroker->outerPoints_;
        DynamicArray<float>* inner = &stroker->innerPoints_;

        Winding winding = stroker->determineWinding(beforeNormal,afterNormal);
        Vector normal   = afterNormal;

        if(winding == cCounterclockwise)
        {
            swap(outer,inner);
            normal.invert();
        }

        stroker->addPoint(outer,point + normal);

        stroker->innerJoiner(inner,point,normal);
    }

    void Stroker::miterJoiner(Stroker* stroker,
                              const Vector& point,
                              const Vector& beforeNormal,
                              const Vector& afterNormal)
    {
        Vector realPoint = point;
        Vector after = afterNormal;
        Vector before = beforeNormal;

        stroker->inverseScale_.transform(after);
        stroker->inverseScale_.transform(before);
        stroker->inverseScale_.transform(realPoint);

        DynamicArray<float>* outer = &stroker->outerPoints_;
        DynamicArray<float>* inner = &stroker->innerPoints_;

        Winding winding = stroker->determineWinding(before,after);

        if(winding == cCounterclockwise)
        {
            swap(outer,inner);
            before.invert();
            after.invert();
        }

        // Undo perpendiculate
        Vector prevDir(-before.y_,before.x_);
        Vector dir(-after.y_,after.x_);
        Vector intersection;

        dir.invert();

        bool intersectionExists = computeIntersection(realPoint + before,
                                                      realPoint + after,
                                                      prevDir,dir,
                                                      intersection);

        if(intersectionExists)
        {
            float angle = computeAngle(dir,prevDir);
            float limit = 1/sin(angle/2);
           
            if(limit > stroker->miterLimit_)
            {
                stroker->bevelJoiner(stroker,point,beforeNormal,afterNormal);
                return;
            }

            stroker->scale_.transform(intersection);
            stroker->scale_.transform(after);

            stroker->addPoint(outer,intersection);
            stroker->addPoint(outer,point + after);
            
            stroker->innerJoiner(inner,point,after);
        }
        else
            stroker->bevelJoiner(stroker,point,beforeNormal,afterNormal);

    }

    void Stroker::roundJoiner(Stroker* stroker,
                              const Vector& point,
                              const Vector& beforeNormal,
                              const Vector& afterNormal)
    {
        DynamicArray<float>* outer = &stroker->outerPoints_;
        DynamicArray<float>* inner = &stroker->innerPoints_;

        Winding winding = stroker->determineWinding(beforeNormal,afterNormal);
        Vector after    = afterNormal;
        Vector before   = beforeNormal;

        if(winding == cCounterclockwise)
        {
            swap(outer,inner);
            after.invert();
            before.invert();
        }

        stroker->generateArc(point,before,after,winding,outer);

        stroker->innerJoiner(inner,point,after);
    }
};
