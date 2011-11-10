/* VPL - Vector path library
 * Copyright (C) 2009 - 2011 Mattias Sahlén: <mattias.sahlen@gmail.com>
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

#include <string>
#include "vplRender.h"
#include "exampleUtils.h"

using namespace vpl;

int main()
{
    PixelBuffer pixelBuffer;
    PixelBuffer finalBuffer;
    Renderer renderer;

    // Initialize renderer
    pixelBuffer.initialize(700,250);
    pixelBuffer.clear();

    finalBuffer.initialize(700,250);
    finalBuffer.clear(cWhite);

    Path blueRect;

    drawRect(blueRect,50.0f,50.0f,100.0f,100.0f);
    drawRect(blueRect,350.0f,50.0f,100.0f,100.0f);
    
    Path transparentBlueRect;

    drawRect(transparentBlueRect,200.0f,50.0f,100.0f,100.0f);
    drawRect(transparentBlueRect,500.0f,50.0f,100.0f,100.0f);

    Path redRect;

    drawRect(redRect,100.0f,100.0f,100.0f,100.0f);
    drawRect(redRect,250.0f,100.0f,100.0f,100.0f);

    Path transparentRedRect;

    drawRect(transparentRedRect,400.0f,100.0f,100.0f,100.0f);
    drawRect(transparentRedRect,550.0f,100.0f,100.0f,100.0f);
    
    Brush redBrush(cRed);
    Brush blueBrush(cBlue);
    Brush transparentRedBrush(Color(128,255,0,0));
    Brush transparentBlueBrush(Color(128,0,0,255));

    std::string fileName;
    
    for(vplUint i = 0; i < cSrcXorDest + 1;i++)
    {
        renderer.setTarget(&pixelBuffer);
        renderer.addPath(&blueRect,&blueBrush);
        renderer.addPath(&transparentBlueRect,&transparentBlueBrush);

        redBrush.setBlendMode((BlendMode)i);
        transparentRedBrush.setBlendMode((BlendMode)i);

        renderer.addPath(&redRect,&redBrush);
        renderer.addPath(&transparentRedRect,&transparentRedBrush);
		
		renderer.draw();
        
		renderer.setTarget(&finalBuffer);
        renderer.compose(pixelBuffer,cSrcOverDest,0,0);
        
        fileName =  "blend example ";
        fileName += toString(redBrush.getBlendMode());
        fileName += ".png";
        
        saveImage(pixelBuffer,fileName);
        
        pixelBuffer.clear();
        finalBuffer.clear(cWhite);
		renderer.clearPaths();
    }
    
    return 0;
}
