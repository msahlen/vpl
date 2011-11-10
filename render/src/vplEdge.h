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

#ifndef VPL_EDGE_H_INCLUDED_
#define VPL_EDGE_H_INCLUDED_

#include "vplVector.h"
#include "vplFixedPoint.h"
#include "vplLog.h"

namespace vpl
{
	class Edge
    {
    public:

        Edge():nextEdge_(0),firstScanLine_(0),lastScanLine_(0),winding_(0){}
        ~Edge(){}


        // Create edge between two points
        inline void makeEdge(const float p1x,const float p1y,
                      const float p2x,const float p2y)
        {
		    float dirX = p2x - p1x;
		    float dirY = p2y - p1y;

		    if(p1y <= p2y)
		    {
			    firstScanLine_ = static_cast<int>(p1y);
			    lastScanLine_  = static_cast<int>(p2y);
			    x_             = FixedPoint(p1x);
		    }
		    else
		    {
			    firstScanLine_ = static_cast<int>(p2y);
			    lastScanLine_  = static_cast<int>(p1y);
			    x_             = FixedPoint(p2x);
		    }
		
		    if(firstScanLine_ != lastScanLine_)
			    slope_ = FixedPoint(dirX / dirY);
		    else
			    slope_ = FixedPoint(0);
	    }

        // Insert edge
        inline void insert(Edge* edge)
        {
            // TODO: Find out why this is needed
            // Avoid circular lists
            if(this == nextEdge_ || edge == this)
				return;
            
            edge->nextEdge_ = nextEdge_;
            nextEdge_       = edge;
        }
       
        // Data
        FixedPoint x_;
        FixedPoint slope_;
        Edge*      nextEdge_;
        int        firstScanLine_;
        int        lastScanLine_;
        int        winding_;
    };
    
    class ActiveEdgeList
    {
    public:
        
        ActiveEdgeList():list_(0){}
        ~ActiveEdgeList(){}
        
        inline Edge* getList()   {return list_;}
        inline void  clearList() {list_ = 0;}
        inline Edge* insertAndGetNext(Edge* edge)
        {
            Edge* next = edge->nextEdge_;
            
            edge->nextEdge_ = 0;
            
            if(list_)
                list_->insert(edge);
            else
                list_ = edge;
            
            return next;
        }
        
        inline void remove(Edge* edge)
        {
            Edge* next = list_;
            Edge* prev = 0;

            // Find edge
            while(next)
            {
                // If found
                if(next == edge)
                {
                    // Update node and break loop
                    if(prev)
                        prev->nextEdge_ = next->nextEdge_;
                    else
                        list_ = next->nextEdge_;
                    
                    break;
                }
                
                prev = next;
				next = next->nextEdge_;
            }
        }
        
    private:
        
        Edge* list_;
    };
}
#endif
