/* VPL - Vector path library
 * Copyright (C) 2009 - 2011 Enlightenment Developers: mattias.sahlen@gmail.com>
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

// STL headers
#include <algorithm>
#include <cctype>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>

#include "SVGParser.h"

// one degree in radians
static const float cDegree = 0.0174532925f;

static inline int round(float f)
{
    return f >= 0.0f ? int(f + 0.5f) : int(f - int(f - 1) + 0.5f) + int(f - 1);
}
// Tokenizer function
static inline size_t tokenize(const std::string& str,
                              std::vector<std::string>& tokens,
                              const std::string& delimiters = " ")
{
    // Skip delimiters at beginning.
    std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);

    // Find first "non-delimiter".
    std::string::size_type pos = str.find_first_of(delimiters, lastPos);

    // Bail if no delimiter found
    if(std::string::npos == pos)
        return tokens.size();

    while(std::string::npos != pos || std::string::npos != lastPos)
    {
        //DEBUG() << "Token: " << str.substr(lastPos, pos - lastPos);
        // Found a token, add it to the vector.

        tokens.push_back(str.substr(lastPos, pos - lastPos));

        // Skip delimiters"
        lastPos = str.find_first_not_of(delimiters, pos);

        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }

    return tokens.size();
}
static inline vplUint tokenize(const std::string& str,
                               std::vector<std::string>& tokens,
                               std::vector<char>& delimitersFound,
                               const std::string& delimiters = " ")
{
    // Skip delimiters at beginning.
    std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);

    // Find first "non-delimiter".
    std::string::size_type pos = str.find_first_of(delimiters, lastPos);

    // Bail if no delimiter found
    if(std::string::npos == pos)
        pos = str.size();

    while(std::string::npos != pos || std::string::npos != lastPos)
    {
        // Found a token, add it to the vector, store delimiter
        if(lastPos != 0)
        {
            //DEBUG() << "Token: " << str.substr(lastPos, pos - lastPos);

            delimitersFound.push_back(str.at(lastPos - 1));
            tokens.push_back(str.substr(lastPos, pos - lastPos));
        }
        // Skip delimiters"
        lastPos = str.find_first_not_of(delimiters, pos);

        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
    return delimitersFound.size();
}
static inline void replace(std::string& str,
                           const std::string& oldString,
                           const std::string& newString)
{
    size_t pos = 0;
    while((pos = str.find(oldString, pos)) != std::string::npos)
    {
        str.replace(pos, oldString.length(), newString);
        pos += newString.length();
    }
}

static inline void removeWhiteSpaces(std::string& str)
{
    str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end());
}
static inline void removeCharacters(std::string& str)
{
    str.erase(std::remove_if(str.begin(), str.end(), ::isalpha), str.end());
}
static inline bool isNewLine(char c)
{
    return c == '\n';
}
static inline void replaceNewLines(std::string& str)
{
    std::replace_if(str.begin(),
                    str.end(),
					std::bind1st(std::equal_to<char>(), '\n'),
                    ' ');
}
static inline void removeNewLines(std::string& str)
{
    std::remove_if(str.begin(),
                    str.end(),
					std::bind1st(std::equal_to<char>(), '\n'));

    std::remove_if(str.begin(),
                   str.end(),
				   std::bind1st(std::equal_to<char>(), '\r'));
}
static inline int toInt(std::string& s)
{
    return atoi(s.c_str());
}
static inline float toFloat(std::string& s)
{
    return static_cast<float>(atof(s.c_str()));
}
static inline float toFloat(char* s)
{
    return static_cast<float>(atof(s));
}

static const vpl::Color toColor(std::string& str)
{
    vpl::Color color;

    // String is #rrggbb or #rgb
    if(str[0] == '#')
    {
        // Convert color channels into floats
        std::stringstream ss;
        int channel;
        bool expand = (str.size() == 4);

        if(expand)
            ss << std::hex << str.substr(1,1) << str.substr(1,1);
        else
            ss << std::hex << str.substr(1,2);

        ss >> channel;
        ss.clear();

        color.setRed(channel);

        if(expand)
            ss << std::hex << str.substr(2,1) << str.substr(2,1);
        else
            ss << std::hex << str.substr(3,2);

        ss >> channel;
        ss.clear();

        color.setGreen(channel);

        if(expand)
            ss << std::hex << str.substr(3,1) << str.substr(3,1);
        else
            ss << std::hex << str.substr(5,2);

        ss >> channel;

        color.setBlue(channel);

        return color;
    }

    if(str[0] == 'r' &&
       str[1] == 'g' &&
       str[2] == 'b')
    {
        // Try syntax,rgb(R%, G%, B%)
        std::vector<std::string> tokens;
        std::vector<char> delims;

        if(tokenize(str,tokens,delims,"%"))
        {
            tokenize(str,tokens,delims,"rgb(),");

            if(delims.size())
            {
                color.setRed(round(toFloat(tokens[0])/100.0f*255.0f));
                color.setGreen(round(toFloat(tokens[1])/100.0f*255.0f));
                color.setBlue(round(toFloat(tokens[2])/100.0f*255.0f));
            }

            return color;
        }

        // And next,rgb(rrr, ggg, bbb)
        tokens.clear();
        delims.clear();

        if(tokenize(str,tokens,delims,"rgb(),"))
        {
            color.setRed(toInt(tokens[0]));
            color.setGreen(toInt(tokens[1]));
            color.setBlue(toInt(tokens[2]));

            return color;
        }
    }

	if(str.compare(std::string("cornsilk")) <= 0)
	{
		if(str.compare(std::string("aliceblue")) == 0)
			return vpl::cAliceBlue;
		else if(str.compare(std::string("antiquewhite")) == 0)
			return vpl::cAntiqueWhite;
		else if(str.compare(std::string("aqua")) == 0)
			return vpl::cAqua;
		else if(str.compare(std::string("aquamarine")) == 0)
			return vpl::cAquaMarine;
		else if(str.compare(std::string("azure")) == 0)
			return vpl::cAzure;
		else if(str.compare(std::string("beige")) == 0)
			return vpl::cBeige;
		else if(str.compare(std::string("bisque")) == 0)
			return vpl::cBisque;
		else if(str.compare(std::string("black")) == 0)
			return vpl::cBlack;
		else if(str.compare(std::string("blanchedalmond")) == 0)
			return vpl::cBlanchedAlmond;
		else if(str.compare(std::string("blue")) == 0)
			return vpl::cBlue;
		else if(str.compare(std::string("blueviolet")) == 0)
			return vpl::cBlueViolet;
		else if(str.compare(std::string("brown")) == 0)
			return vpl::cBrown;
		else if(str.compare(std::string("burlywood")) == 0)
			return vpl::cBurlyWood;
		else if(str.compare(std::string("cadetblue")) == 0)
			return vpl::cCadetBlue;
		else if(str.compare(std::string("chartreuse")) == 0)
			return vpl::cChartreuse;
		else if(str.compare(std::string("chocolate")) == 0)
			return vpl::cChocolate;
		else if(str.compare(std::string("coral")) == 0)
			return vpl::cCoral;
		else if(str.compare(std::string("cornflowerblue")) == 0)
			return vpl::cCornflowerBlue;
		else if(str.compare(std::string("cornsilk")) == 0)
			return vpl::cCornSilk;
	}

	else if((str.compare(std::string("darkslategrey")) <= 0))
	{
		if(str.compare(std::string("crimson")) == 0)
			return vpl::cCrimson;
		else if(str.compare(std::string("cyan")) == 0)
			return vpl::cCyan;
		else if(str.compare(std::string("darkblue")) == 0)
			return vpl::cDarkBlue;
		else if(str.compare(std::string("darkcyan")) == 0)
			return vpl::cDarkCyan;
		else if(str.compare(std::string("darkgoldenrod")) == 0)
			return vpl::cDarkGoldenrod;
		else if(str.compare(std::string("darkgray")) == 0)
			return vpl::cDarkGrey;
		else if(str.compare(std::string("darkgreen")) == 0)
			return vpl::cDarkGreen;
		else if(str.compare(std::string("darkgrey")) == 0)
			return vpl::cDarkGrey;
		else if(str.compare(std::string("darkkhaki")) == 0)
			return vpl::cDarkKhaki;
		else if(str.compare(std::string("darkmagenta")) == 0)
			return vpl::cDarkMagenta;
		else if(str.compare(std::string("darkolivegreen")) == 0)
			return vpl::cDarkOliveGreen;
		else if(str.compare(std::string("darkorange")) == 0)
			return vpl::cDarkOrange;
		else if(str.compare(std::string("darkorchid")) == 0)
			return vpl::cDarkOrchid;
		else if(str.compare(std::string("darkred")) == 0)
			return vpl::cDarkRed;
		else if(str.compare(std::string("darksalmon")) == 0)
			return vpl::cDarkSalmon;
		else if(str.compare(std::string("darkseagreen")) == 0)
			return vpl::cDarkSeaGreen;
		else if(str.compare(std::string("darkslateblue")) == 0)
			return vpl::cDarkSlateBlue;
		else if(str.compare(std::string("darkslategray")) == 0)
			return vpl::cDarkSlateGrey;
		else if(str.compare(std::string("darkslategrey")) == 0)
			return vpl::cDarkSlateGrey;
	}
	else if(str.compare(std::string("honeydew")) <= 0)
	{
		if(str.compare(std::string("darkturquoise")) == 0)
			return vpl::cDarkTurquoise;
		else if(str.compare(std::string("darkviolet")) == 0)
			return vpl::cDarkViolet;
		else if(str.compare(std::string("deeppink")) == 0)
			return vpl::cDeepPink;
		else if(str.compare(std::string("deepskyblue")) == 0)
			return vpl::cDeepSkyBlue;
		else if(str.compare(std::string("dimgray")) == 0)
			return vpl::cDimGrey;
		else if(str.compare(std::string("dimgrey")) == 0)
			return vpl::cDimGrey;
		else if(str.compare(std::string("dodgerblue")) == 0)
			return vpl::cDodgerBlue;
		else if(str.compare(std::string("firebrick")) == 0)
			return vpl::cFireBrick;
		else if(str.compare(std::string("floralwhite")) == 0)
			return vpl::cFloralWhite;
		else if(str.compare(std::string("forestgreen")) == 0)
			return vpl::cForestGreen;
		else if(str.compare(std::string("fuchsia")) == 0)
			return vpl::cFuchsia;
		else if(str.compare(std::string("gainsboro")) == 0)
			return vpl::cGainsboro;
		else if(str.compare(std::string("ghostwhite")) == 0)
			return vpl::cGhostWhite;
		else if(str.compare(std::string("gold")) == 0)
			return vpl::cGold;
		else if(str.compare(std::string("goldenrod")) == 0)
			return vpl::cGoldenrod;
		else if(str.compare(std::string("gray")) == 0)
			return vpl::cGrey;
		else if(str.compare(std::string("green")) == 0)
			return vpl::cGreen;
		else if(str.compare(std::string("greenyellow")) == 0)
			return vpl::cGreenYellow;
		else if(str.compare(std::string("grey")) == 0)
			return vpl::cGrey;
		else if(str.compare(std::string("honeydew")) == 0)
			return vpl::cHoneyDew;
	}
	else if(str.compare(std::string("lightskyblue")) <= 0)
	{
		if(str.compare(std::string("hotpink")) == 0)
			return vpl::cHotPink;
		else if(str.compare(std::string("indianred")) == 0)
			return vpl::cIndianRed;
		else if(str.compare(std::string("indigo")) == 0)
			return vpl::cIndigo;
		else if(str.compare(std::string("ivory")) == 0)
			return vpl::cIvory;
		else if(str.compare(std::string("khaki")) == 0)
			return vpl::cKhaki;
		else if(str.compare(std::string("lavender")) == 0)
			return vpl::cLavender;
		else if(str.compare(std::string("lavenderblush")) == 0)
			return vpl::cLavenderBlush;
		else if(str.compare(std::string("lawngreen")) == 0)
			return vpl::cLawnGreen;
		else if(str.compare(std::string("lemonchiffon")) == 0)
			return vpl::cLemonChiffon;
		else if(str.compare(std::string("lightblue")) == 0)
			return vpl::cLightBlue;
		else if(str.compare(std::string("lightcoral")) == 0)
			return vpl::cLightCoral;
		else if(str.compare(std::string("lightcyan")) == 0)
			return vpl::cLightCyan;
		else if(str.compare(std::string("lightgoldenrodyellow")) == 0)
			return vpl::cLightGoldenrodYellow;
		else if(str.compare(std::string("lightgray")) == 0)
			return vpl::cLightGrey;
		else if(str.compare(std::string("lightgreen")) == 0)
			return vpl::cLightGreen;
		else if(str.compare(std::string("lightgrey")) == 0)
			return vpl::cLightGrey;
		else if(str.compare(std::string("lightpink")) == 0)
			return vpl::cLightPink;
		else if(str.compare(std::string("lightsalmon")) == 0)
			return vpl::cLightSalmon;
		else if(str.compare(std::string("lightseagreen")) == 0)
			return vpl::cLightSeaGreen;
		else if(str.compare(std::string("lightskyblue")) == 0)
			return vpl::cLightSkyBlue;
	}
	else if(str.compare(std::string("mistyrose")) <= 0)
	{
		if(str.compare(std::string("lightslategrey")) == 0)
			return vpl::cLightSlateGrey;
		else if(str.compare(std::string("lightsteelblue")) == 0)
			return vpl::cLightsteelBlue;
		else if(str.compare(std::string("lightyellow")) == 0)
			return vpl::cLightYellow;
		else if(str.compare(std::string("lime")) == 0)
			return vpl::cLime;
		else if(str.compare(std::string("limegreen")) == 0)
			return vpl::cLimeGreen;
		else if(str.compare(std::string("linen")) == 0)
			return vpl::cLinen;
		else if(str.compare(std::string("magenta")) == 0)
			return vpl::cMagenta;
		else if(str.compare(std::string("maroon")) == 0)
			return vpl::cMaroon;
		else if(str.compare(std::string("mediumaquamarine")) == 0)
			return vpl::cMediumAquaMarine;
		else if(str.compare(std::string("mediumblue")) == 0)
			return vpl::cMediumBlue;
		else if(str.compare(std::string("mediumorchid")) == 0)
			return vpl::cMediumOrchid;
		else if(str.compare(std::string("mediumpurple")) == 0)
			return vpl::cMediumpurple;
		else if(str.compare(std::string("mediumseagreen")) == 0)
			return vpl::cMediumSeaGreen;
		else if(str.compare(std::string("mediumslateblue")) == 0)
			return vpl::cMediumSlateBlue;
		else if(str.compare(std::string("mediumspringgreen")) == 0)
			return vpl::cMediumSpringGreen;
		else if(str.compare(std::string("mediumturquoise")) == 0)
			return vpl::cMediumTurquoise;
		else if(str.compare(std::string("mediumvioletred")) == 0)
			return vpl::cMediumVioletRed;
		else if(str.compare(std::string("midnightblue")) == 0)
			return vpl::cMidnightBlue;
		else if(str.compare(std::string("mintcream")) == 0)
			return vpl::cMintcream;
		else if(str.compare(std::string("mistyrose")) == 0)
			return vpl::cMistyrose;
	}
	else if(str.compare(std::string("purple")) <= 0)
	{
		if(str.compare(std::string("moccasin")) == 0)
			return vpl::cMoccasin;
		else if(str.compare(std::string("navajowhite")) == 0)
			return vpl::cNavajoWhite;
		else if(str.compare(std::string("navy")) == 0)
			return vpl::cNavy;
		else if(str.compare(std::string("oldlace")) == 0)
			return vpl::cOldlace;
		else if(str.compare(std::string("olive")) == 0)
			return vpl::cOlive;
		else if(str.compare(std::string("olivedrab")) == 0)
			return vpl::cOliveDrab;
		else if(str.compare(std::string("orange")) == 0)
			return vpl::cOrange;
		else if(str.compare(std::string("orangered")) == 0)
			return vpl::cOrangeRed;
		else if(str.compare(std::string("orchid")) == 0)
			return vpl::cOrchid;
		else if(str.compare(std::string("palegoldenrod")) == 0)
			return vpl::cPaleGoldenrod;
		else if(str.compare(std::string("palegreen")) == 0)
			return vpl::cPaleGreen;
		else if(str.compare(std::string("paleturquoise")) == 0)
			return vpl::cPaleTurquoise;
		else if(str.compare(std::string("palevioletred")) == 0)
			return vpl::cPaleVioletRed;
		else if(str.compare(std::string("papayawhip")) == 0)
			return vpl::cPapayaWhip;
		else if(str.compare(std::string("peachpuff")) == 0)
			return vpl::cPeachPuff;
		else if(str.compare(std::string("peru")) == 0)
			return vpl::cPeru;
		else if(str.compare(std::string("pink")) == 0)
			return vpl::cPink;
		else if(str.compare(std::string("plum")) == 0)
			return vpl::cPlum;
		else if(str.compare(std::string("powderblue")) == 0)
			return vpl::cPowderBlue;
		else if(str.compare(std::string("purple")) == 0)
			return vpl::cPurple;
	}
    else if(str.compare(std::string("red")) == 0)
        return vpl::cRed;
    else if(str.compare(std::string("rosybrown")) == 0)
        return vpl::cRosyBrown;
    else if(str.compare(std::string("royalblue")) == 0)
        return vpl::cRoyalBlue;
    else if(str.compare(std::string("saddlebrown")) == 0)
        return vpl::cSaddleBrown;
    else if(str.compare(std::string("salmon")) == 0)
        return vpl::cSalmon;
    else if(str.compare(std::string("sandybrown")) == 0)
        return vpl::cSandyBrown;
    else if(str.compare(std::string("seagreen")) == 0)
        return vpl::cSeaGreen;
    else if(str.compare(std::string("seashell")) == 0)
        return vpl::cSeaShell;
    else if(str.compare(std::string("sienna")) == 0)
        return vpl::cSienna;
    else if(str.compare(std::string("silver")) == 0)
        return vpl::cSilver;
    else if(str.compare(std::string("skyblue")) == 0)
        return vpl::cSkyBlue;
    else if(str.compare(std::string("slateblue")) == 0)
        return vpl::cSlateBlue;
    else if(str.compare(std::string("slategrey")) == 0)
        return vpl::cSlateGrey;
    else if(str.compare(std::string("snow")) == 0)
        return vpl::cSnow;
    else if(str.compare(std::string("springgreen")) == 0)
        return vpl::cSpringGreen;
    else if(str.compare(std::string("steelblue")) == 0)
        return vpl::cSteelBlue;
    else if(str.compare(std::string("tan")) == 0)
        return vpl::cTan;
    else if(str.compare(std::string("teal")) == 0)
        return vpl::cTeal;
    else if(str.compare(std::string("thistle")) == 0)
        return vpl::cThistle;
    else if(str.compare(std::string("tomato")) == 0)
        return vpl::cTomato;
    else if(str.compare(std::string("turquoise")) == 0)
        return vpl::cTurquoise;
    else if(str.compare(std::string("violet")) == 0)
        return vpl::cViolet;
    else if(str.compare(std::string("wheat")) == 0)
        return vpl::cWheat;
    else if(str.compare(std::string("white")) == 0)
        return vpl::cWhite;
    else if(str.compare(std::string("whitesmoke")) == 0)
        return vpl::cWhiteSmoke;
    else if(str.compare(std::string("yellow")) == 0)
        return vpl::cYellow;
    else if(str.compare(std::string("yellowgreen")) == 0)
        return vpl::cYellowGreen;

    return vpl::cBlack;
}

// Convert string of floats separated by whitespace into vector of floats
static inline void toFloats(std::string& str,std::vector<float>& floats)
{
    std::vector<std::string> splitString;

    replace(str,"-"," -");

    tokenize(str,splitString," ,");

    // Remove "" and " "
    if(str.compare(std::string("")) == 0 || str.compare(std::string(" ")) == 0)
        return;

    // Handle case of single argument
    if(splitString.size() == 0)
    {
        floats.push_back(toFloat(str));

        return;
    }

    // Handle typical case
    for(vplUint i = 0; i < splitString.size() ; i++)
        floats.push_back(toFloat(splitString[i]));

}
// Convert polyline and polygon strings into an array of floats
static inline void convertPolyStringToFloats(const std::string& str,
                                             std::vector<float>& result)
{
    // Skip delimiters at beginning.
    std::string::size_type lastPos = str.find_first_not_of(" ,", 0);

    // Find first "non-delimiter".
    std::string::size_type pos = str.find_first_of(" ,",lastPos);

    float f;
    while(std::string::npos != pos || std::string::npos != lastPos)
    {
        // Found a token, convert to float add it to the vector
        std::string c = str.substr(lastPos, pos - lastPos);
        f = toFloat(c);
        result.push_back(f);

        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(" ,", pos);

        // Find next "non-delimiter"
        pos = str.find_first_of(" ,", lastPos);
    }
}
// Units
enum Unit
{
    cNone,
    cCm,
    cMm,
    cPt,
    cPc,
    cPx,
    cPercent
};

inline Unit findUnit(const std::string& s)
{
    Unit u = cNone;

    if(s.find("px") != std::string::npos)
        u = cPx;
    else if(s.find('%')  != std::string::npos)
        u = cPercent;
    else if(s.find("cm")  != std::string::npos)
        u = cCm;
    else if(s.find("mm")  != std::string::npos)
        u = cMm;
    else if(s.find("pt")  != std::string::npos)
        u = cPt;
    else if(s.find("pc")  != std::string::npos)
        u = cPc;

    return u;
}

PathTokenizer::PathTokenizer():nextCommand_(0),lastCommand_(0)
{
    numbers_ = cNumeric;
    commands_ = cCommands;
    separators_ = cSeparators;
}

void PathTokenizer::getArguments(vpl::DynamicArray<float>& args)
{
    // Buffer to hold argument
    char buffer[512];
    char* bufferPointer = buffer;

    // Where to start collecting argument
    size_t index = lastCommand_ + 1;

    const char* path = path_.c_str() + index;

    while(*path)
    {
        // If we find a separator, start again
        if(separators_.find(*path) != std::string::npos)
        {
            *bufferPointer = '\0';

            if(*buffer != '\0')
                args.add(static_cast<float>(atof(buffer)));

            bufferPointer = buffer;

            if((*path) == '-' || (*path) == '+')
                *bufferPointer++ = *path;

        }
         // Add numbers to buffer
        else if(numbers_.find(*path) != std::string::npos)
            *bufferPointer++ = *path;

        // Quit when we find new command
        else if(commands_.find(*path) != std::string::npos)
        {
            *bufferPointer = '\0';

            if(*buffer != '\0')
                args.add(static_cast<float>(atof(buffer)));

            return;
        }

        ++path;
        ++index;
    }

    *bufferPointer = '\0';

    if(*buffer != '\0')
        args.add(static_cast<float>(atof(buffer)));
}
SVGParser::SVGState* SVGParser::SVGStateStack::push(SVGParser::SVGState* state)
{
	SVGParser::SVGState* newState = new SVGParser::SVGState();

	if(stack_)
		stack_->next_ = newState;

	newState->prev_ = stack_;
	newState->next_ = 0;
	stack_ = newState;

	if(state)
	{
		newState->affineTransform_ = state->affineTransform_;

		for(vplUint i = 0;i < SVGParser::cNumStylingProperties;++i)
			newState->properties_[i] = state->properties_[i];
	}
	else
	{
		newState->properties_[cStroke]           = std::string("none");
		newState->properties_[cStrokeWidth]      = std::string("1");
		newState->properties_[cStrokeLineJoin]   = std::string("miter");
		newState->properties_[cStrokeMiterLimit] = std::string("4");
		newState->properties_[cStrokeDashArray]  = std::string("none");
		newState->properties_[cStrokeOpacity]    = std::string("1");
		newState->properties_[cStrokeDashOffset] = std::string("0");
		newState->properties_[cStrokeLineCap]    = std::string("butt");
		newState->properties_[cFill]             = std::string("none");
		newState->properties_[cFillRule]         = std::string("nonzero");
		newState->properties_[cFillOpacity]      = std::string("1");
	}

	return stack_;
}
void SVGParser::SVGStateStack::pop()
{
	SVGParser::SVGState* last = stack_;

	stack_ = stack_->prev_;
	stack_->next_ = 0;

	delete last;
}

SVGParser::SVGState* SVGParser::SVGStateStack::getTopOfStack()
{
	return stack_;
}

void SVGParser::SVGStateStack::deleteStack()
{
	SVGParser::SVGState* last = stack_;

    while(last)
    {
        stack_ = stack_->prev_;

		if(stack_)
			stack_->next_ = 0;

        delete last;

		last = stack_;
    }
}

SVGParser::SVGParser():buffer_(0),depth_(0),width_(0),height_(0),
                       currentState_(0){}

SVGParser::~SVGParser()
{
    if(buffer_)
        delete buffer_;

    for(vplUint i = 0; i < svgPaths_.getItemCount();i++)
    {
        delete svgPaths_[i];
    }
}
bool SVGParser::loadDocument(const char* fileName)
{
    int length = 0;
    std::ifstream is;
    is.open(fileName, std::ios::binary );

    if(!is.is_open())
        return false;

    is.seekg (0, std::ios::end);
    length = is.tellg();
    is.seekg (0, std::ios::beg);

    // allocate memory:
    buffer_ = new char[length + 1];

    // read data as a block:
    is.read(buffer_,length);

    // Terminate string
    buffer_[length] = '\0';

    return true;
}

bool SVGParser::parseDocument()
{
    // Setup processors
    processorMap_[std::string("path")]      = parsePath;
    processorMap_[std::string("rect")]      = parseRect;
    processorMap_[std::string("circle")]    = parseCircle;
    processorMap_[std::string("ellipse")]   = parseEllipse;
    processorMap_[std::string("line")]      = parseLine;
    processorMap_[std::string("polyline")]  = parsePolyline;
    processorMap_[std::string("polygon")]   = parsePolygon;
    processorMap_[std::string("transform")] = parseTransform;
    processorMap_[std::string("g")]         = parseGrouping;
    processorMap_[std::string("style")]     = parseStyle;

    // Setup styles
    stylingMap_[std::string("font-family")]           = cFontFamily;
    stylingMap_[std::string("font-size")]             = cFontSize;
    stylingMap_[std::string("font-style")]            = cFontStyle;
    stylingMap_[std::string("font-weight")]           = cFontWeight;
    stylingMap_[std::string("direction")]             = cTextDirection;
    stylingMap_[std::string("unicode-bidi")]          = cTextUnicodeBdi;
    stylingMap_[std::string("color")]                 = cColor;
    stylingMap_[std::string("display")]               = cDisplay;
    stylingMap_[std::string("visibility")]            = cVisibility;
    stylingMap_[std::string("opacity")]               = cOpacity;
    stylingMap_[std::string("stop-color")]            = cStopColor;
    stylingMap_[std::string("stop-opacity")]          = cStopOpacity;
    stylingMap_[std::string("pointer-events")]        = cPointerEvents;
    stylingMap_[std::string("audio-level")]           = cAudioLevel;
    stylingMap_[std::string("buffered-rendering")]    = cBufferedRendering;
    stylingMap_[std::string("color-rendering")]       = cColorRendering;
    stylingMap_[std::string("fill")]                  = cFill;
    stylingMap_[std::string("fill-opacity")]          = cFillOpacity;
    stylingMap_[std::string("fill-rule")]             = cFillRule;
    stylingMap_[std::string("image-rendering")]       = cImageRendering;
    stylingMap_[std::string("shape-rendering")]       = cShapeRendering;
    stylingMap_[std::string("solid-color")]           = cSolidColor;
    stylingMap_[std::string("solid-opacity")]         = cSolidOpacity;
    stylingMap_[std::string("stroke")]                = cStroke;
    stylingMap_[std::string("stroke-dasharray")]      = cStrokeDashArray;
    stylingMap_[std::string("stroke-dashoffset")]     = cStrokeDashOffset;
    stylingMap_[std::string("stroke-linecap")]        = cStrokeLineCap;
    stylingMap_[std::string("stroke-linejoin")]       = cStrokeLineJoin;
    stylingMap_[std::string("stroke-miterlimit")]     = cStrokeMiterLimit;
    stylingMap_[std::string("stroke-opacity")]        = cStrokeOpacity;
    stylingMap_[std::string("stroke-width")]          = cStrokeWidth;
    stylingMap_[std::string("text-rendering")]        = cTextRendering;
    stylingMap_[std::string("vector-effect")]         = cVectorEffect;
    stylingMap_[std::string("viewport-fill")]         = cViewportFill;
    stylingMap_[std::string("viewport-fill-opacity")] = cViewportFillOpacity;
    stylingMap_[std::string("display-align")]         = cDisplayAlign;
    stylingMap_[std::string("line-increment")]        = cLineIncrement;
    stylingMap_[std::string("text-anchor")]           = cTextAnchor;

    if(!buffer_)
    {
        width_ = height_ = 0;
        return false;
    }

    // 0 means default parse flags
    // parse_no_data_nodes prevents RapidXML from using the somewhat surprising
    // behavior of having both values and data nodes, and having data nodes take
    // precedence over values when printing
    doc_.parse<rapidxml::parse_no_data_nodes>(buffer_);

    // Determine size
    rapidxml::xml_node<>* node = doc_.first_node("svg");
    if(!node)
        return false;

    determineSize(node);

    // Iterate over document
    parseElement(node);

    return true;
}

void SVGParser::parseElement(rapidxml::xml_node<>* node)
{
    // Increase depth
    depth_++;

    // Find processor for node
    std::map<std::string,svgProcessor>::iterator it;
    std::string key(node->name());

    it = processorMap_.find(key);

    if(it != processorMap_.end())
        it->second(node,this);

    // Call all siblings recursively
    for(rapidxml::xml_node<>* child = node->first_node();
        child != 0;
        child = child->next_sibling())
        {
            parseElement(child);
        }

    if(strcmp(node->name(),"g") == 0)
        popState();

    // Decrease depth
    depth_--;
}
void SVGParser::parseAttribute(rapidxml::xml_attribute<>* attr)
{
    // Get parent of attribute so it can be removed after processing
    rapidxml::xml_node<>* parent = attr->parent();

    // See if it is a styling property
    std::string attrName(attr->name());
    std::map<std::string,StylingProperty>::iterator it = stylingMap_.find(attrName);

    // If it is, store it for later
    if(it != stylingMap_.end())
        currentState_->properties_[it->second] = attr->value();

    else if(strcmp("style",attr->name()) == 0)
        parseStyle(attr);

    else if(strcmp("transform",attr->name()) == 0)
        parseTransform(attr);

    parent->remove_attribute(attr);

    if(parent->first_attribute())
        parseAttribute(parent->first_attribute());
}
void SVGParser::parseTransform(rapidxml::xml_attribute<>* attr)
{
    std::string transform(attr->value());
    std::vector<std::string> splitTransform;
    std::vector<std::string> args;

    tokenize(transform,splitTransform,"()");

    for(vplUint i = 0;i < splitTransform.size();i+=2)
    {
        // Parse all transform and apply to current transform
        if(strcmp(splitTransform[i].c_str(),"translate") == 0)
        {
            tokenize(splitTransform[i + 1],args,", ");

            currentState_->affineTransform_.translate(toFloat(args[0]),
                                                      toFloat(args[1]));
        }
        else if(strcmp(splitTransform[i].c_str(),"matrix") == 0)
        {
            tokenize(splitTransform[i + 1],args,", ");

            currentState_->affineTransform_.setMatrix(toFloat(args[0]),toFloat(args[1]),
                                                      toFloat(args[4]),toFloat(args[2]),
                                                      toFloat(args[3]),toFloat(args[5]));
        }
        else if(strcmp(splitTransform[i].c_str(),"scale") == 0)
        {
            tokenize(splitTransform[i + 1],args,", ");

            if(splitTransform.size() == 2)
                currentState_->affineTransform_.scale(toFloat(splitTransform[1]));
            else
                currentState_->affineTransform_.scale(toFloat(args[0]),
                                                       toFloat(args[1]));
        }
        else if(strcmp(splitTransform[i].c_str(),"rotate") == 0)
        {
            tokenize(splitTransform[i + 1],args,", ");

            if(args.size() == 1)
                currentState_->affineTransform_.rotate(toFloat(args[0]) * cDegree);
            else
            {
                currentState_->affineTransform_.translate(toFloat(args[1]),
                                                           toFloat(args[2]));

                currentState_->affineTransform_.rotate(toFloat(args[0]) * cDegree);

                currentState_->affineTransform_.translate(-toFloat(args[1]),
                                                           -toFloat(args[2]));
            }
        }
        else if(strcmp(splitTransform[i].c_str(),"skewX") == 0)
        {
            tokenize(splitTransform[i + 1],args,", ");

            currentState_->affineTransform_.shear(toFloat(args[0]) * cDegree,0.0f);
        }
        else if(strcmp(splitTransform[i].c_str(),"skewY") == 0)
        {
            tokenize(splitTransform[i + 1],args,", ");

            currentState_->affineTransform_.shear(0.0f,toFloat(args[0]) * cDegree);
        }
    }
}
void SVGParser::parseStyle(rapidxml::xml_attribute<> *attr)
{
    // First find all parameters
    std::string allParameters(attr->value());
    std::vector<std::string> tokens;

    // Remove whitespaces
    removeWhiteSpaces(allParameters);

    // Tokenize
    tokenize(allParameters,tokens,";:");

    std::map<std::string,StylingProperty>::iterator it;

    for(vplUint i = 0; i < tokens.size() ; i++)
    {
        it = stylingMap_.find(tokens[i++]);

        if(it != stylingMap_.end())
            currentState_->properties_[it->second] = tokens[i];
    }
}
void SVGParser::determineSize(rapidxml::xml_node<>* node)
{
    // Initial size
    unsigned int w = 500;
    unsigned int h = 500;
    Unit unit;

    // Find width and height
    rapidxml::xml_attribute<>* attribute = node->first_attribute("width");
    if(attribute)
    {
        std::string width = attribute->value();
        std::string trimmedWidth = width;

        unit = findUnit(width);

        removeCharacters(trimmedWidth);

        if(unit == cNone || unit == cPx)
        {
            w = atoi(trimmedWidth.c_str());
            viewPort_.right_ = static_cast<float>(w);
        }
    }

    attribute = node->first_attribute("height");
    if(attribute)
    {
        std::string height = attribute->value();
        std::string trimmedHeight = height;

        unit = findUnit(height);

        removeCharacters(trimmedHeight);

        if(unit == cNone || unit == cPx)
        {
            h = atoi(trimmedHeight.c_str());
            viewPort_.bottom_ = static_cast<float>(h);
        }
    }

    // Find viewbox
    attribute = node->first_attribute("viewBox");
    if(attribute)
    {
        std::string viewBox = attribute->value();
        std::vector<std::string> values;

        tokenize(viewBox,values);

        viewPort_.left_ = toFloat(values[0]);
        viewPort_.top_ = toFloat(values[1]);
        viewPort_.right_ = viewPort_.left_ + toFloat(values[2]);
        viewPort_.bottom_ = viewPort_.top_  + toFloat(values[3]);
    }
    else
    {
    }

    // By now unit should be pixels,validate
    if(w != 0 && h != 0)
    {
        width_  = w;
        height_ = h;

        pushState();
    }
}

// SVG processors
void SVGParser::parsePath(rapidxml::xml_node<>* node,SVGParser* self)
{
    // Push state
    self->pushState();

    // Get d attribute of path
    rapidxml::xml_attribute<>* attribute = node->first_attribute("d");
    if(!attribute)
        return;

    //DEBUG() << "Parsing path:" << attribute->value();
    self->tokenizer_.setPath(attribute->value());

    // Create path
    SVGPath* path = new SVGPath();

    // Apply current transform
    path->transform_ = self->currentState_->affineTransform_;

    // Copy path data
    path->pathData_ = new char[attribute->value_size()];
    memcpy(path->pathData_,attribute->value(),attribute->value_size());

    //  Execute all path commands
    while(self->tokenizer_.hasMoreCommands())
    {
        char command = self->tokenizer_.getNextCommand();

        vpl::DynamicArray<float> args;

        self->tokenizer_.getArguments(args);

        switch(command)
        {
            // Move to commands
            case 'M':
            {
                SVGSegment segment(vpl::Path::cMoveToAbs,args[0],args[1]);

                path->segments_.add(segment);
            }
                break;

            case 'm':
            {
                SVGSegment segment(vpl::Path::cMoveToRel,args[0],args[1]);

                path->segments_.add(segment);
            }
                break;

            // Close path commands
            case 'Z':
            case 'z':
            {
                SVGSegment segment(vpl::Path::cClosePath);

                path->segments_.add(segment);
            }
                break;

            // Line to commands
            case 'L':
                for(vplUint j = 0;j < args.getItemCount() / 2;++j)
                {
                    SVGSegment segment(vpl::Path::cLineToAbs,
                                       args[j*2],
                                       args[j*2 + 1]);
                    path->segments_.add(segment);
                }
                break;

            case 'l':
                for(vplUint j = 0;j < args.getItemCount() / 2 ;++j)
                {
                    SVGSegment segment(vpl::Path::cLineToRel,
                                       args[j*2],
                                       args[j*2 + 1]);
                    path->segments_.add(segment);
                }
                break;

            case 'H':
                for(vplUint j = 0;j < args.getItemCount();++j)
                {
                    SVGSegment segment(vpl::Path::cHorizontalLineToAbs,args[j]);

                    path->segments_.add(segment);
                }
                break;
            case 'h':
                for(vplUint j = 0;j < args.getItemCount() ;++j)
                {
                    SVGSegment segment(vpl::Path::cHorizontalLineToRel,args[j]);

                    path->segments_.add(segment);
                }
                break;
            case 'V':
                for(vplUint j = 0;j < args.getItemCount() ;++j)
                {
                    SVGSegment segment(vpl::Path::cVerticalLineToAbs,args[j]);

                    path->segments_.add(segment);
                }
                break;
            case 'v':
                for(vplUint j = 0;j < args.getItemCount() ;++j)
                {
                    SVGSegment segment(vpl::Path::cVerticalLineToRel,args[j]);

                    path->segments_.add(segment);
                }
                break;

            // Bezier commands
            case 'C':
                for(vplUint j = 0;j < args.getItemCount() / 6 ;++j)
                {
                    SVGSegment segment(vpl::Path::cCubicBezierToAbs,
                                       args[j*6+4],args[j*6+5],args[j*6],
                                       args[j*6+1],args[j*6+2],args[j*6+3]);

                    path->segments_.add(segment);
                }
                    break;

            case 'c':
                for(vplUint j = 0;j < args.getItemCount() / 6;++j)
                {
                    SVGSegment segment(vpl::Path::cCubicBezierToRel,
                                       args[j*6+4],args[j*6+5],args[j*6],
                                       args[j*6+1],args[j*6+2],args[j*6+3]);

                    path->segments_.add(segment);
                }
                    break;

            case 'S':
                for(vplUint j = 0;j < args.getItemCount() / 4;++j)
                {
                    SVGSegment segment(vpl::Path::cSmoothCubicBezierToAbs,
                                       args[j*4+2],args[j*4+3],
                                       args[j*4],args[j*4+1]);

                    path->segments_.add(segment);
                }
                break;

            case 's':
                for(vplUint j = 0;j < args.getItemCount() / 4;++j)
                {
                    SVGSegment segment(vpl::Path::cSmoothCubicBezierToRel,
                                       args[j*4+2],args[j*4+3],
                                       args[j*4],args[j*4+1]);

                    path->segments_.add(segment);
                }
                break;

            case 'Q':
                for(vplUint j = 0;j < args.getItemCount() / 4;++j)
                {
                    SVGSegment segment(vpl::Path::cQuadBezierToAbs,
                                       args[j*4+2],args[j*4+3],
                                       args[j*4],args[j*4+1]);

                    path->segments_.add(segment);
                }
                break;

            case 'q':
                for(vplUint j = 0;j < args.getItemCount() / 4;++j)
                {
                     SVGSegment segment(vpl::Path::cQuadBezierToRel,
                                       args[j*4+2],args[j*4+3],
                                       args[j*4],args[j*4+1]);

                    path->segments_.add(segment);
                }
                break;

            case 'T':
                for(vplUint j = 0;j < args.getItemCount() / 2;++j)
                {
                    SVGSegment segment(vpl::Path::cSmoothQuadBezierToAbs,
                                       args[j*2],
                                       args[j*2+1]);
                    path->segments_.add(segment);
                }
                break;

            case 't':
                for(vplUint j = 0;j < args.getItemCount() / 2;++j)
                {
                    SVGSegment segment(vpl::Path::cSmoothQuadBezierToRel,
                                       args[j*2],
                                       args[j*2+1]);
                    path->segments_.add(segment);
                }
                break;
            default:
                break;
        }
    }

    // Handle possible attributes
    if(node->first_attribute())
        self->parseAttribute(node->first_attribute());

    // Stroke and/or fill
    self->finalizePath(path);
}
void SVGParser::parseRect(rapidxml::xml_node<>* node,SVGParser* self)
{
    float x  = 0.0f;
    float y  = 0.0f;
    float w  = 0.0f;
    float h  = 0.0f;
    float rx = 0.0f;
    float ry = 0.0f;

    // Push state
    self->pushState();

    // Parse rect parameters
    rapidxml::xml_attribute<>* attribute = node->first_attribute("x");
    if(attribute)
    {
        x = toFloat(attribute->value());
        node->remove_attribute(attribute);
    }

    attribute = node->first_attribute("y");
    if(attribute)
    {
        y = toFloat(attribute->value());
        node->remove_attribute(attribute);
    }
    attribute = node->first_attribute("width");
    if(attribute)
    {
        w = toFloat(attribute->value());
        node->remove_attribute(attribute);
    }

    attribute = node->first_attribute("height");
    if(attribute)
    {
        h = toFloat(attribute->value());
        node->remove_attribute(attribute);
    }

    attribute = node->first_attribute("rx");
    if(attribute)
    {
        rx = toFloat(attribute->value());
        node->remove_attribute(attribute);
    }

    attribute = node->first_attribute("ry");
    if(attribute)
    {
        ry = toFloat(attribute->value());
        node->remove_attribute(attribute);
    }

    // Parse remaining attributes
    if(node->first_attribute())
        self->parseAttribute(node->first_attribute());

    if(w != 0.0f && h != 0.0f)
    {
        SVGPath* path = new SVGPath();

        // Apply current transform
        path->transform_ = self->currentState_->affineTransform_;

        if(ry != 0.0f || rx != 0.0f)
        {
            SVGSegment moveTo(vpl::Path::cMoveToAbs,x + rx,y);
            SVGSegment lineTo1(vpl::Path::cLineToRel,w - 2 * rx);
            SVGSegment arcTo1(vpl::Path::cSmallCounterclockwiseArcToRel,rx,ry,rx,ry);
            SVGSegment lineTo2(vpl::Path::cLineToRel,0.0f,h - 2 * ry);
            SVGSegment arcTo2(vpl::Path::cSmallCounterclockwiseArcToRel,-rx,ry,rx,ry);
            SVGSegment lineTo3(vpl::Path::cLineToRel,-(w - 2*rx));
            SVGSegment arcTo3(vpl::Path::cSmallCounterclockwiseArcToRel,-rx,-ry,rx,ry);
            SVGSegment lineTo4(vpl::Path::cLineToRel,0.0f,-(h - 2*ry));
            SVGSegment arcTo4(vpl::Path::cSmallCounterclockwiseArcToRel,rx,-ry,rx,ry);
            SVGSegment closePath(vpl::Path::cClosePath);

            path->segments_.add(moveTo);
            path->segments_.add(lineTo1);
            path->segments_.add(arcTo1);
            path->segments_.add(lineTo2);
            path->segments_.add(arcTo2);
            path->segments_.add(lineTo3);
            path->segments_.add(arcTo3);
            path->segments_.add(lineTo4);
            path->segments_.add(arcTo4);
            path->segments_.add(closePath);
        }
        else
        {
            SVGSegment moveTo(vpl::Path::cMoveToAbs,x,y);
            SVGSegment vLineTo1(vpl::Path::cLineToRel,0.0f,h);
            SVGSegment hLineTo(vpl::Path::cLineToRel,w);
            SVGSegment vLineTo2(vpl::Path::cLineToRel,0.0f,-h);
            SVGSegment closePath(vpl::Path::cClosePath);

            path->segments_.add(moveTo);
            path->segments_.add(vLineTo1);
            path->segments_.add(hLineTo);
            path->segments_.add(vLineTo2);
            path->segments_.add(closePath);
        }

        self->finalizePath(path);
    }
}
void SVGParser::parseCircle(rapidxml::xml_node<>* node,SVGParser* self)
{
    float cx = 0.0f;
    float cy = 0.0f;
    float r  = 0.0f;

    // Push state
    self->pushState();

    // Parse circle parameters
    rapidxml::xml_attribute<>* attribute = node->first_attribute("cx");
    if(attribute)
    {
        cx = toFloat(attribute->value());
        node->remove_attribute(attribute);
    }

    attribute = node->first_attribute("cy");
    if(attribute)
    {
        cy = toFloat(attribute->value());
        node->remove_attribute(attribute);
    }

    attribute = node->first_attribute("r");
    if(attribute)
    {
        r = toFloat(attribute->value());
        node->remove_attribute(attribute);
    }

    // Parse remaining attributes
    if(node->first_attribute())
        self->parseAttribute(node->first_attribute());

    if(cx != 0.0f && cy != 0.0f && r != 0.0f)
    {
        SVGPath* path = new SVGPath();

        // Apply current transform
        path->transform_ = self->currentState_->affineTransform_;

        SVGSegment moveTo(vpl::Path::cMoveToAbs,cx - r,cy);
        SVGSegment arcTo1(vpl::Path::cLargeCounterclockwiseArcToAbs,cx + r,cy,r,r);
        SVGSegment arcTo2(vpl::Path::cLargeCounterclockwiseArcToAbs,cx - r,cy,r,r);
        SVGSegment close(vpl::Path::cClosePath);

        path->segments_.add(moveTo);
        path->segments_.add(arcTo1);
        path->segments_.add(arcTo2);
        path->segments_.add(close);

        self->finalizePath(path);
    }
}
void SVGParser::parseEllipse(rapidxml::xml_node<>* node,SVGParser* self)
{
    float cx = 0.0f;
    float cy = 0.0f;
    float rx = 0.0f;
    float ry = 0.0f;

    // Push state
    self->pushState();

    // Parse ellipse parameters
    rapidxml::xml_attribute<>* attribute = node->first_attribute("cx");
    if(attribute)
    {
        cx = toFloat(attribute->value());
        node->remove_attribute(attribute);
    }

    attribute = node->first_attribute("cy");
    if(attribute)
    {
        cy = toFloat(attribute->value());
        node->remove_attribute(attribute);
    }

    attribute = node->first_attribute("rx");
    if(attribute)
    {
        rx = toFloat(attribute->value());
        node->remove_attribute(attribute);
    }

    attribute = node->first_attribute("ry");
    if(attribute)
    {
        ry = toFloat(attribute->value());
        node->remove_attribute(attribute);
    }

    // Parse remaining attributes
    if(node->first_attribute())
        self->parseAttribute(node->first_attribute());

    if(cx != 0.0f && cy != 0.0f && rx != 0.0f && ry != 0.0f)
    {
        SVGPath* path = new SVGPath();

        // Apply current transform
        path->transform_ = self->currentState_->affineTransform_;

        SVGSegment moveTo(vpl::Path::cMoveToAbs,cx - rx,cy);
        SVGSegment arcTo1(vpl::Path::cLargeCounterclockwiseArcToAbs,cx + rx,cy,rx,ry);
        SVGSegment arcTo2(vpl::Path::cLargeCounterclockwiseArcToAbs,cx - rx,cy,rx,ry);
        SVGSegment closePath(vpl::Path::cClosePath);

        path->segments_.add(moveTo);
        path->segments_.add(arcTo1);
        path->segments_.add(arcTo2);
        path->segments_.add(closePath);

        self->finalizePath(path);
    }
}

void SVGParser::parseLine(rapidxml::xml_node<>* node,SVGParser* self)
{
    float x1 = 0.0f;
    float y1 = 0.0f;
    float x2 = 0.0f;
    float y2 = 0.0f;

    // Push state
    self->pushState();

    // Parse line parameters
    rapidxml::xml_attribute<>* attribute = node->first_attribute("x1");
    if(attribute)
    {
        x1 = toFloat(attribute->value());
        node->remove_attribute(attribute);
    }

    attribute = node->first_attribute("y1");
    if(attribute)
    {
        y1 = toFloat(attribute->value());
        node->remove_attribute(attribute);
    }

    attribute = node->first_attribute("x2");
    if(attribute)
    {
        x2 = toFloat(attribute->value());
        node->remove_attribute(attribute);
    }

    attribute = node->first_attribute("y2");
    if(attribute)
    {
        y2 = toFloat(attribute->value());
        node->remove_attribute(attribute);
    }

    // Parse remaining attributes
    if(node->first_attribute())
        self->parseAttribute(node->first_attribute());

    if(x1 != x2 && y1 != y2)
    {
        SVGPath* path = new SVGPath();

        // Apply current transform
        path->transform_ = self->currentState_->affineTransform_;

        SVGSegment moveTo(vpl::Path::cMoveToAbs,x1,y1);
        SVGSegment lineTo(vpl::Path::cLineToAbs,x2,y2);

        path->segments_.add(moveTo);
        path->segments_.add(lineTo);

        self->finalizePath(path);
    }
}
void SVGParser::parsePolyline(rapidxml::xml_node<>* node,SVGParser* self)
{
    std::vector<float> coords;

    // Push state
    self->pushState();

    // Find points
    rapidxml::xml_attribute<>* attribute = node->first_attribute("points");
    if(!attribute)
        return;

    // Convert to floats
    std::string points(attribute->value());
    convertPolyStringToFloats(points,coords);

    // Remove attribute
    node->remove_attribute(attribute);

    // Parse remaining attributes
    if(node->first_attribute())
        self->parseAttribute(node->first_attribute());

    // Draw polyline

    if(coords.size() > 1)
    {
        SVGPath* path = new SVGPath();

        // Apply current transform
        path->transform_ = self->currentState_->affineTransform_;

        SVGSegment moveTo(vpl::Path::cMoveToAbs,coords[0],coords[1]);

        path->segments_.add(moveTo);;

        for(vplUint i = 2; i < coords.size(); i+=2)
        {
            SVGSegment lineTo(vpl::Path::cLineToAbs,coords[i],coords[i + 1]);

            path->segments_.add(lineTo);
        }

        self->finalizePath(path);
    }
}
void SVGParser::parsePolygon(rapidxml::xml_node<>* node,SVGParser* self)
{
    std::vector<float> coords;

    // Push state
    self->pushState();

    // Find points
    rapidxml::xml_attribute<>* attribute = node->first_attribute("points");
    if(!attribute)
        return;

    // Convert to floats
    std::string points(attribute->value());
    convertPolyStringToFloats(points,coords);

    // Remove attribute
    node->remove_attribute(attribute);

    // Parse remaining attributes
    if(node->first_attribute())
        self->parseAttribute(node->first_attribute());

    // Draw polyline
    if(coords.size() > 1)
    {
        SVGPath* path = new SVGPath();

        // Apply current transform
        path->transform_ = self->currentState_->affineTransform_;

        SVGSegment moveTo(vpl::Path::cMoveToAbs,coords[0],coords[1]);

        path->segments_.add(moveTo);

        // Since we always use close path, make sure
        // first and last point isn't the same
        vplUint endIndex = coords.size();

        if((coords[0] == coords[endIndex - 2]) &&
           (coords[1] == coords[endIndex - 1]))
        {
            endIndex -= 2;
        }

        for(vplUint i = 2; i < endIndex; i+=2)
        {
            SVGSegment lineTo(vpl::Path::cLineToAbs,coords[i],coords[i + 1]);

            path->segments_.add(lineTo);
        }

        SVGSegment closePath(vpl::Path::cClosePath);

        path->segments_.add(closePath);

        self->finalizePath(path);
    }
}

void SVGParser::parseGrouping(rapidxml::xml_node<>* node,SVGParser* self)
{
    // Push state
    self->pushState();

    rapidxml::xml_attribute<>* attribute = node->first_attribute();
    if(!attribute)
        return;

    // Parse attributes
    self->parseAttribute(attribute);
}

void SVGParser::parseTransform(rapidxml::xml_node<>*,SVGParser*)
{
}

void SVGParser::parseStyle(rapidxml::xml_node<>*,SVGParser*)
{
}

void SVGParser::finalizePath(SVGPath* path)
{
    bool opaque = false;
    int opacity = 0;

    vpl::Brush brush;
    vpl::Pen pen;

	svgPaths_.add(path);

    for(vplUint i = 0; i < cNumStylingProperties ;i++)
    {
        if(strcmp(currentState_->properties_[i].c_str(),"") != 0)
        {
		    switch(i)
            {
                case cFontFamily:
                    break;
                case cFontSize:
                    break;
                case cFontStyle:
                    break;
                case cFontWeight:
                    break;
                case cTextDirection:
                    break;
                case cTextUnicodeBdi:
                    break;
                case cColor:
                    break;
                case cDisplay:
                    break;
                case cVisibility:
                    break;
                case cOpacity:
                {
                    opacity = round(toFloat(currentState_->properties_[i])*255.0f);
                    opaque  = true;
                }
                case cStopColor:
                    break;
                case cStopOpacity:
                    break;
                case cPointerEvents:
                    break;
                case cAudioLevel:
                    break;
                case cBufferedRendering:
                    break;
                case cColorRendering:
                    break;
                case cFill:
                {
                    if(!strcmp(currentState_->properties_[i].c_str(),"none") == 0)
                    {
                        path->fill_ = true;
                        brush.setColor(toColor(currentState_->properties_[i]));
                    }
                    else if(strcmp(currentState_->properties_[cStroke].c_str(),"none") == 0)
                    {
                        std::string black("black");
                        path->fill_ = true;
                        brush.setColor(toColor(black));
                    }
                }
                break;
                case cFillRule:
                {
                    if(strcmp("evenodd",currentState_->properties_[i].c_str()) == 0)
                        brush.setFillMode(vpl::Brush::cEvenOdd);
                    else if(strcmp("nonzero",currentState_->properties_[i].c_str()) == 0)
                        brush.setFillMode(vpl::Brush::cNonZero);
                }
                break;
                case cFillOpacity:
                {
                    brush.setOpacity(round(toFloat(currentState_->properties_[i])*255.0f));
                }
                break;
                case cImageRendering:
                    break;
                case cShapeRendering:
                    break;
                case cSolidColor:
                    break;
                case cSolidOpacity:
                    break;
                case cStroke:
                {
                    if(!strcmp(currentState_->properties_[i].c_str(),"none") == 0)
                    {
                        path->stroke_ = true;
                        pen.setColor(toColor(currentState_->properties_[i]));
                    }
                }
                break;
                case cStrokeDashArray:
                    break;
                case cStrokeDashOffset:
                    break;
                case cStrokeLineCap:
                {
                    if(strcmp("butt",currentState_->properties_[i].c_str()) == 0)
                        pen.setEndType(vpl::Stroker::cButtEnd);
                    else if(strcmp("round",currentState_->properties_[i].c_str()) == 0)
                        pen.setEndType(vpl::Stroker::cRoundEnd);
                    else if(strcmp("square",currentState_->properties_[i].c_str()) == 0)
                        pen.setEndType(vpl::Stroker::cSquareEnd);
                }
                break;
                case cStrokeLineJoin:
                {
                    if(strcmp("miter",currentState_->properties_[i].c_str()) == 0)
                        pen.setJoinType(vpl::Stroker::cMiterJoin);
                    else if(strcmp("bevel",currentState_->properties_[i].c_str()) == 0)
                        pen.setJoinType(vpl::Stroker::cBevelJoin);
                    else if(strcmp("round",currentState_->properties_[i].c_str()) == 0)
                        pen.setJoinType(vpl::Stroker::cRoundJoin);
                }
                break;
                case cStrokeMiterLimit:
                    pen.setMiterLimit(toFloat(currentState_->properties_[i]));
                    break;
                case cStrokeOpacity:
                    pen.setOpacity(round(toFloat(currentState_->properties_[i])*255.0f));
                break;
                case cStrokeWidth:
                    pen.setSize(toFloat(currentState_->properties_[i]));
                    break;
                case cTextRendering:
                    break;
                case cVectorEffect:
                    break;
                case cViewportFill:
                    break;
                case cViewportFillOpacity:
                    break;
                case cDisplayAlign:
                    break;
                case cLineIncrement:
                    break;
                case cTextAnchor:
                    break;
                default:
                    break;
            }
        }
    }

    if(opaque)
    {
        brush.setOpacity(opacity);
        pen.setOpacity(opacity);
    }

    path->brush_ = brush;
    path->pen_ = pen;

    popState();
}

void SVGParser::popState()
{
    stateStack_.pop();
	currentState_ = stateStack_.getTopOfStack();
}

void SVGParser::pushState()
{
	currentState_ = stateStack_.push(currentState_);
}
