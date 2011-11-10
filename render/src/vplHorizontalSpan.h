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

#ifndef VPL_HORIZONTAL_SPAN_H_INCLUDED_
#define VPL_HORIZONTAL_SPAN_H_INCLUDED_

#include <climits>

namespace vpl
{
    class HorizontalSpan
    {
    public:
        HorizontalSpan():max_(INT_MIN),min_(INT_MAX){}

        inline void reset()
        {
            max_ = INT_MIN;
            min_ = INT_MAX;
        }
        inline void mark(int start,int end)
        {
            if(end < start)
                mark(end,start);
            else
            {
                if(start < min_)
                    min_ = start;

                if(end > max_)
                    max_= end;
            }
        }
        inline int getMax() {return max_;}
        inline int getMin() {return min_;}

    private:

        int max_;
        int min_;
    };
}
#endif