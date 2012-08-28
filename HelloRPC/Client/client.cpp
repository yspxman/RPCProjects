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
	/*
    _TCHAR* pszUuid             = NULL;
    _TCHAR* pszProtocolSequence = "ncalrpc";
    _TCHAR* pszNetworkAddress   = NULL;
    _TCHAR* pszEndpoint         = "hello";
    _TCHAR* pszOptions          = NULL;
	*/
    unsigned char* pszStringBinding    = NULL;
     char* pszString("hello, world");
    DWORD ulCode;
 
    status = RpcStringBindingCompose(NULL,
                                     (unsigned char*)"ncalrpc",
                                     NULL,
                                     (unsigned char*)"hello",
                                     NULL,
                                     &pszStringBinding);
    if (status) exit(status);

    status = RpcBindingFromStringBinding(pszStringBinding, &hello_IfHandle);
 
	//status = RpcBindingFromStringBinding(pszStringBinding, &hello_v1_0_c_ifspec);
 

    if (status) exit(status);
 
    RpcTryExcept  
    {
		int res;
        HelloProc((unsigned char*)pszString, &res);
		printf("Server return %d\n", res);

        //Shutdown();
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

