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

#include "vplEvenOddMaskGenerator.h"
#include "vplFixedPoint.h"
#include "vplSampleMasks.h"
#include "vplEdge.h"
#include "vplHorizontalSpan.h"

namespace vpl
{
	// Initialize mask generator with mask buffer,
    // subpixel count and the span
	void IEvenOddMaskGenerator::initMaskGenerator(vplUint32* buffer,
                                                  vplUint spCount,
                                                  HorizontalSpan* span)
	{
		maskBuffer_    = buffer;
        subPixelCount_ = spCount;
        span_          = span;

        // Choose offsets
         if(subPixelCount_ == 8)
            offsets_ = SAMPLEMASK8x8;
        else if(subPixelCount_ == 16)
            offsets_ = SAMPLEMASK16x16;
        else if(subPixelCount_ == 32)
            offsets_ = SAMPLEMASK32x32;

    }
    // Process edges that start on the current scanline
    void IEvenOddMaskGenerator::processEdgeOnFirstScanline(Edge* edge,
		                                                   int subPixelEnd)
    {
        FixedPoint x      = edge->x_;
        FixedPoint slope  = edge->slope_;
        int xStart        = x.toInt();
        int subPixelStart = edge->firstScanLine_ & (subPixelCount_ - 1);
        long dataMask     = 0x1 << subPixelStart;
        int offsetIndex;

        for(int i = subPixelStart; i < subPixelEnd; ++i)
        {
            offsetIndex = (x + offsets_[i]).toInt();
            maskBuffer_[offsetIndex] ^= dataMask;
            dataMask <<= 1;
            x += slope;
        }

        // Last x-value rounded down.
        int xEnd = (x - slope).toInt();

        // Mark the span.
        span_->mark(xStart,xEnd);

        // Update the edge
        edge->x_ = x;
    }
    // Process edges that end on this scanline
    void IEvenOddMaskGenerator::processEdgeOnLastScanline(Edge* edge)
    {
        FixedPoint x     = edge->x_;
        FixedPoint slope = edge->slope_;
        int xStart       = x.toInt();
        int subPixelEnd  = edge->lastScanLine_ & (subPixelCount_ - 1);
        long dataMask    = 0x1;
        int offsetIndex;

        for(int i = 0; i < subPixelEnd; ++i)
        {
            offsetIndex = (x + offsets_[i]).toInt();
            maskBuffer_[offsetIndex] ^= dataMask;
            dataMask <<= 1;
            x += slope;
        }

        // Last x-value rounded down.
        int xEnd = (x - slope).toInt();

        // Mark the span.
        span_->mark(xStart,xEnd);
    }

	void EvenOddMaskGenerator8::processEdge(Edge* edge)
    {
        FixedPoint x     = edge->x_;
        FixedPoint slope = edge->slope_;
        int xStart       = x.toInt();
        long dataMask    = 0x1;
        int offsetIndex;
        
        offsetIndex = (x + SAMPLEMASK8x8[0]).toInt();
        maskBuffer_[offsetIndex] ^= dataMask;
        dataMask <<= 1;
        x += slope;
        
        offsetIndex = (x + SAMPLEMASK8x8[1]).toInt();
        maskBuffer_[offsetIndex] ^= dataMask;
        dataMask <<= 1;
        x += slope;

        offsetIndex = (x + SAMPLEMASK8x8[2]).toInt();
        maskBuffer_[offsetIndex] ^= dataMask;
        dataMask <<= 1;
        x += slope;

        offsetIndex = (x + SAMPLEMASK8x8[3]).toInt();
        maskBuffer_[offsetIndex] ^= dataMask;
        dataMask <<= 1;
        x += slope;

        offsetIndex = (x + SAMPLEMASK8x8[4]).toInt();
        maskBuffer_[offsetIndex] ^= dataMask;
        dataMask <<= 1;
        x += slope;
        
        offsetIndex = (x + SAMPLEMASK8x8[5]).toInt();
        maskBuffer_[offsetIndex] ^= dataMask;
        dataMask <<= 1;
        x += slope;

        offsetIndex = (x + SAMPLEMASK8x8[6]).toInt();
        maskBuffer_[offsetIndex] ^= dataMask;
        dataMask <<= 1;
        x += slope;

        offsetIndex = (x + SAMPLEMASK8x8[7]).toInt();
        maskBuffer_[offsetIndex] ^= dataMask;
        dataMask <<= 1;
        x += slope;

       // Last x-value rounded down.
        int xEnd = (x - slope).toInt();

        // Mark the span.
        span_->mark(xStart,xEnd);

        // Update the edge
        edge->x_ = x;
    }
	
	void EvenOddMaskGenerator16::processEdge(Edge* edge)
    {
        FixedPoint x     = edge->x_;
        FixedPoint slope = edge->slope_;
        int xStart       = x.toInt();
        long dataMask    = 0x1;
        int offsetIndex;
        
        offsetIndex = (x + SAMPLEMASK16x16[0]).toInt();
        maskBuffer_[offsetIndex] ^= dataMask;
        dataMask <<= 1;
        x += slope;
        
        offsetIndex = (x + SAMPLEMASK16x16[1]).toInt();
        maskBuffer_[offsetIndex] ^= dataMask;
        dataMask <<= 1;
        x += slope;

        offsetIndex = (x + SAMPLEMASK16x16[2]).toInt();
        maskBuffer_[offsetIndex] ^= dataMask;
        dataMask <<= 1;
        x += slope;

        offsetIndex = (x + SAMPLEMASK16x16[3]).toInt();
        maskBuffer_[offsetIndex] ^= dataMask;
        dataMask <<= 1;
        x += slope;

        offsetIndex = (x + SAMPLEMASK16x16[4]).toInt();
        maskBuffer_[offsetIndex] ^= dataMask;
        dataMask <<= 1;
        x += slope;
        
        offsetIndex = (x + SAMPLEMASK16x16[5]).toInt();
        maskBuffer_[offsetIndex] ^= dataMask;
        dataMask <<= 1;
        x += slope;

        offsetIndex = (x + SAMPLEMASK16x16[6]).toInt();
        maskBuffer_[offsetIndex] ^= dataMask;
        dataMask <<= 1;
        x += slope;

        offsetIndex = (x + SAMPLEMASK16x16[7]).toInt();
        maskBuffer_[offsetIndex] ^= dataMask;
        dataMask <<= 1;
        x += slope;

        offsetIndex = (x + SAMPLEMASK16x16[8]).toInt();
        maskBuffer_[offsetIndex] ^= dataMask;
        dataMask <<= 1;
        x += slope;
        
        offsetIndex = (x + SAMPLEMASK16x16[9]).toInt();
        maskBuffer_[offsetIndex] ^= dataMask;
        dataMask <<= 1;
        x += slope;

        offsetIndex = (x + SAMPLEMASK16x16[10]).toInt();
        maskBuffer_[offsetIndex] ^= dataMask;
        dataMask <<= 1;
        x += slope;

        offsetIndex = (x + SAMPLEMASK16x16[11]).toInt();
        maskBuffer_[offsetIndex] ^= dataMask;
        dataMask <<= 1;
        x += slope;

        offsetIndex = (x + SAMPLEMASK16x16[12]).toInt();
        maskBuffer_[offsetIndex] ^= dataMask;
        dataMask <<= 1;
        x += slope;
        
        offsetIndex = (x + SAMPLEMASK16x16[13]).toInt();
        maskBuffer_[offsetIndex] ^= dataMask;
        dataMask <<= 1;
        x += slope;

        offsetIndex = (x + SAMPLEMASK16x16[14]).toInt();
        maskBuffer_[offsetIndex] ^= dataMask;
        dataMask <<= 1;
        x += slope;

        offsetIndex = (x + SAMPLEMASK16x16[15]).toInt();
        maskBuffer_[offsetIndex] ^= dataMask;
        dataMask <<= 1;
        x += slope;

        // Last x-value rounded down.
        int xEnd = (x - slope).toInt();

        // Mark the span.
        span_->mark(xStart,xEnd);

        // Update the edge
        edge->x_ = x;
    }

	void EvenOddMaskGenerator32::processEdge(Edge* edge)
    {
        FixedPoint x     = edge->x_;
        FixedPoint slope = edge->slope_;
        int xStart       = x.toInt();
        long dataMask    = 0x1;
        int offsetIndex;
            
        offsetIndex = (x + SAMPLEMASK32x32[0]).toInt();
        maskBuffer_[offsetIndex] ^= dataMask;
        dataMask <<= 1;
        x += slope;
        
        offsetIndex = (x + SAMPLEMASK32x32[1]).toInt();
        maskBuffer_[offsetIndex] ^= dataMask;
        dataMask <<= 1;
        x += slope;

        offsetIndex = (x + SAMPLEMASK32x32[2]).toInt();
        maskBuffer_[offsetIndex] ^= dataMask;
        dataMask <<= 1;
        x += slope;

        offsetIndex = (x + SAMPLEMASK32x32[3]).toInt();
        maskBuffer_[offsetIndex] ^= dataMask;
        dataMask <<= 1;
        x += slope;

        offsetIndex = (x + SAMPLEMASK32x32[4]).toInt();
        maskBuffer_[offsetIndex] ^= dataMask;
        dataMask <<= 1;
        x += slope;
        
        offsetIndex = (x + SAMPLEMASK32x32[5]).toInt();
        maskBuffer_[offsetIndex] ^= dataMask;
        dataMask <<= 1;
        x += slope;

        offsetIndex = (x + SAMPLEMASK32x32[6]).toInt();
        maskBuffer_[offsetIndex] ^= dataMask;
        dataMask <<= 1;
        x += slope;

        offsetIndex = (x + SAMPLEMASK32x32[7]).toInt();
        maskBuffer_[offsetIndex] ^= dataMask;
        dataMask <<= 1;
        x += slope;

        offsetIndex = (x + SAMPLEMASK32x32[8]).toInt();
        maskBuffer_[offsetIndex] ^= dataMask;
        dataMask <<= 1;
        x += slope;
        
        offsetIndex = (x + SAMPLEMASK32x32[9]).toInt();
        maskBuffer_[offsetIndex] ^= dataMask;
        dataMask <<= 1;
        x += slope;

        offsetIndex = (x + SAMPLEMASK32x32[10]).toInt();
        maskBuffer_[offsetIndex] ^= dataMask;
        dataMask <<= 1;
        x += slope;

        offsetIndex = (x + SAMPLEMASK32x32[11]).toInt();
        maskBuffer_[offsetIndex] ^= dataMask;
        dataMask <<= 1;
        x += slope;

        offsetIndex = (x + SAMPLEMASK32x32[12]).toInt();
        maskBuffer_[offsetIndex] ^= dataMask;
        dataMask <<= 1;
        x += slope;
        
        offsetIndex = (x + SAMPLEMASK32x32[13]).toInt();
        maskBuffer_[offsetIndex] ^= dataMask;
        dataMask <<= 1;
        x += slope;

        offsetIndex = (x + SAMPLEMASK32x32[14]).toInt();
        maskBuffer_[offsetIndex] ^= dataMask;
        dataMask <<= 1;
        x += slope;

        offsetIndex = (x + SAMPLEMASK32x32[15]).toInt();
        maskBuffer_[offsetIndex] ^= dataMask;
        dataMask <<= 1;
        x += slope;
        
        offsetIndex = (x + SAMPLEMASK32x32[16]).toInt();
        maskBuffer_[offsetIndex] ^= dataMask;
        dataMask <<= 1;
        x += slope;
        
        offsetIndex = (x + SAMPLEMASK32x32[17]).toInt();
        maskBuffer_[offsetIndex] ^= dataMask;
        dataMask <<= 1;
        x += slope;

        offsetIndex = (x + SAMPLEMASK32x32[18]).toInt();
        maskBuffer_[offsetIndex] ^= dataMask;
        dataMask <<= 1;
        x += slope;

        offsetIndex = (x + SAMPLEMASK32x32[19]).toInt();
        maskBuffer_[offsetIndex] ^= dataMask;
        dataMask <<= 1;
        x += slope;

        offsetIndex = (x + SAMPLEMASK32x32[20]).toInt();
        maskBuffer_[offsetIndex] ^= dataMask;
        dataMask <<= 1;
        x += slope;
        
        offsetIndex = (x + SAMPLEMASK32x32[21]).toInt();
        maskBuffer_[offsetIndex] ^= dataMask;
        dataMask <<= 1;
        x += slope;

        offsetIndex = (x + SAMPLEMASK32x32[22]).toInt();
        maskBuffer_[offsetIndex] ^= dataMask;
        dataMask <<= 1;
        x += slope;

        offsetIndex = (x + SAMPLEMASK32x32[23]).toInt();
        maskBuffer_[offsetIndex] ^= dataMask;
        dataMask <<= 1;
        x += slope;

        offsetIndex = (x + SAMPLEMASK32x32[24]).toInt();
        maskBuffer_[offsetIndex] ^= dataMask;
        dataMask <<= 1;
        x += slope;
        
        offsetIndex = (x + SAMPLEMASK32x32[25]).toInt();
        maskBuffer_[offsetIndex] ^= dataMask;
        dataMask <<= 1;
        x += slope;

        offsetIndex = (x + SAMPLEMASK32x32[26]).toInt();
        maskBuffer_[offsetIndex] ^= dataMask;
        dataMask <<= 1;
        x += slope;

        offsetIndex = (x + SAMPLEMASK32x32[27]).toInt();
        maskBuffer_[offsetIndex] ^= dataMask;
        dataMask <<= 1;
        x += slope;

        offsetIndex = (x + SAMPLEMASK32x32[28]).toInt();
        maskBuffer_[offsetIndex] ^= dataMask;
        dataMask <<= 1;
        x += slope;
        
        offsetIndex = (x + SAMPLEMASK32x32[29]).toInt();
        maskBuffer_[offsetIndex] ^= dataMask;
        dataMask <<= 1;
        x += slope;

        offsetIndex = (x + SAMPLEMASK32x32[30]).toInt();
        maskBuffer_[offsetIndex] ^= dataMask;
        dataMask <<= 1;
        x += slope;

        offsetIndex = (x + SAMPLEMASK32x32[31]).toInt();
        maskBuffer_[offsetIndex] ^= dataMask;
        dataMask <<= 1;
        x += slope;

        // Last x-value rounded down.
        int xEnd = (x - slope).toInt();

        // Mark the span.
        span_->mark(xStart,xEnd);

        // Update the edge
        edge->x_ = x;
    }

    IEvenOddMaskGenerator& chooseEvenOddMaskGen(vplUint subPixelCount)
    {
        switch(subPixelCount)
        {
            case 32:
			{
                static EvenOddMaskGenerator32 eom32;
                return eom32;
			}

            case 16:
			{
                static EvenOddMaskGenerator16 eom16;
                return eom16;
			}

            case 8:
            default:
			{
                static EvenOddMaskGenerator8 eom8;
                return eom8;
			}
        }
    }
}