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

#ifndef VPL_EVEN_ODD_MASK_GENERATOR_H_INCLUDED_
#define VPL_EVEN_ODD_MASK_GENERATOR_H_INCLUDED_

#include "vplConfig.h"

namespace vpl
{
	// Forward declarations
	class Edge;
	class HorizontalSpan;
	class FixedPoint;

    class IEvenOddMaskGenerator
    {
    public:
        IEvenOddMaskGenerator():maskBuffer_(0),subPixelCount_(0),
                               span_(0),offsets_(0){}

        ~IEvenOddMaskGenerator(){}

        // Initialize mask generator with mask buffer,
        // subpixel count and the span
        void initMaskGenerator(vplUint32* buffer,
                               unsigned int spCount,
                               HorizontalSpan* span);

        // Process different types of edges
        void processEdgeOnFirstScanline(Edge* edge,int subPixelEnd);
        void processEdgeOnLastScanline(Edge* edge);
        virtual void processEdge(Edge* edge) = 0;

    protected:

        vplUint32* maskBuffer_;
        unsigned int subPixelCount_;
        HorizontalSpan* span_;
        const FixedPoint* offsets_;
    };

	class EvenOddMaskGenerator8:public IEvenOddMaskGenerator
    {
    public:
        EvenOddMaskGenerator8(){};
        ~EvenOddMaskGenerator8(){};

        void processEdge(Edge* edge);
	};

	class EvenOddMaskGenerator16:public IEvenOddMaskGenerator
    {
    public:
        EvenOddMaskGenerator16(){};
        ~EvenOddMaskGenerator16(){};

        void processEdge(Edge* edge);
	};

	class EvenOddMaskGenerator32:public IEvenOddMaskGenerator
    {
    public:
        EvenOddMaskGenerator32(){};
        ~EvenOddMaskGenerator32(){};

        void processEdge(Edge* edge);
	};

    IEvenOddMaskGenerator& chooseEvenOddMaskGen(vplUint subPixelCount);

}
#endif