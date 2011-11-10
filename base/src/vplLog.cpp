/* VPL - Vector path library
 * Copyright (C) 2009 - 2011 Mattias Sahlén <mattias.sahlen@gmail.com>
 *
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


#include <ctime>
#include <cstring>
#include <cstdio>
#include <iostream>
#include <fstream>
#include "vplLog.h"

// Get the current time
static vpl::String getTime()
{
    // Get time as string
	time_t rawtime;
	struct tm * timeinfo;

    time(&rawtime);

    timeinfo = localtime(&rawtime);

    char* time   = asctime(timeinfo);

    time[strlen(time) - 1] = '\0';

	vpl::String timeString = time;

    return timeString;
}
static const char* getLogLevel(vpl::Log::LogLevel level)
{
    static const char* const levels[] = {" ERROR: "," WARNING: ",
                                         " INFO: "," DEBUG: "};

    return levels[level];
}
namespace vpl
{
	Log::Log(const char* path,LogLevel level):path_(path)
	{
		log_.append("- ");
        log_.append(getTime().getString());
        log_.append(getLogLevel(level));
	}

    Log::Log(LogLevel level)
    {
        log_.append("- ");
        log_.append(getTime());
        log_.append(getLogLevel(level));
    }

	Log& Log::operator << (const float f)
	{
		sprintf(conversionBuffer_,"%f",f);

		log_.append(conversionBuffer_);

		return *this;
	}

    Log& Log::operator << (const double d)
	{
		sprintf(conversionBuffer_,"%f",d);

		log_.append(conversionBuffer_);

		return *this;
	}

    Log& Log::operator << (const char* string)
	{
		log_.append(string);

		return *this;
	}

    Log& Log::operator << (const String& string)
	{
		log_.append(string);

		return *this;
	}

    Log& Log::operator << (const vplInt8 i)
	{
		sprintf(conversionBuffer_,"%i",i);

		log_.append(conversionBuffer_);

		return *this;
	}

    Log& Log::operator << (const vplInt16 i)
	{
		sprintf(conversionBuffer_,"%i",i);

		log_.append(conversionBuffer_);

		return *this;
	}

    Log& Log::operator << (const vplInt32 i)
	{
		sprintf(conversionBuffer_,"%i",i);

		log_.append(conversionBuffer_);

		return *this;
	}

    Log& Log::operator << (const vplInt64 i)
	{
		sprintf(conversionBuffer_,"%li",i);

		log_.append(conversionBuffer_);

		return *this;
	}

    Log& Log::operator << (const vplUint8 i)
	{
		sprintf(conversionBuffer_,"%u",i);

		log_.append(conversionBuffer_);

		return *this;
	}

    Log& Log::operator << (const vplUint16 i)
	{
		sprintf(conversionBuffer_,"%u",i);

		log_.append(conversionBuffer_);

		return *this;
	}

    Log& Log::operator << (const vplUint32 i)
	{
		sprintf(conversionBuffer_,"%u",i);

		log_.append(conversionBuffer_);

		return *this;
	}

    Log& Log::operator << (const vplUint64 i)
	{
		sprintf(conversionBuffer_,"%lu",i);

		log_.append(conversionBuffer_);

		return *this;
	}

	Log::LogLevel& Log::logLevel()
	{
		return logLevel_;
	}
    // Write to file or standard out
	Log::~Log()
	{
        log_.append("\n");

		if(path_.getLength() != 0)
		{
			std::ofstream logFile(path_.getString(),std::ios::app);

			if(logFile.is_open())
			{
                logFile << log_.getString();
				logFile.flush();
				logFile.close();
			}
			else
			{
				fprintf(stderr, "%s", log_.getString());
				fflush(stderr);
			}
		}
		else
			std::cout << log_.getString();
	}

	VPL_API Log::LogLevel Log::logLevel_ = Log::cLogLevelWarning;
}
