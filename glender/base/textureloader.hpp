#pragma once

#include "texture.hpp"

namespace mlib{namespace glender
{

//
// load texture from stream using sf::Image
//

bool loadTextureImage(Texture &t, std::istream &in);

}}