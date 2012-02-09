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

#ifndef VPL_GRADIENT_H_INCLUDED_
#define VPL_GRADIENT_H_INCLUDED_

#include "vplColor.h"
#include "vplArray.h"

namespace vpl
{   
	// Base class for gradients
    class VPL_API Gradient
    {
    public:

		// Gradient properties
        enum Spread
        {
            cPad = 0,
            cReflect,
            cRepeat
        };

		enum Type
		{
			cLinear,
			cRadial
		};

        struct Stop
        {
            Stop(float offset,const Color color):offset_(offset),color_(color){}
            Stop():offset_(0.0f),color_(0,0,0,0){}

            float offset_;
            Color color_;
        };

		union Data
		{
			struct
			{
				float x_;
				float y_;
				float dirX_;
				float dirY_;
			}LinearGradientData;

			struct 
			{	
				float cx_;
				float cy_;
				float fx_;
				float fy_;
				float radius_;
			}RadialGradientData;
		};
		
		static const int cGradientColorTableSize = 1024;
		static const int cMaxGradientPixels = 8192;

		Gradient():spread_(cPad),type_(cLinear),colorTable_(0),
			       gradientPixels_(0),valid_(true){}
        
		virtual ~Gradient()
		{
			if(colorTable_)
				delete [] colorTable_;

			if(gradientPixels_)
				delete [] gradientPixels_;
		}

        inline void setSpread(Spread spread) 
		{
			spread_ = spread;
		}
        
		inline Spread getSpread()            
		{
			return spread_;
		}
        
        inline void addStop(float offset,const Color& color)
        {
            Stop stop(offset,color);
            
            // Verify input
            if(offset > 1.0f)
                offset = 1.0f;
            else if(offset < 0.0f)
                offset = 0.0f;
            
            stops_.add(stop);

			valid_ = false;
        }
        
		// Getters
        inline Stop* getStops()           
		{
			return stops_.getContents();
		}
        
		inline vplUint getNumberOfStops() 
		{
			return stops_.getItemCount();
		}
		
		inline Type getType()             
		{
			return type_;
		}

		inline bool isValid()
		{
			return valid_;
		}

		void generateGradient();
		virtual vplUint32* fetchGradient(vplUint x,vplUint y,vplUint length) = 0;

    protected:

		friend class Renderer;

        Spread spread_;
		Type type_;
        DynamicArray<Stop> stops_;
		Data data_;
		vplUint32* colorTable_;
        vplUint32* gradientPixels_;
		bool valid_;
    };
    
	// Specialiations
    class VPL_API LinearGradient:public Gradient
    {
        
    public:

		LinearGradient(float x,float y,float dirX,float dirY);
		~LinearGradient(){}

		vplUint32* fetchGradient(vplUint x,vplUint y,vplUint length);

	private:
		 
		 float deltaX_;
         float deltaY_;
         float length_;
         float offset_;
    };

    class VPL_API RadialGradient:public Gradient
    {
    public:

		RadialGradient(float cx,float cy,float fx,float fy,float radius);
		~RadialGradient(){}

		vplUint32* fetchGradient(vplUint x,vplUint y,vplUint length);

	private:

		float c_;
    };
}

#endif