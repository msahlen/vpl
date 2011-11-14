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

#ifndef VPL_RENDER_H_
#define VPL_RENDER_H_

#include "vpl.h"
#include "vplColor.h"
#include "vplGradient.h"

namespace vpl
{
    // The different blending modes
    enum BlendMode
    {
        cClear = 0,
        cSrc,
        cDest,
        cSrcOverDest,
        cDestOverSrc,
        cSrcInDest,
        cDestInSrc,
        cSrcOutDest,
        cDestOutSrc,
        cSrcAtopDest,
        cDestAtopSrc,
        cSrcXorDest,
    };

    class VPL_API Pen
    {
    public:

        Pen();
        ~Pen(){}

        // Setters
        inline void setColor(const Color& color)
        {
            color_ = color;
        }
        
		inline void setOpacity(vplUchar alpha)
        {
            color_.setAlpha(alpha);
        }
        
		inline void setGradient(const Gradient* gradient)
        {
            gradient_ = gradient;
        }
        
		inline void setSize(float size)
        {
            size_ = size;
        }
        
		inline void setMiterLimit(float limit)
        {
            miterLimit_ = limit;
        }
        
		inline void setEndType(Stroker::EndType endType)
        {
            endType_ = endType;
        }
        
		inline void setJoinType(Stroker::JoinType joinType)
        {
            joinType_ = joinType;
        }
        
		inline void setBlendMode(BlendMode blendMode)
        {
            blendMode_ = blendMode;
        }

        // Getters
        inline vplUint32 getColorAsRGBA() const
        {
            return color_.getColorRGBA();
        }
        
		inline vplUint32 getColorAsARGB() const
        {
            return color_.getColorARGB();
        }
        
		inline const Gradient* getGradient() const
        {
            return gradient_;
        }
        
		inline float getSize() const
        {
            return size_;
        }
        
		inline float getMiterLimit() const
        {
            return miterLimit_;
        }
        
		inline Stroker::EndType getEndType() const
        {
            return endType_;
        }
        
		inline Stroker::JoinType getJoinType() const
        {
            return joinType_;
        }
        
		inline BlendMode getBlendMode() const
        {
            return blendMode_;
        }

    private:

        Color color_;
        const Gradient* gradient_;
        float size_;
        float miterLimit_;
        Stroker::EndType endType_;
        Stroker::JoinType joinType_;
        BlendMode blendMode_;
    };

    class VPL_API Brush
    {
    public:

        // The different fillmodes
        enum FillMode
        {
            cEvenOdd = 0,
            cNonZero
        };

        Brush();
        Brush(Color color);
        ~Brush(){}

        // Setters
        inline void setColor(const Color& color)
        {
            color_ = color;
        }
        
		inline void setOpacity(vplUchar alpha)
        {
            color_.setAlpha(alpha);
        }
        
		inline void setGradient( const Gradient* gradient)
        {
            gradient_ = gradient;
        }
        
		inline void setBlendMode(BlendMode blendMode)
        {
            blendMode_ = blendMode;
        }
        
		inline void setFillMode(FillMode fillMode)
        {
            fillMode_ = fillMode;
        }

        // Getters
        inline vplUint32 getColorAsRGBA() const
        {
            return color_.getColorRGBA();
        }
        
		inline vplUint32 getColorAsARGB() const
        {
            return color_.getColorARGB();
        }
        
		inline const Gradient* getGradient() const
        {
            return gradient_;
        }
        
		inline BlendMode getBlendMode() const
        {
            return blendMode_;

        }
        
		inline FillMode getFillMode() const
        {
            return fillMode_;
        }

   private:

        Color color_;
        const Gradient* gradient_;
        FillMode fillMode_;
        BlendMode blendMode_;
    };

    class VPL_API PixelBuffer
    {
    public:

        PixelBuffer();
        virtual ~PixelBuffer();

        void initialize(vplUint width,
			            vplUint height,
						Alignment alignment = cNoAlignment);
        
		void clear();
		void clear(const Color& color);

        inline vplUint getWidth() const
        {
            return width_;
        }
        inline vplUint getHeight() const
        {
            return height_;
        }
        inline vplUint getPitch() const
        {
            return pitch_;
        }
        inline vplUint getSizeInBytes() const
        {
            return height_*pitch_*sizeof(vplUint32);
        }
        inline vplUint32* getBuffer()
        {
            return buffer_->getMemory();
        }
        inline const vplUint32* getBuffer() const
        {
			return buffer_->getMemory();
        }

    private:

        vplUint width_;
        vplUint height_;
        vplUint pitch_;
        DynamicMemory<vplUint32>* buffer_;
    };

	// Forward declarations
	class VPL_API PathReference;
	class VPL_API ScanLineList;
    class VPL_API EdgeGenerator;
	class VPL_API NonZeroMaskBuffer;
    struct VPL_API BlendData;
    struct VPL_API EvenOddMaskBuffer;
    struct VPL_API FillParameters;

    class VPL_API Renderer
    {
        public:

            enum Sampling
            {
                cSample8  = 8,
                cSample16 = 16,
                cSample32 = 32
            };

            Renderer();
            ~Renderer();

            void setTarget(PixelBuffer* pixelBuffer);
			void setClipRect(const Rect& rect);
			void setTransform(const AffineMatrix& transform);
			void transform(const AffineMatrix& transform);
            void setSamplingValue(Sampling sampling);
			void compose(const PixelBuffer& src,BlendMode blendMode,
                         vplUint offsetX,vplUint offsetY);
			PathReference* addPath(const Path* path,const Pen* pen);
            PathReference* addPath(const Path* path,const Brush* brush);
            PathReference* addPath(const Path* path,const Pen* pen,const Brush* brush);
            void removePath(PathReference** reference);
			void clearPaths();
            void draw();

        private:

			void initialize();
			void updatePathEdges();
            void fill(Brush::FillMode fillMode,BlendData& data,ScanLineList* scanLines);
            void drawNewEdges();
            PathReference* allocateReference(const Path* path,
                                             const Pen* pen,
								             const Brush* brush);
			void deAllocateReference(PathReference* reference);
			void generateEdges(PathReference* reference);
			void setGradientData(BlendData& data,const Gradient* gradient);

			vplUint width_;
            vplUint height_;
            vplUint32 maxCoverage_;
            PixelBuffer* target_;
            EdgeGenerator* edgeGenerator_;
            EvenOddMaskBuffer* evenOddMaskBuffer_;
            NonZeroMaskBuffer* nonZeroMaskBuffer_;
            FillParameters* fillParameters_;
            vplUchar subPixelCount_;
            vplUchar subPixelShift_;
            Rect clipRect_;
			bool targetUpdated_;
			bool transformUpdated_;
			List<PathReference*> references_;
            List<PathReference*> newReferences_;
			AffineMatrix transform_;
    };

    VPL_API const char* toString(Stroker::EndType t);
    VPL_API const char* toString(Stroker::JoinType t);
    VPL_API const char* toString(Brush::FillMode t);
    VPL_API const char* toString(BlendMode m);

    VPL_API void drawLine(Path& path,float startX,float startY,float endX,float endY);
    VPL_API void drawRect(Path& path,float startX,float startY,float width,float height);
    VPL_API void drawRoundCornerRect(Path& path,float startX,float startY,
                                     float width,float height,
                                     float radiusX,float radiusY);
    VPL_API void drawEllipse(Path& path,float centerX,float centerY,
                             float radiusX,float radiusY);

}
#endif
