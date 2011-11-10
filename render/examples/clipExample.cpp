/* VPL - Vector path library
 * Copyright (C) 2009 - 2011 Enlightenment Developers: mattias.sahlen@gmail.com>
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

#include <cstdio>
#include "vplRender.h"
#include "exampleUtils.h"

using namespace vpl;

int main()
{
    PixelBuffer pixelBuffer;
    Renderer renderer;

	// Initialize renderer
    pixelBuffer.initialize(400,400);
    pixelBuffer.clear(cWhite);

    renderer.setTarget(&pixelBuffer);

    Path path;
    path.moveToAbs(150.0f,150.0f);
    path.smallCWArcToRel(200.0f,0.0f,100.0f,100.0f,0.0f);
    path.closePath();

    path.moveToAbs(25.6f,230.0f);
    path.cubicBezierToAbs(230.4f,230.0f,102.4f,332.4f,153.6f,127.6f);

    Brush brush;
    brush.setColor(cGreenYellow);
    brush.setFillMode(Brush::cNonZero);

    Pen pen;
    pen.setColor(cBlack);
    pen.setSize(20.0f);
    pen.setJoinType(Stroker::cRoundJoin);
    pen.setEndType(Stroker::cRoundEnd);

    renderer.addPath(&path,&pen,&brush);
    renderer.draw();

    showImage(pixelBuffer,"VPL Clip example, unclipped","");

    Rect clipRect(150.0f,400.0f,200.0f,0.0f);
    renderer.setClipRect(clipRect);

    pixelBuffer.clear(cWhite);

    renderer.draw();

    showImage(pixelBuffer,"VPL Clip example, clipped","");

	return 0;

}
