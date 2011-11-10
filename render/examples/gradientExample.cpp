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
    pixelBuffer.initialize(600,400);
    pixelBuffer.clear(cWhite);

    renderer.setTarget(&pixelBuffer);

    Path path1;
	Path path2;
	Path path3;
    Brush brush;
    RadialGradient gradient(150.0f,150.0f,75.0f,75.0f,100.0f);
    
    drawRect(path1,25.0f,25.0f,200.0f,200.0f);
    
    gradient.addStop(0.0f,cBlue);
    gradient.addStop(0.33f,cRed);
    gradient.addStop(0.66f,cGreen);
    gradient.setSpread(Gradient::cPad);
    
    brush.setGradient(&gradient);

    renderer.addPath(&path1,&brush);
    
    drawRect(path2,250.0f,25.0f,200.0f,200.0f);
    
    gradient.setSpread(Gradient::cReflect);

    renderer.addPath(&path2,&brush);
    
    drawRect(path3,10.0f,250.0f,300.0f,200.0f);
    
    gradient.setSpread(Gradient::cRepeat);

    renderer.addPath(&path3,&brush);
    
	renderer.draw();

	showImage(pixelBuffer,"VPL Gradient example","");

    return 0;

}
