#pragma once

#include <cstdint>

namespace mlib::codec::video
{

//
// pixel formats and layouts
//
// <components><bits per pixel><planar (P)|interleaved (I)|semiplanar (S)>
//
    
enum class Pixelformat
{
    YUV12P,
    RGBA32I,
    RGB24I
};

}