#pragma once
typedef struct
{
	uint64_t totalMem;
	uint64_t usedMem;
	uint16_t cpuCount;
} lori_sysinfo_t;

namespace Lori{ extern char* versionString; }
