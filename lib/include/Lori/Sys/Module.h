#pragma once

#include <lori/syscall.h>

namespace Lori
{
	inline static long LoadKernelModule(const char* path) { return syscall(SYS_LOAD_KERNEL_MODULE, path); }
	inline static long UnloadKernelModule(const char* name) { return syscall(SYS_UNLOAD_KERNEL_MODULE, name); }
}
