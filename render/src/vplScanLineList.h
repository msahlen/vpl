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

#ifndef VPL_SCAN_LINE_LIST_H_INCLUDED_
#define VPL_SCAN_LINE_LIST_H_INCLUDED_

#include "vplArray.h"
#include "vplConfig.h"
#include "vplEdge.h"

namespace vpl
{
	class VPL_API ScanLineList
    {
    public:
        ScanLineList():edges_(0),scanLines_(0),numSubPixels_(0),
                       edgeCount_(0),numAllocatedEdges_(0),
                       numScanLines_(0),maxY_(INT_MIN),minY_(INT_MAX){}

        ~ScanLineList();

        // Set initial size
        void init(vplUint numLines,vplUint numEdges,vplUint numSubPixels);

        // Clear the list, this will delete all arrays
        // that have been added by resizing and
        // clear the initial array
        void clear();

        // Edges inserted have scanlines in subpixels
        void insert(const Edge& edge);

        // Getters
        Edge* getLine(int line);

        inline int getMax() 
		{
			return maxY_;
		}
        
		inline int getMin() 
		{
			return minY_;
		}

		inline vplUint getNumberOfEdges()     
		{
			return edgeCount_; 
		}
        
		inline vplUint getNumberOfScanLines() 
		{ 
			return numScanLines_;
		}
        
		inline vplUint getNumberOfSubpixels() 
		{ 
			return numSubPixels_;
		}

    private:

        void resizeEdgeStorage();

        Edge*  edges_;
        Edge** scanLines_;
        DynamicArray<Edge*> allEdges_;
        vplUint numSubPixels_;
        vplUint edgeCount_;
        vplUint numAllocatedEdges_;
        vplUint numScanLines_;
        int  maxY_;
        int  minY_;
    };
}

#endif