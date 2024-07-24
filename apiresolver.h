#ifndef _APIRESOLVER_H
#define _APIRESOLVER_H

#include <Windows.h>
#include <math.h>

// Function declarations
void  InitApiResolver(void);
PVOID ApiResolver(unsigned int API);

// Some global values
#define APILENGTH 4
static unsigned int ApiResolverCounter = 0;
unsigned int ApiResolverSeed;
PVOID ApiResolverAPIs[] = { &VirtualProtect, &VirtualAlloc, &LoadLibraryA, &GetProcAddress };

typedef LPVOID  (WINAPI * VIRTUALALLOC)( LPVOID, SIZE_T, DWORD, DWORD );
typedef BOOL  (WINAPI * VIRTUALPROTECT)( LPVOID, SIZE_T, DWORD, PDWORD );
typedef HMODULE (WINAPI * LOADLIBRARYA)( LPCSTR );
typedef FARPROC (WINAPI * GETPROCADDRESS)( HMODULE, LPCSTR );

#pragma pack(push, 1)
struct ApiResolver {
    UINT sVirtualProtect[APILENGTH];
    UINT sVirtualAlloc[APILENGTH];
    UINT sLoadLibraryA[APILENGTH];
    UINT sGetProcAddress[APILENGTH];
} apiresolverstruct;
#pragma pack(pop)

enum StructResolver {
    ApiVirtualProtect,
    ApiVirtualAlloc,
    ApiLoadLibraryA,
    ApiGetProcAddress,
};

#define VirtualProtect ((VIRTUALPROTECT)ApiResolver(((UINT*)&apiresolverstruct)[APILENGTH*ApiVirtualProtect + (ApiResolverCounter++%APILENGTH)]))
#define VirtualAlloc     ((VIRTUALALLOC)ApiResolver(((UINT*)&apiresolverstruct)[APILENGTH*ApiVirtualAlloc   + (ApiResolverCounter++%APILENGTH)]))
#define LoadLibraryA     ((LOADLIBRARYA)ApiResolver(((UINT*)&apiresolverstruct)[APILENGTH*ApiLoadLibraryA   + (ApiResolverCounter++%APILENGTH)]))
#define GetProcAddress ((GETPROCADDRESS)ApiResolver(((UINT*)&apiresolverstruct)[APILENGTH*ApiGetProcAddress + (ApiResolverCounter++%APILENGTH)]))

// Function definitions if using as single header
void InitApiResolver(void)
{
    ApiResolverSeed = time(NULL);
    srand(ApiResolverSeed);
    unsigned int delta = rand()%(sizeof(apiresolverstruct)/sizeof(UINT));
    unsigned int offset = 0;
    size_t real_size = sizeof(apiresolverstruct)/sizeof(UINT);
    for (size_t i = 0; i < real_size; i++)
    {
        // printf("addr: %p\n", &((UINT*)&structresolver)[(i+delta)%real_size]);
        unsigned int bump = rand()%512;
        ((UINT*)&apiresolverstruct)[(i+delta)%real_size] = offset + bump;
        offset += bump + rand()%512;
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