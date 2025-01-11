#ifndef _APIRESOLVER_H
#define _APIRESOLVER_H

#include <Windows.h>
#include <math.h>
#include <time.h>
#include <stdio.h>

// Function declarations
void  InitApiResolver(void);
PVOID ApiResolver(unsigned int API);

// Some global values
#define APILENGTH 4                     // How many random values point to the real function. See apiresolverstruct for how its used.
unsigned int ApiResolverCounter = 0;    // This is used to not get array accesses optimized away into a single value.
unsigned int ApiResolverSeed = 0;       // We are using rand() deterministically, so save the seed(). Not for crypto, its fine to use time()
PVOID ApiResolverAPIs[] = { &VirtualProtect, &VirtualAlloc, &LoadLibraryA, &GetProcAddress };

typedef LPVOID  (WINAPI * VIRTUALALLOC)( LPVOID, SIZE_T, DWORD, DWORD );
typedef BOOL  (WINAPI * VIRTUALPROTECT)( LPVOID, SIZE_T, DWORD, PDWORD );
typedef HMODULE (WINAPI * LOADLIBRARYA)( LPCSTR );
typedef FARPROC (WINAPI * GETPROCADDRESS)( HMODULE, LPCSTR );
typedef BOOL (WINAPI * LOOKUPPRIVILEGEVALUEA)( LPCSTR, LPCSTR, PLUID );

enum ApiOffsetOrder {
    ApiVirtualProtect,
    ApiVirtualAlloc,
    ApiLoadLibraryA,
    ApiGetProcAddress,
    ApiLookupPrivilegeValueA,
    ApiEnumCount
};

UINT apiresolverstruct[APILENGTH*ApiEnumCount];

#define VirtualProtect ((VIRTUALPROTECT)ApiResolver(((UINT*)&apiresolverstruct)[APILENGTH*ApiVirtualProtect + (ApiResolverCounter++%APILENGTH)]))
#define VirtualAlloc     ((VIRTUALALLOC)ApiResolver(((UINT*)&apiresolverstruct)[APILENGTH*ApiVirtualAlloc   + (ApiResolverCounter++%APILENGTH)]))
#define LoadLibraryA     ((LOADLIBRARYA)ApiResolver(((UINT*)&apiresolverstruct)[APILENGTH*ApiLoadLibraryA   + (ApiResolverCounter++%APILENGTH)]))
#define GetProcAddress ((GETPROCADDRESS)ApiResolver(((UINT*)&apiresolverstruct)[APILENGTH*ApiGetProcAddress + (ApiResolverCounter++%APILENGTH)]))
#define LookupPrivilegeValueA ((LOOKUPPRIVILEGEVALUEA)ApiResolver(((UINT*)&apiresolverstruct)[APILENGTH*ApiLookupPrivilegeValueA + (ApiResolverCounter++%APILENGTH)]))

// Function definitions if using as single header
void InitApiResolver(void)
{
    ApiResolverSeed = time(NULL);
    srand(ApiResolverSeed);

    unsigned int delta  = rand()%(sizeof(apiresolverstruct)/sizeof(UINT));
    size_t real_size    = sizeof(apiresolverstruct)/sizeof(UINT);
    unsigned int offset = rand()%512;
    
    for (size_t i = 0; i < real_size; i++)
    {
        // printf("[%zu] addr: %p = %u (offset)\n", i, &((UINT*)&apiresolverstruct)[(i+delta)%real_size], offset);
        ((UINT*)&apiresolverstruct)[(i+delta)%real_size] = offset;
        offset += rand()%512;
    }
}

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
