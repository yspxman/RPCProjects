// ServiceController.cpp : Defines the entry point for the console application.
//

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>

#include "ServiceControl.h"

#include "../inc/Common.h"


int _tmain(int argc, _TCHAR* argv[])
{
    if ((argc > 1) && ((*argv[1] == L'-' || (*argv[1] == L'/'))))
    {
        if (_wcsicmp(L"start", argv[1] + 1) == 0)
        {
            HRESULT ok = StartSvc(SERVICE_NAME);
            if ( SUCCEEDED(ok))
            {
                wprintf(L"Start SmsRSvc successfully!\n");
            }
            else 
            {
                wprintf(L"Start SmsRSvc failed! Error code: %d\n",GetLastError());
            }
        }
        else if (_wcsicmp(L"stop", argv[1] + 1) == 0)
        {
            HRESULT ok = StopSvc(SERVICE_NAME);
            if ( SUCCEEDED(ok))
            {
                wprintf(L"Stop PIAsvc successfully!\n");
            }
            else 
            {
                wprintf(L"Stop PIAsvc failed! Error code: %d\n",GetLastError());
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
