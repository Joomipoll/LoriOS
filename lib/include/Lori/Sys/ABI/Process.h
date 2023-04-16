#pragma once

#include <abi-bits/pid_t.h>

typedef struct LoriProcessInfo
{
    pid_t pid;
    int32_t uid;
    int32_t gid;
	
	uint32_t threadCount;
    uint8_t state;

    char name[256]; // Process Name

    uint64_t runningTime; // sec.
    uint64_t activeUs;
    bool isCPUIdle = false; // Whether or not the process is an idle process

    uint64_t usedMem; // KB
} lori_process_info_t;
