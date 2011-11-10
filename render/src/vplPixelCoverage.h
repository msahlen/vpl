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

#ifndef VPL_PIXEL_COVERAGE_H_INCLUDED_
#define VPL_PIXEL_COVERAGE_H_INCLUDED_

#include "vplConfig.h"

namespace vpl
{
    #define COVERAGE(a) (((a) & 1) + (((a) >> 1) & 1) + (((a) >> 2) & 1) +     \
                        (((a) >> 3) & 1) +(((a) >> 4) & 1) + (((a) >> 5) & 1) +\
                        (((a) >> 6) & 1) + (((a) >> 7) & 1))

    static const vplUchar coverageTable[256] =
    {
        COVERAGE(0x00),COVERAGE(0x01),COVERAGE(0x02),COVERAGE(0x03),
        COVERAGE(0x04),COVERAGE(0x05),COVERAGE(0x06),COVERAGE(0x07),
        COVERAGE(0x08),COVERAGE(0x09),COVERAGE(0x0a),COVERAGE(0x0b),
        COVERAGE(0x0c),COVERAGE(0x0d),COVERAGE(0x0e),COVERAGE(0x0f),
        COVERAGE(0x10),COVERAGE(0x11),COVERAGE(0x12),COVERAGE(0x13),
        COVERAGE(0x14),COVERAGE(0x15),COVERAGE(0x16),COVERAGE(0x17),
        COVERAGE(0x18),COVERAGE(0x19),COVERAGE(0x1a),COVERAGE(0x1b),
        COVERAGE(0x1c),COVERAGE(0x1d),COVERAGE(0x1e),COVERAGE(0x1f),
        COVERAGE(0x20),COVERAGE(0x21),COVERAGE(0x22),COVERAGE(0x23),
        COVERAGE(0x24),COVERAGE(0x25),COVERAGE(0x26),COVERAGE(0x27),
        COVERAGE(0x28),COVERAGE(0x29),COVERAGE(0x2a),COVERAGE(0x2b),
        COVERAGE(0x2c),COVERAGE(0x2d),COVERAGE(0x2e),COVERAGE(0x2f),
        COVERAGE(0x30),COVERAGE(0x31),COVERAGE(0x32),COVERAGE(0x33),
        COVERAGE(0x34),COVERAGE(0x35),COVERAGE(0x36),COVERAGE(0x37),
        COVERAGE(0x38),COVERAGE(0x39),COVERAGE(0x3a),COVERAGE(0x3b),
        COVERAGE(0x3c),COVERAGE(0x3d),COVERAGE(0x3e),COVERAGE(0x3f),
        COVERAGE(0x40),COVERAGE(0x41),COVERAGE(0x42),COVERAGE(0x43),
        COVERAGE(0x44),COVERAGE(0x45),COVERAGE(0x46),COVERAGE(0x47),
        COVERAGE(0x48),COVERAGE(0x49),COVERAGE(0x4a),COVERAGE(0x4b),
        COVERAGE(0x4c),COVERAGE(0x4d),COVERAGE(0x4e),COVERAGE(0x4f),
        COVERAGE(0x50),COVERAGE(0x51),COVERAGE(0x52),COVERAGE(0x53),
        COVERAGE(0x54),COVERAGE(0x55),COVERAGE(0x56),COVERAGE(0x57),
        COVERAGE(0x58),COVERAGE(0x59),COVERAGE(0x5a),COVERAGE(0x5b),
        COVERAGE(0x5c),COVERAGE(0x5d),COVERAGE(0x5e),COVERAGE(0x5f),
        COVERAGE(0x60),COVERAGE(0x61),COVERAGE(0x62),COVERAGE(0x63),
        COVERAGE(0x64),COVERAGE(0x65),COVERAGE(0x66),COVERAGE(0x67),
        COVERAGE(0x68),COVERAGE(0x69),COVERAGE(0x6a),COVERAGE(0x6b),
        COVERAGE(0x6c),COVERAGE(0x6d),COVERAGE(0x6e),COVERAGE(0x6f),
        COVERAGE(0x70),COVERAGE(0x71),COVERAGE(0x72),COVERAGE(0x73),
        COVERAGE(0x74),COVERAGE(0x75),COVERAGE(0x76),COVERAGE(0x77),
        COVERAGE(0x78),COVERAGE(0x79),COVERAGE(0x7a),COVERAGE(0x7b),
        COVERAGE(0x7c),COVERAGE(0x7d),COVERAGE(0x7e),COVERAGE(0x7f),
        COVERAGE(0x80),COVERAGE(0x81),COVERAGE(0x82),COVERAGE(0x83),
        COVERAGE(0x84),COVERAGE(0x85),COVERAGE(0x86),COVERAGE(0x87),
        COVERAGE(0x88),COVERAGE(0x89),COVERAGE(0x8a),COVERAGE(0x8b),
        COVERAGE(0x8c),COVERAGE(0x8d),COVERAGE(0x8e),COVERAGE(0x8f),
        COVERAGE(0x90),COVERAGE(0x91),COVERAGE(0x92),COVERAGE(0x93),
        COVERAGE(0x94),COVERAGE(0x95),COVERAGE(0x96),COVERAGE(0x97),
        COVERAGE(0x98),COVERAGE(0x99),COVERAGE(0x9a),COVERAGE(0x9b),
        COVERAGE(0x9c),COVERAGE(0x9d),COVERAGE(0x9e),COVERAGE(0x9f),
        COVERAGE(0xa0),COVERAGE(0xa1),COVERAGE(0xa2),COVERAGE(0xa3),
        COVERAGE(0xa4),COVERAGE(0xa5),COVERAGE(0xa6),COVERAGE(0xa7),
        COVERAGE(0xa8),COVERAGE(0xa9),COVERAGE(0xaa),COVERAGE(0xab),
        COVERAGE(0xac),COVERAGE(0xad),COVERAGE(0xae),COVERAGE(0xaf),
        COVERAGE(0xb0),COVERAGE(0xb1),COVERAGE(0xb2),COVERAGE(0xb3),
        COVERAGE(0xb4),COVERAGE(0xb5),COVERAGE(0xb6),COVERAGE(0xb7),
        COVERAGE(0xb8),COVERAGE(0xb9),COVERAGE(0xba),COVERAGE(0xbb),
        COVERAGE(0xbc),COVERAGE(0xbd),COVERAGE(0xbe),COVERAGE(0xbf),
        COVERAGE(0xc0),COVERAGE(0xc1),COVERAGE(0xc2),COVERAGE(0xc3),
        COVERAGE(0xc4),COVERAGE(0xc5),COVERAGE(0xc6),COVERAGE(0xc7),
        COVERAGE(0xc8),COVERAGE(0xc9),COVERAGE(0xca),COVERAGE(0xcb),
        COVERAGE(0xcc),COVERAGE(0xcd),COVERAGE(0xce),COVERAGE(0xcf),
        COVERAGE(0xd0),COVERAGE(0xd1),COVERAGE(0xd2),COVERAGE(0xd3),
        COVERAGE(0xd4),COVERAGE(0xd5),COVERAGE(0xd6),COVERAGE(0xd7),
        COVERAGE(0xd8),COVERAGE(0xd9),COVERAGE(0xda),COVERAGE(0xdb),
        COVERAGE(0xdc),COVERAGE(0xdd),COVERAGE(0xde),COVERAGE(0xdf),
        COVERAGE(0xe0),COVERAGE(0xe1),COVERAGE(0xe2),COVERAGE(0xe3),
        COVERAGE(0xe4),COVERAGE(0xe5),COVERAGE(0xe6),COVERAGE(0xe7),
        COVERAGE(0xe8),COVERAGE(0xe9),COVERAGE(0xea),COVERAGE(0xeb),
        COVERAGE(0xec),COVERAGE(0xed),COVERAGE(0xee),COVERAGE(0xef),
        COVERAGE(0xf0),COVERAGE(0xf1),COVERAGE(0xf2),COVERAGE(0xf3),
        COVERAGE(0xf4),COVERAGE(0xf5),COVERAGE(0xf6),COVERAGE(0xf7),
        COVERAGE(0xf8),COVERAGE(0xf9),COVERAGE(0xfa),COVERAGE(0xfb),
        COVERAGE(0xfc),COVERAGE(0xfd),COVERAGE(0xfe),COVERAGE(0xff)
    };

    static const vplUint32 cFullSubCoverage8  = 0xff;
    static const vplUint32 cFullSubCoverage16 = 0xffff;
    static const vplUint32 cFullSubCoverage32 = 0xffffffff;

    //Compute pixel coverage
    inline vplUchar getPixelCoverage(vplUint32 mask)
    {
        return coverageTable[mask & 0xff] +         \
               coverageTable[(mask >> 8) & 0xff] +  \
               coverageTable[(mask >> 16) & 0xff] + \
               coverageTable[(mask >> 24) & 0xff];
    }
}
#endif