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
#include "vplColorHelper.h"

void saveImage(vpl::PixelBuffer& pixelBuffer,const std::string& name)
{
    sf::Image image;
    vplUint32* buffer = pixelBuffer.getBuffer();
    
    image.LoadFromPixels(pixelBuffer.getWidth(),pixelBuffer.getHeight(),(const sf::Uint8*)buffer);
                         
    image.SaveToFile(name);                     
                         
}
inline static void loadImage(vpl::PixelBuffer& pixelBuffer,sf::Image& image,
                             sf::Sprite& sprite)
{
     // Transform premul ARGB -> non premul rgba
    vplUint32* buffer = pixelBuffer.getBuffer();

    image.LoadFromPixels(pixelBuffer.getWidth(),pixelBuffer.getHeight(),(const sf::Uint8*)buffer);

    sprite.SetImage(image);
}

void showImage(vpl::PixelBuffer& pixelBuffer,const std::string& title,
               const std::string& instructions,KeyPressedCallback callback)
{
    sf::RenderWindow window(sf::VideoMode(pixelBuffer.getWidth(),
                                          pixelBuffer.getHeight(),32),title);
    sf::Image image;
    sf::Sprite sprite;

    loadImage(pixelBuffer,image,sprite);

    sf::String string(instructions,sf::Font::GetDefaultFont(),30.0f);
    string.SetColor(sf::Color(255,0,0));
    string.SetPosition(1.f,1.f);

    while(window.IsOpened())
    {
        sf::Event event;

        while(window.GetEvent(event))
        {
            // Window closed
            if(event.Type == sf::Event::Closed)
                window.Close();

            if(event.Type == sf::Event::KeyPressed)
            {
                if(callback)
                {
                    if(callback(event.Key.Code))
						loadImage(pixelBuffer,image,sprite);
					
                }     
            }
        }

        window.Clear();
        window.Draw(sprite);
        window.Draw(string);
        window.Display();
    }    
}