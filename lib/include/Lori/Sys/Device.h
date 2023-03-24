#pragma once
#include <lori/syscall.h>
#include <stddef.h>

namespace Lori
{
    enum DeviceType
    {
        DeviceTypeUNIXPseudo,
        DeviceTypeUNIXPseudoTerminal,
        DeviceTypeUnknown,
        DeviceTypeDevFS,
        DeviceTypeTypeKernelLog,
        DeviceTypeGenericPCI,
        DeviceTypeGenericACPI,
        DeviceTypeGenericUSB,
        DeviceTypeStorageDevice,
        DeviceTypePartition,
        DeviceTypeNetworkStack,
        DeviceTypeNetworkAdapter,
        DeviceTypeUSBController,
        DeviceTypeLegacyHID,
        DeviceTypeUSBHID,
    };
    enum DeviceManagementRequests
    {
        RequestsDeviceManagerGetRootDeviceCount,
        RequestsDeviceManagetEnumerateRootDevice,
        RequestsDeviceGetName,
        RequestsDeviceResolveFromPath,
        RequestsDeviceGetDescription,
        RequestsDeviceGetInstanceName,
        RequestsDeviceGetPCIInformation,
        RequestsDeviceGetType,
        RequestsDeviceIOControl,
        RequestsDeviceGetChildCount,
        RequestsDeviceEnumerateChildren,
    };

    long GetRootDeviceCount();
    long EnumerateRootDevices(int64_t offset, int64_t count, int64_t*);
    long ResolveDevice(const cahr* path);
    long GetType(int64_t id);
    long DeviceGetName(int64_t id, char* name, size_t nameBufferSize);
    long DeviceGetInstanceName(int64_t id, char* name, size_t nameBufferSize);
}
