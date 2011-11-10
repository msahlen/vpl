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

#include "vplRender.h"
#include "vplColorHelper.h"

namespace vpl
{

    Pen::Pen():color_(cBlack),gradient_(0),size_(1.0f),miterLimit_(5.0f),
               endType_(Stroker::cButtEnd),joinType_(Stroker::cMiterJoin),
               blendMode_(cSrcOverDest){}

    Brush::Brush():color_(cBlack),gradient_(0),fillMode_(cEvenOdd),
                   blendMode_(cSrcOverDest){}

    Brush::Brush(Color color):color_(color),gradient_(0),fillMode_(cEvenOdd),
                              blendMode_(cSrcOverDest){}

    const char* toString(Stroker::EndType t)
    {
        static const char* const ends[] = {"butt end","round end","square end"};

        return ends[t];
    }
    const char* toString(Stroker::JoinType t)
    {
        static const char* const joins[] = {"bevel join","round join","miter join"};

        return joins[t];
    }
    const char* toString(Brush::FillMode t)
    {
        static const char* const fills[] = {"even odd fill","non zero fill"};

        return fills[t];
    }
    const char* toString(BlendMode m)
    {
        static const char* const modes[] = {"clear","source","destination",
                                            "source over destination",
                                            "destination over source",
                                            "source in destination",
                                            "destination in source",
                                            "source out destination",
                                            "destination out source",
                                            "source atop destination",
                                            "destination atop source",
                                            "source xor destination"};

        return modes[m];
    }

    VPL_API void drawLine(Path& path,float startX,float startY,
                          float endX,float endY)
    {
        path.moveToAbs(startX,startY);
        path.lineToAbs(endX,endY);
    }

	VPL_API void drawRect(Path& path,float startX,float startY,
                          float width,float height)
    {
        path.moveToAbs(startX,startY);
        path.verticalLineToRel(height);
        path.horizontalLineToRel(width);
        path.verticalLineToRel(-height);
        path.closePath();
    }

    VPL_API void drawRoundCornerRect(Path& path,float startX,float startY,
                                     float width,float height,
                                     float radiusX,float radiusY)
    {
        // Move to start
        path.moveToAbs(startX + radiusX,startY);

        // Walk the path
        path.lineToRel(width - 2*radiusX,0.0f);
        path.smallCCWArcToRel(radiusX,radiusY,radiusX,radiusY,0.0f);
        path.lineToRel(0.0f,height - 2*radiusY);
        path.smallCCWArcToRel(-radiusX,radiusY,radiusX,radiusY,0.0f);
        path.lineToRel(-(width - 2*radiusX),0.0f);
        path.smallCCWArcToRel(-radiusX,-radiusY,radiusX,radiusY,0.0f);
        path.lineToRel(0.0f,-(height - 2*radiusY));
        path.smallCCWArcToRel(radiusX,-radiusY,radiusX,radiusY,0.0f);
        path.closePath();
    }

    VPL_API void drawEllipse(Path& path,float centerX,float centerY,
                             float radiusX,float radiusY)
    {
        path.moveToAbs(centerX - radiusX,centerY);
        path.largeCWArcToAbs(centerX + radiusX,centerY,radiusX,radiusY,0.0f);
        path.largeCWArcToAbs(centerX - radiusX,centerY,radiusX,radiusY,0.0f);
        path.closePath();
    }
}
