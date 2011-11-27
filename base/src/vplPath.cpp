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
#include "vplPath.h"

#ifdef USE_SSE_

extern "C" void PRE_CDECL_ estimateFlatness(float* bezier,float* deltas) POST_CDECL_;

#define DECLARE_ARRAY(a) MEM_ALIGN(a,16)
#define ALIGN_ARRAY(b) b.align(cAlign16)

#else

inline static void estimateFlatness(float* bezier,float* deltas)
{
    deltas[0] = 3.0f*bezier[2] - 2.0f*bezier[0] - bezier[6];
    deltas[0]*= deltas[0];
    deltas[1] = 3.0f*bezier[3] - 2.0f*bezier[1] - bezier[7];
    deltas[1]*= deltas[1];
    deltas[2] = 3.0f*bezier[4] - 2.0f*bezier[6] - bezier[0];
    deltas[2]*= deltas[2];
    deltas[3] = 3.0f*bezier[5] - 2.0f*bezier[7] - bezier[1];
    deltas[3]*= deltas[3];
}

#define DECLARE_ARRAY(a) a
#define ALIGN_ARRAY(b)

#endif // USE_SSE__

namespace vpl
{
	const char* commandToString(Path::Command command)
	{
		static const char* const cmds[] = {"Close path",
                                           "Move to, abs",
						                   "Move to, rel",
                                           "Line to, abs",
                                           "Line to, rel",
                                           "Horizontal line to, abs",
                                           "Horizontal line to, rel",
                                           "Vertical line to, abs",
                                           "Vertical line to, rel",
                                           "Quadratic bezier to, abs",
                                           "Quadratic bezier to, rel",
                                           "Cubic bezier to, abs",
                                           "Cubic bezier to, rel",
                                           "Smooth quadratic bezier to,abs",
                                           "Smooth quadratic bezier to,rel",
                                           "Smooth cubic bezier to,abs",
                                           "Smooth cubic bezier to,rel",
                                           "Small counter clockwise arc to, abs",
                                           "Small counter clockwise arc to, rel" ,
                                           "Small clockwise arc to, abs"  ,
                                           "Small clockwise arc to, rel"   ,
                                           "Large counter clockwise arc to, abs",
                                           "Large counter clockwise arc to, rel",
                                           "Large clockwise arc to, abs",
                                           "Large clockwise arc to, abs",
                                           "Unknown command!"};

        switch(command)
        {
            case Path::cClosePath:
                return cmds[0];
            case Path::cMoveToAbs:
                return cmds[1];
            case Path::cMoveToRel:
                return cmds[2];
            case Path::cLineToAbs:
                return cmds[3];
             case Path::cLineToRel:
                 return cmds[4];
            case Path::cHorizontalLineToAbs:
                return cmds[5];
            case Path::cHorizontalLineToRel:
                return cmds[6];
            case Path::cVerticalLineToAbs:
                return cmds[7];
            case Path::cVerticalLineToRel:
                return cmds[8];
             case Path::cQuadBezierToAbs:
                return cmds[9];
             case Path::cQuadBezierToRel:
                return cmds[10];
             case Path::cCubicBezierToAbs:
                return cmds[11];
             case Path::cCubicBezierToRel:
                return cmds[12];
             case Path::cSmoothQuadBezierToAbs:
                return cmds[13];
             case Path::cSmoothQuadBezierToRel:
                return cmds[14];
             case Path::cSmoothCubicBezierToAbs:
                return cmds[15];
             case Path::cSmoothCubicBezierToRel:
                return cmds[16];
             case Path::cSmallCounterclockwiseArcToAbs:
                return cmds[17];
             case Path::cSmallCounterclockwiseArcToRel:
                return cmds[18];
             case Path::cSmallClockwiseArcToAbs:
                return cmds[19];
             case Path::cSmallClockwiseArcToRel:
                return cmds[20];
             case Path::cLargeCounterclockwiseArcToAbs:
                return cmds[21];
             case Path::cLargeCounterclockwiseArcToRel:
                return cmds[22];
             case Path::cLargeClockwiseArcToAbs:
                return cmds[23];
             case Path::cLargeClockwiseArcToRel:
                return cmds[24];
             default:
                return cmds[25];
        }
	}

	static bool isLarge(Path::Command command)
	{
		return (command & (Path::cLargeClockwiseArcSegment   |
                           Path::cLargeCounterclockwiseArcSegment)) ==
                          (Path::cLargeClockwiseArcSegment   |
                           Path::cLargeCounterclockwiseArcSegment);
	}

	static bool isClockwise(Path::Command command)
	{
		return (command & (Path::cLargeClockwiseArcSegment   |
                           Path::cSmallClockwiseArcSegment)) ==
                          (Path::cLargeClockwiseArcSegment   |
                           Path::cSmallClockwiseArcSegment);
	}

	static bool isLine(Path::Command command)
	{
		vplUint segment = command & 0x1e;

		    return (segment == Path::cLineSegment)         |
                   (segment == Path::cVerticalLineSegment) |
                   (segment == Path::cHorizontalLineSegment);

	}

	static bool isArc(Path::Command command)
	{
		vplUint segment = command & 0x1e;

		return (segment == Path::cLargeClockwiseArcSegment)        |
               (segment == Path::cSmallClockwiseArcSegment)        |
               (segment == Path::cLargeCounterclockwiseArcSegment) |
			   (segment == Path::cSmallCounterclockwiseArcSegment);
	}

    void Path::addSegment(Command command,float a,float b,
                           float c,float d,float e,float f)
    {
        Segment segment;

        segment.command_ = command;

        segment.args_[0] = a;
        segment.args_[1] = b;
        segment.args_[2] = c;
        segment.args_[3] = d;
        segment.args_[4] = e;
        segment.args_[5] = f;

        segments_.add(segment);

		if(command != cMoveToAbs && command != cMoveToRel)
            segmentsValid_ = false;
    }

    void Path::clearPath()
    {
        transform_.makeIdentity();

        segments_.clear();
    }

	PointGenerator::PointGenerator():deltaLimit_(0.25f),previousCommand_(0),
			                         subPathIterator_(0)
	{
		ALIGN_ARRAY(points_);
	}

	void PointGenerator::generatePoints(const Path& path,const AffineMatrix& transform)
	{
        // Apply correct transform
		transform_ = path.getTransform();
		transform_ *= transform;

        // Clear old data
		points_.clear();
        subPaths_.clear();
		segments_.clear();

        // Reset state variables
        start_ = end_ = lastControl_ = Vector(0.0f,0.0f);

		const Path::Segment* segments = path.segments_.getContents();

        vplUint command;

        bool rel;

		if(path.segments_.getItemCount())
        {
			if(segments[0].command_ != Path::cMoveToAbs &&
               segments[0].command_ != Path::cMoveToRel)
			{
				SubPath subPath;

                subPath.startSegment_ = 0;
                subPath.startPoint_ = 0;
                subPath.points_ = &points_;
                subPath.segments_ = &segments_;

                subPaths_.add(subPath);
			}
        }

        for(vplUint i = 0; i < path.segments_.getItemCount() ; i++)
        {
            // Store segment
            if(segments_.getItemCount())
                segments_.getLast().endPoint_ = points_.getItemCount();

			Segment segment(segments,&points_);

			segments_.add(segment);

            // Determine if command is absolute or relative
            command = segments->command_ & 0x1e;
			rel = (segments->command_ & 0x1) == Path::cRelative;

            // Handle close command
            if(command == Path::cClosePath)
            {
                if(previousCommand_ != Path::cMoveSegment)
                    finishSubPath();
                else
                    subPaths_.clear();
            }

            // Handle line tos
            else if(isLine(segments->command_))
            {
                if(command == Path::cLineSegment)
                {
                    if(rel)
                        lineTo(end_.x_ + segments->args_[0],end_.y_ + segments->args_[1]);
                    else
                        lineTo(segments->args_[0],segments->args_[1]);
                }

                if(command == Path::cVerticalLineSegment)
                {
                    if(rel)
                        lineTo(end_.x_ ,end_.y_ + segments->args_[0]);
                    else
                        lineTo(end_.x_,segments->args_[0]);
                }

                if(command == Path::cHorizontalLineSegment)
                {
                    if(rel)
                        lineTo(end_.x_ + segments->args_[0] ,end_.y_);
                    else
                        lineTo(segments->args_[0],end_.y_);
                }
            }

            // Handle move tos
            else if(command == Path::cMoveSegment)
            {
                if(rel)
					moveTo(end_.x_ + segments->args_[0],end_.y_ + segments->args_[1]);
                else
                    moveTo(segments->args_[0],segments->args_[1]);
            }

            // Handle quadratic beziers
            else if(command == Path::cQuadBezierSegment)
            {
                if(rel)
                {
                    quadBezierTo(end_.x_ + segments->args_[0],
								 end_.y_ + segments->args_[1],
                                 end_.x_ + segments->args_[2],
                                 end_.y_ + segments->args_[3]);
                }
                else
                {
                    quadBezierTo(segments->args_[0],segments->args_[1],
                                 segments->args_[2],segments->args_[3]);
                }
            }

            // Handle smooth quadratic beziers
            else if(command == Path::cSmoothQuadBezierSegment)
            {
                // Generate internal control point
                float cx = (end_.x_*2 - lastControl_.x_);
                float cy = (end_.y_*2 - lastControl_.y_);

                if(rel)
                {
                    quadBezierTo(end_.x_ + segments->args_[0],
                                 end_.y_ + segments->args_[1],cx,cy);
                }
                else
                {
                    quadBezierTo(segments->args_[0],
                                 segments->args_[1],cx,cy);
                }
            }

            // Handle cubic beziers
            else if(command == Path::cCubicBezierSegment)
            {
                if(rel)
                {
                    cubicBezierTo(end_.x_ + segments->args_[0],
                                  end_.y_ + segments->args_[1],
                                  end_.x_ + segments->args_[2],
                                  end_.y_ + segments->args_[3],
                                  end_.x_ + segments->args_[4],
                                  end_.y_ + segments->args_[5]);
                }
                else
                {
                    cubicBezierTo(segments->args_[0],segments->args_[1],
                                  segments->args_[2],segments->args_[3],
                                  segments->args_[4],segments->args_[5]);
                }
            }

            // Handle smooth cubic beziers
            else if(command == Path::cSmoothCubicBezierSegment)
            {
                // Generate internal control point
                float cx = end_.x_*2 - lastControl_.x_;
                float cy = end_.y_*2 - lastControl_.y_;

                if(rel)
                {
                    cubicBezierTo(end_.x_ + segments->args_[0],
                                  end_.y_ + segments->args_[1],
                                  cx,cy,
                                  end_.x_ + segments->args_[2],
                                  end_.y_ + segments->args_[3]);
                }
                else
                {
                    cubicBezierTo(segments->args_[0],segments->args_[1],cx,
                                  cy,segments->args_[2],segments->args_[3]);
                }
            }

            // Handle arcs
            else if(isArc(segments->command_))
            {
                bool large = isLarge(segments->command_);
                bool cw = isClockwise(segments->command_);

                if(rel)
                {
                    arcTo(end_.x_ + segments->args_[0],
                          end_.y_ + segments->args_[1],
                          segments->args_[2],segments->args_[3],
                          segments->args_[4],large,cw);
                }
                else
                {
                    arcTo(segments->args_[0],segments->args_[1],
                          segments->args_[2],segments->args_[3],
                          segments->args_[4],large,cw);
                }
            }

			segments++;
            previousCommand_ = command;
        }

        // Flush last point
		if(command != Path::cClosePath)
		{
            Vector end = end_;

			transform_.transform(end);

			addPoint(end);
		}

		// Fix subpaths
		if(subPaths_.getItemCount() > 0)
        {
			subPaths_.getLast().endSegment_ = segments_.getItemCount();
            subPaths_.getLast().endPoint_ = points_.getItemCount();
            segments_.getLast().endPoint_ = points_.getItemCount();
        }
    }

	void PointGenerator::moveTo(float x,float y)
    {
		if(points_.getItemCount() && previousCommand_ != Path::cClosePath)
		{
			Vector end = end_;

			transform_.transform(end);

			addPoint(end);
		}

        if(subPaths_.getItemCount())
        {
            SubPath& previous = subPaths_.getLast();

            previous.endSegment_ = segments_.getItemCount() - 1;
            previous.endPoint_   = points_.getItemCount();
        }

        SubPath next;

        next.startSegment_ = segments_.getItemCount() - 1;
        next.startPoint_ = points_.getItemCount();
        next.points_ = &points_;
        next.segments_ = &segments_;

        subPaths_.add(next);

        // Update internal reference points
        start_ = end_ = lastControl_ = Vector(x,y);
    }
    void PointGenerator::lineTo(float x,float y)
    {
        // Generate point
        Vector end = end_;

		transform_.transform(end);

		addPoint(end);

        // Update internal reference points
        end_ = lastControl_ = Vector(x,y);
    }
    void PointGenerator::arcTo(float x,float y,float rh,float rv,
                               float rot,bool large,bool cw)
    {
        // If same as current point
        if(end_.x_ == x && end_.y_ == y)
            return;

       // Treat as line?
       if(rh < cEpsilon || rv < cEpsilon )
       {
           lineTo(x,y);
           return;
       }

       // Use absolute values if negative
        if(rv < 0.0f)
            rv = -rv;
        if(rv < 0.0f)
            rv = -rv;

        // Convert to center parameterization

        // Transform start and end points into unit space
        // by rotating and scaling
        AffineMatrix mat;
        mat.rotate(-rot);
        mat.scale(1.0f/rh,1.0f/rv);

        Vector startPrime(end_.x_,end_.y_);
        Vector endPrime(x,y);

        mat.transform(startPrime);
        mat.transform(endPrime);

        // Compute difference and average
        Vector diffPrime    = startPrime - endPrime;
        Vector averagePrime = (startPrime + endPrime)/2;

        // Solve for intersecting unit circles
        float dsq = diffPrime.x_*diffPrime.x_;
        dsq      += diffPrime.y_*diffPrime.y_;

        // Points are coincident?
        if(dsq == 0.0)
            return;

        float disc = 1.0f/dsq - 1.0f/4.0f;

        // Points are too far apart?
        if(disc < 0.0)
            return;

        // Compute two possible centers
        float s   = sqrt(disc);
        Vector sd = diffPrime*s;

        Vector c0Prime(averagePrime.x_ + sd.y_,
                       averagePrime.y_ - sd.x_);
        Vector c1Prime(averagePrime.x_ - sd.y_,
                       averagePrime.y_ + sd.x_);

        // Choose correct center
        Vector center;

        if((!large && cw) || (large && !cw))
            center = c1Prime;
        else
            center = c0Prime;

        // Transform back
        AffineMatrix invMat;
        invMat.rotate(rot);
        invMat.scale(rh,rv);

        invMat.transform(center);

        // Calculate vectors toward center
        Vector to(x,y);

        Vector v1 = end_ - center;
        Vector v2 = to  - center;

        // Compute start and end angle for vectors
        float startAngle = computeOrientation(v1);
        float diff;
        if(large)
            diff = 2*cPi - computeAngle(v1,v2);
        else
            diff = computeAngle(v1,v2);

        // Clock wise?
        float endAngle;
        if(cw)
            endAngle = startAngle - diff;
        else
            endAngle = startAngle + diff;

        // Generate points
        Vector newEnd(x,y);

        subDivideArc(end_,newEnd,center,rh,rv,rot,startAngle,endAngle);

        // Update control points
        end_ = lastControl_ = newEnd;
    }
    void PointGenerator::quadBezierTo(float x,float y,float cx,float cy)
    {
        // Generate control points
        Vector p(x,y);
        Vector ctrl(cx,cy);

        Vector c1 = end_ + (ctrl - end_) * 2 / 3;
        Vector c2 = p + (ctrl - p) * 2 / 3;

        // Generate points
        subDivideBezier(end_,c1,c2,p);

        // Update internal reference points
        end_         = p;
        lastControl_ = c2;
    }
    void PointGenerator::cubicBezierTo(float x,float y,float c1x,
                                       float c1y,float c2x,float c2y)
    {
        // Generate control points
        Vector p(x,y);
        Vector c1(c1x,c1y);
        Vector c2(c2x,c2y);

        //Generate points
        subDivideBezier(end_,c1,c2,p);

        // Update internal reference points
        end_         = p;
        lastControl_ = c2;
    }
    void PointGenerator::finishSubPath()
    {
        // Generate points
		Vector end = end_;
		Vector start = start_;

		transform_.transform(end);
		transform_.transform(start);

		addPoint(end);

        if(end_ != start_)
            addPoint(start);

        // Update internal reference points
        end_ = lastControl_ = start_;
    }

    // The max recurse depth
    static const char cMaxRecurseStackSize = 32;

    // A bezier segment looks like this:
    // BezierSegment
    //
    //    Vector p1;
    //    Vector p2;
    //    Vector p3;
    //    Vector p4;
    //
    // I use an array of 8 floats instead since it is
    // easier to pass to the SSE function


    void PointGenerator::subDivideBezier(const Vector& from,const Vector& control1,
                                         const Vector& control2,const Vector& to)
    {
		Vector newFrom = from;
		Vector newControl1 = control1;
        Vector newControl2 = control2;
		Vector newTo = to;

		transform_.transform(newFrom);
		transform_.transform(newTo);
		transform_.transform(newControl1);
		transform_.transform(newControl2);

        // IF we use SSE, data is aligned via this macro
        DECLARE_ARRAY(float bezier[8*cMaxRecurseStackSize]);
        DECLARE_ARRAY(float deltas[4]);

        // Pointer into stack
        float* currentBez;
        float* rightBez;
        float* leftBez;

        // Temporary variables for calculating midpoints of new beziers
        float p23x;
        float p23y;
        float p34x;
        float p34y;
        float p234x;
        float p234y;

        // Load first bezier
        bezier[0] = newFrom.x_;
        bezier[1] = newFrom.y_;
        bezier[2] = newControl1.x_;
        bezier[3] = newControl1.y_;
        bezier[4] = newControl2.x_;
        bezier[5] = newControl2.y_;
        bezier[6] = newTo.x_;
        bezier[7] = newTo.y_;

        int bezIndex = 0;

        // Add very first point
        addPoint(newFrom);

        while(bezIndex >= 0)
        {
            // Set current bezier
            currentBez = bezier + bezIndex*8;

            // Call SSE subdivison
            estimateFlatness(bezier + bezIndex*8,deltas);

            // Choose max
            if(*deltas < *(deltas+2))
                *deltas = *(deltas+2);
            if(*(deltas+1) < *(deltas+3))
                *(deltas+1) = *(deltas+3);

            // Stop if flat enough
            if(*deltas + *(deltas+1) <= deltaLimit_ ||
                bezIndex == (cMaxRecurseStackSize - 1))
            {
                if(bezIndex)
                    addPoint(*(currentBez+6),*(currentBez+7));

                bezIndex--;
            }
            else
            {
                // Put left subdivision on top of stack
                // and increse stack size
                rightBez = bezier + 8*bezIndex;
                bezIndex++;
                leftBez  = bezier + 8*bezIndex;

                // Calculate the mid points
                *leftBez      = *currentBez;
                *(leftBez+1)  = *(currentBez+1);
                *(leftBez+2)  = (*currentBez + *(currentBez+2))/2.0f;
                *(leftBez+3)  = (*(currentBez+1) + *(currentBez+3))/2.0f;
                p23x          = (*(currentBez+2) + *(currentBez+4))/2.0f;
                p23y          = (*(currentBez+3) + *(currentBez+5))/2.0f;
                *(leftBez+4)  = (*(leftBez+2) + p23x)/2.0f;
                *(leftBez+5)  = (*(leftBez+3) + p23y)/2.0f;
                p34x          = (*(currentBez+4) + *(currentBez+6))/2.0f;
                p34y          = (*(currentBez+5) + *(currentBez+7))/2.0f;
                p234x         = (p23x + p34x)/2.0f;
                p234y         = (p23y + p34y)/2.0f;
                *(leftBez+6)  = (*(leftBez+4) + p234x)/2.0f;
                *(leftBez+7)  = (*(leftBez+5) + p234y)/2.0f;
                *rightBez     = *(leftBez+6);
                *(rightBez+1) = *(leftBez+7);
                *(rightBez+2) = p234x;
                *(rightBez+3) = p234y;
                *(rightBez+4) = p34x;
                *(rightBez+5) = p34y;
                *(rightBez+6) = *(currentBez+6);
                *(rightBez+7) = *(currentBez+7);
            }
        }
    }

    // Arc segment
    struct ArcSegment
    {
        Vector start;
        Vector end;
        float startAngle;
        float endAngle;
    };

    void PointGenerator::subDivideArc(const Vector& from,const Vector& to,
                                      const Vector& center,float rh,float rv,float rot,
                                      float startAngle,float endAngle)
    {
        ArcSegment arcStack[cMaxRecurseStackSize];
        ArcSegment* currentArc;
        ArcSegment* rightArc;
        ArcSegment* leftArc;

        int arcIndex = 0;

        float middleAngle = 0.0f;
        float cosAngle    = 0.0f;
        float sinAngle    = 0.0f;
        float deltaX      = 0.0f;
        float deltaY      = 0.0f;
        float horizOffset = 0.0f;
        float vertOffset  = 0.0f;
        float cosRot      = cos(rot);
        float sinRot      = sin(rot);

        Vector midPoint;
        Vector newPoint;

        // Initialize first segment
        arcStack[0].start      = from;
        arcStack[0].end        = to;
        arcStack[0].startAngle = startAngle;
        arcStack[0].endAngle   = endAngle;

        // Add first point
        Vector transformedPoint;
        transform_.transform(from,transformedPoint);

        addPoint(transformedPoint);

        while(arcIndex >= 0)
        {
            // Select arc from stack
            currentArc = &arcStack[arcIndex];

            // Grab the middle angle and its cos/sin
            middleAngle = (currentArc->startAngle + currentArc->endAngle)/2;
            cosAngle    = cos(middleAngle);
            sinAngle    = sin(middleAngle);

            // Calculate new point
            horizOffset = rh*cosAngle*cosRot - rv*sinAngle*sinRot;
            vertOffset  = rh*cosAngle*sinRot + rv*sinAngle*cosRot;

            newPoint.x_ = center.x_ + horizOffset;
            newPoint.y_ = center.y_ + vertOffset;

            // How far is it from the linear midpoint?
            midPoint = currentArc->start + (currentArc->end - currentArc->start)/2;

            deltaX = newPoint.x_ - midPoint.x_;
            deltaY = newPoint.y_ - midPoint.y_;

            if(deltaX < 0.0f)
                deltaX = -deltaX;
            if(deltaY < 0.0f)
                deltaY = -deltaY;

            // Enough?
            if(deltaX + deltaY <= deltaLimit_ ||
               std::fabs(currentArc->startAngle - currentArc->endAngle) < cDegree)
            {
              // Add middle and end points
              arcIndex--;

              transform_.transform(newPoint,transformedPoint);

              addPoint(transformedPoint);

              if(arcIndex >= 0)
              {
                  transform_.transform(currentArc->end,transformedPoint);

                  addPoint(transformedPoint);
              }
            }
            else
            {
              // Update stack
              rightArc = currentArc;
              leftArc  = &arcStack[++arcIndex];

              leftArc->start      = currentArc->start;
              leftArc->startAngle = currentArc->startAngle;
              leftArc->end        = newPoint;
              leftArc->endAngle   = middleAngle;

              rightArc->start      = newPoint;
              rightArc->startAngle = middleAngle;
              rightArc->end        = currentArc->end;
              rightArc->endAngle   = currentArc->endAngle;
            }
        }
    }
}
