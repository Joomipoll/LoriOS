#ifndef _LIBALLOC_H
#define _LIBALLOC_H

#include <stdint.h>
#include <stddef.h>

#define PREFIX(func)	k ## func
#define alloc_lock liballoc_lock
#define alloc_unlock liballoc_unlock

#ifdef __cplusplus
extern "C"
{
#endif

	void liballoc_kprintf(const char* __restrict fmt, ...);
	extern int liballoc_lock();
	extern int liballoc_unlock();
	extern void* liballoc_alloc(size_t);
	extern int liballoc_free(void*, size_t);
	extern int templiballoc_unlock();
#if defined DEBUG || defined INFO || true
	void liballoc_dump();
#endif


	extern void    *PREFIX(malloc)(size_t);
	extern void    *PREFIX(malloc_unlocked)(size_t);
	extern void    *PREFIX(realloc)(void *, size_t);
	extern void    *PREFIX(calloc)(size_t, size_t);
	extern void     PREFIX(free)(void *);
	extern void     PREFIX(free_unlocked)(void *);
#ifdef __cplusplus
}
#endif
