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

#ifndef VPL_ARRAY_H_INCLUDED_
#define VPL_ARRAY_H_INCLUDED_

#include "vplMemory.h"

namespace vpl
{

	// A simple dynamic array with data alignment support
	template<typename T> class VPL_API DynamicArray
    {
    public:

		// Initial size off array
		static const vplUint cInitialDynamicArraySize = 128;

        // Standard constructor, no alignment
        DynamicArray():itemCount_(0),memory_(0)
        {
            memory_ = new DynamicMemory<T>(cInitialDynamicArraySize);
        }
        DynamicArray(vplUint size):itemCount_(0),memory_(0)
        {
            memory_ = new DynamicMemory<T>(size);
        }
		DynamicArray(Alignment alignment):itemCount_(0),memory_(0)
        {
            memory_ = new DynamicMemory<T>(cInitialDynamicArraySize,alignment);
        }
        DynamicArray(vplUint size,Alignment alignment):itemCount_(0),memory_(0)
        {
            memory_ = new DynamicMemory<T>(size,alignment);
        }
        ~DynamicArray()
        {
            delete memory_;
        }

        // Return number of elements in array
        inline vplUint getItemCount() const {return itemCount_;}

        // Return last element
        inline T& getLast()             {return memory_->getAt(itemCount_ - 1);}
		inline const T& getLast() const {return memory_->getAt(itemCount_ - 1);}

        // Remove last element
        inline void removeLast()
        {
            if(itemCount_)
                itemCount_--;
        }

        // Access elements, no bounds checking
        inline T& operator[](vplUint i)             {return memory_->getAt(i);}
		inline const T& operator[](vplUint i) const {return memory_->getAt(i);}

        // Return a pointer to the start of the data
        inline T* getContents()             {return memory_->getMemory();}
		inline const T* getContents() const {return memory_->getMemory();}

        // Clear the content of the array
        inline void clear() {itemCount_ = 0;}

        // Add item to array
        inline void add(const T& item)
        {
            if(++itemCount_ > memory_->getSize())
            {
                T newItem = item;

                memory_->reAllocate();

                memory_->getAt(itemCount_ - 1) = newItem;
            }
            else
                memory_->getAt(itemCount_ - 1) = item;
        }

        // Align
        inline void align(Alignment alignment)
        {
            memory_->align(alignment);
        }

    private:

        vplUint itemCount_;
        DynamicMemory<T>* memory_;
    };
}
#endif
