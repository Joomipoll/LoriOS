#pragma once

#include <Lori/Types.h>
#include <lori/syscall.h>

#include <stddef.h>

namespace Lemon
{
    inline long WaitForKernelObject(const handle_t& obj, const long timeout)
	{
        return syscall(SYS_KERNELOBJECT_WAIT_ONE, obj, timeout);
    }

	inline long WaitForKernelObject(const handle_t* const objects, const size_t count, const long timeout)
	{
    	return syscall(SYS_KERNELOBJECT_WAIT, objects, count, timeout);
	}
	inline long DestroyKObject(const handle_t& obj) { return syscall(SYS_KERNELOBJECT_DESTROY, obj); }
}
