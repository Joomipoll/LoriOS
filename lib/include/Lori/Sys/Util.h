#pragma once

#ifndef __lori__
#error "Lori OS Only"
#endif

#include <Lori/Sys/Info.h>
#include <Lori/Sys/ABI/Process.h>

#include <sys/types.h>
#include <stdint.h>
#include <limits.h>

#include <vector>

namespace Lori
{
    void Yield();

	long InterruptThread(pid_t tid);

    int GetProcessInfo(pid_t pid, lemon_process_info_t& pInfo);

    int GetNextProcessInfo(pid_t* pid, lemon_process_info_t& pInfo);

    void GetProcessList(std::vector<lemon_process_info_t>& list);
}
