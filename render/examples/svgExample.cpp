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

#include "exampleUtils.h"
#include "SVGParser.h"

vpl::DynamicArray<vpl::Path*> paths;
vpl::Renderer* svgRenderer;
vpl::Path* currentPath = 0;

SVGPath** svgPaths = 0;
vplUint numberOfPaths = 0;

void drawPath(SVGPath* path)
{
    // Add segments
    if(path->segments_.getItemCount())
    {
        vpl::Path* newPath = new vpl::Path();

        newPath->setTransform(path->transform_);

        for(vplUint i = 0 ; i < path->segments_.getItemCount();i++)
        {

            newPath->addSegment(path->segments_[i].command_,
                                path->segments_[i].args_[0],
                                path->segments_[i].args_[1],
                                path->segments_[i].args_[2],
                                path->segments_[i].args_[3],
                                path->segments_[i].args_[4],
                                path->segments_[i].args_[5]);
        }

        if(path->fill_ && path->stroke_)
            svgRenderer->addPath(newPath,&path->pen_,&path->brush_);
        else if(path->fill_)
            svgRenderer->addPath(newPath,&path->brush_);
        else
            svgRenderer->addPath(newPath,&path->pen_);

        paths.add(newPath);

        svgRenderer->draw();
    }
}

void drawAllPaths()
{
    for(vplUint i = 0; i < numberOfPaths; i++)
    {
        if(svgPaths[i]->segments_.getItemCount())
        {
            vpl::Path* newPath = new vpl::Path();

            newPath->setTransform(svgPaths[i]->transform_);

            for(vplUint j = 0 ; j < svgPaths[i]->segments_.getItemCount();j++)
            {

                newPath->addSegment(svgPaths[i]->segments_[j].command_,
                                    svgPaths[i]->segments_[j].args_[0],
                                    svgPaths[i]->segments_[j].args_[1],
                                    svgPaths[i]->segments_[j].args_[2],
                                    svgPaths[i]->segments_[j].args_[3],
                                    svgPaths[i]->segments_[j].args_[4],
                                    svgPaths[i]->segments_[j].args_[5]);
            }

            if(svgPaths[i]->fill_ && svgPaths[i]->stroke_)
                svgRenderer->addPath(newPath,&svgPaths[i]->pen_,&svgPaths[i]->brush_);
            else if(svgPaths[i]->fill_)
                svgRenderer->addPath(newPath,&svgPaths[i]->brush_);
            else
                svgRenderer->addPath(newPath,&svgPaths[i]->pen_);

            paths.add(newPath);
        }
    }

    svgRenderer->draw();
}
bool keyPressedCallback(sf::Key::Code key)
{
    static vplUint pathIndex = 0;

    if(key == sf::Key::Space)
    {
        if(pathIndex < numberOfPaths && svgPaths)
            drawPath(svgPaths[pathIndex++]);

        return true;
    }

    if(key == sf::Key::A)
    {
        drawAllPaths();

        return true;
    }

    if(key == sf::Key::Q)
    {
        STD_DEBUG() << "Shear X";

        vpl::ShearMatrix matrix(0.2f,0.0f);

        svgRenderer->transform(matrix);
		svgRenderer->draw();

        return true;
    }
    if(key == sf::Key::W)
    {
        STD_DEBUG() << "Shear Y";

        vpl::ShearMatrix matrix(0.0f,0.2f);

        svgRenderer->transform(matrix);
		svgRenderer->draw();

        return true;
    }
    else if(key == sf::Key::Z)
    {
        STD_DEBUG() << "Rotate CW";

		vpl::RotationMatrix matrix(0.2f);

		svgRenderer->transform(matrix);
		svgRenderer->draw();

        return true;
    }
    else if(key == sf::Key::X)
    {
        STD_DEBUG() << "Rotate CCW";

		vpl::RotationMatrix matrix(-0.2f);

		svgRenderer->transform(matrix);
		svgRenderer->draw();

        return true;
    }
    else if(key == sf::Key::Add)
    {
        STD_DEBUG() << "Zooming in";

		vpl::ScalingMatrix matrix(1.5f);

		svgRenderer->transform(matrix);
		svgRenderer->draw();

        return true;
    }
    else if(key == sf::Key::Subtract)
    {
        STD_DEBUG() << "Zooming out";

		vpl::ScalingMatrix matrix(0.67f);

		svgRenderer->transform(matrix);
		svgRenderer->draw();

        return true;
    }
    else if(key == sf::Key::Up)
    {
        STD_DEBUG() << "Moving up";

		vpl::TranslationMatrix matrix(0.0f,-20.0f);

		svgRenderer->transform(matrix);
		svgRenderer->draw();

		return true;
    }
    else if(key == sf::Key::Down)
    {
        STD_DEBUG() << "Moving down";

		vpl::TranslationMatrix matrix(0.0f,20.0f);

		svgRenderer->transform(matrix);
		svgRenderer->draw();

		return true;
    }
    else if(key == sf::Key::Left)
    {
        STD_DEBUG() << "Moving left";

		vpl::TranslationMatrix matrix(-20.0f,0.0f);

		svgRenderer->transform(matrix);
		svgRenderer->draw();

        return true;
    }
    else if(key == sf::Key::Right)
    {
        STD_DEBUG() << "Moving right";

		vpl::TranslationMatrix matrix(20.0f,0.0f);

		svgRenderer->transform(matrix);
		svgRenderer->draw();

        return true;
    }

    return false;
}

int main(int,char** argv)
{
    SVGParser parser;
    bool ok = parser.loadDocument(argv[1]);

    vpl::PixelBuffer pixelBuffer;
    vpl::Renderer renderer;

	renderer.setSamplingValue(vpl::Renderer::cSample8);

	svgRenderer = &renderer;

    if(ok)
    {
       ok = parser.parseDocument();

       if(ok)
       {
           pixelBuffer.initialize(parser.getWidth(),parser.getHeight());
           pixelBuffer.clear(vpl::cWhite);

           renderer.setSamplingValue(vpl::Renderer::cSample8);
           renderer.setTarget(&pixelBuffer);

           svgPaths = parser.getPaths();
           numberOfPaths = parser.getNumberOfPaths();

           drawAllPaths();
       }
    }
    else
        return -1;

	renderer.draw();

    showImage(pixelBuffer,argv[1],"",keyPressedCallback);

    // Clean up
    for(vplUint i = 0; i < paths.getItemCount();i++)
        delete paths[i];

    return 0;
}
