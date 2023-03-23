#pragma once

#include <FileSystem/Filesystem.h>
#include <List.h>

namespace fs
{

class FsVolume;

namespace VolumeManager
{
enum
{
    VolumeErrorNone,
    VolumeErrorMisc,
    VolumeErrorNotDevice,         // File is not a device
    VolumeErrorInvalidFilesystem, // No filesystem for device
    VolumeErrorVolumeNotFound,    // Volume not found
};

extern List<FsVolume*>* volumes;

void Initialize();

FsVolume* FindVolume(const char* name);

int Mount(FsNode* device, const char* name = nullptr);
int Mount(FsNode* device, ::fs::FsDriver* driver, const char* name = nullptr);

int Unmount(const char* name);

void RegisterVolume(FsVolume* vol);
int UnregisterVolume(FsVolume* vol);

void MountSystemVolume();
FsVolume* SystemVolume();

} // namespace VolumeManager
} // namespace fs
