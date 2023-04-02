#include <Lori/Info.h>
#include <Lori/syscall.h>

namespace Lori
{
    lori_sysinfo_t SysInfo()
    {
        lori_sysinfo_t info;
        syscall(SYS_INFO, &info);
        return info;
    }
}
