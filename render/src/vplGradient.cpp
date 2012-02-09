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

#include "vplGradient.h"
#include "vplColorHelper.h"
#include "vplVector.h"

namespace vpl
{
	inline int round(float f)
    {
        return f >= 0.0f ? int(f + 0.5f) : int(f - int(f - 1) + 0.5f) + int(f - 1);
    }

	static inline vplUint32 fetchGradientPixel(vplUint32* colorTable,
                                               float pos,
                                               Gradient::Spread spread)
    {
		int iPos = int(pos * (Gradient::cGradientColorTableSize - 1) + 0.5f);
       
        if(iPos < 0 || iPos >= Gradient::cGradientColorTableSize)
        {
            if(spread == Gradient::cReflect)
            { 
                const int limit = Gradient::cGradientColorTableSize * 2 - 1;
                iPos = iPos % limit;
                iPos = iPos < 0 ? limit + iPos : iPos;
                iPos = iPos >= Gradient::cGradientColorTableSize ? limit - iPos : iPos;
            }
            else if(spread == Gradient::cRepeat)
            {
                iPos = iPos % Gradient::cGradientColorTableSize;
                iPos = iPos < 0 ? Gradient::cGradientColorTableSize + iPos : iPos;   
            }
            else
            {
                if(iPos < 0)
                    iPos = 0;
                else if(iPos >= Gradient::cGradientColorTableSize)
                    iPos = Gradient::cGradientColorTableSize - 1;
            }
        }
        
        return colorTable[iPos];
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
            for(int i = 0; i < Gradient::cGradientColorTableSize;i++)
                colorTable[i] = color;
            
            return;
        }
        
        // The general case
        vplUint ctIndex = 0;
        vplUint currStop = 0;
        float startPos = stops[0].offset_;
        float endPos = stops[stopCount - 1].offset_;
        float increment = 1.0f/Gradient::cGradientColorTableSize;
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
            
            float diff = stops[currStop+1].offset_ - stops[currStop].offset_;
            float c = (diff == 0) ? 0.0f : 255.0f / diff;
            float t = (currPos - stops[currStop].offset_) * c;
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
                    diff = stops[currStop + 1].offset_ - stops[currStop].offset_;
                    c = (diff == 0.0f) ? 0.0f : 255.0f / diff;
                    t = (currPos - stops[currStop].offset_) * c;
                    tDelta = increment * c;
                }
            }
        }
        
        // After last point
        color = preMultiplyColorRGBA(stops[stopCount - 1].color_.getColorRGBA());
        
        while (ctIndex < Gradient::cGradientColorTableSize - 1)
            colorTable[ctIndex++] = color;
        
        // Make sure the last color stop is represented at the end of the table
        colorTable[Gradient::cGradientColorTableSize - 1] = color;
    }

	void Gradient::generateGradient()
	{
		if(!colorTable_)
			colorTable_ = new vplUint32[cGradientColorTableSize];
		
	    generateGradientColorTable(colorTable_,
			                       stops_.getContents(),
								   stops_.getItemCount());

		valid_ = true;
	}

	LinearGradient::LinearGradient(float x,float y,float dirX,float dirY)
	{
		type_ = cLinear;
		data_.LinearGradientData.x_ = x;
		data_.LinearGradientData.y_ = y;
		data_.LinearGradientData.dirX_ = dirX;
		data_.LinearGradientData.dirY_ = dirY;

        length_ = sqrt(dirX*dirX + dirY*dirY);
            
        if(length_ != 0.0f)
        {
            deltaX_ = dirX / length_;
            deltaY_ = dirY / length_;
            offset_ = -deltaX_ * x - deltaY_ * y;
		}
	}

	vplUint32* LinearGradient::fetchGradient(vplUint x,vplUint y,vplUint length)
    {
		if(!valid_)
			generateGradient();

        float xPos = x + 0.5f;
        float yPos = y + 0.5f;
        float inc = (deltaX_ + deltaY_) / length_;     
        float pos = deltaX_ * xPos + deltaY_ * yPos + offset_;
        
        pos /= length_;
        
		if(!gradientPixels_)
			gradientPixels_ = new vplUint32[cMaxGradientPixels];

        vplUint32* dest = gradientPixels_;
        vplUint32* end  = dest + length;
        
        if( length_ == 0.0f)
            pos = inc = 0.0f;
        
        while(dest < end)
        { 
            *dest++ = fetchGradientPixel(colorTable_,pos,spread_);
            pos += inc;
        } 

		return gradientPixels_;
    }

	RadialGradient::RadialGradient(float cx,float cy,float fx,float fy,float radius)
	{
		type_ = cRadial;
		data_.RadialGradientData.cx_ = cx;
		data_.RadialGradientData.cy_ = cy;
		data_.RadialGradientData.fx_ = fx;
		data_.RadialGradientData.fy_ = fy;
		data_.RadialGradientData.radius_ = radius;

		Vector focalPoint(fx,fy);           
		Vector centre(cx,cy);
			
		// Validate input, make sure focal point is inside circle
        Vector diff = focalPoint - centre;   
		float length = diff.computeLength() ;
            
        if(length >= radius)
        {
            diff *= 0.999f * radius / length;
            focalPoint = centre + diff;
        }
            
        float r2 = radius * radius;			
		c_ = computeDotProduct(focalPoint - centre,focalPoint - centre) - r2;
	}

	// Intersection of a ray and a circle. Parametrize the ray as
	// i(t) = f + (p-f)*t
	// computeDotProduct(i-c,i-c) = radius^2
	// computeDotProduct((f-c) + (p-f)*t, (f-c) + (p-f)*t) = radius^2
	// computeDotProduct(p-f,p-f)*t^2 + 2*dot_product(f-c,p-f)*t + dot_product(f-c,f-c) - radius^2 = 0
	// This gives the follwing pseudo code
	// float a = Vector2.Dot(currentPoint - focus, currentPoint - focus);
	// float b = 2 * Vector2.Dot(focus-centre, currentPoint-focus);
	// float c = Vector2.Dot(focus-centre, focus-centre)- (radius * radius);
    // float t = (-b + (float)Math.Sqrt((b*b) - 4*a*c)) / (2*a);
	// float lerp = 1/t;
	vplUint32* RadialGradient::fetchGradient(vplUint x,vplUint y,vplUint length)
	{
		if(!valid_)
			generateGradient();

		Vector focalPoint(data_.RadialGradientData.fx_,data_.RadialGradientData.fy_);                          		
		Vector centre(data_.RadialGradientData.cx_,data_.RadialGradientData.cy_);        
		Vector currentPoint(x + 0.5f,y + 0.5f);        
		Vector inc(1.0f,0.0f);                          
		
		if(!gradientPixels_)
			gradientPixels_ = new vplUint32[cMaxGradientPixels];

		vplUint32* dest = gradientPixels_;        
		vplUint32* end  = dest + length;        	    
		
		while(dest < end)        
		{             
			float a = computeDotProduct(currentPoint - focalPoint,                                        
				                        currentPoint - focalPoint);

			float b = 2 * computeDotProduct(focalPoint - centre,                                            
				                            currentPoint - focalPoint);

			float pos = 1.0f / ((-b + sqrtf((b*b) - 4*a*c_)) / (2*a));			
			
			*dest++ = fetchGradientPixel(colorTable_,pos,spread_);           
			
			currentPoint += inc;        
		}

		return gradientPixels_;
	}
}