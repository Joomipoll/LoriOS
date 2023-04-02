#pragma once

#include <Lori/Types.h>
#include <lori/syscall.h>

namespace Lori
{
	struct LoriEndpointInfo
	{
    	uint16_t msgSize;
	};

	inline handle_t CreateService(const char* name) { return syscall(SYS_CREATE_SERVICE, name); }

	inline handle_t CreateInterface(handle_t svc, const char* name, uint16_t msgSize)
	{
    	return syscall(SYS_CREATE_INTERFACE, svc, name, msgSize);
	}
	inline handle_t InterfaceAccept(handle_t interface) { return syscall(SYS_INTERFACE_ACCEPT, interface); }
	inline handle_t InterfaceConnect(const char* path) { return syscall(SYS_INTERFACE_CONNECT, path); }

	__attribute__((always_inline)) inline long EndpointQueue(handle_t endpoint, uint64_t id, uint16_t size, uintptr_t data)
	{
    	return syscall(SYS_ENDPOINT_QUEUE, endpoint, id, size, data);
	}

	template <typename T>
	__attribute__((always_inline)) inline long EndpointQueue(handle_t endpoint, uint64_t id, const T& data)
	{
    	return syscall(SYS_ENDPOINT_QUEUE, endpoint, id, sizeof(T), &data);
	}

	__attribute__((always_inline)) inline long EndpointDequeue(handle_t endpoint, uint64_t* id, uint16_t* size, uint8_t* data)
	{
    	return syscall(SYS_ENDPOINT_DEQUEUE, endpoint, id, size, data);
	}

	__attribute__((always_inline)) inline long EndpointCall(handle_t endpoint, uint64_t id, uintptr_t data, uint64_t rID, uintptr_t rData, uint16_t* size)
	{
    	return syscall(SYS_ENDPOINT_CALL, endpoint, id, data, rID, rData, size);
	}

	__attribute__((always_inline)) inline long EndpointInfo(handle_t endp, LemonEndpointInfo& info)
	{
    	return syscall(SYS_ENDPOINT_INFO, endp, &info);
	}
}
