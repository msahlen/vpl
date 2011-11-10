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

#ifndef VPL_EDGE_GENERATOR_H_INCLUDED_
#define VPL_EDGE_GENERATOR_H_INCLUDED_

#include <climits>
#include "vplConfig.h"
#include "vplEdge.h"
#include "vplPath.h"
#include "vplRect.h"
#include "vplRender.h"
#include "vplScanLineList.h"

namespace vpl
{
    class VPL_API PathReference
	{
	public:
		PathReference():fillEdges_(0),strokeEdges_(0),generator_(0){}
		~PathReference(){}

	private:

		friend class Renderer;
        friend class EdgeGenerator;

		const Path* path_;
		Pen pen_;
		Brush brush_;
		ScanLineList* fillEdges_;
		ScanLineList* strokeEdges_;
        PointGenerator* generator_;
        AffineMatrix externalTransform_;
	};

	class VPL_API EdgeGenerator
	{
    public:

        EdgeGenerator():width_(0.0f),height_(0.0f),subPixelCount_(32){}
        ~EdgeGenerator(){}

        void initialize(vplUint subPixelCount,float width,float height);

        void setClipRect(const Rect& rect);

        void generateEdges(PathReference* reference);

    private:

         // Clipping of edges
        void clip(const float* points,
                  const vplUint numPointsIn,
                  DynamicArray<float>& out);

        // Calculate internal clip rect
        void updateInternalClipRect();

        Rect  clipRect_;
        Rect  internalClipRect_;
        float width_;
        float height_;
        vplUint  subPixelCount_;
	};
}

#endif
