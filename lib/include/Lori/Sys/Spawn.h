#pragma once

#ifndef __lori__
#error "Lori OS Only"
#endif

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

#define EXEC_CHILD 1

pid_t lori_spawn(const char* path, int argc, char* const argv[], int flags = 0);
pid_t lori_spawn(const char* path, int argc, char* const argv[], int flags, char** envp);
