#pragma once
#include <stdint.h>

enum
{
	VideoModeIndexed = 0,
	VideoModeRGB = 1,
	VideoModeText = 2,
};

typedef struct
{
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
  
    uint16_t bpp;
    
    void* address;
	  uintptr_t physicalAddress;

    int type;
} video_mode_t;

typedef struct
{
	uintptr_t totalMemory;
} memory_info_t;

typedef struct
{
	uintptr_t base;
	uintptr_t size;
} boot_module_t;
