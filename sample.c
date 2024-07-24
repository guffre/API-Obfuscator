#include "apiresolver.h"

// Sample Usage:
int main(void)
{
    InitApiResolver();

    HANDLE advapi = LoadLibraryA("advapi32.dll");
    advapi = LoadLibraryA("advapi32.dll");
    advapi = LoadLibraryA("advapi32.dll");
    advapi = LoadLibraryA("advapi32.dll");
    advapi = LoadLibraryA("advapi32.dll");

    PVOID func = GetProcAddress(advapi, "AdjustTokenPrivileges");
    func = GetProcAddress(advapi, "AdjustTokenPrivileges");
    func = GetProcAddress(advapi, "AdjustTokenPrivileges");
    func = GetProcAddress(advapi, "AdjustTokenPrivileges");
    func = GetProcAddress(advapi, "AdjustTokenPrivileges");

    printf("advapi32: %p AdjustTokenPrivileges: %p\n", advapi, func);

}