#pragma once

#include <Windows.h>
#include <string>

// Read the payload chunks from NVRAM vars, assemble and return it
// ------------------------------------------------------------------------

std::string read_payload();

// Our pseudo entry-point
// ------------------------------------------------------------------------

void go(void);