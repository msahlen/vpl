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

#ifndef VPL_COLOR_H_INCLUDED_
#define VPL_COLOR_H_INCLUDED_

#include "vplConfig.h"

namespace vpl
{
	// 8-8-8-8 RGBA format is used internally by renderer
    // API is ARGB
    class VPL_API Color
    {
    public:

        Color():alpha_(255),red_(0),green_(0),blue_(0){}

        Color(vplUchar a,vplUchar r,vplUchar g,vplUchar b):alpha_(a),red_(r),
			                                               green_(g),blue_(b){}

        Color(vplUint32 argbColor):alpha_((argbColor >> 24) & 0xFF),
                                   red_((argbColor >> 16) & 0xFF),
                                   green_((argbColor >> 8) & 0xFF),
                                   blue_(argbColor & 0xFF){}

        ~Color(){}

        inline void setAlpha(vplUchar alpha) {alpha_ = alpha;}
        inline void setRed(vplUchar red)     {red_   = red;}
        inline void setGreen(vplUchar green) {green_ = green;}
        inline void setBlue(vplUchar blue)   {blue_  = blue;}

        inline vplUint32 getColorRGBA() const
        {
            return ((alpha_ << 24)|(blue_ << 16)|(green_ << 8)|red_);
        }
        inline vplUint32 getColorARGB() const
        {
            return ((alpha_ << 24)|(red_ << 16)|(green_ << 8)|blue_);
        }
        inline vplUchar getAlpha() {return alpha_;}
        inline vplUchar getRed()   {return red_;}
        inline vplUchar getGreen() {return green_;}
        inline vplUchar getBlue()  {return blue_;}

    private:

		vplUchar alpha_;
		vplUchar red_;
		vplUchar green_;
		vplUchar blue_;
    };

    // Color constant
    static const Color cAliceBlue = Color(255,240,248,255);
    static const Color cAntiqueWhite = Color(255,250,235,215);
    static const Color cAqua = Color(255,0,255,255);
    static const Color cAquaMarine = Color(255,127,255,212);
    static const Color cAzure = Color(255,240,255,255);
    static const Color cBeige = Color(255,245,245,220);
    static const Color cBisque = Color(255,255,228,196);
    static const Color cBlack = Color(255,0,0,0);
    static const Color cBlanchedAlmond = Color(255,255,235,205);
    static const Color cBlue = Color(255,0,0,255);
    static const Color cBlueViolet = Color(255,138,43,226);
    static const Color cBrown = Color(255,165,42,42);
    static const Color cBurlyWood = Color(255,222,184,135);
    static const Color cCadetBlue = Color(255,95,158,160);
    static const Color cChartreuse = Color(255,127,255,0);
    static const Color cChocolate = Color(255,210,105,30);
    static const Color cCoral = Color(255,255,127,80);
    static const Color cCornflowerBlue = Color(255,100,149,237);
    static const Color cCornSilk = Color(255,255,248,220);
    static const Color cCrimson = Color(255,220,20,60);
    static const Color cCyan = Color(255,0,255,255);
    static const Color cDarkBlue = Color(255,0,0,139);
    static const Color cDarkCyan = Color(255,0,139,139);
    static const Color cDarkGoldenrod = Color(255,184,134,11);
    static const Color cDarkGrey = Color(255,169,169,169);
    static const Color cDarkGreen = Color(255,0,100,0);
    static const Color cDarkKhaki = Color(255,189,183,107);
    static const Color cDarkMagenta = Color(255,139,0,139);
    static const Color cDarkOliveGreen = Color(255,85,107,47);
    static const Color cDarkOrange = Color(255,255,140,0);
    static const Color cDarkOrchid = Color(255,153,50,204);
    static const Color cDarkRed = Color(255,139,0,0);
    static const Color cDarkSalmon = Color(255,233,150,122);
    static const Color cDarkSeaGreen = Color(255,143,188,143);
    static const Color cDarkSlateBlue = Color(255,72,61,139);
    static const Color cDarkSlateGrey = Color(255,47,79,79);
    static const Color cDarkTurquoise = Color(255,0,206,209);
    static const Color cDarkViolet = Color(255,148,0,211);
    static const Color cDeepPink = Color(255,255,20,147);
    static const Color cDeepSkyBlue = Color(255,0,191,255);
    static const Color cDimGrey = Color(255,105,105,105);
    static const Color cDodgerBlue = Color(255,30,144,255);
    static const Color cFireBrick = Color(255,178,34,34);
    static const Color cFloralWhite = Color(255,255,250,240);
    static const Color cForestGreen = Color(255,34,139,34);
    static const Color cFuchsia = Color(255,255,0,255);
    static const Color cGainsboro = Color(255,220,220,220);
    static const Color cGhostWhite = Color(255,248,248,255);
    static const Color cGold = Color(255,255,215,0);
    static const Color cGoldenrod = Color(255,218,165,32);
    static const Color cGreen = Color(255,0,128,0);
    static const Color cGreenYellow = Color(255,173,255,47);
    static const Color cGrey = Color(255,128,128,128);
    static const Color cHoneyDew = Color(255,240,255,240);
    static const Color cHotPink = Color(255,255,105,180);
    static const Color cIndianRed = Color(255,205,92,92);
    static const Color cIndigo = Color(255,75,0,130);
    static const Color cIvory = Color(255,255,255,240);
    static const Color cKhaki = Color(255,240,230,140);
    static const Color cLavender = Color(255,230,230,250);
    static const Color cLavenderBlush = Color(255,255,240,245);
    static const Color cLawnGreen = Color(255,124,252,0);
    static const Color cLemonChiffon = Color(255,255,250,205);
    static const Color cLightBlue = Color(255,173,216,230);
    static const Color cLightCoral = Color(255,240,128,128);
    static const Color cLightCyan = Color(255,224,255,255);
    static const Color cLightGoldenrodYellow = Color(255,250,250,210);
    static const Color cLightGrey = Color(255,211,211,211);
    static const Color cLightGreen = Color(255,144,238,144);
    static const Color cLightPink = Color(255,255,182,193);
    static const Color cLightSalmon = Color(255,255,160,122);
    static const Color cLightSeaGreen = Color(255,32,178,170);
    static const Color cLightSkyBlue = Color(255,135,206,250);
    static const Color cLightSlateGrey = Color(255,119,136,153);
    static const Color cLightsteelBlue = Color(255,176,196,222);
    static const Color cLightYellow = Color(255,255,255,224);
    static const Color cLime = Color(255,0,255,0);
    static const Color cLimeGreen = Color(255,50,205,50);
    static const Color cLinen = Color(255,250,240,230);
    static const Color cMagenta = Color(255,255,0,255);
    static const Color cMaroon = Color(255,128,0,0);
    static const Color cMediumAquaMarine = Color(255,102,205,170);
    static const Color cMediumBlue = Color(255,0,0,205);
    static const Color cMediumOrchid = Color(255,186,85,211);
    static const Color cMediumpurple = Color(255,147,112,219);
    static const Color cMediumSeaGreen = Color(255,60,179,113);
    static const Color cMediumSlateBlue = Color(255,123,104,238);
    static const Color cMediumSpringGreen = Color(255,0,250,154);
    static const Color cMediumTurquoise = Color(255,72,209,204);
    static const Color cMediumVioletRed = Color(255,199,21,133);
    static const Color cMidnightBlue = Color(255,25,25,112);
    static const Color cMintcream = Color(255,245,255,250);
    static const Color cMistyrose = Color(255,255,228,225);
    static const Color cMoccasin = Color(255,255,228,181);
    static const Color cNavajoWhite = Color(255,255,222,173);
    static const Color cNavy = Color(255,0,0,128);
    static const Color cOldlace = Color(255,253,245,230);
    static const Color cOlive = Color(255,128,128,0);
    static const Color cOliveDrab = Color(255,107,142,35);
    static const Color cOrange = Color(255,255,165,0);
    static const Color cOrangeRed = Color(255,255,69,0);
    static const Color cOrchid = Color(255,218,112,214);
    static const Color cPaleGoldenrod = Color(255,238,232,170);
    static const Color cPaleGreen = Color(255,152,251,152);
    static const Color cPaleTurquoise = Color(255,175,238,238);
    static const Color cPaleVioletRed = Color(255,219,112,147);
    static const Color cPapayaWhip = Color(255,255,239,213);
    static const Color cPeachPuff = Color(255,255,218,185);
    static const Color cPeru = Color(255,205,133,63);
    static const Color cPink = Color(255,255,192,203);
    static const Color cPlum = Color(255,221,160,221);
    static const Color cPowderBlue = Color(255,176,224,230);
    static const Color cPurple = Color(255,128,0,128);
    static const Color cRed = Color(255,255,0,0);
    static const Color cRosyBrown = Color(255,188,143,143);
    static const Color cRoyalBlue = Color(255,65,105,225);
    static const Color cSaddleBrown = Color(255,139,69,19);
    static const Color cSalmon = Color(255,250,128,114);
    static const Color cSandyBrown = Color(255,244,164,96);
    static const Color cSeaGreen = Color(255,46,139,87);
    static const Color cSeaShell = Color(255,255,245,238);
    static const Color cSienna = Color(255,160,82,45);
    static const Color cSilver = Color(255,192,192,192);
    static const Color cSkyBlue = Color(255,135,206,235);
    static const Color cSlateBlue = Color(255,106,90,205);
    static const Color cSlateGrey = Color(255,112,128,144);
    static const Color cSnow = Color(255,255,250,250);
    static const Color cSpringGreen = Color(255,0,255,127);
    static const Color cSteelBlue = Color(255,70,130,180);
    static const Color cTan = Color(255,210,180,140);
    static const Color cTeal = Color(255,0,128,128);
    static const Color cThistle = Color(255,216,191,216);
    static const Color cTomato = Color(255,255,99,71);
    static const Color cTurquoise = Color(255,64,224,208);
    static const Color cViolet = Color(255,238,130,238);
    static const Color cWheat = Color(255,245,222,179);
    static const Color cWhite = Color(255,255,255,255);
    static const Color cWhiteSmoke = Color(255,245,245,245);
    static const Color cYellow = Color(255,255,255,0);
    static const Color cYellowGreen = Color(255,154,205,50);

    inline vplUint32 ARGBtoRGBA(vplUint32 argb)
    {
        return
        (argb & 0xFF00FF00) |   // copy A and G channels
        ((argb >> 16) & 0xFF) | // move B channel down
        ((argb & 0xFF) << 16);  // move R channel up
    }

    inline vplUint32 RGBAtoARGB(vplUint32 rgba)
    {
        return
        (rgba & 0xFF00FF00) |   // copy A and G channels
        ((rgba >> 16) & 0xFF) | // move R channel down
        ((rgba & 0xFF) << 16);  // move B channel up
    }
}
#endif
