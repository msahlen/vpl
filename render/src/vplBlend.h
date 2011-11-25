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

#ifndef VPL_BLEND_H_INCLUDED_
#define VPL_BLEND_H_INCLUDED_

#include "vplRender.h"
#include "vplColorHelper.h"

namespace vpl
{
    // Blenders
    // Porter Duff blending modes
    // Cd, Ad are the Component and Alpha of the destination pixel.
    // Cs, As are the Component and Alpha of the source pixel.
    // Cr, Ar are the Component and Alpha of the resulting pixel.

    // Ar = As
    // Cr = Cs
    class VPL_API SrcBlender
    {
    public:

        SrcBlender():color_(0){}
        ~SrcBlender(){}

        inline void blend(vplUint32* dest,vplUint,vplUint,vplUint count) const
        {
			vplMemFill(dest,color_,count);  
        }

        inline void blend(vplUint32* dest,const vplUint32* src,vplUint count) const
        {
            vplMemCopy(dest,src,count);  
        }

        inline void setColor(vplUint32 color)
        {
            color_ = color;
		}

		inline vplUint32 getColor()
        {
            return color_;
        }

    private:

		vplUint32 color_;
    };

    class VPL_API SrcGradientBlender
    {
    public:

        SrcGradientBlender():gradient_(0){}
        ~SrcGradientBlender(){}

        inline void blend(vplUint32* dest,vplUint x,vplUint y,vplUint count) const
        {
			vplUint32* gradientPixels = gradient_->fetchGradient(x,y,count);

            vplMemCopy(dest,gradientPixels,count);  
        }

        inline void setGradient(Gradient* gradient)
		{
			gradient_ = gradient;
		}

		inline vplUint32 getColor()
        {
            return 0;
        }

    private:

        Gradient* gradient_;
    };

    // Destination blend is like a nop
    // Ar = Ad
    // Cr = Cd
    class VPL_API DestBlender
    {
    public:

        DestBlender():color_(0){}
        ~DestBlender(){}

        inline void blend(vplUint32*,vplUint,vplUint,vplUint) const
        {
        }

        void blend(vplUint32*,const vplUint32*,vplUint) const
        {
        }
       
		inline void setColor(vplUint32 color)
        {
            color_ = color;
        }

		inline vplUint32 getColor()
        {
            return color_;
        }

    private:

		vplUint32 color_;
    };

    class VPL_API DestGradientBlender
    {
    public:

        DestGradientBlender():gradient_(0){}
        ~DestGradientBlender(){}

        inline void blend(vplUint32*,vplUint,vplUint,vplUint) const
        {
        }

        inline void setGradient(Gradient* gradient)
		{
			gradient_ = gradient;
		}

		inline vplUint32 getColor()
        {
            return 0;
        }


    private:

        Gradient* gradient_;
    };

    // Ar = As + Ad * (1 - As)
    // Cr = Cs + Cd * (1 - As)
    class VPL_API SrcOverDestBlender
    {
    public:

        SrcOverDestBlender():color_(0){}
        ~SrcOverDestBlender(){}

        inline void blend(vplUint32* dest,vplUint,vplUint,vplUint count) const
        {
			vplUchar oneMinusAlpha = getAlphaChannelFromRGBA(~color_);

			if(0 == oneMinusAlpha)
				vplMemFill(dest,color_,count);  
			else
			{
				for(vplUint i = 0; i < count; i++)
					dest[i] = color_ + multiplyPixel(dest[i],oneMinusAlpha);
			}
        }

        inline void blend(vplUint32* dest,const vplUint32* src,vplUint count) const
        {
			vplUchar oneMinusAlpha = getAlphaChannelFromRGBA(~color_);
            
			if(0 == oneMinusAlpha)
				vplMemCopy(dest,src,count);  
			else
			{
				for(vplUint i = 0; i < count; i++)
					dest[i] = src[i] + multiplyPixel(dest[i],oneMinusAlpha);
			}
        }
        
		inline void setColor(vplUint32 color)
        {
            color_ = color;
        }

		inline vplUint32 getColor()
        {
            return color_;
        }

    private:

		vplUint32 color_;
    };

    class VPL_API SrcOverDestGradientBlender
    {
    public:

        SrcOverDestGradientBlender():gradient_(0){}
        ~SrcOverDestGradientBlender(){}

        inline void blend(vplUint32* dest,vplUint x,vplUint y,vplUint count) const
        {
            vplUint32* gradientPixels = gradient_->fetchGradient(x,y,count);

            for(vplUint i = 0; i < count; i++)
            {
                dest[i] = gradientPixels[i] + \
					multiplyPixel(dest[i],getAlphaChannelFromRGBA(~gradientPixels[i]));
            }
        }
        
		inline void setGradient(Gradient* gradient)
		{
			gradient_ = gradient;
		}

		inline vplUint32 getColor()
        {
            return 0;
        }

    private:

        Gradient* gradient_;
    };
    // Ar = As * (1 - Ad) + Ad
    // Cr = Cs * (1 - Ad) + Cd
    class VPL_API DestOverSrcBlender
    {
    public:

        DestOverSrcBlender():color_(0){}
        ~DestOverSrcBlender(){}

        inline void blend(vplUint32* dest,vplUint,vplUint,vplUint count) const
        {
            for(vplUint i = 0; i < count; i++)
				dest[i] =  dest[i] + multiplyPixel(color_,getAlphaChannelFromRGBA(~dest[i]));
            
        }
        inline void blend(vplUint32* dest,const vplUint32* src,vplUint count) const
        {
            for(vplUint i = 0; i < count; i++)
				dest[i] = dest[i] + multiplyPixel(src[i],getAlphaChannelFromRGBA(~dest[i]));
        }
        
		inline void setColor(vplUint32 color)
        {
            color_ = color;
		}

		inline vplUint32 getColor()
        {
            return color_;
        }

    private:

		vplUint32 color_;
	};

    class VPL_API DestOverSrcGradientBlender
    {
    public:

        DestOverSrcGradientBlender():gradient_(0){}
        ~DestOverSrcGradientBlender(){}

        inline void blend(vplUint32* dest,vplUint x,vplUint y,vplUint count) const
        {
            vplUint32* gradientPixels = gradient_->fetchGradient(x,y,count);

            for(vplUint i = 0; i < count; i++)
            {
				dest[i] = dest[i] + \
					multiplyPixel(gradientPixels[i],getAlphaChannelFromRGBA(~dest[i]));
            }

        }
        
		inline void setGradient(Gradient* gradient)
		{
			gradient_ = gradient;
		}

		inline vplUint32 getColor()
        {
            return 0;
        }


    private:

        Gradient* gradient_;
    };

    // Ar = As * Ad
    // Cr = Cs * Ad
    class VPL_API SrcInDestBlender
    {
    public:

        SrcInDestBlender():color_(0){}
        ~SrcInDestBlender(){}

        inline void blend(vplUint32* dest,vplUint,vplUint,vplUint count) const
        {
            for(vplUint i = 0; i < count; i++)
                dest[i] = multiplyPixel(color_,getAlphaChannelFromRGBA( dest[i]));
        }

        inline void blend(vplUint32* dest,const vplUint32* src,vplUint count) const
        {
            for(vplUint i = 0; i < count; i++)
				dest[i] = multiplyPixel(src[i],getAlphaChannelFromRGBA(dest[i]));
        }

		inline void setColor(vplUint32 color)
        {
            color_ = color;
        }

		inline vplUint32 getColor()
        {
            return color_;
        }

    private:

		vplUint32 color_;
	};

    class VPL_API SrcInDestGradientBlender
    {
    public:

        SrcInDestGradientBlender():gradient_(0){}
        ~SrcInDestGradientBlender(){}

        inline void blend(vplUint32* dest,vplUint x,vplUint y,vplUint count) const
        {
           vplUint32* gradientPixels = gradient_->fetchGradient(x,y,count);

            for(vplUint i = 0; i < count; i++)
				dest[i] = multiplyPixel(gradientPixels[i],getAlphaChannelFromRGBA(dest[i]));
        }
        
		inline void setGradient(Gradient* gradient)
		{
			gradient_ = gradient;
		}

		inline vplUint32 getColor()
        {
            return 0;
        }

    private:

        Gradient* gradient_;
    };

    // Ar = Ad * As
    // Cr = Cd * As
    class VPL_API DestInSrcBlender
    {
    public:

        DestInSrcBlender():color_(0){}
        ~DestInSrcBlender(){}

        inline void blend(vplUint32* dest,vplUint,vplUint,vplUint count) const
        {
            for(vplUint i = 0; i < count; i++)
                dest[i] = multiplyPixel(dest[i],getAlphaChannelFromRGBA(color_));
        }

        inline void blend(vplUint32* dest,const vplUint32* src,vplUint count) const
        {
            for(vplUint i = 0; i < count; i++)
                dest[i] = multiplyPixel(dest[i],getAlphaChannelFromRGBA(src[i]));
        }
        
		inline void setColor(vplUint32 color)
        {
            color_ = color;
        }

		inline vplUint32 getColor()
        {
            return color_;
        }

    private:

		vplUint32 color_;
    };

    class VPL_API DestInSrcGradientBlender
    {
    public:

        DestInSrcGradientBlender():gradient_(0){}
        ~DestInSrcGradientBlender(){}

        inline void blend(vplUint32* dest,vplUint x,vplUint y,vplUint count) const
        {
            vplUint32* gradientPixels = gradient_->fetchGradient(x,y,count);

            for(vplUint i = 0; i < count; i++)
                dest[i] = multiplyPixel(dest[i],getAlphaChannelFromRGBA(gradientPixels[i]));

        }
        
		inline void setGradient(Gradient* gradient)
		{
			gradient_ = gradient;
		}

		inline vplUint32 getColor()
        {
            return 0;
        }

    private:

        Gradient* gradient_;
    };

    // Ar = As * (1 - Ad)
    // Cr = Cs * (1 - Ad)
    class VPL_API SrcOutDestBlender
    {
    public:

        SrcOutDestBlender():color_(0){}
        ~SrcOutDestBlender(){}

        inline void blend(vplUint32* dest,vplUint,vplUint,vplUint count) const
        {
            for(vplUint i = 0; i < count; i++)
                dest[i] = multiplyPixel(color_,getAlphaChannelFromRGBA(~dest[i]));
        }

        inline void blend(vplUint32* dest,const vplUint32* src,vplUint count) const
        {
            for(vplUint i = 0; i < count; i++)
                dest[i] = multiplyPixel(src[i],getAlphaChannelFromRGBA(~dest[i]));
        }
        
		inline void setColor(vplUint32 color)
        {
            color_ = color;
        }

		inline vplUint32 getColor()
        {
            return color_;
        }

    private:

		vplUint32 color_;
    };

    class VPL_API SrcOutDestGradientBlender
    {
    public:

        SrcOutDestGradientBlender():gradient_(0){}
        ~SrcOutDestGradientBlender(){}

        inline void blend(vplUint32* dest,vplUint x,vplUint y,vplUint count) const
        {
            vplUint32* gradientPixels = gradient_->fetchGradient(x,y,count);

            for(vplUint i = 0; i < count; i++)
				dest[i] = multiplyPixel(gradientPixels[i],getAlphaChannelFromRGBA(~dest[i]));
        }
        
		inline void setGradient(Gradient* gradient)
		{
			gradient_ = gradient;
		}

		inline vplUint32 getColor()
        {
            return 0;
        }

    private:

        Gradient* gradient_;
    };

    // Ar = Ad * (1 - As)
    // Cr = Cd * (1 - As)
    class VPL_API DestOutSrcBlender
    {
    public:

        DestOutSrcBlender():color_(0){}
        ~DestOutSrcBlender(){}

        inline void blend(vplUint32* dest,vplUint,vplUint,vplUint count) const
        {
            for(vplUint i = 0; i < count; i++)
                dest[i] = multiplyPixel(dest[i],getAlphaChannelFromRGBA(~color_));
        }

        inline void blend(vplUint32* dest,const vplUint32* src,vplUint count) const
        {
            for(vplUint i = 0; i < count; i++)
                dest[i] = multiplyPixel(dest[i],getAlphaChannelFromRGBA(~src[i]));
        }
        
		inline void setColor(vplUint32 color)
        {
            color_ = color;
        }

		inline vplUint32 getColor()
        {
            return color_;
        }

    private:

		vplUint32 color_;
    };

    class VPL_API DestOutSrcGradientBlender
    {
    public:

        DestOutSrcGradientBlender():gradient_(0){}
        ~DestOutSrcGradientBlender(){}

        inline void blend(vplUint32* dest,vplUint x,vplUint y,vplUint count) const
        {
           vplUint32* gradientPixels = gradient_->fetchGradient(x,y,count);

            for(vplUint i = 0; i < count; i++)
			{
                dest[i] = multiplyPixel(dest[i],
					                    getAlphaChannelFromRGBA(~gradientPixels[i]));
			}
        }
        
		inline void setGradient(Gradient* gradient)
		{
			gradient_ = gradient;
		}

		inline vplUint32 getColor()
        {
            return 0;
        }

    private:

        Gradient* gradient_;
    };

    // Ar = As * Ad + Ad * (1 - As)
    // Ar = Ad
    // Cr = Cs * Ad + Cd * (1 - As)
    class VPL_API SrcAtopDestBlender
    {
    public:

        SrcAtopDestBlender():color_(0){}
        ~SrcAtopDestBlender(){}

        inline void blend(vplUint32* dest,vplUint,vplUint,vplUint count) const
        {
            vplUint32 inverseAlpha = getAlphaChannelFromRGBA(~color_);

            for(vplUint i = 0; i < count; i++)
            {
                dest[i] = interpolatePixel(color_,
					                       getAlphaChannelFromRGBA(dest[i]),
										   dest[i],
										   inverseAlpha);
            }

        }

        inline void blend(vplUint32* dest,const vplUint32* src,vplUint count) const
        {
            for(vplUint i = 0; i < count; i++)
            {
                dest[i] = interpolatePixel(src[i],
					                       getAlphaChannelFromRGBA(dest[i]),
										   dest[i],getAlphaChannelFromRGBA(~src[i]));
            }
        }
        
		inline void setColor(vplUint32 color)
        {
            color_ = color;
        }

		inline vplUint32 getColor()
        {
            return color_;
        }

    private:

		vplUint32 color_;
	};

    class VPL_API SrcAtopDestGradientBlender
    {
    public:

        SrcAtopDestGradientBlender():gradient_(0){}
        ~SrcAtopDestGradientBlender(){}

        inline void blend(vplUint32* dest,vplUint x,vplUint y,vplUint count) const
        {
           vplUint32* gradientPixels = gradient_->fetchGradient(x,y,count);

            for(vplUint i = 0; i < count; i++)
            {
                dest[i] = interpolatePixel(gradientPixels[i],
					                       getAlphaChannelFromRGBA(dest[i]),
										   dest[i],
										   getAlphaChannelFromRGBA(~gradientPixels[i]));
            }
        }
        
		inline void setGradient(Gradient* gradient)
		{
			gradient_ = gradient;
		}
		
		inline vplUint32 getColor()
        {
            return 0;
        }

    private:

        Gradient* gradient_;
    };

    // Ar = As * (1 - Ad) + Ad * As
    // Ar = As
    // Cr = Cs * (1 - Ad) + Cd * As
    class VPL_API DestAtopSrcBlender
    {
    public:

        DestAtopSrcBlender():color_(0){}
        ~DestAtopSrcBlender(){}

        inline void blend(vplUint32* dest,vplUint,vplUint,vplUint count) const
        {
            for(vplUint i = 0; i < count; i++)
            {
                dest[i] = interpolatePixel(dest[i],
					                       getAlphaChannelFromRGBA(color_),
										   color_, 
										   getAlphaChannelFromRGBA(~dest[i]));
            }
        }

        inline void blend(vplUint32* dest,const vplUint32* src,vplUint count) const
        {
            for(vplUint i = 0; i < count; i++)
            {
                dest[i] = interpolatePixel(dest[i],
					                       getAlphaChannelFromRGBA(src[i]),
										   src[i], 
										   getAlphaChannelFromRGBA(~dest[i]));
            }
        }
        
		inline void setColor(vplUint32 color)
        {
            color_ = color;
        }

		inline vplUint32 getColor()
        {
            return color_;
        }

    private:

		vplUint32 color_;
    };

    class VPL_API DestAtopSrcGradientBlender
    {
    public:

        DestAtopSrcGradientBlender():gradient_(0){}
        ~DestAtopSrcGradientBlender(){}

        inline void blend(vplUint32* dest,vplUint x,vplUint y,vplUint count) const
        {
            vplUint32* gradientPixels = gradient_->fetchGradient(x,y,count);

            for(vplUint i = 0; i < count; i++)
            {
                dest[i] = interpolatePixel(dest[i],
					                       getAlphaChannelFromRGBA(gradientPixels[i]),
										   gradientPixels[i], 
										   getAlphaChannelFromRGBA(~dest[i]));
            }
        }
        
		inline void setGradient(Gradient* gradient)
		{
			gradient_ = gradient;
		}

		inline vplUint32 getColor()
        {
            return 0;
        }

    private:

        Gradient* gradient_;
    };

    // Ar = As * (1 - Ad) + Ad * (1 - As)
    // Cr = Cs * (1 - Ad) + Cd * (1 - As)
    class VPL_API SrcXorDestBlender
    {
    public:

        SrcXorDestBlender():color_(0){}
        ~SrcXorDestBlender(){}

        inline void blend(vplUint32* dest,vplUint,vplUint,vplUint count) const
        {
            vplUint32 inverseAlpha = getAlphaChannelFromRGBA(~color_);

            for(vplUint i = 0; i < count; i++)
            {
                dest[i] = interpolatePixel(color_,
					                       getAlphaChannelFromRGBA(~dest[i]),
										   dest[i],
										   inverseAlpha);
            }
        }

        inline void blend(vplUint32* dest,const vplUint32* src,vplUint count) const
        {
            for(vplUint i = 0; i < count; i++)
            {
                dest[i] = interpolatePixel(src[i],
					                       getAlphaChannelFromRGBA(~dest[i]),
										   dest[i],
										   getAlphaChannelFromRGBA(~src[i]));
            }
        }
       
		inline void setColor(vplUint32 color)
        {
            color_ = color;
        }

		inline vplUint32 getColor()
        {
            return color_;
        }

    private:

		vplUint32 color_;
	};

    class VPL_API SrcXorDestGradientBlender
    {
    public:

        SrcXorDestGradientBlender():gradient_(0){}
        ~SrcXorDestGradientBlender(){}

        inline void blend(vplUint32* dest,vplUint x,vplUint y,vplUint count) const
        {
            vplUint32* gradientPixels = gradient_->fetchGradient(x,y,count);

            for(vplUint i = 0; i < count; i++)
            {
                dest[i] = interpolatePixel(gradientPixels[i],
					                       getAlphaChannelFromRGBA(~dest[i]),
										   dest[i],
										   getAlphaChannelFromRGBA(~gradientPixels[i]));
            }
        }
        
		inline void setGradient(Gradient* gradient)
		{
			gradient_ = gradient;
		}

		inline vplUint32 getColor()
        {
            return 0;
        }

    private:

        Gradient* gradient_;
    };
	
	// Ar = 0
    // Cr = 0
    class VPL_API ClearBlender
    {
    public:

        ClearBlender():color_(0){}
        ~ClearBlender(){}

        inline void blend(vplUint32* dest,vplUint,vplUint,vplUint count) const
        {
           vplMemFill(dest,(vplUint32)0,count);
        }

        inline void blend(vplUint32* dest,const vplUint32*,vplUint count) const
        {
           vplMemFill(dest,(vplUint32)0,count);
        }

        inline void setColor(vplUint32 color)
        {
            color_ = color;
        }

		inline vplUint32 getColor()
        {
            return color_;
        }

    private:

		vplUint32 color_;
     };

     template<typename T> class VPL_API Composer
     {
	 public:
		
		Composer(){}
        ~Composer(){}

        inline void compose(PixelBuffer* target,const PixelBuffer& src,Rect rect)
        {
            vplUint bottom = static_cast<vplUint>(rect.bottom_);
            vplUint top    = static_cast<vplUint>(rect.top_);
            vplUint count  = static_cast<vplUint>(rect.right_ - rect.left_);

            for(vplUint i = bottom ;i < top;i++)
            {
                blender_.blend(target->getBuffer()  \
					+ i*target->getWidth(),src.getBuffer() + i * src.getWidth(),count);
            }
        }

    private:

        T blender_;
     };
}
#endif
