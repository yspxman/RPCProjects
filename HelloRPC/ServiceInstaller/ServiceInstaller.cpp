
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>

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

HRESULT InstallService(_TCHAR* name, _TCHAR* path )
{
		// 注册一个服务
	 auto_schandle scmHandle = OpenSCManager ( NULL, NULL, SC_MANAGER_CREATE_SERVICE );
    if (scmHandle.valid() == FALSE) {
        wprintf(L"scmHandle.valid() == FALSE\n");
        return HRESULT_FROM_WIN32(GetLastError());
    }
    
	    auto_schandle newServiceHandle = CreateServiceW ( 
            scmHandle, 
            name, 
			name, 
			SERVICE_ALL_ACCESS,
			SERVICE_WIN32_OWN_PROCESS,
			SERVICE_DEMAND_START,
			SERVICE_ERROR_NORMAL,
			path,
            0,0,0,0,0) ;


    if (newServiceHandle.valid() == FALSE) {
        wprintf(L"serviceHandle.valid() == FALSE");
        return HRESULT_FROM_WIN32(GetLastError());
    }
}

HRESULT UninstallService(_TCHAR* name)
{
	SERVICE_STATUS s_status;


	 auto_schandle scmHandle = OpenSCManager ( NULL, NULL, SC_MANAGER_CREATE_SERVICE );
    if (scmHandle.valid() == FALSE) {
        wprintf(L"scmHandle.valid() == FALSE\n");
        return HRESULT_FROM_WIN32(GetLastError());
    }
    
	    auto_schandle serviceHandle = OpenServiceW ( 
            scmHandle, 
            name,
			SERVICE_ALL_ACCESS|DELETE) ;


    if (serviceHandle.valid() == FALSE) {
        wprintf(L"serviceHandle.valid() == FALSE");
        return HRESULT_FROM_WIN32(GetLastError());
    }
	// stop the service if necessary

	BOOL res = ::QueryServiceStatus(serviceHandle, &s_status);
	if (!res)
	{
		wprintf(L"QueryServiceStatus Delete Service %s failed! err = %d", name, ::GetLastError());
		return GetLastError();
	}

	if (s_status.dwCurrentState !=  SERVICE_STOPPED)
	{
		// stop the service first
		res = ::ControlService(serviceHandle, SERVICE_CONTROL_STOP, &s_status);
		if (!res)
		{
			wprintf(L"ControlService Delete Service %s failed! err = %d", name, ::GetLastError());
			return GetLastError();
		}	

		Sleep(1000);
	}
	res = ::DeleteService(serviceHandle);

	if (res)
		wprintf(L"Service %s deleted", name);
	else
	{
		wprintf(L"Delete Service %s failed! err = %d", name, ::GetLastError());
		return GetLastError();
	}
	
}


int _tmain(int argc, _TCHAR* argv[])
{
    if ((argc > 1) && ((*argv[1] == L'-' || (*argv[1] == L'/'))))
    {
        if (_wcsicmp(L"install", argv[1] + 1) == 0)
        {
			if (argc == 4)
			{
				InstallService(argv[2], argv[3]);

			}
			else
			{
				wprintf(L"Usage: -install testservice c:\service.exe \n");
			}
            
        }
        else if (_wcsicmp(L"delete", argv[1] + 1) == 0)
        {
			if (argc == 3)
			{
				HRESULT ok = UninstallService(argv[2]);

			}

        }
    }

    else 
    {
        wprintf(L"Parameters:\n");
        wprintf(L" -start  to start the service.\n");
        wprintf(L" -stop   to stop the service.\n");
    }
    
    return 0;
}

