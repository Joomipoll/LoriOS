#pragma once
#include <CPU.h>
#include <List.h>
#include <MiscHdr.h>
#include <Signal.h>
#include <Spinlock.h>
#include <Timer.h>
#include <stdint.h>
#include <abi-bits/pid_t.h>
#define THREAD_TIMESLICE_DEFAULT 10
enum { ThreadStateRunning = 0, ThreadStateBlocked = 1, ThreadStateZombie  = 2, ThreadStateDying   = 3, };
class Process;
struct Thread;
class ThreadBlocker {
    friend struct Thread;
protected:
    lock_t lock = 0;
    Thread* thread = nullptr;
    bool shouldBlock = true;
    bool interrupted = false;
    bool removed = false;
public:
    virtual ~ThreadBlocker() = default;
    virtual void Interrupt();
    virtual void Unblock();
    inline bool ShouldBlock() { return shouldBlock; }
    inline bool WasInterrupted() { return interrupted; }
};

class GenericThreadBlocker : public ThreadBlocker {
public:
    inline void Interrupt() {}
};

using FutexThreadBlocker = GenericThreadBlocker;

struct Thread {
    lock_t stateLock = 0;
    lock_t kernelLock = 0;
    Process* parent;
    pid_t tid = 1;
    void* stack = nullptr;
    void* stackLimit = nullptr;
    void* kernelStack = nullptr;
    void* kernelStackBase = 0;
    uint32_t timeSlice = THREAD_TIMESLICE_DEFAULT;
    uint32_t timeSliceDefault = THREAD_TIMESLICE_DEFAULT;
    uint32_t ticksSinceBalance = 0;
    uint32_t ticksGivenSinceBalance = 0;
    RegisterContext registers;
    struct {
        RegisterContext regs;
        long result;
    } lastSyscall;
    void* fxState;
    int cpu = -1;
    Thread* next = nullptr;
    Thread* prev = nullptr;
    uint8_t priority = 0;
    uint8_t state = ThreadStateRunning;
    uint64_t fsBase = 0;
    bool blockTimedOut = false;
    ThreadBlocker* blocker = nullptr;
    uint64_t pendingSignals = 0;
    uint64_t signalMask = 0;
    Thread(class Process* _parent, pid_t _tid);
    ~Thread();
    ALWAYS_INLINE static Thread* Current() { return GetCurrentThread(); }
    void Signal(int signal);
    void HandlePendingSignal(RegisterContext* regs);
    ALWAYS_INLINE uint64_t EffectiveSignalMask() const { return signalMask & (~UNMASKABLE_SIGNALS); }
    ALWAYS_INLINE bool HasPendingSignals() { return (~EffectiveSignalMask()) & pendingSignals; }
    [[nodiscard]] bool Block(ThreadBlocker* blocker);
    [[nodiscard]] bool Block(ThreadBlocker* blocker, long& usTimeout);
    void Sleep(long us);
    void Unblock();
};
