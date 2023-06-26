#pragma once

#include <stdint.h>
#include <stddef.h>
#include <smmintrin.h>

extern "C" void memcpy_sse2(void* dest, void* src, size_t count);
extern "C" void memcpy_sse2_unaligned(void* dest, void* src, size_t count);
extern "C" void memset32_sse2(void* dest, uint32_t c, uint64_t count);
extern "C" void memset64_sse2(void* dest, uint64_t c, uint64_t count);

inline void memset32_optimized(void* _dest, uint32_t c, size_t count)
{
    uint32_t* dest = reinterpret_cast<uint32_t*>(_dest);
    while (count--) *(dest++) = c;
    return;
}

inline void memset64_optimized(void* _dest, uint64_t c, size_t count)
{
    uint64_t* dest = reinterpret_cast<uint64_t*>(_dest);
    if (((size_t)dest & 0x7))
    {
        while (count--) *(dest++) = c;
        return;
    }

    size_t overflow = (count & 0x7);
    size_t size_aligned = (count - overflow);

    memset64_sse2(dest, c, size_aligned >> 3);

    while (overflow--) *(dest++) = c;
}

// Alpha blending:
// a0 = aa + ab(255 - aa)
// c0 = (ca * aa + cb * ab(255 - aa)) / a0

inline void alphablend_optimized(uint32_t* dest, uint32_t* src, size_t count)
{
    for(; count; count--, dest++, src++) *dest = Lori::Graphics::AlphaBlendInt(*dest, *src);
}

inline void alphafill_optimized(uint32_t* dest, uint32_t colour, size_t count)
{
    for(; count; count--, dest++) *dest = Lori::Graphics::AlphaBlendInt(*dest, colour);
}


extern "C" void memcpy_optimized(void* dest, void* src, size_t count);
