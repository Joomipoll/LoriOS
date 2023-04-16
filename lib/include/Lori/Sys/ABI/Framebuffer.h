#pragma once
typedef struct FBInfo
{
    uint32_t width;
    uint32_t height;
    uint16_t bpp;
    uint32_t pitch
} __attribute__((packed)) fb_info_t;
