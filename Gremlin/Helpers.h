#pragma once

#include <Windows.h>
#include <stdio.h>
#include <Tlhelp32.h>

// To convert the variable name to GUID
// ------------------------------------------------------------------------

LPCSTR convert_name_to_guid(LPCSTR name) {
	// Compute DJB2 hash of name
	DWORD hash = 5381, c;
	while (c = *name++)
		hash = ((hash << 5) + hash) + c;

	char* guid = (char*)malloc(100);
	sprintf(guid, "{%08X-1337-1337-1337-1337%08X}", hash, hash);

	return (LPCSTR)guid;
}

// Get PID by process name
// ------------------------------------------------------------------------

DWORD find_pid(const char *procname) {
    // Dynamically resolve some functions
    HMODULE kernel32 = GetModuleHandleA("Kernel32.dll");

    using CreateToolhelp32SnapshotPrototype = HANDLE(WINAPI *)(DWORD, DWORD);
    CreateToolhelp32SnapshotPrototype CreateToolhelp32Snapshot = (CreateToolhelp32SnapshotPrototype)GetProcAddress(kernel32, "CreateToolhelp32Snapshot");
    
    using Process32FirstPrototype = BOOL(WINAPI *)(HANDLE, LPPROCESSENTRY32);
    Process32FirstPrototype Process32First = (Process32FirstPrototype)GetProcAddress(kernel32, "Process32First");
    
    using Process32NextPrototype = BOOL(WINAPI *)(HANDLE, LPPROCESSENTRY32);
    Process32NextPrototype Process32Next = (Process32NextPrototype)GetProcAddress(kernel32, "Process32Next");
    
    // Init some important local variables
    HANDLE hProcSnap;
    PROCESSENTRY32 pe32;
    DWORD pid = 0;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    // Find the PID now by enumerating a snapshot of all the running processes
    hProcSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (INVALID_HANDLE_VALUE == hProcSnap)
        return 0;

    if (!Process32First(hProcSnap, &pe32)) {
        CloseHandle(hProcSnap);
        return 0;
    }

    while (Process32Next(hProcSnap, &pe32)) {
        if (lstrcmpiA(procname, pe32.szExeFile) == 0) {
            pid = pe32.th32ProcessID;
            break;
        }
    }
    
    // Cleanup
    CloseHandle(hProcSnap);

    return pid;
}

