#include <Lori/Sys/Util.h>
#include <lori/syscall.h>
#include <sys/types.h>

#include <errno.h>
#include <stdint.h>

using namespace std;

extern char **environ;

pid_t lori_spawn(const char *path, int argc, char* const argv[], int flags)
{
    pid_t ret = syscall(SYS_EXEC, (uintptr_t)path, argc, (uintptr_t)argv, flags, environ);
    if (ret < 0)
    {
        errno = -ret;
        return -1;
    }

    return ret;
}

pid_t lori_spawn(const char *path, int argc, const char *const argv[], int flags, char **envp)
{
    pid_t ret = syscall(SYS_EXEC, (uintptr_t)path, argc, (uintptr_t)argv, flags, envp);
    if (ret < 0)
    {
        errno = -ret;
        return -1;
    }

    return ret;
}

namespace Lori
{
    void Yield() { syscall(SYS_YIELD); }

    long InterruptThread(pid_t tid)
    {
        if (long e = syscall(SYS_INTERRUPT_THREAD, tid); e < 0)
        {
            errno = e;
            return -1;
        }

        return 0;
    }

    int GetProcessInfo(pid_t pid, lori_process_info_t& pInfo)
    {
        long ret = -1;
        if ((ret = syscall(SYS_GET_PROCESS_INFO, pid, &pInfo)))
        {
            errno = -ret;
            return -1;
        }

        return 0;
    }

    int GetNextProcessInfo(pid_t *pid, lori_process_info_t& pInfo)
    {
        long ret = 1;
        if ((ret = syscall(SYS_GET_NEXT_PROCESS_INFO, pid, &pInfo)) < 0)
        {
            errno = -ret;
            return -1;
        }

        return ret;
    }

    void GetProcessList(vector<lori_process_info_t>& list)
    {
        pid_t pid = 0;

        list.clear();

        lori_process_info_t pInfo;
        while (!GetNextProcessInfo(&pid, pInfo)) { list.push_back(pInfo); }
    }
}
