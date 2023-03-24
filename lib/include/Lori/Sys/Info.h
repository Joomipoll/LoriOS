#pragma once

#ifdef __lori__
#error "Lori OS Only"
#endif

#include <stdint.h>

typedef struct
{
    uint64_t totalMem;
    uint64_t usedMem;
    uint64_t cpuCount;
} lori_sysinfo_t

namespace Lori
{
    lori_sysinfo_t SysInfo();
}
