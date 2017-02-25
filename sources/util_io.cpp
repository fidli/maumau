#ifndef UTIL_IO
#define UTIL_IO

#include "util_filesystem.h"

FileHandle getStdOut();
bool writeConsole(const FileContents * source);
int readConsole(char * buffer, uint16 maxlen);
bool initIo();

int printf(const char * format, ...){
    va_list ap;
    va_start(ap, format);
    
    FileContents con;
    con.contents = &PUSHA(char, 1024);
    uint32 successfullyPrinted = printFormatted(con.contents, format, ap);
    con.size = strlen(con.contents);
    ASSERT(con.size < 1024);
    va_end(ap);
    
    writeConsole(&con);
    POP;
    return successfullyPrinted;
}

int scanf(const char * format, ...){
    va_list ap;
    va_start(ap, format);
    
    char * buffer = &PUSHA(char, 1024);
    ASSERT(readConsole(buffer, 1024) > 0);
    
    uint32 succesfullyScanned = scanFormatted(buffer, format, ap);
    POP;
    va_end(ap);
    
    return succesfullyScanned;
}

#endif