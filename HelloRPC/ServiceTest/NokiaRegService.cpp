#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>

#include "../inc/common.h"
#include "RPCServer.h"

int beepDelay = 2000;

SERVICE_STATUS_HANDLE s_statusHandle;
HANDLE terminateEvent = NULL;
HANDLE threadHandle = NULL;


// forward declaration
    // Set the service status and report the status to the SCM.
    void SetServiceStatus(DWORD dwCurrentState, 
        DWORD dwWin32ExitCode = NO_ERROR, 
        DWORD dwWaitHint = 0);



DWORD ServiceThread(LPDWORD param)
{
	while (1)
	{
		//MessageBox(0, L"thread", L"Service", MB_OK | MB_SETFOREGROUND |MB_DEFAULT_DESKTOP_ONLY);
		Beep(200, 200);
		Sleep(beepDelay);
	}
	return 0;
}

void SetServiceStatus(DWORD dwCurrentState, 
                                    DWORD dwWin32ExitCode, 
                                    DWORD dwWaitHint)
{
    static DWORD dwCheckPoint = 1;
	BOOL res;
    // The status of the service
    SERVICE_STATUS s_status;

    // The service status handle
    
    // Fill in the SERVICE_STATUS structure of the service.

	// льсп╣д
	s_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    s_status.dwCurrentState = dwCurrentState;
    s_status.dwWin32ExitCode = dwWin32ExitCode;
    s_status.dwWaitHint = dwWaitHint;
	s_status.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE | SERVICE_ACCEPT_SHUTDOWN;


    s_status.dwCheckPoint = 
        ((dwCurrentState == SERVICE_RUNNING) ||
        (dwCurrentState == SERVICE_STOPPED)) ? 
        0 : dwCheckPoint++;

        // Report the status of the service to the SCM.
    res = SetServiceStatus(s_statusHandle, &s_status);

}

BOOL InitService()
{
	DWORD id;
	
	//start  the service's thread
	//threadHandle = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)ServiceThread, 0, 0, &id);

	return RPCServerInit();
	//return true;

}

void Start(DWORD dwArgc, PWSTR *pszArgv)
{
    try
    {
        SetServiceStatus(SERVICE_RUNNING, NO_ERROR, 0);

        // Perform service-specific initialization.
        //OnStart(dwArgc, pszArgv);
		InitService();
        // Tell SCM that the service is started.
        SetServiceStatus(SERVICE_RUNNING);
    }
    catch (DWORD dwError)
    {
        SetServiceStatus(SERVICE_STOPPED, dwError);
    }

}

void Pause()
{
    try
    {
        // Tell SCM that the service is pausing.
        SetServiceStatus(SERVICE_PAUSE_PENDING);

        // Perform service-specific pause operations.
        //OnPause();

        // Tell SCM that the service is paused.
        SetServiceStatus(SERVICE_PAUSED);
    }
    catch (DWORD dwError)
    {
        // Log the error.
        //WriteErrorLogEntry(L"Service Pause", dwError);

        // Tell SCM that the service is still running.
        SetServiceStatus(SERVICE_RUNNING);
    }
    catch (...)
    {
        // Log the error.
        //WriteErrorLogEntry(L"Service failed to pause.", EVENTLOG_ERROR_TYPE);

        // Tell SCM that the service is still running.
        SetServiceStatus(SERVICE_RUNNING);
    }
}

void Continue()
{
    try
    {
        // Tell SCM that the service is resuming.
        SetServiceStatus(SERVICE_CONTINUE_PENDING);

        // Perform service-specific continue operations.
        //OnContinue();

        // Tell SCM that the service is running.
        SetServiceStatus(SERVICE_RUNNING);
    }
    catch (DWORD dwError)
    {
        // Log the error.
        //WriteErrorLogEntry(L"Service Continue", dwError);

        // Tell SCM that the service is still paused.
        SetServiceStatus(SERVICE_PAUSED);
    }
    catch (...)
    {
        // Log the error.
        //WriteErrorLogEntry(L"Service failed to resume.", EVENTLOG_ERROR_TYPE);

        // Tell SCM that the service is still paused.
        SetServiceStatus(SERVICE_PAUSED);
    }
}

void Stop()
{

    {
        // Tell SCM that the service is stopping.
        SetServiceStatus(SERVICE_STOP_PENDING);

        // Perform service-specific stop operations.
        //OnStop();
        //SMSR_DEBUG_MSG(L"SetServiceStatus(SERVICE_STOPPED)");

        // Tell SCM that the service is stopped.
        SetServiceStatus(SERVICE_STOPPED);
        //SMSR_DEBUG_MSG(L"SetServiceStatus(SERVICE_STOPPED) end");

		SetEvent(terminateEvent);
    }

}

void Shutdown()
{
        // Perform service-specific shutdown operations.
        //OnShutdown();

        // Tell SCM that the service is stopped.
        SetServiceStatus(SERVICE_STOPPED);

		//close event 
		if (terminateEvent)
			CloseHandle(terminateEvent);
		if (threadHandle)
			CloseHandle(threadHandle);
}

void WINAPI ServiceCtrlHandler(DWORD dwCtrl)
{
    switch (dwCtrl)
    {
    case SERVICE_CONTROL_STOP: Stop(); break;
    case SERVICE_CONTROL_PAUSE: Pause(); break;
    case SERVICE_CONTROL_CONTINUE: Continue(); break;
    case SERVICE_CONTROL_SHUTDOWN: Shutdown(); break;
    case SERVICE_CONTROL_INTERROGATE: break;
    default: break;
    }
}

void WINAPI ServiceMain(DWORD dwArgc, PWSTR *pszArgv)
{
	    // Register the handler function for the service
    s_statusHandle = RegisterServiceCtrlHandler(
        SERVICE_NAME, ServiceCtrlHandler);
    if (s_statusHandle == NULL)
    {
        throw GetLastError();
    }

	// create event 
	terminateEvent =  CreateEvent(0, TRUE, FALSE, 0);

	if (terminateEvent == 0)
	{
		Shutdown();
	}

    // Start the service.
    Start(dwArgc, pszArgv);


	//MessageBox(0, L"qrewerwe", L"Service", MB_OK | MB_SETFOREGROUND |MB_DEFAULT_DESKTOP_ONLY);

	WaitForSingleObject(terminateEvent, INFINITE);
}

#if 1
void _tmain(void)
#else

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
#endif
{

	//MessageBox(0, L"thread", L"Service", MB_OK | MB_SETFOREGROUND |MB_DEFAULT_DESKTOP_ONLY);

	//DebugBreak();
	SERVICE_TABLE_ENTRY serviceTable[] = 
    {
        { SERVICE_NAME, 
		(LPSERVICE_MAIN_FUNCTION)ServiceMain },
        { NULL, NULL }
    };

    // Connects the main thread of a service process to the service control 
    // manager, which causes the thread to be the service control dispatcher 
    // thread for the calling process. This call returns when the service has 
    // stopped. The process should simply terminate when the call returns.
	
	//Sleep(1000*22);

    BOOL ret = StartServiceCtrlDispatcher(serviceTable);
	 if (!ret)
		 ExitProcess(::GetLastError());
    //SMSR_INFO_MSG(L"the SMSR Service exited\n");
    //return ret;

}

