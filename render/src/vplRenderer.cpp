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

#include "vplConfig.h"
#include "vplAffineMatrix.h"
#include "vplEdgeGenerator.h"
#include "vplFill.h"

namespace vpl
{
	Renderer::Renderer():width_(0),height_(0),
		                 maxCoverage_(cFullSubCoverage8),target_(0),
                         subPixelCount_(8),subPixelShift_(3),
                         targetUpdated_(false),transformUpdated_(false)
    {
        edgeGenerator_ = new EdgeGenerator();
        evenOddMaskBuffer_ = new EvenOddMaskBuffer();
        nonZeroMaskBuffer_ = new NonZeroMaskBuffer();
        fillParameters_ = new FillParameters();
    }

	Renderer::~Renderer()
    {
		// Clean up
		clearPaths();

        delete edgeGenerator_;
        delete evenOddMaskBuffer_;
        delete nonZeroMaskBuffer_;
        delete fillParameters_;
    }

	void Renderer::setTarget(PixelBuffer* pixelBuffer)
    {
        target_ = pixelBuffer;

        initialize();
    }

    void Renderer::setSamplingValue(Sampling sampling)
    {
        switch(sampling)
        {
            case cSample32:
                subPixelCount_ = 32;
                subPixelShift_ = 5;
                maxCoverage_   = cFullSubCoverage32;
                break;

            case cSample16:
                subPixelCount_ = 16;
                subPixelShift_ = 4;
                maxCoverage_   = cFullSubCoverage16;
                break;

            case cSample8:
                subPixelCount_ = 8;
                subPixelShift_ = 3;
                maxCoverage_   = cFullSubCoverage8;
                break;
        }

        // Redo initialization
        if(target_)
            initialize();
    }

	void Renderer::initialize()
    {
		// Initalize
        width_ = target_->getWidth();
        height_ = target_->getHeight();

        // Set initial clipping rectangle
        clipRect_.left_ = 0.0f;
        clipRect_.right_ = static_cast<float>(width_);
        clipRect_.top_ = static_cast<float>(height_);
        clipRect_.bottom_ = 0.0f;

        edgeGenerator_->setClipRect(clipRect_);
        edgeGenerator_->initialize(subPixelCount_,
                                   static_cast<float>(width_),
                                   static_cast<float>(height_));

    }

    void Renderer::setClipRect(const Rect& rect)
    {
        clipRect_ = rect;
        edgeGenerator_->setClipRect(clipRect_);

		transformUpdated_ = true;
    }
	
	void Renderer::setTransform(const AffineMatrix& transform)
	{
		transform_ = transform;

		transformUpdated_ = true;
	}
	
	void Renderer::transform(const AffineMatrix& transform)
	{
		transform_ *= transform;

		transformUpdated_ = true;
	}
	
	PathReference* Renderer::addPath(const Path* path,const Pen* pen)
	{
		return allocateReference(path,pen,0);
	}
    
	PathReference* Renderer::addPath(const Path* path,const Brush* brush)
	{
		return allocateReference(path,0,brush);
	}
    
	PathReference* Renderer::addPath(const Path* path,const Pen* pen,const Brush* brush)
	{
		return allocateReference(path,pen,brush);
	}
	
	PathReference* Renderer::allocateReference(const Path* path,const Pen* pen,const Brush* brush)
	{
		PathReference* ref = new PathReference();

		// Allocate and copy data if needed
		if(pen)
		{
			ref->strokeEdges_ = new ScanLineList();
			ref->pen_ = *pen;
		}
		if(brush)
		{
			ref->fillEdges_ = new ScanLineList();
			ref->brush_ = *brush;
		}

        ref->generator_ = new PointGenerator();
       
		// Store path
		ref->path_ = path;

		// Insert for future use
        newReferences_.insert(ref);

		return ref;
	}
	
	// Deallocate data used for edge generation
	void Renderer::deAllocateReference(PathReference* reference)
	{
		if(reference->fillEdges_)
			delete reference->fillEdges_;
		
        if(reference->strokeEdges_)
			delete reference->strokeEdges_;

        if(reference->generator_)
            delete reference->generator_;
	}
	
	// Use reference to remove path
    void Renderer::removePath(PathReference** reference)
	{
		deAllocateReference(*reference);

		references_.remove(*reference);
        newReferences_.remove(*reference);

		delete *reference;

		*reference = 0;

		targetUpdated_ = true;
	}

	void Renderer::clearPaths()
	{
		// Iterate and clean up all allocated data
		ListIterator<PathReference*> it;

		it.begin(references_);

		while(!it.atLast())
		{
			deAllocateReference(*it);

			delete *it;

			++it;
		}

        // Paranoia?
        it.begin(newReferences_);

		while(!it.atLast())
		{
			deAllocateReference(*it);

			delete *it;

			++it;
		}

		references_.clear();
        newReferences_.clear();

        targetUpdated_ = true;
	}
    
	void Renderer::compose(const PixelBuffer& src,BlendMode blendMode,
                           vplUint offsetX,vplUint offsetY)
	{
        Rect destRect(0.0f,static_cast<float>(width_),
                      static_cast<float>(height_),0.0f);

        Rect srcRect(0.0f + offsetX,
                     static_cast<float>(src.getWidth() + offsetX),
                     static_cast<float>(src.getHeight() + offsetY),
                     static_cast<float>(offsetY));

        Rect finalRect = clipRect_;

        // Do inital clipping to clipping rect
        if(destRect.left_ > finalRect.left_)
            finalRect.left_ = destRect.left_;

        if(destRect.right_ < finalRect.right_)
            finalRect.right_ = destRect.right_;

        if(destRect.bottom_ > finalRect.bottom_)
            finalRect.bottom_ = destRect.bottom_;

        if(destRect.top_ < finalRect.top_)
            finalRect.top_ = destRect.top_;

        // Clip to source
        if(srcRect.left_ > finalRect.left_)
            finalRect.left_ = srcRect.left_;

        if(srcRect.right_ < finalRect.right_)
            finalRect.right_ = srcRect.right_;

        if(srcRect.bottom_ > finalRect.bottom_)
            finalRect.bottom_ = srcRect.bottom_;

        if(srcRect.top_ < finalRect.top_)
            finalRect.top_ = srcRect.top_;

        composeBuffers(blendMode,target_,src,destRect);
	}

	void Renderer::generateEdges(PathReference* reference)
	{
        // Apply renderer transform and subpixeltransform
        AffineMatrix matrix = transform_;

        // The sampling point for the sub-pixel is at the top right corner. This
        // adjustment moves it to the pixel center
        matrix.scale(1.0f,static_cast<float>(subPixelCount_));
        matrix.translate(0.5f/subPixelCount_,0.5f);
        
        // Save for stroker
        reference->externalTransform_ = matrix;
        
        // Generate points
        reference->generator_->generatePoints(*reference->path_,matrix);

        // Generate edges
        edgeGenerator_->generateEdges(reference);

        // Update path
        reference->path_->segmentsValid_ = true;
	}

    // See if we need to regenerate paths
	void Renderer::updatePathEdges()
	{
        // Iterate over all path references
		ListIterator<PathReference*> it;

		it.begin(references_);

		while(!it.atLast())
		{
            if(!(*it)->path_->segmentsValid_ || transformUpdated_)
			    targetUpdated_ = true;

			++it;
		}

		transformUpdated_ = false;
	}
	// Choose correct fill function
    void Renderer::fill(BlendMode blendMode,Brush::FillMode fillMode,vplUint32 color,
				        Gradient* gradient,ScanLineList* scanLines)
    {
        if(fillMode == Brush::cEvenOdd)
		{
            if(gradient)
			{
                gradientFillEvenOdd(blendMode,target_,scanLines,gradient,
					                *fillParameters_,evenOddMaskBuffer_);
			}
			else
			{
                fillEvenOdd(blendMode,target_,scanLines,color,
					        *fillParameters_,evenOddMaskBuffer_);
			}
		}
		else
		{
			if(gradient)
			{
                gradientFillNonZero(blendMode,target_,scanLines,gradient,
					                *fillParameters_,nonZeroMaskBuffer_);
			}
			else
			{
				fillNonZero(blendMode,target_,scanLines,color,
					        *fillParameters_,nonZeroMaskBuffer_);
			}
		}
    }

    void Renderer::drawNewEdges()
    {
        ListIterator<PathReference*> it;

		it.begin(newReferences_);

		while(!it.atLast())
        {
             generateEdges(*it);

            // Fill
			if((*it)->fillEdges_)
			{
                fill((*it)->brush_.getBlendMode(),
					 (*it)->brush_.getFillMode(),
					 preMultiplyColorRGBA((*it)->brush_.getColorAsRGBA()),
					 (*it)->brush_.getGradient(),
					 (*it)->fillEdges_);
			}

			// Stroke
			if((*it)->strokeEdges_)
			{
				fill((*it)->pen_.getBlendMode(),
					 Brush::cNonZero,
					 preMultiplyColorRGBA((*it)->pen_.getColorAsRGBA()),
					 (*it)->pen_.getGradient(),
					 (*it)->strokeEdges_);
			}

            // Add to references
            references_.insert((*it));

            // Move to next
			++it;
        }

        newReferences_.clear();
    }

    void Renderer::draw()
	{
        // Set fill parameter
		fillParameters_->height_ = height_;
		fillParameters_->width_ = width_;
		fillParameters_->maxCoverage_ = maxCoverage_;
		fillParameters_->subPixelShift_ = subPixelShift_;
		fillParameters_->subPixelCount_ = subPixelCount_;

		// Do we need to update edges?
		updatePathEdges();

		// Do we need to update target?
		if(targetUpdated_)
		{
            // Clear and draw all "old" paths
			target_->clear(cWhite);

			ListIterator<PathReference*> it;

			it.begin(references_);

			while(!it.atLast())
			{
                generateEdges(*it);

				// Fill
				if((*it)->fillEdges_)
				{
					 fill((*it)->brush_.getBlendMode(),
					      (*it)->brush_.getFillMode(),
					      preMultiplyColorRGBA((*it)->brush_.getColorAsRGBA()),
					      (*it)->brush_.getGradient(),
					      (*it)->fillEdges_);
				}

				// Stroke
				if((*it)->strokeEdges_)
				{
					fill((*it)->pen_.getBlendMode(),
					     Brush::cNonZero,
					     preMultiplyColorRGBA((*it)->pen_.getColorAsRGBA()),
					     (*it)->pen_.getGradient(),
					     (*it)->strokeEdges_);
				}
				// Move to next
				++it;
            }
        }
        // Draw the new edges
        drawNewEdges();
	}

}
