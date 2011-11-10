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

#ifndef VPL_NON_ZERO_MASK_GENERATOR_H_
#define VPL_NON_ZERO_MASK_GENERATOR_H_

#include "vplConfig.h"

namespace vpl
{
	// Forward declarations
	class NonZeroMaskBuffer;
	class HorizontalSpan;
	class FixedPoint;
	class Edge;

	class INonZeroMaskGenerator
    {
    public:
        INonZeroMaskGenerator():maskBuffer_(0),subPixelCount_(0),
                               span_(0),offsets_(0){}

        ~INonZeroMaskGenerator(){}

        // Initialization
        void initMaskGenerator(NonZeroMaskBuffer* buffer,
                               vplUint subPixelCount,
                               HorizontalSpan* span);

        // Process different edge types
        void processEdgeOnFirstScanline(Edge* edge,int subPixelEnd);
        void processEdgeOnLastScanline(Edge* edge);
        virtual void processEdge(Edge* edge) = 0;

    protected:

        NonZeroMaskBuffer* maskBuffer_;
        vplUint subPixelCount_;
        HorizontalSpan* span_;
        const FixedPoint* offsets_;
    };

	class NonZeroMaskGenerator8:public INonZeroMaskGenerator
    {
    public:
        NonZeroMaskGenerator8(){}
        ~NonZeroMaskGenerator8(){}

        void processEdge(Edge* edge);
	};

	class NonZeroMaskGenerator16:public INonZeroMaskGenerator
    {
    public:
        NonZeroMaskGenerator16(){}
        ~NonZeroMaskGenerator16(){}

        void processEdge(Edge* edge);
	};

	class NonZeroMaskGenerator32:public INonZeroMaskGenerator
    {
    public:
        NonZeroMaskGenerator32(){}
        ~NonZeroMaskGenerator32(){}

        void processEdge(Edge* edge);
	};

    INonZeroMaskGenerator& chooseNonZeroMaskGen(vplUint subPixelCount);
}

#endif
