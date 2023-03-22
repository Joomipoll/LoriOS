#pragma once

#define SMEM_FLAGS_PRIVATE 1

#include <Memory/VMObject.h>
#include <Scheduler.h>
#include <abi-bits/pid_t.h>

class SharedVMObject : public PhysicalVMObject
{
public:
    SharedVMObject(size_t size, int64_t key, pid_t owner, pid_t recipient, bool isPrivate);

    ALWAYS_INLINE int64_t Key() const { return key; }
    ALWAYS_INLINE pid_t Owner() const { return owner; }
    ALWAYS_INLINE pid_t Recipient() const { return recipient; }
    ALWAYS_INLINE bool IsPrivate() const { return isPrivate; }
    ALWAYS_INLINE bool CanMunmap() const override { return true; }
private:
    int64_t key;

    pid_t owner;
    pid_t recipient; 

    bool isPrivate : 1 = false;
};

namespace Memory
{
    FancyRefPtr<SharedVMObject> GetSharedMemory(int64_t key);
    int CanModifySharedMemory(pid_t pid, int64_t key);
    
    int64_t CreateSharedMemory(uint64_t size, uint64_t flags, pid_t owner, pid_t recipient);

    void* MapSharedMemory(int64_t key, Process* proc, uint64_t hint);
    void DestroySharedMemory(int64_t key);
}
