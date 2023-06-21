#pragma once

#include <Lori/Graphics/Vector.h>
#include <Lori/Graphics/Rect.h>

#include <stdint.h>

typedef struct Surface
{
    int width = 0;
    int height = 0;
    uint8_t depth = 32;

    uint8_t* buffer = nullptr;

    // Returns buffer size in bytes
    inline unsigned BufferSize() const { return width * height * (depth >> 3); }

    void Blit(const Surface* src);
    void Blit(const Surface* src, const Vector2i& offset);
    void Blit(const Surface* src, const Vector2i& offset, const struct Rect& region);
    
    void AlphaBlit(const Surface* src, const Vector2i& offset, const Rect& region = {0, 0, INT_MAX, INT_MAX});
} surface_t;
