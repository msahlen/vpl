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

#include "vplBlend.h"

namespace vpl
{
    inline int round(float f)
    {
        return f >= 0.0f ? int(f + 0.5f) : int(f - int(f - 1) + 0.5f) + int(f - 1);
    }
    static void generateGradientColorTable(vplUint32* colorTable,
                                           Gradient::Stop* stops,
                                           vplUint stopCount)
    {
        // Input validation
        if(stopCount == 0)
			return;
      
        vplUint32 color = preMultiplyColorRGBA(stops[0].color_.getColorRGBA());
        
        // Easy case when we only have one stop
        if(stopCount == 1)
        {
            for(int i = 0; i < cGradientColorTableSize;i++)
                colorTable[i] = color;
            
            return;
        }
        
        // The general case
        vplUint ctIndex  = 0;
        vplUint currStop = 0;
        float startPos = stops[0].offset_;
        float endPos = stops[stopCount - 1].offset_;
        float increment = 1.0f/cGradientColorTableSize;
        float currPos = increment * 1.5f;
        
        // Fill table up to first stop
        colorTable[ctIndex++] = color;
        
        while(currPos <= startPos)
        {
            colorTable[ctIndex++] = color;
            
            currPos += increment;
        }
        
        // Interpolate
        vplUint32 nextColor;
        
        if(currPos < endPos)
        {
            // Move to correct stop
            while(currPos > stops[currStop + 1].offset_)
                currStop++;
            
            if(currStop != 0)
                color = preMultiplyColorRGBA(stops[currStop].color_.getColorRGBA());
            
            nextColor = preMultiplyColorRGBA(stops[currStop + 1].color_.getColorRGBA());
            
            float diff   = stops[currStop+1].offset_ - stops[currStop].offset_;
            float c      = (diff == 0) ? 0.0f : 255.0f / diff;
            float t      = (currPos - stops[currStop].offset_) * c;
            float tDelta = increment * c;
            
            while(true)
            {
                int dist    = round(t);
                int invDist = 255 - dist;
                
                colorTable[ctIndex++] = interpolatePixel(color,invDist,
                                                        nextColor,dist);
                
				currPos += increment;                                             
                
				if(currPos >= endPos)
                    break;
                
                t += tDelta;
                
                int skip = 0;
                
                while(currPos > stops[currStop + skip + 1].offset_)
                    ++skip;
                
                if(skip != 0)
                {
                    currStop += skip;
                    
                    if(skip == 1)
                        color = nextColor;
                    else
                        color = preMultiplyColorRGBA(stops[currStop].color_.getColorRGBA());
                    
                    nextColor = preMultiplyColorRGBA(stops[currStop + 1].color_.getColorRGBA());
                    diff      = stops[currStop + 1].offset_ - stops[currStop].offset_;
                    c         = (diff == 0.0f) ? 0.0f : 255.0f / diff;
                    t         = (currPos - stops[currStop].offset_) * c;
                    tDelta    = increment * c;
                }
            }
        }
        
        // After last point
        color = preMultiplyColorRGBA(stops[stopCount - 1].color_.getColorRGBA());
        
        while (ctIndex < cGradientColorTableSize - 1)
            colorTable[ctIndex++] = color;
        
        // Make sure the last color stop is represented at the end of the table
        colorTable[cGradientColorTableSize - 1] = color;
    }
    
    static inline vplUint32 fetchGradientPixel(vplUint32* colorTable,
                                              float pos,
                                              Gradient::Spread spread)
    {
		int iPos = int(pos * (cGradientColorTableSize - 1) + 0.5f);
       
        if(iPos < 0 || iPos >= cGradientColorTableSize)
        {
            if(spread == Gradient::cReflect)
            { 
                const int limit = cGradientColorTableSize * 2 - 1;
                iPos = iPos % limit;
                iPos = iPos < 0 ? limit + iPos : iPos;
                iPos = iPos >= cGradientColorTableSize ? limit - iPos : iPos;
            }
            else if(spread == Gradient::cRepeat)
            {
                iPos = iPos % cGradientColorTableSize;
                iPos = iPos < 0 ? cGradientColorTableSize + iPos : iPos;   
            }
            else
            {
                if(iPos < 0)
                    iPos = 0;
                else if(iPos >= cGradientColorTableSize)
                    iPos = cGradientColorTableSize - 1;
            }
        }
        
        return colorTable[iPos];
    }
	
	// Intersection of a ray and a circle. Parametrize the ray as
	// i(t) = f + (p-f)*t
	// dot_product(i-c,i-c) = radius^2
	// dot_product((f-c) + (p-f)*t, (f-c) + (p-f)*t) = radius^2
	// dot_product(p-f,p-f)*t^2 + 2*dot_product(f-c,p-f)*t + dot_product(f-c,f-c) - radius^2 = 0
	// This gives the follwing pseudo code
	// float a = Vector2.Dot(currentPoint - focus, currentPoint - focus);
	// float b = 2 * Vector2.Dot(focus-centre, currentPoint-focus);
	// float c = Vector2.Dot(focus-centre, focus-centre)- (radius * radius);
    // float t = (-b + (float)Math.Sqrt((b*b) - 4*a*c)) / (2*a);
	// float lerp = 1/t;
    
	void fetchRadialGradient(BlendData* gradient,vplUint x,vplUint y,vplUint length)
    {
		Vector focalPoint(gradient->gradientData_.RadialGradientData.fx_,
                          gradient->gradientData_.RadialGradientData.fy_);
                          
		Vector centre(gradient->gradientData_.RadialGradientData.cx_,
                      gradient->gradientData_.RadialGradientData.cy_);

        Vector currentPoint(x + 0.5f,y + 0.5f);
        Vector inc(1.0f,0.0f);
                  
        vplUint32* dest = gradient->gradientPixels_;
        vplUint32* end  = dest + length;
        
		float c = gradient->gradientValues_.RadialGradientValues.c_;

        while(dest < end)
        { 
            float a = computeDotProduct(currentPoint - focalPoint,
                                        currentPoint - focalPoint);
                                        
            float b = 2 * computeDotProduct(focalPoint - centre,
                                            currentPoint - focalPoint);
                                                                        
            float pos = 1.0f / ((-b + sqrtf((b*b) - 4*a*c)) / (2*a));

			*dest++ = fetchGradientPixel(gradient->colorTable_,
                                         pos,gradient->spread_);
           currentPoint += inc;
        }
    }
    
    void fetchLinearGradient(BlendData* gradient,vplUint x,vplUint y,vplUint length)
    {
        float xPos = x + 0.5f;
        float yPos = y + 0.5f;
        float inc  = (gradient->gradientValues_.LinearGradientValues.deltaX_ + \
                      gradient->gradientValues_.LinearGradientValues.deltaY_) /\
                      gradient->gradientValues_.LinearGradientValues.length_;
                      
        float pos  = gradient->gradientValues_.LinearGradientValues.deltaX_ * xPos + \
                     gradient->gradientValues_.LinearGradientValues.deltaY_ * yPos + \
                     gradient->gradientValues_.LinearGradientValues.offset_;
        
        pos /= gradient->gradientValues_.LinearGradientValues.length_;
        
        vplUint32* dest = gradient->gradientPixels_;
        vplUint32* end  = dest + length;
        
        if(gradient->gradientValues_.LinearGradientValues.length_ == 0.0f)
            pos = inc = 0.0f;
        
        while(dest < end)
        { 
            *dest++ = fetchGradientPixel(gradient->colorTable_,
                                         pos,gradient->spread_);
            pos += inc;
        } 
    }
    
    void generateGradientData(BlendData& data)
    {
        
        generateGradientColorTable(data.colorTable_,
                                   data.stops_.getContents(),
                                   data.stops_.getItemCount());
                                   
        if(data.type_ == Gradient::cLinear)
        {
            float x = data.gradientData_.LinearGradientData.dirX_;
            float y = data.gradientData_.LinearGradientData.dirY_;  
            float length = sqrt(x*x + y*y);
            float deltaX;
            float deltaY;
            
            if(length != 0.0f)
            {
                deltaX = x / length;
                deltaY = y / length;
                
                x = data.gradientData_.LinearGradientData.x_;
                y = data.gradientData_.LinearGradientData.y_; 
                
                data.gradientValues_.LinearGradientValues.deltaX_ = deltaX;
                data.gradientValues_.LinearGradientValues.deltaY_ = deltaY;
                data.gradientValues_.LinearGradientValues.length_ = length;
                data.gradientValues_.LinearGradientValues.offset_ = -deltaX * x - deltaY * y;
            }
            
            data.fetch_ = fetchLinearGradient;
        }
        else if(data.type_ == Gradient::cRadial)
        { 
			Vector focalPoint(data.gradientData_.RadialGradientData.fx_,
                              data.gradientData_.RadialGradientData.fy_);
                          
			Vector centre(data.gradientData_.RadialGradientData.cx_,
                          data.gradientData_.RadialGradientData.cy_);
			
			// Validate input, make sure focal point is inside circle
            Vector diff = focalPoint - centre;
            float radius = data.gradientData_.RadialGradientData.radius_;               
			float length = diff.computeLength() ;
            
            if(length >= radius)
            {
                diff *= 0.999f * radius / length;
                focalPoint = centre + diff;
            }
            
            float r2 = radius * radius;			
			float c = computeDotProduct(focalPoint - centre,
                                                      focalPoint - centre) - r2;
			
            data.gradientValues_.RadialGradientValues.c_ = c;
            data.fetch_ = fetchRadialGradient;
        }
    }
}