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

#include "vplScanLineList.h"
#include "vplLog.h"

namespace vpl
{
	 // Scanline list implementation
    ScanLineList::~ScanLineList()
    {
        // Delete edge storage
		for(vplUint i = 0; i < allEdges_.getItemCount();i++)
			delete [] allEdges_[i];

        if(scanLines_)
            delete [] scanLines_;
    }
    void ScanLineList::init(vplUint numLines,vplUint numEdges,vplUint numSubPixels)
    {
        // Clear old edges
        for(vplUint i = 0; i < allEdges_.getItemCount();i++)
			delete [] allEdges_[i];

		allEdges_.clear();

        if(scanLines_)
            delete [] scanLines_;

        numAllocatedEdges_ = numEdges;
        numSubPixels_ = numSubPixels;
        numScanLines_ = numLines + 1;
        edgeCount_ = 0;
        maxY_ = INT_MIN;
        minY_ = INT_MAX;

        // Allocate new space
        edges_ = new Edge[numAllocatedEdges_];
        scanLines_ = new Edge*[numScanLines_];

		allEdges_.add(edges_);

        memset(scanLines_,0x0,sizeof(Edge*)*numScanLines_);
    }
	void ScanLineList::clear()
	{
		 // Clear old edges
        for(vplUint i = 1; i < allEdges_.getItemCount();i++)
			delete [] allEdges_[i];

		// Save first edge chunk
		Edge* initialEdges = allEdges_[0];

		// Clear and add initial
		allEdges_.clear();
		allEdges_.add(initialEdges);

		// Reset counter and scanline pointers
		edgeCount_ = 0;
		memset(scanLines_,0x0,sizeof(Edge*)*numScanLines_);
	}
    // Insert edge into scanline list
	void ScanLineList::insert(const Edge& edge)
    {
		// Get scanlines, take subpixels into account
		int firstLine = edge.firstScanLine_/ numSubPixels_;
		int lastLine  = edge.lastScanLine_ / numSubPixels_;

		// Add to scan line list
		edges_[edgeCount_] = edge;

        if(scanLines_[firstLine] == 0)
            scanLines_[firstLine] = &edges_[edgeCount_];
        else
            scanLines_[firstLine]->insert(&edges_[edgeCount_]);

        edgeCount_++;

        if(edgeCount_ == numAllocatedEdges_)
            resizeEdgeStorage();

		// See if we need to update min/max
		if(firstLine < minY_)
	        minY_ = firstLine;

        if(lastLine > maxY_)
            maxY_ = lastLine;

    }
    void ScanLineList::resizeEdgeStorage()
    {
		// Allocate new storage
        Edge* newEdges = new Edge[numAllocatedEdges_];

		// Update pointer
        edges_ = newEdges;

		// Store new pointer
		allEdges_.add(newEdges);

		//Reset index counter
		edgeCount_ = 0;
    }
    // Return edges in a scanline
    Edge* ScanLineList::getLine(int line)
    {
        return scanLines_[line];
    }
}