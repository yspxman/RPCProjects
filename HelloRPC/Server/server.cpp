#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "hello.h"
#include <windows.h>

void HelloProc(unsigned char * pszString, int* res)
{
    printf("%s\n", pszString);
	*res = 250;
}
 
/* add this function to hellop.c */
void Shutdown(void)
{
    RPC_STATUS status;
 
    status = RpcMgmtStopServerListening(NULL);
 
    if (status) 
    {
       exit(status);
    }
 
    status = RpcServerUnregisterIf(NULL, NULL, FALSE);
 
    if (status) 
    {
       exit(status);
    }
} //end Shutdown

void main()
{
    RPC_STATUS status;
    /*
	unsigned char * pszProtocolSequence = "ncalrpc";
    unsigned char * pszEndpoint         = "hello";//"\\pipe\\hello";
	*/
    unsigned int    cMinCalls = 1;
    unsigned int    fDontWait = FALSE;
 
    status = RpcServerUseProtseqEp((unsigned char *)"ncalrpc",
                                   RPC_C_LISTEN_MAX_CALLS_DEFAULT,
                                   (unsigned char *)"hello",
                                   NULL); 
 
    if (status) exit(status);
 
    status = RpcServerRegisterIf(hello_v1_0_s_ifspec,								
                                 NULL,   
                                 NULL); 
 
    if (status) exit(status);
 
    status = RpcServerListen(cMinCalls,
                             RPC_C_LISTEN_MAX_CALLS_DEFAULT,
                             fDontWait);
 
    if (status) exit(status);
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