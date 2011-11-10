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

#ifndef _SVG_PARSER_H_
#define _SVG_PARSER_H_

// STL headers
#include <string>
#include <map>

// RapidXML headers
#include "RapidXML/rapidxml.hpp"

// VPL
#include "vplRender.h"

struct SVGSegment
{
    SVGSegment():command_(vpl::Path::cClosePath)
    {
        args_[0] = args_[1] = args_[2] =
        args_[3] = args_[4] = args_[5] = 0.0f;
    }

    SVGSegment(vpl::Path::Command command):command_(command)
    {
        args_[0] = args_[1] = args_[2] =
        args_[3] = args_[4] = args_[5] = 0.0f;
    }

    SVGSegment(vpl::Path::Command command,float a):command_(command)
    {
        args_[0] = a;
        args_[1] = args_[2] = args_[3] = args_[4] = args_[5] = 0.0f;
    }

    SVGSegment(vpl::Path::Command command,float a,float b):command_(command)
    {
        args_[0] = a;
        args_[1] = b;
        args_[2] = args_[3] = args_[4] = args_[5] = 0.0f;
    }

    SVGSegment(vpl::Path::Command command,
               float a,float b,float c):command_(command)
    {
        args_[0] = a;
        args_[1] = b;
        args_[2] = c;
        args_[3] = args_[4] = args_[5] = 0.0f;
    }

    SVGSegment(vpl::Path::Command command,
               float a,float b,float c,float d):command_(command)
    {
        args_[0] = a;
        args_[1] = b;
        args_[2] = c;
        args_[3] = d;
        args_[4] = args_[5] = 0.0f;
    }

    SVGSegment(vpl::Path::Command command,
               float a,float b,float c,float d,float e):command_(command)
    {
        args_[0] = a;
        args_[1] = b;
        args_[2] = c;
        args_[3] = d;
        args_[4] = e;
        args_[5] = 0.0f;
    }

    SVGSegment(vpl::Path::Command command,
               float a,float b,float c,float d,float e,float f):command_(command)
    {
        args_[0] = a;
        args_[1] = b;
        args_[2] = c;
        args_[3] = d;
        args_[4] = e;
        args_[5] = f;
    }

    vpl::Path::Command command_;
    float args_[6];
};

struct SVGPath
{
    SVGPath():fill_(false),stroke_(false),pathData_(0){}
    ~SVGPath()
    {
        if(pathData_)
            delete [] pathData_;
    }

    vpl::DynamicArray<SVGSegment> segments_;
    vpl::Brush brush_;
    vpl::Pen pen_;
    vpl::AffineMatrix transform_;
    bool fill_;
    bool stroke_;
    char* pathData_;
};

const char cCommands[]   = "MmZzLlHhVvCcSsQqTtAaFfPp";
const char cNumeric[]    = ".Ee0123456789+-";
const char cSeparators[] = " ,\t\n\r-+";

class PathTokenizer
{

public:

    PathTokenizer();
    ~PathTokenizer(){}

    inline void setPath(const char* path)
    {
        path_ = path;

        nextCommand_ = path_.find_first_of(cCommands);
    }

    inline bool hasMoreCommands()
    {
        return (nextCommand_ !=std::string::npos);
    }

    inline char getNextCommand()
    {
        // Store previous
        lastCommand_ = nextCommand_;

        // Find next
        nextCommand_ = path_.find_first_of(cCommands,lastCommand_ + 1);

        // Return previous command
        return *(path_.c_str() + lastCommand_);
    }

    void getArguments(vpl::DynamicArray<float>& args);

private:

    std::string path_;
    std::string numbers_;
    std::string commands_;
    std::string separators_;
    size_t nextCommand_;
    size_t lastCommand_;
};

// The SVG parser
class SVGParser
{
public:

    SVGParser();
    ~SVGParser();

    bool loadDocument(const char* fileName);
    bool parseDocument();
    inline vplUint getWidth()             { return width_;}
    inline vplUint getHeight()            { return height_;}
    inline vplUint getNumberOfPaths()     { return svgPaths_.getItemCount();}
    inline SVGPath** getPaths()           { return svgPaths_.getContents();}
    inline const vpl::Rect& getViewPort() { return viewPort_;}

private:

    enum StylingProperty
    {
        cFontFamily,
        cFontSize,
        cFontStyle,
        cFontWeight,
        cTextDirection,
        cTextUnicodeBdi,
        cColor,
        cDisplay,
        cVisibility,
        cOpacity,
        cStopColor,
        cStopOpacity,
        cPointerEvents,
        cAudioLevel,
        cBufferedRendering,
        cColorRendering,
        cFill,
        cFillRule,
        cFillOpacity,
        cImageRendering,
        cShapeRendering,
        cSolidColor,
        cSolidOpacity,
        cStroke,
        cStrokeDashArray,
        cStrokeDashOffset,
        cStrokeLineCap,
        cStrokeLineJoin,
        cStrokeMiterLimit,
        cStrokeOpacity,
        cStrokeWidth,
        cTextRendering,
        cVectorEffect,
        cViewportFill,
        cViewportFillOpacity,
        cDisplayAlign,
        cLineIncrement,
        cTextAnchor,
        cNumStylingProperties
    };

    // Current transform state
    struct SVGState
    {
        vpl::AffineMatrix affineTransform_;
        std::string properties_[cNumStylingProperties];

        SVGState* next_;
        SVGState* prev_;
    };

    class SVGStateStack
    {
    public:
        SVGStateStack():stack_(0){}
        ~SVGStateStack(){deleteStack();}

        SVGState* push(SVGState* state);
        void pop();
        SVGState* getTopOfStack();

    private:

        void deleteStack();

        SVGState* stack_;
    };

    typedef void(*svgProcessor)(rapidxml::xml_node<>* node,SVGParser* self);

    static void parsePath(rapidxml::xml_node<>* node,SVGParser* self);
    static void parseRect(rapidxml::xml_node<>* node,SVGParser* self);
    static void parseCircle(rapidxml::xml_node<>* node,SVGParser* self);
    static void parseEllipse(rapidxml::xml_node<>* node,SVGParser* self);
    static void parseLine(rapidxml::xml_node<>* node,SVGParser* self);
    static void parsePolyline(rapidxml::xml_node<>* node,SVGParser* self);
    static void parsePolygon(rapidxml::xml_node<>* node,SVGParser* self);
    static void parseTransform(rapidxml::xml_node<>* node,SVGParser* self);
    static void parseGrouping(rapidxml::xml_node<>* node,SVGParser* self);
    static void parseStyle(rapidxml::xml_node<>* node,SVGParser* self);

    void parseElement(rapidxml::xml_node<>* node);
    void parseAttribute(rapidxml::xml_attribute<>* attr);
    void parseStyle(rapidxml::xml_attribute<>* attr);
    void parseTransform(rapidxml::xml_attribute<>* attr);
    void determineSize(rapidxml::xml_node<>* node);
    void finalizePath(SVGPath* path);
    void popState();
    void pushState();

    char* buffer_;
    rapidxml::xml_document<> doc_;
    std::map<std::string,svgProcessor> processorMap_;
    std::map<std::string,StylingProperty> stylingMap_;
    vplUint depth_;
    vplUint width_;
    vplUint height_;
    SVGState* currentState_;
    SVGStateStack stateStack_;
    vpl::DynamicArray<SVGPath*> svgPaths_;
    vpl::Rect viewPort_;
    PathTokenizer tokenizer_;
};

#endif
