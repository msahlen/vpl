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

#ifndef VPL_FILL_H_INCLUDED_
#define VPL_FILL_H_INCLUDED_

#include "vplConfig.h"
#include "vplBlend.h"
#include "vplEdge.h"
#include "vplEvenOddMaskGenerator.h"
#include "vplNonZeroMask.h"
#include "vplNonZeroMaskGenerator.h"
#include "vplPixelCoverage.h"
#include "vplScanLineList.h"
#include "vplHorizontalSpan.h"

namespace vpl
{
    // Templated fillers
    struct FillParameters
    {
        vplUint width_;
        vplUint height_;
        vplUint subPixelCount_;
        vplUint maxCoverage_;
        vplUint subPixelShift_;
    };

    struct EvenOddMaskBuffer
    {
        EvenOddMaskBuffer():buffer_(0),width_(0){}
        ~EvenOddMaskBuffer()
        {
            if(buffer_)
                delete [] buffer_;
        }

        // Only allocate new space if needed
        inline void initialize(int width)
        {
            if(width > width_)
            {
                if(width_ > 0)
                    delete [] buffer_;

                // TODO: Ugly hack!
                buffer_ = new vplUint[width + 32];

                memset(buffer_,0x0,sizeof(vplUint32)* width);

                width_ = width;
            }
        }

        vplUint32* buffer_;
        int width_;
    };

    template<typename T> class VPL_API EvenOddFiller
    {
    public:

        EvenOddFiller(){}

        ~EvenOddFiller(){}

        void fill(PixelBuffer* target,
                  ScanLineList* allEdges,
                  T blender,
                  FillParameters& params,
                  EvenOddMaskBuffer* evenOddMaskBuffer)
        {
            ActiveEdgeList activeEdges;

            // Choose super sampling mask
            IEvenOddMaskGenerator& maskGen = chooseEvenOddMaskGen(params.subPixelCount_);

            // Horizontal span of scanline
            HorizontalSpan xSpan;

            // Initialization
            evenOddMaskBuffer->initialize(target->getWidth() + 4);

            maskGen.initMaskGenerator(evenOddMaskBuffer->buffer_,params.subPixelCount_,&xSpan);

            // For traversing list
            Edge* currentEdge = 0;

            // We use premultiplied alpha
            vplUint32 color = blender.getBlendData()->color_;

            // Traverse scanlines
            for(int y = allEdges->getMin(); y <= allEdges->getMax(); ++y)
            {
                // Reset span
                xSpan.reset();

                // Handle active edges
                currentEdge = activeEdges.getList();

                while(currentEdge)
                {
                    // If this is the last scanline for this edge
                    if((currentEdge->lastScanLine_ >> params.subPixelShift_) == y )
                    {
                        // Generate mask for partial subpixel
                        maskGen.processEdgeOnLastScanline(currentEdge);

                        // Remove the edge from the active edge table
                        activeEdges.remove(currentEdge);

                        currentEdge = currentEdge->nextEdge_;
                    }
                    else
                    {
                        // Generate mask for full subpixel
                        maskGen.processEdge(currentEdge);

                        // continue through edge table
                        currentEdge = currentEdge->nextEdge_;
                    }
                }

                // Get current scanline
                currentEdge = allEdges->getLine(y);

                // Traverse edges in this scanline
                while(currentEdge)
                {
                    // If the edge only spans this scan line
                    if((currentEdge->lastScanLine_ >> params.subPixelShift_) == y)
                    {
                        // Generate mask for partial subpixel
                        int subPixelEnd = currentEdge->lastScanLine_ & (params.subPixelCount_ - 1);

                        maskGen.processEdgeOnFirstScanline(currentEdge,subPixelEnd);

                        // Update edge
                        currentEdge = currentEdge->nextEdge_;
                    }
                    // Edge stretches in to next scanline
                    else
                    {
                        // Generate mask for partial subpixel
                        maskGen.processEdgeOnFirstScanline(currentEdge,params.subPixelCount_);

                        // add the edge to active edge table
                        currentEdge = activeEdges.insertAndGetNext(currentEdge);
                    }
                }

                // Fill bitmap
                int minX = xSpan.getMin();
                int maxX = xSpan.getMax() + 1;

                if(minX < 0)
                {
                    STD_LOG_ERROR() << "Wrong range in fill, negative value!";
                    minX = 0;
                }

                if(maxX > evenOddMaskBuffer->width_)
                {
                    STD_LOG_ERROR() << "Wrong range in fill, value bigger than buffer!";
                    maxX = evenOddMaskBuffer->width_;
                }

                // Pixels
                vplUint32* pixels = target->getBuffer() + y * target->getWidth();

                if(minX < maxX)
                {
                    vplUint32* pixelPointer = pixels + minX;
                    vplUint32* maskBufferPointer = evenOddMaskBuffer->buffer_ + minX;
                    vplUint32* maskBufferEnd = evenOddMaskBuffer->buffer_ + maxX + 1;

                    *maskBufferEnd = params.maxCoverage_;

                    vplUint32 mask = *maskBufferPointer;

                    *maskBufferPointer++ = 0;

                    while(maskBufferPointer <= maskBufferEnd)
                    {
                        // Mask is empty, fast forward until it changes.
                        if(mask == 0)
                        {
                            vplUint32* startPointer = maskBufferPointer;

                            do
                                mask = *maskBufferPointer++;

                            while(mask == 0 &&(maskBufferPointer <= maskBufferEnd));

                            maskBufferPointer[-1] = 0;
                            pixelPointer += maskBufferPointer - startPointer;
                        }

                        // Fully covered pixel,
                        // fill target with color until it changes
                        else if(mask == params.maxCoverage_)
                        {
                            vplUint count = 0;
                            vplUint32 temp;
                            do
                            {
                                temp = *maskBufferPointer++;
                                ++count;
                            }
                            while(temp == 0 &&(maskBufferPointer <= maskBufferEnd));

                            // Remove edge plot from mask buffer
                            maskBufferPointer[-1] = 0;
                            mask ^= temp;

                            // Blend
                            blender.blend(pixelPointer,
                                          pixelPointer - pixels,
                                          y,count);
                            pixelPointer += count;
                        }

                        // Partially filled pixel
                        else
                        {
                            do
                            {
                                // alpha is in range of 0 to SUBPIXEL_COUNT
                                vplUint32 alpha = getPixelCoverage(mask);
                                vplUint32 invAlpha = params.subPixelCount_ - alpha;
                                vplUint32 cs = multiplyPixel(color,alpha, params.subPixelShift_);
                                vplUint32 ct = multiplyPixel(*pixelPointer,invAlpha,params.subPixelShift_);

                                *pixelPointer++ = ct + cs;

                                mask ^= *maskBufferPointer;
                                *maskBufferPointer++ = 0;
                            }
                            while(!(mask == 0 || mask == params.maxCoverage_) &&
                                    (maskBufferPointer <= maskBufferEnd));
                        }
                    }
                }
            }
        }
    };

    template<typename T> class VPL_API NonZeroFiller
    {
        public:

            NonZeroFiller(){}

            ~NonZeroFiller(){}


            void fill(PixelBuffer* target,ScanLineList* allEdges,
                      T blender,FillParameters& params,
                      NonZeroMaskBuffer* nonZeroMaskBuffer)
            {
                ActiveEdgeList activeEdges;

                // Choose super sampling mask
                INonZeroMaskGenerator& maskGen = chooseNonZeroMaskGen(params.subPixelCount_);

                // Horizontal span of scanline
                HorizontalSpan xSpan;

                //TODO, Ugly hack!
                int bufferWidth = target->getWidth() + 4 + 32;

                // Initialization
                nonZeroMaskBuffer->initialize(bufferWidth,params.subPixelCount_);

                maskGen.initMaskGenerator(nonZeroMaskBuffer,params.subPixelCount_,&xSpan);

                // For traversing list
                Edge* currentEdge = 0;

                // Mask to work with
                NonZeroMask acc;
                acc.initializeSubPixelBuffer(params.subPixelCount_);

                // We use premultiplied alpha
                vplUint32 color = blender.getBlendData()->color_;

                // Traverse scanlines
                for(int y = allEdges->getMin(); y <= allEdges->getMax(); ++y)
                {
                    // Reset span
                    xSpan.reset();

                    // Handle active edges
                    currentEdge = activeEdges.getList();

                    while(currentEdge)
                    {
                        // If this is the last scanline for this edge
                        if((currentEdge->lastScanLine_ >> params.subPixelShift_) == y )
                        {
                            // Generate mask for partial pixel
                            maskGen.processEdgeOnLastScanline(currentEdge);

                            // Remove the edge from the active edge table
                            activeEdges.remove(currentEdge);

							if(currentEdge != currentEdge->nextEdge_)
								currentEdge = currentEdge->nextEdge_;
							else
								currentEdge = 0;
                        }
                        else
                        {
                            // Generate mask for full pixel
                            maskGen.processEdge(currentEdge);

                            // continue through edge table
                            currentEdge = currentEdge->nextEdge_;
                        }
                    }

                    // Get current scanline
                    currentEdge = allEdges->getLine(y);

                    // Traverse edges in this scanline
                    while(currentEdge)
                    {
                        // If the edge only spans this scan line
                        if((currentEdge->lastScanLine_ >> params.subPixelShift_) == y)
                        {
                            // Generate mask for possibly partial pixel
                            int subPixelEnd   = currentEdge->lastScanLine_ & (params.subPixelCount_ - 1);

                            maskGen.processEdgeOnFirstScanline(currentEdge,subPixelEnd);

                            currentEdge = currentEdge->nextEdge_;
                        }
                        // Edge stretches in to next scanline
                        else
                        {
                            // Generate mask for possibly partial pixel
                            maskGen.processEdgeOnFirstScanline(currentEdge,params.subPixelCount_);

                            // add the edge to active edge table
                            currentEdge = activeEdges.insertAndGetNext(currentEdge);
                        }
                    }

                    // Fill bitmap
                    int minX  = xSpan.getMin();
                    int maxX  = xSpan.getMax() + 1;

                    if(minX < 0)
                    {
                        STD_LOG_ERROR() << "Wrong range in fill, negative value!";
                        minX = 0;
                    }

                    if(maxX > bufferWidth)
                    {
                        STD_LOG_ERROR() << "Wrong range in fill, value bigger than buffer!";
                        maxX = bufferWidth;
                    }

                    int index = minX;

                    // Pixels
                    vplUint32* pixels = target->getBuffer() + y * target->getWidth();

                    if(minX < maxX)
                    {
                        vplUint32* pixelPointer = pixels + minX;
                        acc.reset((*nonZeroMaskBuffer)[index]);

                        while(index < maxX)
                        {
                            // Fast forward if no coverage
                            if(acc.readMask() == 0)
                            {
                                int start = index;

                                do
                                {
                                    (*nonZeroMaskBuffer)[index++].reset();
                                }
                                while((*nonZeroMaskBuffer)[index].readMask() == 0 && index < maxX);

                                acc.initialize((*nonZeroMaskBuffer)[index]);

                                // Increase bitmap pointer
                                pixelPointer += (index - start);
                            }

                            // Fill with solid color until pixels have
                            // different coverage
                            else if(acc.readMask() == params.maxCoverage_)
                            {
                                vplUint count = 0;
                                do
                                {
                                    ++count;

                                    (*nonZeroMaskBuffer)[index++].reset();

                                    if((*nonZeroMaskBuffer)[index].readMask())
                                        acc.accumulate((*nonZeroMaskBuffer)[index]);

                                }
                                while(acc.readMask() == params.maxCoverage_ && index < maxX);

                                // Blend
                                blender.blend(pixelPointer,
                                              pixelPointer - pixels,
                                              y,count);

                                pixelPointer += count;
                            }

                            // Pixels have less than full coverage
                            // Compute coverage and fill
                            else
                            {
                                // Mask is semitransparent.
                                vplUint32 alpha = getPixelCoverage(acc.readMask());

                                // alpha is in range of 0 to subpixel count
                                vplUint32 invAlpha = params.subPixelCount_ - alpha;

                                do
                                {
                                    vplUint32 cs = multiplyPixel(color,alpha,params.subPixelShift_);
                                    vplUint32 ct = multiplyPixel(*pixelPointer,invAlpha,params.subPixelShift_);

                                    *pixelPointer++ = ct + cs;

                                    (*nonZeroMaskBuffer)[index++].reset();

                                    if((*nonZeroMaskBuffer)[index].readMask())
                                    {
                                        acc.accumulate((*nonZeroMaskBuffer)[index]);
                                        alpha = getPixelCoverage(acc.readMask());
                                        invAlpha = params.subPixelCount_ - alpha;
                                    }

                                }
                                while(acc.readMask() != 0 &&
                                      acc.readMask() != params.maxCoverage_ &&
                                    index < maxX);
                            }
                        }
                        (*nonZeroMaskBuffer)[index].reset();
                    }
                }
            }
    };

    void composeBuffers(BlendMode blendMode,PixelBuffer* target,
                        const PixelBuffer& src,Rect rect);

    void fillEvenOdd(PixelBuffer* target,ScanLineList* edges,
                     BlendData& data,FillParameters& params,
                     EvenOddMaskBuffer* evenOddMaskBuffer);

    void gradientFillEvenOdd(PixelBuffer* target,ScanLineList* edges,
                             BlendData& data,FillParameters& params,
                             EvenOddMaskBuffer* evenOddMaskBuffer);

    void fillNonZero(PixelBuffer* target,ScanLineList* edges,
                     BlendData& data,FillParameters& params,
                     NonZeroMaskBuffer* nonZeroMaskBuffer);

    void gradientFillNonZero(PixelBuffer* target,ScanLineList* edges,
                             BlendData& data,FillParameters& params,
                             NonZeroMaskBuffer* nonZeroMaskBuffer);

}
#endif