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

#include "vplConfig.h"
#include "vplArray.h"
#include "vplMap.h"
#include "vplLog.h"

static const vplUint cMask = 0xffff;
static const vplUint cWindowSize = 32768;
static const vplUint cMinMatch = 3;
static const vplUint cMaxMatch = 258;
static const vplInt cInvalidIndex = -1;

inline vplUint hash(const vplUint8* data)
{
    return ((data[0] << 10) ^ (data[1] << 5) ^ data[2]) & cHashMask;
}

class DeflateCompressor
{

public:

    DeflateCompressor():hashCode_(0),matchHead_(0){}
    ~DeflateCompressor(){}

    void init();
    bool compressData(const vplUint8* data, vplUint length);

    inline vplUint8* getCompressedData()
    {
        return compressedData_.getContents();

    }

private:

    inline void insertString(const vplUint8* data,vplUint stringStart)
    {
        hashCode_ = hash(data + stringStart + 2);
        prev_[stringStart & cMask] = matchHead_ = head_[hashCode_];
        head_[hashCode_] = stringStart;
    }

    inline vplUint findMatch(const vplUint8* data,
                             vplUint stringStart,
                             vplUint currentMatch)
    {
        register vplUint8* scan = data + stringStart;           // current string
        register vplUint8* end = data + stringStart + cMaxMatch;// end of string
        register vplUint8* match = data + currentMatch;         // matched string
        register vplInt len;                                    // length of current match

        // Return failure if the match length is less than 2:
        if(match[0] != scan[0] || match[1] != scan[1])
            return cMinMatch - 1;

        // The check at best_len-1 can be removed because it will be made
        // again later. (This heuristic is not always a win.)
        // It is not necessary to compare scan[2] and match[2] since they
        // are always equal when the other bytes match, given that
        // the hash keys are equal and that HASH_BITS >= 8.

        scan  += 2;
        match += 2;

        while (*scan == *match && scan < end)
        {
            scan++;
            match++;
        }

        len = cMaxMatch - (int)(end - scan);

        if(len < cMinMatch)
            return cMinMatch - 1;

        return len;
    }

    vpl::DynamicArray<vplUint8> compressedData_;
    vplInt32 head_[cWindowSize];
    vplInt32 prev_[cWindowSize];
    vplUint hashCode_;
    vplUint matchHead_;
};

void DeflateCompressor::init()
{
    compressedData_.clear();

    vpl::vplMemFill(head_,cInvalidIndex,cWindowSize);
    vpl::vplMemFill(prev_,cInvalidIndex,cWindowSize);

    hashCode_ = matchHead_ = 0;
}

bool DeflateCompressor::compressData(const vplUint8* data, const vplUint length)
{
    vplUint stringStart = 0;
    vplUint matchLength = 0;

    while(true)
    {
        insertString(data,stringStart);

        if((cInvalidIndex != matchHead_) && ((stringStart - matchHead_) <= cWindowSize))
        {
            matchLength = findMatch(data,stringStart,matchHead_);
        }
    }

}

namespace vpl
{
}