#pragma once

#include <cstdint>

namespace mlib::codec::video
{

//
// pixel formats and layouts
//
    
enum class Pixelformat
{
    I420,
    RGBA
};

enum class Pixellayout
{
    Planar, Interleaved, Semiplanar
};

}