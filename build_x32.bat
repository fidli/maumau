@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x86
cls
set SERVER_FILES="%cd%\sources\windows32_server.cpp"
set SERVER_LIBS="Kernel32.lib" "Advapi32.lib" "Shell32.lib" "Ws2_32.lib"
set CLIENT_FILES="%cd%\sources\windows32_client.cpp"
set CLIENT_LIBS="Ws2_32.lib" "%cd%\lib\glew\lib\Release\Win32\glew32.lib" "Kernel32.lib" "User32.lib" "Gdi32.lib" "Gdiplus.lib" "Dsound.lib" "Mfplat.lib" "Ole32.lib" "Mfreadwrite.lib" "Mfuuid.lib" "Advapi32.lib" "Shell32.lib" "Opengl32.lib"
pushd build
del *.pdb
rem CANNOT USE SOME C++ FEATURES, std lib is ripped off (https://hero.handmade.network/forums/code-discussion/t/94)
call cl.exe /nologo /arch:IA32 /W0 /WX /EHa- /GS- /GR- /Od /Zi /FS /Fdserver32.pdb /Feserver.exe %SERVER_FILES%  /link /INCREMENTAL:NO /NODEFAULTLIB /SUBSYSTEM:CONSOLE %SERVER_LIBS%
call cl.exe /nologo /arch:IA32 /W0 /WX /EHa- /GS- /GR- /Od /Zi /FS /Fdclient32.pdb /Feclient32.exe %CLIENT_FILES%  /link /INCREMENTAL:NO /NODEFAULTLIB /SUBSYSTEM:WINDOWS %CLIENT_LIBS%
POPD