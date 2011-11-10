/* VPL - Vector path library
 * Copyright (C) 2009 - 2011Mattias Sahlén: <mattias.sahlen@gmail.com>
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

#include "vpl.h"

int main()
{
	// Set the highest logging threshold, Log::cLogLevelWarning is default
	vpl::Log::logLevel() = vpl::Log::cLogLevelError;

	// Only Error will be printed
	STD_LOG_ERROR()   << "Error\n";
	STD_LOG_WARNING() << "Warning";
	STD_LOG_INFO()    << "Info";
	STD_LOG_DEBUG()   << "Debug";

	vpl::Log::logLevel() = vpl::Log::cLogLevelWarning;

	// Error and Warning will be printed
	STD_LOG_ERROR()   << "Error";
	STD_LOG_WARNING() << "Warning\n";
	STD_LOG_INFO()    << "Info";
	STD_LOG_DEBUG()   << "Debug";

	vpl::Log::logLevel() = vpl::Log::cLogLevelInfo;

	// Error, Warning and Info will be printed
	STD_LOG_ERROR()   << "Error";
	STD_LOG_WARNING() << "Warning";
	STD_LOG_INFO()    << "Info\n";
	STD_LOG_DEBUG()   << "Debug";

	vpl::Log::logLevel() = vpl::Log::cLogLevelDebug;

	// Everything will be printed
	STD_LOG_ERROR()   << "Error";
	STD_LOG_WARNING() << "Warning";
	STD_LOG_INFO()    << "Info";
	STD_LOG_DEBUG()   << "Debug";

	STD_LOG_DEBUG()   << "Done";
}
