#ifndef WINDOWS_TIME
#define WINDOWS_TIME

#include "util_time.h"

static LARGE_INTEGER frequency;

bool initTime(){
    return QueryPerformanceFrequency(&frequency) != 0;
}

float32 getProcessCurrentTime(){
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    float32 result = (uint32)(counter.QuadPart / frequency.QuadPart);
    //((float32)((uint32)(counter.QuadPart % frequency.QuadPart)) / (uint32)frequency.QuadPart);
    return result;
    return ((float64)counter.QuadPart / (float64)frequency.QuadPart);
    
}


LocalTime getLocalTime(){
    
    LocalTime result = {};
    
    SYSTEMTIME time;
    GetLocalTime(&time);
    
    result.year = time.wYear;
    result.month = time.wMonth;
    result.day = time.wDay;
    
    result.hour = time.wHour;
    result.minute = time.wMinute;
    result.second = time.wSecond;
    
    return result;
}

bool setLocalTime(const LocalTime * source){
    SYSTEMTIME time = {};
    
    time.wYear = source->year;
    time.wMonth = source->month;
    time.wDay = source->day;
    
    time.wHour = source->hour;
    time.wMinute = source->minute ;
    time.wSecond = source->second;
    
    return SetLocalTime(&time) > 0;
}

        #endif