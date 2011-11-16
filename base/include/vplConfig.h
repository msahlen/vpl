/* VPL - Vector path library
 * Copyright (C) 2009 - 2011 Mattias Sahlén <mattias.sahlen@gmail.com>>
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
 
#ifndef VPL_CONFIG_H_INCLUDED_
#define VPL_CONFIG_H_INCLUDED_

/* #undef USE_SSE_ */
#define HAVE_STDINT_H
/* #undef SSE_64_ */

// Defines for calling conventions
#ifdef USE_SSE_
    #if defined(__GNUC__)
        #define PRE_CDECL_
        #ifndef SSE_64_
            #define POST_CDECL_ __attribute__((cdecl))
        #else
            #define POST_CDECL_
        #endif
    #elif defined(_MSC_VER)
        #define PRE_CDECL_ __cdecl
        #define POST_CDECL_
#else
    #define PRE_CDECL
    #define POST_CDECL
    #endif
#endif // USE_SSE_

// Standard MS dll immport/export stuff
#ifdef _MSC_VER
    #ifdef VPL_BUILDING_DLL
        #define VPL_API __declspec(dllexport)
    #else
    #ifdef VPL_BUILDING_STATIC_LIB
        #define VPL_API
    #else
        #define VPL_API __declspec(dllimport)
    #endif
#endif

#else
    #define VPL_API
#endif //_MSC_VER

// Standard int stuff
#ifdef HAVE_STDINT_H

#include <stdint.h>
typedef int8_t vplInt8;
typedef int16_t vplInt16;
typedef int32_t vplInt32;
typedef int64_t vplInt64;
typedef uint8_t vplUint8;
typedef uint16_t vplUint16;
typedef uint32_t vplUint32;
typedef uint64_t vplUint64;

#else // HAVE_STDINT_H

#ifdef _MSC_VER

typedef signed char vplInt8;
typedef signed short vplInt16;
typedef signed __int32 vplInt32;
typedef signed __int64 vplInt64;
typedef unsigned char vplUint8;
typedef unsigned short vplUint16;
typedef unsigned __int32 vplUint32;
typedef unsigned __int64 vplUint64;

#endif // _MSC_VER

#endif // HAVE_STDINT_H

typedef unsigned int  vplUint;
typedef unsigned char vplUchar;

#endif // VPL_CONFIG_H_INCLUDED_


