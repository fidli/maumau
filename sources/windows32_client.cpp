 #include "common.h"
 
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
#include "../lib/glew/include/GL/glew.h"
 
#define PERSISTENT_MEM MEGABYTE(250)
#define TEMP_MEM MEGABYTE(500)
#define STACK_MEM MEGABYTE(250)
 
 
#include "util_mem.h"
 
 struct Context{
     HINSTANCE hInstance;
     HWND window;
     bool appRunning;
 };
 
 
 Context context;
 
#include "util_string.cpp"
#include "windows_time.cpp"
#include "windows_render.cpp"
#include "windows_audio.cpp"
#include "clientcode.cpp"
#include "windows_net.cpp"
 
 
 
 
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
 
 LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam){
     switch(message)
     {
         case WM_KEYUP:{
             if(wParam == VK_SPACE){
                 
             }
         }
         break;
         case WM_KEYDOWN:{
             if(wParam == VK_SPACE){                
                 
             }
         }
         break;
         case WM_CLOSE:
         case WM_DESTROY:
         {
             context.appRunning = false;
             return 0;
         } break;
     }
     
     return DefWindowProc (window, message, wParam, lParam);
 }
 
 
 
 static inline int main(LPWSTR * argvW, int argc) {
     
     LPVOID memoryStart = VirtualAlloc(NULL, TEMP_MEM + PERSISTENT_MEM + STACK_MEM, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
     
     if (memoryStart) {
         initMemory(memoryStart);
         game = (World *) mem.persistent;
         HRESULT jettisonPrivilegesResult = jettisonAllPrivileges();
         ASSERT(jettisonPrivilegesResult == ERROR_SUCCESS);
         
         
         char ** argv = &PUSHA(char *, argc);
         
         for(int i = 0; i < argc; i++){
             int toAlloc = WideCharToMultiByte(CP_UTF8, 0, argvW[i], -1, NULL, 0, NULL, NULL);
             ASSERT(toAlloc != 0);
             argv[i] = &PUSHA(char, toAlloc);
             int res = WideCharToMultiByte(CP_UTF8, 0, argvW[i], -1, argv[i], toAlloc, NULL, NULL);
             ASSERT(res != 0);
         }
         
         
         WNDCLASSEX style = {};
         style.cbSize = sizeof(WNDCLASSEX);
         style.style = CS_OWNDC;
         style.lpfnWndProc = WindowProc;
         style.hInstance = context.hInstance;
         style.lpszClassName = "GameMainClass";
         if(RegisterClassEx(&style) != 0){
             context.window = CreateWindowEx(NULL,
                                             "GameMainClass", "MauMau", WS_OVERLAPPEDWINDOW | WS_SIZEBOX,
                                             CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, context.hInstance, NULL);
             
             if(context.window){
                 
                 PIXELFORMATDESCRIPTOR contextFormat = {
                     sizeof(PIXELFORMATDESCRIPTOR),
                     1,
                     PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
                     PFD_TYPE_RGBA,
                     32, //32 bits per color
                     0,
                     0,
                     0,
                     0,
                     0,
                     0,
                     0,
                     0,
                     0,
                     0,
                     0,
                     0,
                     0,
                     24, //24 bit z buffer
                     0, //0 bit stencil buffer (these two values must not exceed 32 bit)
                     0,
                     0,
                     0,
                     0
                 };
                 
                 HDC dc = GetDC(context.window);
                 int pixelFormat = ChoosePixelFormat(dc, &contextFormat);//windows matches our wishes
                 if(pixelFormat != 0){
                     if(SetPixelFormat(dc, pixelFormat, &contextFormat) == TRUE){
                         HGLRC opengl = wglCreateContext(dc);
                         
                         if(opengl != NULL){
                             //load gl fucntions
                             glewExperimental = TRUE; //some buggy version failing to load libs
                             if(wglMakeCurrent(dc, opengl) == TRUE){
                                 if(glewInit() == GLEW_OK){
                                     
                                     if(initNet()){
                                         if(initGraphics()){
                                             if(initAudio()){
                                                 if(initTime()){
                                                     ShowWindow(context.window, SW_SHOWMAXIMIZED);
                                                     
                                                     
                                                     float64 currentTime = getProcessCurrentTime();
                                                     float64 timeAccumulator = 0.0f;
                                                     
                                                     
                                                     
                                                     //-----------------------------------------------------------------------------------------------------------------------------------------------
                                                     while (context.appRunning) {
                                                         float64 newTime = getProcessCurrentTime();
                                                         float64 frameTime = newTime - currentTime;
                                                         currentTime = newTime;
                                                         timeAccumulator += frameTime;
                                                         
                                                         
                                                         FLUSH;
                                                         
                                                         
                                                         MSG msg;
                                                         while(PeekMessage(&msg, context.window, 0, 0, PM_REMOVE))
                                                         {
                                                             TranslateMessage(&msg);
                                                             DispatchMessage(&msg);
                                                         }
                                                         
                                                         
                                                         while(timeAccumulator >= TIME_STEP){
                                                             update(TIME_STEP);
                                                             
                                                             timeAccumulator -= TIME_STEP;
                                                         }
                                                         
                                                         
                                                         
                                                         render(getRenderQueue());
                                                         mix(getAudioQueue());
                                                         
                                                         
                                                         
                                                         
                                                         InvalidateRect(context.window, NULL, TRUE);                       
                                                     }
                                                     
                                                     
                                                     
                                                     
                                                     
                                                 }else{
                                                     //log
                                                     ASSERT(!"failed to init time");
                                                 }
                                                 closeAudio();
                                             }else{
                                                 //log
                                                 ASSERT(!"failed to init audio");
                                             }
                                             closeGraphics();
                                         }else{
                                             //log
                                             ASSERT(!"failed to init graphics");
                                         }
                                     }else{
                                         //log
                                         ASSERT(!"failed to init net");
                                     }
                                 }else{
                                     //log
                                     ASSERT(!"failed to init opengl functions");
                                 }
                                 
                             }else{
                                 //log
                                 ASSERT(!"failed to set current context");
                             }
                             
                         }else{
                             //log
                             ASSERT(!"failed to create context");
                         }
                     }else{
                         //log
                         ASSERT(!"failed to set pixel format");
                     }
                 }else{
                     //log
                     ASSERT(!"failed to obtain desired pixel format");
                 }
             }else{
                 //log
                 ASSERT(!"failed to create window");
             }
             
             UnregisterClass("GameMainClass", context.hInstance);
         }
         
         
         
         if (!VirtualFree(memoryStart, 0, MEM_RELEASE)) {
             //more like log it
             ASSERT(!"Failed to free memory");
         }
         
         
     }else{
         //more like log it
         ASSERT(!"failed to init memory");
     }
     return 0;
     
     
 }
 
 
 
 void __stdcall WinMainCRTStartup(){
     //this might fail, consider, whether stop the app, or continue
     SetProcessDEPPolicy(PROCESS_DEP_ENABLE);
     int argc = 0;
     LPWSTR * argv =  CommandLineToArgvW(GetCommandLineW(), &argc);
     
     context.hInstance = GetModuleHandle(NULL);
     context.appRunning = true;
     int result = main(argv,argc);
     LocalFree(argv);
     ExitProcess(result);
 }
 
