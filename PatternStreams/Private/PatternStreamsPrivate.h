#pragma once

#define WIN32_LEAN_AND_MEAN
#include <map>
#include <Windows.h>

#include "PatternStreams.h"

struct BytePtrInterval
{
    BytePtr Start;
    size_t Length;
};

namespace Info
{
    static void InitModuleInfo(std::string module);
    static inline std::map<std::string, BytePtrInterval> ModuleIntervals;

    static void InitProcessInfo();
    static inline HANDLE ProcessHandle = nullptr;
}
