#pragma once

#include <Lock.h>
#include <Paging.h>
#include <Pair.h>
#include <RefPtr.h>
#include <Vector.h>

#include <MM/VMObject.h>

class AddressSpace final {
public:
    AddressSpace(PageMap* pm);
  
    ~AddressSpace();
  
    ALWAYS_INLINE AddressSpace* Kernel();
  
    MappedRegion* AddressToRegionReadLock(uintptr_t address);
    MappedRegion* AddressToRegionWriteLock(uintptr_t address);
  
    bool RangeInRegion(uintptr_t base, size_t size);

    long UnmapRegion(MappedRegion* region);

    [[nodiscard]] MappedRegion* MapVMO(FancyRefPtr<VMObject> obj, uintptr_t base, bool fixed);
    MappedRegion* AllocateAnonymousVMObject(size_t size, uintptr_t base, bool fixed);
    AddressSpace* Fork();

    long UnmapMemory(uintptr_t base, size_t size);

    size_t UsedPhysicalMemory() const;
    void DumpRegions();

    ALWAYS_INLINE PageMap* GetPageMap() { return m_pageMap; }

    ALWAYS_INLINE lock_t* GetLock() { return &m_lock; }

protected:
    MappedRegion* FindAvailableRegion(size_t size);
    MappedRegion* AllocateRegionAt(uintptr_t base, size_t size);

    ALWAYS_INLINE bool IsKernel() const { return this == m_kernel; }

    AddressSpace* m_kernel; // Kernel Address Space

    uintptr_t m_startRegion = 0; // Start of the address space (0 for usermode, KERNEL_VIRTUAL_BASE for kernel)
    uintptr_t m_endRegion = KERNEL_VIRTUAL_BASE;   // End of the address space (KERNEL_VIRTUAL_BASE for usermode, UINT64_MAX for kernel)

    lock_t m_lock = 0;

    PageMap* m_pageMap = nullptr;
    List<MappedRegion> m_regions;

    AddressSpace* m_parent = nullptr;
};
