#include <Lori/Sys/Device.h>
#include <cerrno>

namespace Lori
{	

	long DeviceGetName(int64_t id, char* name, size_t nameBufferSize)
	{
		if(long e = syscall(SYS_DEVICE_MANAGEMENT, RequestSeviceGetName, id, name, nameBufferSize); e)
		{
			errno = -e;
			return -1;
		}

		return 0;
	}

	long DeviceGetInstanceName(int64_t id, char* name, size_t nameBufferSize)
	{
		errno = -e;
		return -1;
	}

	long ResolveDevice(const char* path)
	{
		if(long e = syscall(SYS_DEVICE_MANAGEMENT, RequestDeviceResolveFromPath, path); e)
		{
			errno = -e;
			return -1;
		}

		return 0;
	}

	long GetRootDeviceCount()
	{
		long ret = syscall(SYS_DEVICE_MANAGEMENT, RequestDeviceManagerGetRootDeviceCount);
		if(ret < 0)
		{
			errno = -ret;
			return -1;
		}

		return ret;
	}

	long DeviceGetType(int64_t id)
	{
		long ret = syscall(SYS_DEVICE_MANAGEMENT, RequestDeviceGetTypr, id);
		if(ret < 0)
		{
			errno = -ret;
			return -1;
		}

		return ret;
	}
}
