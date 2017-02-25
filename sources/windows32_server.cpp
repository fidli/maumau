                                                                                                                                                                     
                                                                                                                                                                         extern "C"{
#pragma function(memset)
                                                                                                                                                                         void * memset(void * dest, int c, size_t count)
                                                                                                                                                                         {
                                                                                                                                                                             char * bytes = (char *) dest;
                                                                                                                                                                             while (count--)
                                                                                                                                                                             {
                                                                                                                                                                                 (*bytes) = (char) c;
                                                                                                                                                                                 (*bytes++);
                                                                                                                                                                             }
                                                                                                                                                                             return dest;
                                                                                                                                                                         }
                                                                                                                                                                         
#pragma function(memcpy)
                                                                                                                                                                         void *memcpy(void *dest, const void *src, size_t count)
                                                                                                                                                                         {
                                                                                                                                                                             char *dest8 = (char *)dest;
                                                                                                                                                                             const char *src8 = (const char *)src;
                                                                                                                                                                             while (count--)
                                                                                                                                                                             {
                                                                                                                                                                                 *dest8++ = *src8++;
                                                                                                                                                                             }
                                                                                                                                                                             return dest;
                                                                                                                                                                         }
                                                                                                                                                                     }
                                                                                                                                                                     extern "C"{
                                                                                                                                                                         int _fltused;
                                                                                                                                                                     };
#include "windows32_64inst.cpp"
                                                                                                                                                                     
#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include "windows_types.h"
                                                                                                                                                                     
#include "common.h"
                                                                                                                                                                     
#define PERSISTENT_MEM MEGABYTE(10)
#define TEMP_MEM MEGABYTE(100)
#define STACK_MEM MEGABYTE(0)
                                                                                                                                                                     
                                                                                                                                                                     
#include "util_mem.h"
                                                                                                                                                                     
#include "windows_thread.cpp"
#include "windows_net.cpp"
#include "windows_time.cpp"
#include "windows_io.cpp"
                                                                                                                                                                     
#include "servercode.cpp"
                                                                                                                                                                     
                                                                                                                                                                     static inline DWORD jettisonAllPrivileges() {
                                                                                                                                                                         DWORD result = ERROR_SUCCESS;
                                                                                                                                                                         HANDLE processToken  = NULL;
                                                                                                                                                                         if(OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, &processToken)){
                                                                                                                                                                             DWORD privsize = 0;
                                                                                                                                                                             GetTokenInformation(processToken, TokenPrivileges, NULL, 0, &privsize);
                                                                                                                                                                             TOKEN_PRIVILEGES * priv = (TOKEN_PRIVILEGES *) &PUSHS(byte, privsize);
                                                                                                                                                                             if(GetTokenInformation(processToken, TokenPrivileges, priv , privsize, &privsize)){
                                                                                                                                                                                 
                                                                                                                                                                                 for(DWORD i = 0; i < priv->PrivilegeCount; ++i ){ 
                                                                                                                                                                                     priv->Privileges[i].Attributes = SE_PRIVILEGE_REMOVED;
                                                                                                                                                                                 }
                                                                                                                                                                                 if(AdjustTokenPrivileges(processToken, TRUE, priv, NULL, NULL, NULL) == 0){
                                                                                                                                                                                     result = GetLastError();
                                                                                                                                                                                 }
                                                                                                                                                                             }else{
                                                                                                                                                                                 result = GetLastError();
                                                                                                                                                                             }
                                                                                                                                                                             POP;
                                                                                                                                                                         }else{
                                                                                                                                                                             result = GetLastError();
                                                                                                                                                                         }
                                                                                                                                                                         CloseHandle(processToken);
                                                                                                                                                                         return result;
                                                                                                                                                                     }
                                                                                                                                                                     
                                                                                                                                                                     
                                                                                                                                                                     static inline int main(LPWSTR * argvW, int argc) {
                                                                                                                                                                         
                                                                                                                                                                         
                                                                                                                                                                         
                                                                                                                                                                         LPVOID memoryStart = VirtualAlloc(NULL, TEMP_MEM + PERSISTENT_MEM + STACK_MEM, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
                                                                                                                                                                         
                                                                                                                                                                         
                                                                                                                                                                         if (memoryStart) {
                                                                                                                                                                             initMemory(memoryStart);
                                                                                                                                                                             
                                                                                                                                                                             HRESULT jettisonPrivilegesResult = jettisonAllPrivileges();
                                                                                                                                                                             ASSERT(jettisonPrivilegesResult == ERROR_SUCCESS);
                                                                                                                                                                             
                                                                                                                                                                             char ** argv = &PUSHA(char *, argc);
                                                                                                                                                                             
                                                                                                                                                                             for(int i = 0; i < argc; i++){
                                                                                                                                                                                 int toAlloc = WideCharToMultiByte(CP_UTF8, 0, argvW[i], -1, NULL, 0, NULL, NULL);
                                                                                                                                                                                 ASSERT(toAlloc != 0);
                                                                                                                                                                                 argv[i] = &PUSHA(char, toAlloc);
                                                                                                                                                                                 ASSERT(WideCharToMultiByte(CP_UTF8, 0, argvW[i], -1, argv[i], toAlloc, NULL, NULL) != 0);
                                                                                                                                                                             }
                                                                                                                                                                             
                                                                                                                                                                             
                                                                                                                                                                             ASSERT(initNet());
                                                                                                                                                                             ASSERT(initTime());
                                                                                                                                                                             ASSERT(initIo());
                                                                                                                                                                             
                                                                                                                                                                             run();
                                                                                                                                                                             
                                                                                                                                                                             ASSERT(closeNet());
                                                                                                                                                                             
                                                                                                                                                                             if (!VirtualFree(memoryStart, 0, MEM_RELEASE)) {
                                                                                                                                                                                 ASSERT(!"Failed to free memory");
                                                                                                                                                                             }
                                                                                                                                                                             
                                                                                                                                                                             
                                                                                                                                                                         }
                                                                                                                                                                         return 0;
                                                                                                                                                                         
                                                                                                                                                                         
                                                                                                                                                                     }
                                                                                                                                                                     
                                                                                                                                                                     
                                                                                                                                                                     void __stdcall mainCRTStartup(){
                                                                                                                                                                         //this might fail, consider, whether stop the app, or continue
                                                                                                                                                                         SetProcessDEPPolicy(PROCESS_DEP_ENABLE);
                                                                                                                                                                         int argc = 0;
                                                                                                                                                                         LPWSTR * argv =  CommandLineToArgvW(GetCommandLineW(), &argc);
                                                                                                                                                                         int result = main(argv,argc);
                                                                                                                                                                         LocalFree(argv);
                                                                                                                                                                         ExitProcess(result);
}