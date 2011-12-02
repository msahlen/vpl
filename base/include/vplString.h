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

#ifndef VPL_STRING_H_INCLUDED_
#define VPL_STRING_H_INCLUDED_

#include "vplArray.h"
#include "vplConfig.h"

namespace vpl
{
    class VPL_API String
    {
    public:
        
        String(){}

		String(const String& copy)
		{
			for(vplUint i = 0;i < copy.getLength() + 1;i++)
				string_.add(copy.string_[i]);
		}

        String(const char* string)
        {
            while(*string != '\0')
                string_.add(*string++);

            string_.add('\0');
        }
        ~String(){}

		// Getters
        inline vplUint getLength() const    
		{
			if(string_.getItemCount())
				return (string_.getItemCount() - 1);
			else
				return 0;
		}
        
		inline const char* getString() const    
		{ 
			return string_.getContents();
		}
        
		inline char* getString()                
		{ 
			return string_.getContents();
		}
        
		inline char operator[](vplUint i) const 
		{ 
			return string_[i];
		}
        
		// Operators
        inline String& operator = (const char* string)
        {
            string_.clear();

            while(*string != '\0')
                string_.add(*string++);

            string_.add('\0');

            return *this;
        }
       
        inline bool operator == (const char* string)
        {
            String newString = string;

            if(newString.getLength() != getLength())
                return false;

            for(vplUint i = 0 ; i < getLength();++i)
            {
                if(newString[i] != string_[i])
                    return false;
            }

            return true;
        }
        
        inline bool operator != (const char* string)
        {
            return !(*this == string);
        }

		// For appending strings
        inline void append(const char* string)
        {
            string_.removeLast();

            while(*string != '\0')
                string_.add(*string++);

            string_.add('\0');
        }

		inline void append(const String& string)
        {
            string_.removeLast();

			for(vplUint i = 0; i < string.getLength();++i)
				string_.add(string[i]);	

            string_.add('\0');
        }

    private:

        DynamicArray<char> string_;
    };
}
#endif