// cl.exe /O2 sample.c apiresolver.c
// cl.exe /DNOAPI /O2 sample.c apiresolver.c /link mglib.lib

#include "apiresolver.h"

typedef int (*printf_t)(const char*, ...);

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
    printf("advapi32: %p AdjustTokenPrivileges: %p\n", advapi, func);

    HANDLE hModule = LoadLibraryA("msvcrt.dll");

    printf_t print = (printf_t)GetProcAddress(hModule, "printf");
    printf("msvcrt: %p printf: %p print: %p\n", hModule, printf, print);

    if (print == NULL) {
        printf("Failed to get printf addr.\n");
        return 1;
    }
    
    print("[+] This is the printf function loaded from LoadLibrary and GetProcAddress!\n");
    print("[+] Everything worked!\n");
}