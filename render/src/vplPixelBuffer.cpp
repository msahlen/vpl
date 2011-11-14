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

#include "vplRender.h"
#include "vplColorHelper.h"

static inline vplUint findNextPowerOf2(vplUint& i,vpl::Alignment alignment)
{
	if(i < static_cast<vplUint>(alignment))
		i = alignment;

  vplUint k = alignment;
  
  while (k < i)
    k *= 2;

  return k;
}

namespace vpl
{
    PixelBuffer::PixelBuffer():width_(0),height_(0),pitch_(0),buffer_(0){}
    PixelBuffer::~PixelBuffer(){}

    void PixelBuffer::initialize(vplUint width,vplUint height,Alignment alignment)
    {
        if(buffer_)
			delete buffer_;

        pitch_  = width;
        width_  = width;
        height_ = height;

		if(alignment != cNoAlignment)
		{
			pitch_ = findNextPowerOf2(width_,alignment);
			buffer_ = new DynamicMemory<vplUint32>(height_*pitch_,alignment);
		}
		else
			buffer_ = new DynamicMemory<vplUint32>(height_*pitch_);

		memset(buffer_->getMemory(),0x00,pitch_*height_*sizeof(vplUint32));
    }
    void PixelBuffer::clear()
    {
        memset(buffer_->getMemory(),0x00,pitch_*height_*sizeof(vplUint32));
    }
    void PixelBuffer::clear(const Color& color)
    {
        vplUint32 c = color.getColorRGBA();

        c = preMultiplyColorRGBA(c);

		vplUint32* dest = buffer_->getMemory();

        // Duff it
        vplUint count = pitch_ * height_;

        register vplUint n = (count + 7) / 8;

        switch(count & 0x07)
        {
            case 0: do
            {   *dest++ = c;
                case 7: *dest++ = c;
                case 6: *dest++ = c;
                case 5: *dest++ = c;
                case 4: *dest++ = c;
                case 3: *dest++ = c;
                case 2: *dest++ = c;
                case 1: *dest++ = c;
            } while (--n > 0);
        }
    }
}