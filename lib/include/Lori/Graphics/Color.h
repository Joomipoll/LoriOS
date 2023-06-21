#pragma once

#include <stdint.h>
#include <string>

typedef struct RGBAColor
{
    union
    {
        struct
        {
            uint8_t r, g, b, a;
        } __attribute__((packed));
        unsigned int value;
    };

    inline static constexpr RGBAColor FromRGB(unsigned int rgb)
    {
        return {.value = __builtin_bswap32((rgb << 8) | 0xff)}; // Set alpha channel to 255
    }

    inline static constexpr RGBAColor FromARGB(unsigned int argb)
    {
        return {.value = __builtin_bswap32((argb << 8) | ((argb >> 24) & 0xff))}; // Swap alpha and byte order
    }

    inline static constexpr unsigned int ToARGB(const RGBAColour& color)
    {
        return __builtin_bswap32(colour.a | (colour.value << 8)); // Swap alpha and byte order
    }

    inline static constexpr unsigned int ToARGBBigEndian(const RGBAColor& color)
    {
        return color.a | (color.value << 8); // Swap alpha
    }

    inline static constexpr RGBAColor Interpolate(const RGBAColor& l, const RGBAColor& r)
    {
        return RGBAColor{.r = static_cast<uint8_t>((l.r + r.r) / 2),
                         .g = static_cast<uint8_t>((l.g + r.g) / 2),
                         .b = static_cast<uint8_t>((l.b + r.b) / 2),
                         .a = static_cast<uint8_t>((l.a + r.a) / 2)
          };
    }

    static const RGBAColor white;
    static const RGBAColor black;
    static const RGBAColor red;
    static const RGBAColor yellow;
    static const RGBAColor green;
    static const RGBAColor blue;
    static const RGBAColor grey;
    static const RGBAColor cyan;
    static const RGBAColor magenta;
    static const RGBAColor transparent;
} __attribute__((packed)) rgba_color_t;

using Color = RGBAColor;
