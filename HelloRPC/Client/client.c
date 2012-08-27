/* file: helloc.c */
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "hello.h" 
#include <windows.h>
#include <tchar.h>

void _tmain()
{
    RPC_STATUS status;
    _TCHAR* pszUuid             = NULL;
    _TCHAR* pszProtocolSequence = L"ncalrpc";
    _TCHAR* pszNetworkAddress   = NULL;
    _TCHAR* pszEndpoint         = L"hello";
    _TCHAR* pszOptions          = NULL;
    _TCHAR* pszStringBinding    = NULL;
    _TCHAR* pszString           = L"hello, world";
    DWORD ulCode;
 
    status = RpcStringBindingCompose(pszUuid,
                                     pszProtocolSequence,
                                     pszNetworkAddress,
                                     pszEndpoint,
                                     pszOptions,
                                     &pszStringBinding);
    if (status) exit(status);

    status = RpcBindingFromStringBinding(pszStringBinding, &hello_IfHandle);
 
	//status = RpcBindingFromStringBinding(pszStringBinding, &hello_v1_0_c_ifspec);
 

    if (status) exit(status);
 
    RpcTryExcept  
    {
        HelloProc(pszString);
        Shutdown();
    }
    RpcExcept(1) 
    {
        ulCode = RpcExceptionCode();
        printf("Runtime reported exception 0x%lx = %ld\n", ulCode, ulCode);
    }
    RpcEndExcept
 
    status = RpcStringFree(&pszStringBinding); 
 
    if (status) exit(status);
 
    status = RpcBindingFree(&hello_IfHandle);
 
	//RpcBindingFree(&hello_v1_0_c_ifspec);

    if (status) exit(status);

    exit(0);
}

/******************************************************/
/*         MIDL allocate and free                     */
/******************************************************/
 
void __RPC_FAR * __RPC_USER midl_user_allocate(size_t len)
{
    return(malloc(len));
}
 
void __RPC_USER midl_user_free(void __RPC_FAR * ptr)
{
    free(ptr);
}

