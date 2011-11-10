/* VPL - Vector path library
 * Copyright (C) 2009 - 2011 Mattias Sahlén <Mattias Sahlén <mattias.sahlen@gmail.com>>
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

#ifndef VPL_LOG_H_INCLUDED_
#define VPL_LOG_H_INCLUDED_

#include "vplConfig.h"
#include "vplString.h"

// Logging defines
#define STD_LOG_ERROR()   (vpl::Log(vpl::Log::cLogLevelError))

#define STD_LOG_WARNING() if(vpl::Log::cLogLevelWarning > vpl::Log::logLevel());\
                          else vpl::Log(vpl::Log::cLogLevelWarning)

#define STD_LOG_INFO()    if(vpl::Log::cLogLevelInfo > vpl::Log::logLevel());\
                          else vpl::Log(vpl::Log::cLogLevelInfo)

#define STD_LOG_DEBUG()   if(vpl::Log::cLogLevelDebug > vpl::Log::logLevel());\
                          else vpl::Log(vpl::Log::cLogLevelDebug)

#define FILE_LOG_ERROR(f) if(vpl::Log::cLogLevelInfo > vpl::Log::logLevel());\
                          else (vpl::Log(f,vpl::Log::cLogLevelError)

#define FILE_LOG_WARNING(f) if(vpl::Log::cLogLevelInfo > vpl::Log::logLevel());\
                            else vpl::Log(f,vpl::Log::cLogLevelWarning)

#define FILE_LOG_INFO(f) if(vpl::Log::cLogLevelInfo > vpl::Log::logLevel());\
                         else vpl::Log(f,vpl::Log::cLogLevelInfo)

#define FILE_LOG_DEBUG(f) if(vpl::Log::cLogLevelInfo > vpl::Log::logLevel());\
                          else vpl::Log(f,vpl::Log::cLogLevelDebug)

// Macros for debugging, will only be compiled in debug release
#ifdef _DEBUG

#define FILE_DEBUG_(f) if (0) ; else vpl::Log(f,vpl::Log::cLogLevelDebug)
#define STD_DEBUG_() if (0) ; else vpl::Log(vpl::Log::cLogLevelDebug)

#else

#define FILE_DEBUG(f) if (1) ; else vpl::Log(f,vpl::Log::cLogLevelDebug)
#define STD_DEBUG() if (1) ; else vpl::Log(vpl::Log::cLogLevelDebug)

#endif

// Log class
namespace vpl
{
	class VPL_API Log
	{   
    public:
        // Logging levels
        enum LogLevel
        {
            cLogLevelError = 0,
            cLogLevelWarning,
            cLogLevelInfo,
            cLogLevelDebug,
        };

        Log(const char* path,LogLevel level);
        Log(LogLevel level = cLogLevelError);
		~Log();

        Log& operator << (const float f);
        Log& operator << (const double d);
        Log& operator << (const char* string);
        Log& operator << (const String& string);
        Log& operator << (const vplInt8 i);
        Log& operator << (const vplInt16 i);
        Log& operator << (const vplInt32 i);
        Log& operator << (const vplInt64 i);
        Log& operator << (const vplUint8 i);
        Log& operator << (const vplUint16 i);
        Log& operator << (const vplUint32 i);
        Log& operator << (const vplUint64 i);

		static LogLevel& logLevel();
		
   private:

        String log_;
        String path_;
        char conversionBuffer_[128];
		static LogLevel logLevel_;

        // Prevent copying
        Log(const Log&){}
        Log& operator =(const Log&);
	};
}
#endif