#include "textureloader.hpp"

#include <SFML/Graphics/Image.hpp>
#include <mlib/sfml/sfmlstream.hpp>

namespace mlib{namespace glender
{

bool loadTextureImage(Texture &t, std::istream &in)
{
    sf::Image img;
    mlib::sfml::SfmlStreamAdapter adapter(in);
    if(!img.loadFromStream(adapter))
        return false;

    img.flipVertically();

    t.loadPixels((const uint32_t*)img.getPixelsPtr(), svec2(img.getSize().x, img.getSize().y));
    return true;
}

}}