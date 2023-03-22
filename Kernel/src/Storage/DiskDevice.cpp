#include <Device.h>
#include <Errno.h>
#include <FileSystem/Fat32.h>
#include <Filesystem/VolumeManager.h>
#include <Log.h>

static int nextDeviceNumber = 0;

DiskDevice::DiskDevice() : Device(DeviceTypeStorageDevice)
{
    flags = FS_NODE_CHARDEVICE;

    char buf[16];
    strcpy(buf, "hd");
    itoa(nextDeviceNumber++, buf + 2, 10);

    SetInstanceName(buf);
}

int DiskDevice::InitializePartitions() { return 0; }
int DiskDevice::ReadDiskBlock(uint64_t lba, uint32_t count, void* buffer) { return -1; }
int DiskDevice::WriteDiskBlock(uint64_t lba, uint32_t count, void* buffer) { return -1; }

ssize_t DiskDevice::Read(size_t off, size_t size, uint8_t* buffer)
{
    if (off & (blocksize - 1))
        return -EINVAL;

    int e = ReadDiskBlock(off / blocksize, size, buffer);

    if (e)
        return -EIO;

    return size;
}

ssize_t DiskDevice::Write(size_t off, size_t size, uint8_t* buffer) { return -ENOSYS; }
DiskDevice::~DiskDevice() {}
