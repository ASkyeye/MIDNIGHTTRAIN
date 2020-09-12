#pragma once

#include <Windows.h>
#include <stdio.h>

// To check whether the target supports UEFI
// ------------------------------------------------------------------------

BOOL check_uefi() {
	// Init the firmware enum
	FIRMWARE_TYPE FirmwareType;

    // Get the firmware type
	GetFirmwareType(&FirmwareType);

    // Check if host supports UEFI
    BOOL ok;
	if (FirmwareType == FirmwareTypeUefi)
		ok = TRUE;

	return ok;
}

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