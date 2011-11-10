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

#ifndef EXAMPLEUTILS_H
#define EXAMPLEUTILS_H

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "vplRender.h"

typedef bool (*KeyPressedCallback)(sf::Key::Code key);

void saveImage(vpl::PixelBuffer& pixelBuffer,const std::string& name);
void showImage(vpl::PixelBuffer& pixelBuffer,const std::string& title,
               const std::string& instructions,KeyPressedCallback callback = 0);
#endif

