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

#ifdef USE_SSE2_

extern "C" void PRE_CDECL_ memFill32SSE2(vplUint32* dest,
                                         vplUint32* value,
                                         vplUint count) POST_CDECL_;

inline void memFill32(vplUint32* dest,vplUint32 value,vplUint count)
{
	// No need to call optimized version if count is small 
    if(count < 7)
	{
        switch(count)
		{
			case 6: *dest++ = value;
			case 5: *dest++ = value;
			case 4: *dest++ = value;
			case 3: *dest++ = value;
			case 2: *dest++ = value;
			case 1: *dest++ = value;
		}
		return;
	}

	// Align destination
	vplUint align = (vplPtr)(dest) & 0xf;
		
	switch (align) 
	{
		case 4:  *dest++ = value; --count;
		case 8:  *dest++ = value; --count;
		case 12: *dest++ = value; --count;
	}

    // Call optimized version
    memFill32SSE2(dest,&value,count);
}

 #else

inline void memFill32(vplUint32* src,vplUint32 value,vplUint count)
{
    register vplUint n = (count + 7) / 8;

    switch(count & 0x07)
    {
        case 0: do
        {       *dest++ = src;
        case 7: *dest++ = src;
        case 6: *dest++ = src;
        case 5: *dest++ = src;
        case 4: *dest++ = src;
        case 3: *dest++ = src;
        case 2: *dest++ = src;
        case 1: *dest++ = src;
        } while (--n > 0);
    }
}

#endif

namespace vpl
{
	// Our own memcopy and memfill
	// http://en.wikipedia.org/wiki/Duffs_device

	template <class T> inline void vplMemFill(T* dest,T src,vplUint count)
	{
		register vplUint n = (count + 7) / 8;

        switch(count & 0x07)
        {
            case 0: do
            {       *dest++ = src;
            case 7: *dest++ = src;
            case 6: *dest++ = src;
            case 5: *dest++ = src;
            case 4: *dest++ = src;
            case 3: *dest++ = src;
            case 2: *dest++ = src;
            case 1: *dest++ = src;
            } while (--n > 0);
        }
	}

	template <class T> inline void vplMemCopy(T* dest,const T* src,vplUint count)
	{
		std::memcpy(dest,src,count*sizeof(T));
	}

    // Specialized version of memfill for 32 bit unsigned integers
    // Used in blendroutines, might be SSE2 accelerated
    inline void vplMemFill32(vplUint32* dest,vplUint32 value,vplUint count)
	{
		memFill32(dest,value,count);
	}

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

		// "Getters"
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

		inline T& getAt(vplUint i)
		{
			return alignedData_[i];
		}

		inline const T& getAt(vplUint i) const
		{
			return alignedData_[i];
		}

		// Operator overload to make it like an array.
		// Note: No bounds checking!
		inline T& operator[](vplUint i)
		{
			return alignedData_[i];
		}

		inline const T& operator[](vplUint i) const
		{
			return alignedData_[i];
		}

        // Align data
        void align(Alignment alignment)
        {
            alignment_ = (alignment - 1);
            align();
        }

		// Reallocate memory, keeping the contents if
		// new size is bigger than the old
        void reAllocate(vplUint newSize)
        {
            vplUint oldSize = size_;
            size_ = newSize;

            // Reallocate data with alignment
            if(alignment_ )
            {
                T* newData        = new T[size_ + alignment_];
                T* newAlignedData = (T*)(vplPtrDiff(newData + alignment_) & ~alignment_);

				if(newSize > oldSize)
					vplMemCopy(newAlignedData,alignedData_,oldSize);

                delete [] data_;

                data_        = newData;
                alignedData_ = newAlignedData;
            }
            // Or without alignment
            else
            {
                T* newData = new T[size_];

				if(newSize > oldSize)
					vplMemCopy(newData,data_,oldSize);

                delete [] data_;

                data_ = alignedData_ = newData;
            }
        }

		// Standard way to increase memory is to just double it
		inline void reAllocate()
		{
			reAllocate(size_*2);
		}

		// Clear data and set a new size
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
            T* newAlignedData = (T*)(vplPtrDiff(newData + alignment_) & ~alignment_);

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
