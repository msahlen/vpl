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

#ifndef VPL_MEMORY_H_INCLUDED_
#define VPL_MEMORY_H_INCLUDED_

#include <cstring>

#include "vplConfig.h"

#if defined(_MSC_VER)
    #define MEM_ALIGN(declaration, alignment) __declspec(align(alignment)) declaration
#elif defined(__GNUC__)
    #define MEM_ALIGN(declaration, alignment) declaration __attribute__ ((aligned (alignment)))
#else
    #define MEM_ALIGN(declaration, alignment) declaration
#endif

namespace vpl
{
	enum Alignment
	{
		cNoAlignment  = 0,
		cAlign8       = 8,
		cAlign16      = 16,
		cAlign32      = 32,
		cAlign64      = 64
	};

    template<typename T> class DynamicMemory
    {
    public:

		static const vplUint cInitialDynamicMemorySize = 64;

		DynamicMemory():size_(cInitialDynamicMemorySize),alignment_(0)
        {
            data_ = alignedData_ = new T[size_];
        }
        DynamicMemory(vplUint size):size_(size),alignment_(0)
        {
            data_ = alignedData_ = new T[size_];
        }
        DynamicMemory(vplUint size,Alignment alignment):size_(size),
										                alignment_(alignment - 1),
                                                        alignedData_(0),data_(0)

        {
            align();
        }

        ~DynamicMemory()
        {

			delete [] data_;

			alignedData_ = data_ = 0;
        }

        inline T* getMemory()                       
		{ 
			return alignedData_;
		}

        inline const T* getMemory() const           
		{ 
			return alignedData_;
		}
        
		inline vplUint getSize() const              
		{ 
			return size_;
		}
        
		inline T& operator[](vplUint i)             
		{ 
			return alignedData_[i];
		}
        
		inline const T& operator[](vplUint i) const 
		{ 
			return alignedData_[i];
		}
        
		inline T& getAt(vplUint i)                  
		{ 
			return alignedData_[i];
		}
        
		inline const T& getAt(vplUint i) const      
		{ 
			return alignedData_[i];
		}

        // Align data
        void align(Alignment alignment)
        {
            alignment_ = (alignment - 1);
            align();
        }

        void reAllocate(vplUint newSize)
        {
            vplUint oldSize = size_;
            size_ = newSize;

            // Reallocate data with alignment
            if(alignment_ )
            {
                T* newData        = new T[size_ + alignment_];
                T* newAlignedData = (T*)(size_t(newData + alignment_) & ~alignment_);

                std::memcpy(newAlignedData,alignedData_,oldSize*sizeof(T));

                delete [] data_;

                data_        = newData;
                alignedData_ = newAlignedData;
            }
            // Or without alignment
            else
            {
                T* newData = new T[size_];
                std::memcpy(newData,data_,oldSize*sizeof(T));

                delete [] data_;

                data_ = alignedData_ = newData;
            }
        }
        
		inline void reAllocate()
		{
			reAllocate(size_*2);
		}
        
		inline void clearAndResize(vplUint newSize)
        {
			if(data_)
				delete [] data_;

            alignedData_ = data_ = 0;
            size_ = newSize;

            if(alignment_)
                align();
            else
                data_ = alignedData_ = new T[size_];
        }

    private:

        void align()
        {
            T* newData        = new T[size_ + alignment_];
            T* newAlignedData = (T*)(size_t(newData + alignment_) & ~alignment_);

            if(data_)
                delete [] data_;

            data_ = newData;
            alignedData_ = newAlignedData;
        }

        vplUint size_;
        vplUint alignment_;
        T* alignedData_;
        T* data_;
    };
}

#endif