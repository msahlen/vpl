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

#ifndef VPL_PATH_H_INCLUDED_
#define VPL_PATH_H_INCLUDED_

#include "vplConfig.h"
#include "vplArray.h"
#include "vplRect.h"
#include "vplVector.h"
#include "vplAffineMatrix.h"
#include "vplLog.h"

namespace vpl
{
    class VPL_API Path
    {
    public:

        enum AbsRel
        {
            cAbsolute = 0,
            cRelative = 1
        };

        enum SegmentType
        {
             cMoveSegment                     = (1 << 1),
             cLineSegment                     = (2 << 1),
             cHorizontalLineSegment           = (3 << 1),
             cVerticalLineSegment             = (4 << 1),
             cQuadBezierSegment               = (5 << 1),
             cCubicBezierSegment              = (6 << 1),
             cSmoothQuadBezierSegment         = (7 << 1),
             cSmoothCubicBezierSegment        = (8 << 1),
             cLargeClockwiseArcSegment        = (9 << 1),
             cSmallClockwiseArcSegment        = (10 << 1),
             cLargeCounterclockwiseArcSegment = (11 << 1),
             cSmallCounterclockwiseArcSegment = (12 << 1)

        };

        enum Command
        {
            cClosePath                     = 0,
            cMoveToAbs                     = cMoveSegment                     | cAbsolute,
            cMoveToRel                     = cMoveSegment                     | cRelative,
            cLineToAbs                     = cLineSegment                     | cAbsolute,
            cLineToRel                     = cLineSegment                     | cRelative,
            cHorizontalLineToAbs           = cHorizontalLineSegment           | cAbsolute,
            cHorizontalLineToRel           = cHorizontalLineSegment           | cRelative,
            cVerticalLineToAbs             = cVerticalLineSegment             | cAbsolute,
            cVerticalLineToRel             = cVerticalLineSegment             | cRelative,
            cQuadBezierToAbs               = cQuadBezierSegment               | cAbsolute,
            cQuadBezierToRel               = cQuadBezierSegment               | cRelative,
            cCubicBezierToAbs              = cCubicBezierSegment              | cAbsolute,
            cCubicBezierToRel              = cCubicBezierSegment              | cRelative,
            cSmoothQuadBezierToAbs         = cSmoothQuadBezierSegment         | cAbsolute,
            cSmoothQuadBezierToRel         = cSmoothQuadBezierSegment         | cRelative,
            cSmoothCubicBezierToAbs        = cSmoothCubicBezierSegment        | cAbsolute,
            cSmoothCubicBezierToRel        = cSmoothCubicBezierSegment        | cRelative,
            cSmallCounterclockwiseArcToAbs = cSmallCounterclockwiseArcSegment | cAbsolute,
            cSmallCounterclockwiseArcToRel = cSmallCounterclockwiseArcSegment | cRelative,
            cSmallClockwiseArcToAbs        = cSmallClockwiseArcSegment        | cAbsolute,
            cSmallClockwiseArcToRel        = cSmallClockwiseArcSegment        | cRelative,
            cLargeCounterclockwiseArcToAbs = cLargeCounterclockwiseArcSegment | cAbsolute,
            cLargeCounterclockwiseArcToRel = cLargeCounterclockwiseArcSegment | cRelative,
            cLargeClockwiseArcToAbs        = cLargeClockwiseArcSegment        | cAbsolute,
            cLargeClockwiseArcToRel        = cLargeClockwiseArcSegment        | cRelative
        };

        struct SubPath
        {
            SubPath():offset_(0),numberOfSegments_(0){}

            vplUint offset_;
            vplUint numberOfSegments_;
        };

        struct Segment
        {
            Segment():offset_(0){}

            Command command_;
            float args_[6];
            vplUint offset_;
        };

        Path():deltaLimit_(0.025f),segmentsValid_(true){}

        ~Path(){}

        // Move to point x,y. Ends the current subpath
        inline void moveToAbs(float x,float y)  
		{
			addSegment(cMoveToAbs,x,y);
		}

        inline void moveToRel(float x,float y)  
		{
			addSegment(cMoveToRel,x,y);
		}

        // Draw a line to point x,y
        inline void lineToAbs(float x,float y)  
		{
			addSegment(cLineToAbs,x,y);
		}

        inline void lineToRel(float x,float y)  
		{
			addSegment(cLineToRel,x,y);
		}

        inline void horizontalLineToAbs(float x)
		{
			addSegment(cHorizontalLineToAbs,x);
		}

        inline void horizontalLineToRel(float x)
		{
			addSegment(cHorizontalLineToRel,x);
		}

        inline void verticalLineToAbs(float y)  
		{
			addSegment(cVerticalLineToAbs,y);
		}

        inline void verticalLineToRel(float y)  
		{
			addSegment(cVerticalLineToRel,y);
		}

        // Draw a quadratic bezier curve to x,y using control points cx,cy
        inline void quadBezierToAbs(float x,float y,float cx,float cy)
        {
            addSegment(cQuadBezierToAbs,x,y,cx,cy);
        }
        inline void quadBezierToRel(float x,float y,float cx,float cy)
        {
            addSegment(cQuadBezierToRel,x,y,cx,cy);
        }
        inline void smoothQuadBezierToAbs(float x,float y)
        {
            addSegment(cSmoothQuadBezierToAbs,x,y);
        }
        inline void smoothQuadBezierToRel(float x,float y)
        {
            addSegment(cSmoothQuadBezierToRel,x,y);
        }

        // Draw a cubic bezier curve to x,y using control points c1x,c1y and
        // c2x,c2y
        inline void cubicBezierToAbs(float x,float y,float c1x,
                                     float c1y,float c2x,float c2y)
        {
            addSegment(cCubicBezierToAbs,x,y,c1x,c1y,c2x,c2y);
        }

        inline void cubicBezierToRel(float x,float y,float c1x,
                                     float c1y,float c2x,float c2y)
        {
            addSegment(cCubicBezierToRel,x,y,c1x,c1y,c2x,c2y);
        }

        inline void smoothCubicBezierToAbs(float x,float y,float cx,float cy)
        {
            addSegment(cSmoothCubicBezierToAbs,x,y,cx,cy);
        }
        inline void smoothCubicBezierToRel(float x,float y,float cx,float cy)
        {
            addSegment(cSmoothCubicBezierToRel,x,y,cx,cy);
        }

        void addSegment(Command command,float a = 0.0f,float b = 0.0f,float c = 0.0f,
                        float d = 0.0f,float e = 0.0f,float f = 0.0f);
        // Draw arcs:
        // CW  = clockwise,
        // CCW = counter clockwise,
        // rh  = horizontal axis,
        // rv  = vertical axis and
        // rot = rotation

        // Small arcs
        inline void smallCWArcToAbs(float x,float y,float rh,float rv,float rot)
        {
            addSegment(cSmallClockwiseArcToAbs,x,y,rh,rv,rot);
        }
        inline void smallCWArcToRel(float x,float y,float rh,float rv,float rot)
        {
            addSegment(cSmallClockwiseArcToRel,x,y,rh,rv,rot);
        }
        inline void smallCCWArcToAbs(float x,float y,float rh,float rv,float rot)
        {
            addSegment(cSmallCounterclockwiseArcToAbs,x,y,rh,rv,rot);
        }
        inline void smallCCWArcToRel(float x,float y,float rh,float rv,float rot)
        {
            addSegment(cSmallCounterclockwiseArcToRel,x,y,rh,rv,rot);
        }

        // Large arcs
        inline void largeCWArcToAbs(float x,float y,float rh,float rv,float rot)
        {
            addSegment(cLargeClockwiseArcToAbs,x,y,rh,rv,rot);
        }
        inline void largeCWArcToRel(float x,float y,float rh,float rv,float rot)
        {
            addSegment(cLargeClockwiseArcToRel,x,y,rh,rv,rot);
        }
        inline void largeCCWArcToAbs(float x,float y,float rh,float rv,float rot)
        {
            addSegment(cLargeCounterclockwiseArcToAbs,x,y,rh,rv,rot);
        }
        inline void largeCCWArcToRel(float x,float y,float rh,float rv,float rot)
        {
            addSegment(cLargeCounterclockwiseArcToRel,x,y,rh,rv,rot);
        }

        // Close the path, line to start point, ends the current subpath
        inline void closePath() {addSegment(cClosePath);}

        // Clear path
        void clearPath();

        // transforms path
        inline void setTransform(const AffineMatrix& matrix)
        {
            transform_ = matrix;
        }
        inline void addTransform(const AffineMatrix& matrix)
        {
            transform_ *= matrix;
        }
        inline void clearTransform()
        {
            transform_.makeIdentity();
        }
        inline const AffineMatrix& getTransform() const
        {
            return transform_;
        }

    private:

        friend class Renderer;
		friend class PointGenerator;

        float deltaLimit_;
        mutable bool segmentsValid_;
        AffineMatrix transform_;
        DynamicArray<Segment> segments_;
    };

	class Segment
	{
	public:

		Segment():segment_(0),startPoint_(0),endPoint_(0),pointIterator_(0),points_(0){}

        Segment(const Path::Segment* segment,
				DynamicArray<float>* points):segment_(segment),
                                             startPoint_(points->getItemCount()),
                                             endPoint_(0),pointIterator_(0),
                                             points_(points){}
		~Segment(){}

		inline const Path::Segment* getSegment() 
		{ 
			return segment_;
		}

        inline vplUint getNumberOfPoints()
        {
            if(endPoint_ > startPoint_)
                return (endPoint_ - startPoint_)/2 - 1;
            else
                return 0;
        }

        // Iterate over points
        inline float* beginPointIteration()
        {
            pointIterator_ = startPoint_;
            return points_->getContents() + pointIterator_;
        }

        inline bool hasMorePoints()
        {
            return (pointIterator_ < endPoint_);
        }

        inline float* getNextPoint()
        {
            pointIterator_ += 2;
			return points_->getContents() + pointIterator_;
        }

        inline bool atSecondToLastPoint()
        {
            return pointIterator_ == (endPoint_ - 4);
        }
	private:

		friend class PointGenerator;

		const Path::Segment* segment_;
		vplUint startPoint_;
        vplUint endPoint_;
        vplUint pointIterator_;
        DynamicArray<float>* points_;
	};

	class SubPath
	{
	public:


        SubPath():startSegment_(0),endSegment_(0),segmentIterator_(0),
                  startPoint_(0),endPoint_(0),pointIterator_(0),
                  points_(0),segments_(0){}

		~SubPath(){}

        // Retrieve points from subpath (for fills)
        inline vplUint getNumberOfPoints()
        {
            if(endPoint_ > startPoint_)
                return (endPoint_ - startPoint_)/2 - 1;
            else
                return 0;
        }

        inline vplUint getNumberOfSegments() 
		{ 
			return endSegment_ - startSegment_;
		}

        inline const float* getPoints()
        {
            return points_->getContents() + startPoint_;
        }

        // Iterate over segments
        inline Segment* beginSegmentIteration()
        {
            segmentIterator_ = startSegment_;
            return segments_->getContents() + segmentIterator_;
        }

        inline bool hasMoreSegments()
        {
            return (segmentIterator_ < endSegment_);
        }

        inline Segment* getNextSegment()
        {
            ++segmentIterator_;
            return segments_->getContents() + segmentIterator_;
        }

        // Iterate over points
        inline float* beginPointIteration()
        {
            pointIterator_ = startPoint_;
            return points_->getContents() + pointIterator_;
        }

        inline bool hasMorePoints()
        {
            return (pointIterator_ < endPoint_);
        }

        inline float* getNextpoint()
        {
            pointIterator_ += 2;
			return points_->getContents() + pointIterator_;
        }

    private:

		friend class PointGenerator;

		vplUint startSegment_;
		vplUint endSegment_;
        vplUint segmentIterator_;
        vplUint startPoint_;
        vplUint endPoint_;
        vplUint pointIterator_;
        DynamicArray<float>* points_;
        DynamicArray<Segment>* segments_;
	};

	class VPL_API PointGenerator
	{
	public:

		PointGenerator();
		~PointGenerator(){}

        void generatePoints(const Path& path,
							const AffineMatrix& transform = AffineMatrix());
        
		inline SubPath* beginSubPathIteration()
        {
            subPathIterator_ = 0;

            return subPaths_.getContents();
        }

		inline bool hasMoreSubPaths()
		{
			if(subPaths_.getItemCount() == 0)
				return false;

			return subPathIterator_ < subPaths_.getItemCount();
		}

		inline SubPath* getNextSubPath()
		{
			return &subPaths_[++subPathIterator_];
		}

        // Point info
		inline float* getPoints()          
		{ 
			return points_.getContents();
		}

        inline vplUint getNumberOfPoints() 
		{ 
			return points_.getItemCount()/2;
		}

	private:

		inline void addPoint(const Vector& p)
		{
            points_.add(p.x_);
            points_.add(p.y_);
		}

        inline void addPoint(float x, float y)
        {
            points_.add(x);
            points_.add(y);
        }

		void moveTo(float x,float y);
        void lineTo(float x,float y);
        void quadBezierTo(float x,float y,float cx,float cy);
		void cubicBezierTo(float x,float y,float c1x,float c1y,float c2x,float c2y);
        void arcTo(float x,float y,float rh,float rv,float rot,bool large,bool cw);
		void generateArc(Vector& from,Vector& to,Vector& center,
			             float rh,float rv,bool cw);
        void subDivideArc(const Vector& from,const Vector& to,const Vector& center,
                          float rh,float rv,float rot,float startAngle,float endAngle);
		void subDivideBezier(const Vector& from,const Vector& control1,
                             const Vector& control2,const Vector& to);
		void finishSubPath();

		DynamicArray<float> points_;
		DynamicArray<Segment> segments_;
		DynamicArray<SubPath> subPaths_;
		float deltaLimit_;
		Vector start_;
		Vector end_;
		Vector lastControl_;
		AffineMatrix transform_;
		vplUint previousCommand_;
		vplUint subPathIterator_;
	};

    const char* commandToString(Path::Command command);
}
#endif
