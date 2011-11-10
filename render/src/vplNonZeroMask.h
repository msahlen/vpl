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

#ifndef VPL_NONZEROMASK_H_INCLUDED_
#define VPL_NONZEROMASK_H_INCLUDED_

#include <cstring>
#include "vplConfig.h"

namespace vpl
{
    class NonZeroMask
    {
    public:

        NonZeroMask():mask_(0),subPixelBuffer_(0),subPixelCount_(0){}

        ~NonZeroMask()
        {
            if(subPixelBuffer_)
            {
                delete [] subPixelBuffer_;
                subPixelBuffer_ = 0;
            }
        }

        // Initialization
        void initializeSubPixelBuffer(vplUint subPixelCount)
        {
            if(subPixelBuffer_)
                delete [] subPixelBuffer_;

            subPixelBuffer_ = new char[subPixelCount];
            memset(subPixelBuffer_,0x0,sizeof(char)*subPixelCount);
            subPixelCount_ = subPixelCount;
        }

        // Accessors
        inline vplUint32& getMask()       { return mask_;}
        inline vplUint32 readMask() const { return mask_;}
        inline char& operator[](int i) const
        {
            return subPixelBuffer_[i];
        }

        // Clear mask
        inline void reset()
        {
            for(vplUint i = 0; i < subPixelCount_;i++)
                subPixelBuffer_[i] = 0;

            mask_ = 0;
        }
        // Set to first value in maskbuffer
        inline NonZeroMask& reset(const NonZeroMask &rhs)
        {
            vplUint32 bit = 0x1;
            mask_ = 0;

            for(vplUint i = 0; i < subPixelCount_;i++)
            {
                if(rhs.mask_ & bit)
                {
                    subPixelBuffer_[i] = rhs.subPixelBuffer_[i];

                    if(subPixelBuffer_[i])
                        mask_ |= bit;
                }

                bit <<= 1;
            }

            return *this;
        }
        // After an "on" span initialize with next mask
        inline NonZeroMask& initialize(const NonZeroMask &rhs)
        {
            vplUint32 bit = 0x1;

            for(vplUint i = 0; i < subPixelCount_;i++)
            {
                if(rhs.mask_ & bit)
                {
                    subPixelBuffer_[i] = rhs.subPixelBuffer_[i];

                    if(subPixelBuffer_[i])
                        mask_ |= bit;
                }
                bit <<= 1;
            }

            return *this;
        }
        // Add masks
        inline NonZeroMask& accumulate(const NonZeroMask &rhs)
        {
            vplUint32 bit = 0x1;
            char temp;

            for(vplUint i = 0; i < subPixelCount_;i++)
            {
                // If bit is set, add winding
                if(rhs.mask_ & bit)
                {
                    temp = subPixelBuffer_[i];
                    subPixelBuffer_[i] += rhs.subPixelBuffer_[i];

                    // If winding changed, toggle bit
                    if((temp == 0) ^ (subPixelBuffer_[i] == 0))
                        mask_ ^= bit;
                }

                bit <<= 1;
            }

            return *this;
        }

    private:

        vplUint32 mask_;
        char* subPixelBuffer_;
        vplUint subPixelCount_;
	};

	class VPL_API NonZeroMaskBuffer
	{
    public:

        NonZeroMaskBuffer():buffer_(0),size_(0){}
        ~NonZeroMaskBuffer()
        {
            if(buffer_)
                delete [] buffer_;
        }

        // Initialization
        inline void initialize(vplUint size,vplUint subPixelCount)
        {

            size += 1;

            if(size > size_)
            {
                delete [] buffer_;

                buffer_ = new NonZeroMask[size];

                size_ = size;
            }

            for(vplUint i = 0; i < size_;i++)
                buffer_[i].initializeSubPixelBuffer(subPixelCount);

        }

        // Accessor
        inline NonZeroMask& operator[](int i) const
        {
            return buffer_[i];
        }

    private:

        NonZeroMask* buffer_;
        vplUint size_;

	};
}
#endif