#ifndef FB_H

#define FB_H
#include <Lori/Graphics/Surface.h>

#ifdef __lori__
#include <Lori/Sys/Framebuffer.h>

#endif

namespace Lori
{
    Surface* CreateFramebufferSurface();
    void CreateFramebufferSurface(Surface& surface);
}

#endif
