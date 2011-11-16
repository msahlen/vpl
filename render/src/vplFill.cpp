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

#include "vplFill.h"

namespace vpl
{
    void composeBuffers(BlendMode blendMode,PixelBuffer* target,
                        const PixelBuffer& src,Rect rect)
    {
        switch(blendMode)
        {
            case cSrcOverDest:
            {
                Composer<SrcOverDestBlender> composer;
                composer.compose(target,src,rect);
            }
            break;
            case cSrc:
            {
                Composer<SrcBlender> composer;
                composer.compose(target,src,rect);
            }
            break;
            case cDest:
            {
                Composer<DestBlender> composer;
                composer.compose(target,src,rect);
            }
            break;
            case cDestOverSrc:
            {
                Composer<DestOverSrcBlender> composer;
                composer.compose(target,src,rect);
            }
            break;
            case cSrcInDest:
            {
                Composer<SrcInDestBlender> composer;
                composer.compose(target,src,rect);
            }
            break;
            case cClear:
            {
                Composer<ClearBlender> composer;
                composer.compose(target,src,rect);
            }
            break;
            case cDestInSrc:
            {
                Composer<DestInSrcBlender> composer;
                composer.compose(target,src,rect);
            }
            break;
            case cSrcOutDest:
            {
                Composer<SrcOutDestBlender> composer;
                composer.compose(target,src,rect);
            }
            break;
            case cDestOutSrc:
            {
                Composer<DestOutSrcBlender> composer;
                composer.compose(target,src,rect);
            }
            break;
            case cSrcAtopDest:
            {
                Composer<SrcAtopDestBlender> composer;
                composer.compose(target,src,rect);
            }
            break;
            case cDestAtopSrc:
            {
                Composer<DestAtopSrcBlender> composer;
                composer.compose(target,src,rect);
            }
            break;
            case cSrcXorDest:
            {
                Composer<SrcXorDestBlender> composer;
                composer.compose(target,src,rect);
            }
            break;
            default:
            {
                Composer<SrcOverDestBlender> composer;
                composer.compose(target,src,rect);

                break;
            }
        }
    }
    void fillEvenOdd(BlendMode blendMode,PixelBuffer* target,ScanLineList* edges,
                     vplUint32 color,FillParameters& params,
                     EvenOddMaskBuffer* evenOddMaskBuffer)
    {
        switch(blendMode)
        {
            case cSrcOverDest:
            {
                SrcOverDestBlender blender;
                EvenOddFiller<SrcOverDestBlender> filler;

                blender.setColor(color);

                filler.fill(target,edges,blender,params,evenOddMaskBuffer);
            }
            break;
            case cSrc:
            {
                SrcBlender blender;
                EvenOddFiller<SrcBlender> filler;

                blender.setColor(color);

                filler.fill(target,edges,blender,params,evenOddMaskBuffer);
            }
            break;
            case cDest:
            {
                DestBlender blender;
                EvenOddFiller<DestBlender> filler;

                blender.setColor(color);

                filler.fill(target,edges,blender,params,evenOddMaskBuffer);
            }
            break;
            case cDestOverSrc:
            {
                DestOverSrcBlender blender;
                EvenOddFiller<DestOverSrcBlender> filler;

                blender.setColor(color);

                filler.fill(target,edges,blender,params,evenOddMaskBuffer);
            }
            break;
            case cSrcInDest:
            {
                SrcInDestBlender blender;
                EvenOddFiller<SrcInDestBlender> filler;

                blender.setColor(color);

                filler.fill(target,edges,blender,params,evenOddMaskBuffer);
            }
            break;
            case cDestInSrc:
            {
                DestInSrcBlender blender;
                EvenOddFiller<DestInSrcBlender> filler;

                blender.setColor(color);

                filler.fill(target,edges,blender,params,evenOddMaskBuffer);
            }
            break;
            case cClear:
            {
                ClearBlender blender;
                EvenOddFiller<ClearBlender> filler;

                blender.setColor(color);

                filler.fill(target,edges,blender,params,evenOddMaskBuffer);
            }
            break;
            case cSrcOutDest:
            {
                SrcOutDestBlender blender;
                EvenOddFiller<SrcOutDestBlender> filler;

                blender.setColor(color);

                filler.fill(target,edges,blender,params,evenOddMaskBuffer);
            }
            break;
            case cDestOutSrc:
            {
                DestOutSrcBlender blender;
                EvenOddFiller<DestOutSrcBlender> filler;

                blender.setColor(color);

                filler.fill(target,edges,blender,params,evenOddMaskBuffer);
            }
            break;
            case cSrcAtopDest:
            {
                SrcAtopDestBlender blender;
                EvenOddFiller<SrcAtopDestBlender> filler;

                blender.setColor(color);

                filler.fill(target,edges,blender,params,evenOddMaskBuffer);
            }
            break;
            case cDestAtopSrc:
            {
                DestAtopSrcBlender blender;
                EvenOddFiller<DestAtopSrcBlender> filler;

                blender.setColor(color);

                filler.fill(target,edges,blender,params,evenOddMaskBuffer);
            }
            break;
            case cSrcXorDest:
            {
                SrcXorDestBlender blender;
                EvenOddFiller<SrcXorDestBlender> filler;

                blender.setColor(color);

                filler.fill(target,edges,blender,params,evenOddMaskBuffer);
            }
            break;
            default:
            {
                SrcOverDestBlender blender;
                EvenOddFiller<SrcOverDestBlender> filler;

                blender.setColor(color);

                filler.fill(target,edges,blender,params,evenOddMaskBuffer);
            }
            break;
        }
    }

    void gradientFillEvenOdd(BlendMode blendMode,PixelBuffer* target,
							 ScanLineList* edges,Gradient* gradient,
							 FillParameters& params,
                             EvenOddMaskBuffer* evenOddMaskBuffer)
    {
        switch(blendMode)
        {
            case cSrcOverDest:
            {
                SrcOverDestGradientBlender blender;
                EvenOddFiller<SrcOverDestGradientBlender> filler;

                blender.setGradient(gradient);

                filler.fill(target,edges,blender,params,evenOddMaskBuffer);
            }
            break;
            case cSrc:
            {
                SrcGradientBlender blender;
                EvenOddFiller<SrcGradientBlender> filler;

                blender.setGradient(gradient);

                filler.fill(target,edges,blender,params,evenOddMaskBuffer);
            }
            break;
            case cDest:
            {
                DestGradientBlender blender;
                EvenOddFiller<DestGradientBlender> filler;

                blender.setGradient(gradient);

                filler.fill(target,edges,blender,params,evenOddMaskBuffer);
            }
            break;
            case cDestOverSrc:
            {
                DestOverSrcGradientBlender blender;
                EvenOddFiller<DestOverSrcGradientBlender> filler;

                blender.setGradient(gradient);

                filler.fill(target,edges,blender,params,evenOddMaskBuffer);
            }
            break;
            case cSrcInDest:
            {
                SrcInDestGradientBlender blender;
                EvenOddFiller<SrcInDestGradientBlender> filler;

                blender.setGradient(gradient);

                filler.fill(target,edges,blender,params,evenOddMaskBuffer);
            }
            break;
            case cDestInSrc:
            {
                DestInSrcGradientBlender blender;
                EvenOddFiller<DestInSrcGradientBlender> filler;

                blender.setGradient(gradient);

                filler.fill(target,edges,blender,params,evenOddMaskBuffer);
            }
            break;
            case cClear:
            {
                ClearBlender blender;
                EvenOddFiller<ClearBlender> filler;

                blender.setColor(0);

                filler.fill(target,edges,blender,params,evenOddMaskBuffer);
            }
            break;
            case cSrcOutDest:
            {
                SrcOutDestGradientBlender blender;
                EvenOddFiller<SrcOutDestGradientBlender> filler;

                blender.setGradient(gradient);

                filler.fill(target,edges,blender,params,evenOddMaskBuffer);
            }
            break;
            case cDestOutSrc:
            {
                DestOutSrcGradientBlender blender;
                EvenOddFiller<DestOutSrcGradientBlender> filler;

                blender.setGradient(gradient);

                filler.fill(target,edges,blender,params,evenOddMaskBuffer);
            }
            break;
            case cSrcAtopDest:
            {
                SrcAtopDestGradientBlender blender;
                EvenOddFiller<SrcAtopDestGradientBlender> filler;

                blender.setGradient(gradient);

                filler.fill(target,edges,blender,params,evenOddMaskBuffer);
            }
            break;
            case cDestAtopSrc:
            {
                DestAtopSrcGradientBlender blender;
                EvenOddFiller<DestAtopSrcGradientBlender> filler;

                blender.setGradient(gradient);

                filler.fill(target,edges,blender,params,evenOddMaskBuffer);
            }
            break;
            case cSrcXorDest:
            {
                SrcXorDestGradientBlender blender;
                EvenOddFiller<SrcXorDestGradientBlender> filler;

                blender.setGradient(gradient);

                filler.fill(target,edges,blender,params,evenOddMaskBuffer);
            }
            break;
            default:
            {
                SrcOverDestGradientBlender blender;
                EvenOddFiller<SrcOverDestGradientBlender> filler;

                blender.setGradient(gradient);

                filler.fill(target,edges,blender,params,evenOddMaskBuffer);
            }
            break;
        }
    }

    void fillNonZero(BlendMode blendMode,PixelBuffer* target,ScanLineList* edges,
                     vplUint32 color,FillParameters& params,
                     NonZeroMaskBuffer* nonZeroMaskBuffer)
    {
        switch(blendMode)
        {
            case cSrcOverDest:
            {
                SrcOverDestBlender blender;
                NonZeroFiller<SrcOverDestBlender> filler;

                blender.setColor(color);

                filler.fill(target,edges,blender,params,nonZeroMaskBuffer);

            }
            break;
            case cSrc:
            {
                SrcBlender blender;
                NonZeroFiller<SrcBlender> filler;

                blender.setColor(color);

                filler.fill(target,edges,blender,params,nonZeroMaskBuffer);

            }
            break;
            case cDest:
            {
                DestBlender blender;
                NonZeroFiller<DestBlender> filler;

                blender.setColor(color);

                filler.fill(target,edges,blender,params,nonZeroMaskBuffer);

            }
            break;
            case cDestOverSrc:
            {
                DestOverSrcBlender blender;
                NonZeroFiller<DestOverSrcBlender> filler;

                blender.setColor(color);

                filler.fill(target,edges,blender,params,nonZeroMaskBuffer);

            }
            break;
            case cSrcInDest:
            {
                SrcInDestBlender blender;
                NonZeroFiller<SrcInDestBlender> filler;

                blender.setColor(color);

                filler.fill(target,edges,blender,params,nonZeroMaskBuffer);

            }
            break;
            case cDestInSrc:
            {
                DestInSrcBlender blender;
                NonZeroFiller<DestInSrcBlender> filler;

                blender.setColor(color);

                filler.fill(target,edges,blender,params,nonZeroMaskBuffer);

            }
            break;
            case cClear:
            {
                ClearBlender blender;
                NonZeroFiller<ClearBlender> filler;

                blender.setColor(color);

                filler.fill(target,edges,blender,params,nonZeroMaskBuffer);

            }
            break;
            case cSrcOutDest:
            {
                SrcOutDestBlender blender;
                NonZeroFiller<SrcOutDestBlender> filler;

                blender.setColor(color);

                filler.fill(target,edges,blender,params,nonZeroMaskBuffer);

            }
            break;
            case cDestOutSrc:
            {
                DestOutSrcBlender blender;
                NonZeroFiller<DestOutSrcBlender> filler;

                blender.setColor(color);

                filler.fill(target,edges,blender,params,nonZeroMaskBuffer);

            }
            break;
            case cSrcAtopDest:
            {
                SrcAtopDestBlender blender;
                NonZeroFiller<SrcAtopDestBlender> filler;

                blender.setColor(color);

                filler.fill(target,edges,blender,params,nonZeroMaskBuffer);

            }
            break;
            case cDestAtopSrc:
            {
                DestAtopSrcBlender blender;
                NonZeroFiller<DestAtopSrcBlender> filler;

                blender.setColor(color);

                filler.fill(target,edges,blender,params,nonZeroMaskBuffer);

            }
            break;
            case cSrcXorDest:
            {
                SrcXorDestBlender blender;
                NonZeroFiller<SrcXorDestBlender> filler;

                blender.setColor(color);

                filler.fill(target,edges,blender,params,nonZeroMaskBuffer);

            }
            break;
            default:
            {
                SrcOverDestBlender blender;
                NonZeroFiller<SrcOverDestBlender> filler;

                blender.setColor(color);

                filler.fill(target,edges,blender,params,nonZeroMaskBuffer);

            }
            break;
        }
    }

    void gradientFillNonZero(BlendMode blendMode,PixelBuffer* target,
		                     ScanLineList* edges,Gradient* gradient,
							 FillParameters& params,
                             NonZeroMaskBuffer* nonZeroMaskBuffer)
    {
        switch(blendMode)
        {
            case cSrcOverDest:
            {
                SrcOverDestGradientBlender blender;
                NonZeroFiller<SrcOverDestGradientBlender> filler;

                blender.setGradient(gradient);

                filler.fill(target,edges,blender,params,nonZeroMaskBuffer);
            }
            break;
            case cSrc:
            {
                SrcGradientBlender blender;
                NonZeroFiller<SrcGradientBlender> filler;

                blender.setGradient(gradient);

                filler.fill(target,edges,blender,params,nonZeroMaskBuffer);
            }
            break;
            case cDest:
            {
                DestGradientBlender blender;
                NonZeroFiller<DestGradientBlender> filler;

                blender.setGradient(gradient);

                filler.fill(target,edges,blender,params,nonZeroMaskBuffer);
            }
            break;
            case cDestOverSrc:
            {
                DestOverSrcGradientBlender blender;
                NonZeroFiller<DestOverSrcGradientBlender> filler;

                blender.setGradient(gradient);

                filler.fill(target,edges,blender,params,nonZeroMaskBuffer);
            }
            break;
            case cSrcInDest:
            {
                SrcInDestGradientBlender blender;
                NonZeroFiller<SrcInDestGradientBlender> filler;

                blender.setGradient(gradient);

                filler.fill(target,edges,blender,params,nonZeroMaskBuffer);
            }
            break;
            case cDestInSrc:
            {
                DestInSrcGradientBlender blender;
                NonZeroFiller<DestInSrcGradientBlender> filler;

                blender.setGradient(gradient);

                filler.fill(target,edges,blender,params,nonZeroMaskBuffer);
            }
            break;
            case cClear:
            {
                ClearBlender blender;
                NonZeroFiller<ClearBlender> filler;

                blender.setColor(0);

                filler.fill(target,edges,blender,params,nonZeroMaskBuffer);
            }
            break;
            case cSrcOutDest:
            {
                SrcOutDestGradientBlender blender;
                NonZeroFiller<SrcOutDestGradientBlender> filler;

                blender.setGradient(gradient);

                filler.fill(target,edges,blender,params,nonZeroMaskBuffer);
            }
            break;
            case cDestOutSrc:
            {
                DestOutSrcGradientBlender blender;
                NonZeroFiller<DestOutSrcGradientBlender> filler;

                blender.setGradient(gradient);

                filler.fill(target,edges,blender,params,nonZeroMaskBuffer);;
            }
            break;
            case cSrcAtopDest:
            {
                SrcAtopDestGradientBlender blender;
                NonZeroFiller<SrcAtopDestGradientBlender> filler;

                blender.setGradient(gradient);

                filler.fill(target,edges,blender,params,nonZeroMaskBuffer);
            }
            break;
            case cDestAtopSrc:
            {
                DestAtopSrcGradientBlender blender;
                NonZeroFiller<DestAtopSrcGradientBlender> filler;

                blender.setGradient(gradient);

                filler.fill(target,edges,blender,params,nonZeroMaskBuffer);
            }
            break;
            case cSrcXorDest:
            {
                SrcXorDestGradientBlender blender;
                NonZeroFiller<SrcXorDestGradientBlender> filler;

                blender.setGradient(gradient);

                filler.fill(target,edges,blender,params,nonZeroMaskBuffer);
            }
            break;
            default:
            {
                SrcOverDestGradientBlender blender;
                NonZeroFiller<SrcOverDestGradientBlender> filler;

                blender.setGradient(gradient);

                filler.fill(target,edges,blender,params,nonZeroMaskBuffer);
            }
            break;
        }
    }
}