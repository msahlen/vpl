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

#include <cfloat>
#include "vplEdgeGenerator.h"
#include "vplStroke.h"

namespace vpl
{
    static const int cPositiveWinding =  1;
    static const int cNegativeWinding = -1;

    // Fill edge creation
    static void createEdges(DynamicArray<float>& points,
                            ScanLineList* scanLineList)
    {
        // Make sure path is closed
        points.add(points[0]);
        points.add(points[1]);

        vplUint numPoints = points.getItemCount()/2;

        // Make sure we have enough points
        if(numPoints < 3)
            return;

        for(vplUint i = 0; i < numPoints - 1;i++)
        {
            Edge edge;

            edge.makeEdge(points[2*i]  ,points[2*i+1],
                          points[2*i+2],points[2*i+3]);

            // Determine winding of edge
            if(points[2*i+1] <= points[2*i+3])
                edge.winding_ = cPositiveWinding;
            else
                edge.winding_ = cNegativeWinding;

            scanLineList->insert(edge);
        }
    }

    // Stroke edge creation
    static void createEdges(DynamicArray<float>& points,
                            ScanLineList* scanLineList,
                            int winding)
    {
        // Make sure path is closed
        points.add(points[0]);
        points.add(points[1]);

        vplUint numPoints = points.getItemCount()/2;

        // Make sure we have enough points
        if(numPoints < 3)
            return;

        for(vplUint i = 0; i < numPoints - 1;i++)
        {
            Edge edge;

            edge.makeEdge(points[2*i]  ,points[2*i+1],
                          points[2*i+2],points[2*i+3]);

            // Determine winding of edge
            if(points[2*i+1] <= points[2*i+3])
                edge.winding_ = cPositiveWinding*winding;
            else
                edge.winding_ = cNegativeWinding*winding;

            scanLineList->insert(edge);
        }
    }

    // Edge generator implementation
    void EdgeGenerator::initialize(vplUint subPixelCount,
                                   float width,
                                   float height)
    {
        width_         = width;
        height_        = height;
        subPixelCount_ = subPixelCount;
    }
    void EdgeGenerator::setClipRect(const Rect& rect)
    {
        clipRect_ = rect;
    }

    void EdgeGenerator::updateInternalClipRect()
    {
         Rect viewPort(0,width_,height_*subPixelCount_,0);

         internalClipRect_.left_   = clipRect_.left_;
         internalClipRect_.right_  = clipRect_.right_;
         internalClipRect_.top_    = clipRect_.top_*subPixelCount_,
         internalClipRect_.bottom_ = clipRect_.bottom_*subPixelCount_;

        if(viewPort.left_ > internalClipRect_.left_)
            internalClipRect_.left_ = viewPort.left_;

        if(viewPort.right_ < internalClipRect_.right_)
            internalClipRect_.right_ = viewPort.right_;

        if(viewPort.bottom_ > internalClipRect_.bottom_)
            internalClipRect_.bottom_ = viewPort.bottom_;

        if(viewPort.top_ < internalClipRect_.top_)
            internalClipRect_.top_ = viewPort.top_;
    }

    void EdgeGenerator::generateEdges(PathReference* reference)
    {
        // Generate fill edges
        if(reference->fillEdges_)
        {
            SubPath* subPath = reference->generator_->beginSubPathIteration();

            reference->fillEdges_->init(static_cast<vplUint>(height_),
                                        reference->generator_->getNumberOfPoints(),
                                        subPixelCount_);

            updateInternalClipRect();

            DynamicArray<float> clippedPoints;

            // Iterate over all subpaths and add edges
            while(reference->generator_->hasMoreSubPaths())
            {
                Edge edge;

                // Clip points
                const float* points = subPath->getPoints();

                clippedPoints.clear();

                clip(points,subPath->getNumberOfPoints(),clippedPoints);

                createEdges(clippedPoints,reference->fillEdges_);

                // Advance to next subpath
                subPath = reference->generator_->getNextSubPath();
            }
        }

        // Generate stroke edges
        if(reference->strokeEdges_)
        {
            // Generate two contours
            Stroker stroker(reference->pen_.getJoinType(),
                            reference->pen_.getEndType(),
                            reference->pen_.getSize(),
                            reference->pen_.getMiterLimit());

            SubPath* subPath = reference->generator_->beginSubPathIteration();

            reference->strokeEdges_->init(static_cast<vplUint>(height_),
                                          reference->generator_->getNumberOfPoints()*4,
                                          subPixelCount_);

            updateInternalClipRect();

            DynamicArray<float> clippedPoints;

            AffineMatrix finalPathTransform = reference->path_->getTransform();

            finalPathTransform *= reference->externalTransform_;

            // Iterate over all subpaths and add edges
            while(reference->generator_->hasMoreSubPaths())
            {
                stroker.generateStrokeOutlines(subPath,finalPathTransform);

                // Clip points
                clippedPoints.clear();

                clip(stroker.getOuterPoints(),
                     stroker.getNumberOfOuterPoints(),clippedPoints);

                createEdges(clippedPoints,reference->strokeEdges_);

                // Do inner points (only if path is not closed)
                if(stroker.getNumberOfInnerPoints())
                {
                    clippedPoints.clear();

                    clip(stroker.getInnerPoints(),
                         stroker.getNumberOfInnerPoints(),
                         clippedPoints);

                    createEdges(clippedPoints,reference->strokeEdges_,cNegativeWinding);
                }

                // Advance to next subpath
                subPath = reference->generator_->getNextSubPath();
            }
        }
    }
    // Clipping using liang barsky algorithm
    static const float cVplInfinity = FLT_MAX;

    void EdgeGenerator::clip(const float* points,
                             const vplUint numPoints,
                             vpl::DynamicArray<float>& out)
    {
        // Entry and exit of line
        float xIn;
        float xOut;
        float yIn;
        float yOut;

        // Parameter values of line
        float tOut1;
        float tIn2;
        float tOut2;

        // Parameter values for intersection
        float tInX;
        float tOutX;
        float tInY;
        float tOutY;

        // Direction of edge
        float deltaX;
        float deltaY;

        // Input points
        float x;
        float y;
        float nextX;
        float nextY;

        // for every edge
        if(numPoints == 0)
            return;

        for(vplUint i = 0; i < numPoints; i++)
        {
            // Get points
            if(i == numPoints - 1)
            {
                x     = points[2*i];
                y     = points[2*i + 1];
                nextX = points[0];
                nextY = points[1];
            }
            else
            {
                x     = points[2*i];
                y     = points[2*i + 1];
                nextX = points[2*i + 2];
                nextY = points[2*i + 3];
            }

            // determine direction of edge
            deltaX = nextX - x;
            deltaY = nextY - y;

            // use this to determine which bounding lines for the clip region the
            // containing line hits first
            if((deltaX > 0.0f) || (deltaX == 0.0f && x > internalClipRect_.right_))
            {
                xIn  = internalClipRect_.left_;
                xOut = internalClipRect_.right_;
            }
            else
            {
                xIn  = internalClipRect_.right_;
                xOut = internalClipRect_.left_;
            }

            if((deltaY > 0.0f) ||
               (deltaY == 0.0f &&
               y > internalClipRect_.top_))
            {
                yIn  = internalClipRect_.bottom_;
                yOut = internalClipRect_.top_;
            }
            else
            {
                yIn  = internalClipRect_.top_;
                yOut = internalClipRect_.bottom_;
            }

            // find the t values for the x and y exit points
            if(deltaX != 0.0f)
            {
                tOutX = (xOut - x) / deltaX;
            }
            else if(x <= internalClipRect_.right_ &&
                    internalClipRect_.left_ <= x)
            {
                tOutX = cVplInfinity;
            }
            else
                tOutX = -cVplInfinity;

            if (deltaY != 0.0f)
            {
                tOutY = (yOut - y) / deltaY;
            }
            else if(y <= internalClipRect_.top_ &&
                   internalClipRect_.bottom_ <= y)
            {
                tOutY = cVplInfinity;
            }
            else
                tOutY = -cVplInfinity;

             // Order the two exit points
            if(tOutX < tOutY)
            {
                tOut1 = tOutX;
                tOut2 = tOutY;
            }
            else
            {
                tOut1 = tOutY;
                tOut2 = tOutX;
            }
            if (tOut2 > 0.0f)
            {
                if(deltaX != 0.0f)
                   tInX = (xIn - x) / deltaX;
                else
                   tInX = -cVplInfinity;

                if (deltaY != 0.0f)
                   tInY = (yIn - y) / deltaY;
                else
                   tInY = -cVplInfinity;

                if(tInX < tInY)
                   tIn2 = tInY;
                else
                   tIn2 = tInX;

                // no visible segment
                if(tOut1 < tIn2)
                {
                    if(0.0f < tOut1 && tOut1 <= 1.0f)
                    {
                        // line crosses over intermediate corner region
                        if(tInX < tInY)
                        {
                            out.add(xOut);
                            out.add(yIn);
                        }
                        else
                        {
                            out.add(xIn);
                            out.add(yOut);
                        }
                    }
                }
                else
                {
                    // line crosses though window
                    if(0.0f < tOut1 && tIn2 <= 1.0f)
                    {
                        // visible segment
                        if(0.0f <= tIn2)
                        {
                            if(tInX > tInY)
                            {
                                out.add(xIn);
                                out.add(y + (tInX * deltaY));
                            }
                            else
                            {
                                out.add(x + (tInY * deltaX));
                                out.add(yIn);
                            }
                        }

                        if(1.0f >= tOut1)
                        {
                            if(tOutX < tOutY)
                            {
                                out.add(xOut);
                                out.add(y + (tOutX * deltaY));
                            }
                            else
                            {
                                out.add(x + (tOutY * deltaX));
                                out.add(yOut);
                            }
                        }
                        else
                        {
                            out.add(nextX);
                            out.add(nextY);
                        }
                    }
                }

                if(0.0f < tOut2 && tOut2 <= 1.0f)
                {
                    out.add(xOut);
                    out.add(yOut);
                }
            }
        } // for loop
    }
}
