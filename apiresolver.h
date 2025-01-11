#ifndef _APIRESOLVER_H
#define _APIRESOLVER_H

#include <Windows.h>
#include <math.h>
#include <time.h>
#include <stdio.h>

// For LookupPrivilegeValueA
#pragma comment(lib, "advapi32.lib")

// Function declarations
void  InitApiResolver(void);
PVOID ApiResolver(unsigned int API);

// Some global values
#define APILENGTH 4                     // How many random values point to the real function. Really this is just "jitter" to add math
unsigned int ApiResolverCounter = 0;    // This is used to not get array accesses optimized away into a single value.
unsigned int ApiResolverSeed = 0;       // We are using rand() deterministically, so save the seed(). Not for crypto, its fine to use time()

typedef LPVOID  (WINAPI * VirtualAlloc_T)( LPVOID, SIZE_T, DWORD, DWORD );
typedef BOOL  (WINAPI * VirtualProtect_T)( LPVOID, SIZE_T, DWORD, PDWORD );
typedef HMODULE (WINAPI * LoadLibraryA_T)( LPCSTR );
typedef FARPROC (WINAPI * GetProcAddress_T)( HMODULE, LPCSTR );
typedef BOOL (WINAPI * LookupPrivilegeValueA_T)( LPCSTR, LPCSTR, PLUID );

// These must be in the same order. The enum is just to give useful names for the API_DEFINE macro
PVOID ApiResolverAPIs[] = { &VirtualProtect, &VirtualAlloc, &LoadLibraryA, &GetProcAddress, &LookupPrivilegeValueA };
enum ApiOffsetOrder {
    ApiVirtualProtect,
    ApiVirtualAlloc,
    ApiLoadLibraryA,
    ApiGetProcAddress,
    ApiLookupPrivilegeValueA,
    ApiEnumCount
};

UINT apiresolverstruct[APILENGTH*ApiEnumCount];

// Name##_T -> VirtualProtect_T ; Api##Name -> ApiVirtualProtect
#define API_DEFINE(Name) ((Name##_T)ApiResolver(((UINT*)&apiresolverstruct)[APILENGTH*Api##Name + (ApiResolverCounter++ % APILENGTH)]))

#define VirtualProtect API_DEFINE(VirtualProtect)
#define VirtualAlloc API_DEFINE(VirtualAlloc)
#define LoadLibraryA API_DEFINE(LoadLibraryA)
#define GetProcAddress API_DEFINE(GetProcAddress)
#define LookupPrivilegeValueA API_DEFINE(LookupPrivilegeValueA)

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
        offset += rand()%512;
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

#endif
