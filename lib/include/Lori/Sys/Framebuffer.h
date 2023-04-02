#pragma once

#include <Lori/Sys/ABI/Framebuffer.h>
#include <stdint.h>

#ifndef __lori__
#error "Lori OS Only"
#endif

namespace Lori
{
    long MapFramebuffer(void** ptr, FBInfo& fbInfo);
}
