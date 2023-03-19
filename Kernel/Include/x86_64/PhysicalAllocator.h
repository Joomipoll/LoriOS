#pragma once
#include <MiscHdr.h>
#include <Paging.h>
#include <Serial.h>
#include <stddef.h>
#define PHYSALLOC_BLOCK_SIZE 4096
#define PHYSALLOC_BLOCK_SHIFT 12
#define PHYSALLOC_BLOCKS_PER_BYTE 8
#define PHYSALLOC_BITMAP_SIZE_DWORDS 524488
extern void* kernel_end;
namespace Memory {
void InitializePhysicalAllocator(memory_info_t* mem_info);
uint64_t GetFirstFreeMemoryBlock();
void MarkMemoryRegionUsed(uint64_t base, size_t size);
void MarkMemoryRegionFree(uint64_t base, size_t size);
uint64_t AllocatePhysicalMemoryBlock();
uint64_t AllocateLargePhysicalMemoryBlock();
void FreePhysicalMemoryBlock(uint64_t addr);
extern uint64_t usedPhysicalBlocks;
extern uint64_t maxPhysicalBlocks;
}
