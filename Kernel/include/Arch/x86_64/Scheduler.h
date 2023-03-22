#pragma once

#include <CPU.h>
#include <ELF.h>
#include <FileSystem/Filesystem.h>
#include <Hash.h>
#include <List.h>
#include <Lock.h>
#include <Memory/AddressSpace.h>
#include <Memory.h>
#include <Obj/Handle.h>
#include <Obj/Process.h>
#include <Paging.h>
#include <Thread.h>
#include <Timer.h>
#include <Vector.h>
#include <stdint.h>

#define KERNEL_CS 0x08
#define KERNEL_SS 0x10
#define USER_SS 0x1B
#define USER_CS 0x23

namespace Scheduler
{
    class ProcessStateThreadBlocker;
}

namespace Scheduler
{
    extern lock_t processesLock;
    extern lock_t destroyedProcessesLock;
    extern List<FancyRefPtr<Process>>* destroyedProcesses;

    void RegisterProcess(FancyRefPtr<Process> proc);
    void MarkProcessForDestruction(Process* proc);

    ALWAYS_INLINE static Process* GetCurrentProcess()
    {
        return Thread::Current()->parent;
    }

    template <typename T>
    ALWAYS_INLINE bool CheckUsermodePointer(T* ptr, AddressSpace* addressSpace = GetCurrentProcess()->addressSpace)
    {
        return addressSpace->RangeInRegion(reinterpret_cast<uintptr_t>(ptr), sizeof(T));
    }

    void Yield();
    void Schedule(void* data, RegisterContext* r);
    void DoSwitch(CPU* cpu);

    pid_t GetNextPID();
    FancyRefPtr<Process> FindProcessByPID(pid_t pid);
    pid_t GetNextProcessPID(pid_t pid);
    void InsertNewThreadIntoQueue(Thread* thread);
    void BalanceRunQueues();

    void Initialize();
    void Tick(RegisterContext* r);
}
