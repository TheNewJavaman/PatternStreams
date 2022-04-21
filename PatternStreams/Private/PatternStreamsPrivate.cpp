#include "PatternStreamsPrivate.h"

#include <Psapi.h>

BytePtrInterval PatternInfo::GetModuleInterval(const std::string& module)
{
    const auto found = ModuleIntervals.find(module);
    if (found == ModuleIntervals.end())
    {
        const auto handle = GetProcessHandle();
        BytePtrInterval interval = {static_cast<BytePtr>(handle)};
        MODULEINFO moduleInfo;
        GetModuleInformation(handle, static_cast<HMODULE>(handle), &moduleInfo, sizeof moduleInfo);
        interval.Length = moduleInfo.SizeOfImage;
        return interval;
    }
    return found->second;
}

HANDLE PatternInfo::GetProcessHandle()
{
    if (ProcessHandle == nullptr)
    {
        ProcessHandle = GetCurrentProcess();
    }
    return ProcessHandle;
}
