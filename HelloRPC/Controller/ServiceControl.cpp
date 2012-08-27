//=============================================================================
// This file implements a function to safely start the service
// It was placed in its own lib to allow reuse, eg in tests.

#include <windows.h>
#include "ServiceControl.h"
#include <stdio.h>

namespace {
    
const static DWORD DEFAULT_SERVICE_STARTUP_TIMEOUT_MSECS = 5000;

// a simple class which closes the service handle on destruction
struct auto_schandle
{
    auto_schandle(SC_HANDLE handle) {
        this->handle = handle;
    }

    bool valid() {
        return handle != NULL;
    }

    ~auto_schandle()
    {
        if (handle != NULL) {
            CloseServiceHandle(handle);
            handle = NULL;
        }
    }

    operator SC_HANDLE() { return handle; }

    SC_HANDLE handle;
};
    
} // unnamed namespace

/// <summary>
///     Check if the specified service with specified prefix is active and if not start it.
/// </summary>
/// <param name="pszServiceName">Service short name.</param>
/// <returns>
///     A standard HRESULT to indicate pass/fail.
/// </returns>
HRESULT StartSvc (
    _In_ PCWSTR pszServiceName )
{    
    auto_schandle scmHandle = OpenSCManager ( NULL, NULL, SC_MANAGER_CONNECT );
    if (scmHandle.valid() == FALSE) {
        wprintf(L"scmHandle.valid() == FALSE\n");
        return HRESULT_FROM_WIN32(GetLastError());
    }
    
    auto_schandle serviceHandle ( 
        OpenServiceW ( 
            scmHandle, 
            pszServiceName, 
            SERVICE_START | SERVICE_QUERY_STATUS ) );
            
    if (serviceHandle.valid() == FALSE) {
        wprintf(L"serviceHandle.valid() == FALSE");
        return HRESULT_FROM_WIN32(GetLastError());
    }

    // Loop twice just in case we happen to catch the service in SERVICE_STOPPED
    // state. After the service enters SERVICE_RUNNING state, it is not expected to
    // stop on idle for several minutes.
    for ( UCHAR i = 0; i < 2; i ++ )
    {
        const BOOL serviceStarted = StartService ( serviceHandle, 0, NULL );
        if (serviceStarted == FALSE) {
            if (ERROR_SERVICE_ALREADY_RUNNING != GetLastError ()) {
                wprintf(L"StartService error %d",GetLastError());
                return HRESULT_FROM_WIN32(GetLastError());
            }
        }  
    }

    return S_OK;
}


HRESULT StopSvc (
    _In_ PCWSTR pszServiceName )
{
    SC_HANDLE schSCManager = NULL;
    SC_HANDLE schService = NULL;
    SERVICE_STATUS ssSvcStatus = {};
    HRESULT hr = S_OK;

    // Open the local default service control manager database
    schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
    if (schSCManager == NULL)
    {
        wprintf(L"OpenSCManager failed w/err 0x%08lx\n", GetLastError());
        goto Cleanup;
    }

    // Open the service with delete, stop, and query status permissions
    schService = OpenService(schSCManager, pszServiceName, SERVICE_STOP | 
        SERVICE_QUERY_STATUS | DELETE);
    if (schService == NULL)
    {
        wprintf(L"OpenService failed w/err 0x%08lx\n", GetLastError());
        goto Cleanup;
    }

    // Try to stop the service
    if (ControlService(schService, SERVICE_CONTROL_STOP, &ssSvcStatus))
    {
        wprintf(L"Stopping %s.", pszServiceName);
        Sleep(1000);

        while (QueryServiceStatus(schService, &ssSvcStatus))
        {
            if (ssSvcStatus.dwCurrentState == SERVICE_STOP_PENDING)
            {
                wprintf(L".");
                Sleep(1000);
            }
            else break;
        }

        if (ssSvcStatus.dwCurrentState == SERVICE_STOPPED)
        {
            wprintf(L"\n%s is stopped.\n", pszServiceName);
            hr = S_OK;
        }
        else
        {
            wprintf(L"\n%s failed to stop.\n", pszServiceName);
            hr = E_FAIL;
        }
    }

Cleanup:
    // Centralized cleanup for all allocated resources.
    if (schSCManager)
    {
        CloseServiceHandle(schSCManager);
        schSCManager = NULL;
    }
    if (schService)
    {
        CloseServiceHandle(schService);
        schService = NULL;
    }
    return hr;
}
