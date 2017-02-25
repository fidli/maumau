#ifndef UTIL_STRING
#define UTIL_STRING

#include <stdarg.h>
#include "common.h"
#include "util_math.cpp"

int32 strcmp(const char * a, const char * b){
    int32 result = 0;
    for(uint32 index = 0; ;index++){
        
        result = a[index] - b[index];
        if(result){
            return result;
        }
        
        if(a[index] == '\0')
            break;
    } 
    
    return result;
}

char * strcpy(char * target, const char * source){
    uint32 i = 0;
    do{
        target[i] = source[i];
        i++;
    }while(source[i-1] != '\0');
    return target;
}

uint32 strlen(const char * source){
    uint32 length = 0;
    while(source[length] != '\0'){
        length++;
    }
    return length;
}

char * strcat(const char * first, const char * second){
    char * result = &PUSHA(char, strlen(first) + strlen(second) + 1);
    uint32 index = 0;
    while(first[index] != '\0'){
        result[index] = first[index];
        index++;
    }
    uint32 index2 = 0;
    while(second[index2] != '\0'){
        result[index + index2] = second[index2];
        index2++;
    }
    result[index + index2] = '\0';
    return result;
}



//todo: overflow control + return 0 on overflow
static uint8 scanNumber16(const char * source, int16 * target, uint8 maxDigits = 5){
    ASSERT(maxDigits <= 5);
    uint8 i = 0;
    bool first = true;
    bool negative = false;
    if(source[i] == '-'){
        negative = true;
        i++;
    }
    for(;source[i] != '\0' && i < maxDigits; i++){
        int8 digit = (int8) source[i];
        if(digit < 48 || digit > 57) break;
        digit -= 48;
        if(first){
            first = false;
            *target = 0;
        }
        *target = 10 * *target + digit; 
    }
    if(negative && i == 1){//only -
        return 0; 
    }
    if(negative){
        *target = -1 * *target;
    }
    return i;
}

static uint8 scanUnumber16(const char * source, uint16 * target, uint8 maxDigits = 5){
    ASSERT(maxDigits <= 5)
        uint8 i = 0;
    bool first = true;
    for(;source[i] != '\0' && i < maxDigits; i++){
        int8 digit = (int8) source[i];
        if(digit < 48 || digit > 57) break;
        digit -= 48;
        if(first){
            first = false;
            *target = 0;
        }
        *target = 10 * *target + digit; 
    }
    return i;
}


//todo: overflow control + return 0 on overflow
static uint8 scanNumber(const char * source, int32 * target, uint8 maxDigits = 10){
    ASSERT(maxDigits <= 10);
    uint8 i = 0;
    bool first = true;
    bool negative = false;
    if(source[i] == '-'){
        negative = true;
        i++;
    }
    for(;source[i] != '\0' && i < maxDigits; i++){
        int8 digit = (int8) source[i];
        
        if(digit < 48 || digit > 57) break;
        digit -= 48;
        if(first){
            first = false;
            *target = 0;
        }
        *target = 10 * *target + digit; 
    }
    if(negative && i == 1){//only -
        return 0; 
    }
    if(negative){
        *target = -1 * *target;
    }
    return i;
}

static uint8 scanUnumber(const char * source, uint32 * target, uint8 maxDigits = 10){
    ASSERT(maxDigits <= 10);
    uint8 i = 0;
    bool first = true;
    for(;source[i] != '\0' && i < maxDigits; i++){
        int8 digit = (int8) source[i];
        if(digit < 48 || digit > 57) break;
        digit -= 48;
        if(first){
            first = false;
            *target = 0;
        }
        *target = 10 * *target + digit; 
    }
    return i;
}


static uint8 printDigits(char * target, int64 number){
    char digitsStack[20];
    int8 stackSize = 0;
    int64 temp = ABS(number);
    do{
        digitsStack[stackSize] = char((temp % 10) + 48);
        temp /= 10;
        stackSize++;
    } while(temp != 0);
    stackSize--;
    
    uint8 i = 0;
    if(number < 0){
        target[i++] = '-';
    }
    for(;stackSize >= 0; stackSize--){
        target[i++] = digitsStack[stackSize];
    }
    return i;
}

static enum FormatTypeSize{
    FormatTypeSize_Default,
    FormatTypeSize_h
};

static enum FormatType{
    FormatType_Invalid,
    FormatType_d,
    FormatType_u,
    FormatType_c,
    FormatType_charlist
};

static struct FormatInfo{
    bool dryRun;
    uint32 maxlen;
    FormatType type;
    FormatTypeSize typeLength;
    union{
        struct{
            bool inverted;
            char digitRangeLow;
            char digitRangeHigh;
            char smallLetterRangeLow;
            char smallLetterRangeHigh;
            char capitalLetterRangeLow;
            char capitalLetterRangeHigh;
            const char * charlist[4];
            uint8 charlistLengths[4];
            uint8 charlistCount;
        } charlist;
    };
};


uint32 printFormatted(char * target, const char * format, va_list ap){
    uint32 formatIndex = 0;
    uint32 targetIndex = 0;
    uint32 successfullyPrinted = 0;
    while(format[formatIndex] != '\0'){
        if(format[formatIndex] == '%'){
            FormatInfo targetFormat = {};
            formatIndex++;
            
            if(format[formatIndex] == 'd' || (format[formatIndex] == '0')){
                int32 source = va_arg(ap, int32);
                
                if(format[formatIndex] == '0'){
                    formatIndex++;
                    uint32 width = 0;
                    formatIndex += scanUnumber(&format[formatIndex], &width);
                    ASSERT(width > 0);
                    for(int i = 0; i < width - numlen(source); i++){
                        target[targetIndex++] = '0';
                    }
                }
                ASSERT(format[formatIndex] == 'd');
                formatIndex++;
                targetIndex += printDigits(target + targetIndex, (int64)source);
                successfullyPrinted++;
            }else if(format[formatIndex] == '.' || format[formatIndex] == ','){
                char delim = format[formatIndex];
                formatIndex++;
                
                int32 precision = 6;
                uint8 scanOffset = scanNumber(format + formatIndex, &precision);
                ASSERT(scanOffset > 0);               
                formatIndex++;
                
                if(format[formatIndex] == 'f'){
                    formatIndex++;
                    float64 source = va_arg(ap, float64);
                    int64 wholePart = (int64) source;
                    targetIndex += printDigits(target + targetIndex, wholePart);
                    target[targetIndex] = delim;
                    targetIndex++;
                    uint64 decimalPart = (source - wholePart) * powd(10, precision);
                    uint8 prependLen = precision - numlen(decimalPart);
                    for(int i = 0; i < prependLen; i++){
                        target[targetIndex] = '0';
                        targetIndex++;
                    }
                    targetIndex += printDigits(target + targetIndex, decimalPart);
                }
                else{
                    ASSERT(!"fuck");
                }
                
                successfullyPrinted++;
            }else if(format[formatIndex] == 'c'){
                formatIndex++;
                char source = va_arg(ap, char);
                target[targetIndex++] = source;
                successfullyPrinted++;
            }
            else{
                ASSERT(!"fuck");
            }
        }else{
            target[targetIndex] = format[formatIndex];
            formatIndex++;
            targetIndex++;
        }
        
    };
    target[targetIndex] = '\0';
    return successfullyPrinted;
}


static inline bool isDigit19(const char digit){
    return digit >= '1' && digit <= '9';
}
/*
 inline bool isInCharlist(const char source, const char * test, uint16 sourceSize, uint16 testSize){
    for(uint16 i = 0; i < sourceSize; i++){
        if(source == test[i]){
            return true;
        }
    }
    return false;
}
*/


uint32 scanFormatted(const char * source, const char * format, va_list ap){
    uint32 formatIndex = 0;
    uint32 sourceIndex = 0;
    uint32 successfullyScanned = 0;
    uint32 maxread = (uint32) -1;
    while(format[formatIndex] != '\0' && sourceIndex < maxread && source[sourceIndex] != '\0'){
        if(format[formatIndex] == '%'){
            formatIndex++;
            if(formatIndex == 1 && isDigit19(format[formatIndex])){
                formatIndex += scanUnumber(&format[formatIndex], &maxread);
            }
            else{
                
                FormatInfo info = {};
                info.dryRun = false;
                info.maxlen = 0;
                info.typeLength = FormatTypeSize_Default;
                if(format[formatIndex] == '*'){
                    info.dryRun = true;
                    ASSERT(false); //implement
                    formatIndex++;
                }
                if(isDigit19(format[formatIndex])){
                    formatIndex+= scanUnumber(&format[formatIndex], &info.maxlen);
                }
                if(format[formatIndex] == 'h'){
                    info.typeLength = FormatTypeSize_h;
                    formatIndex++;
                }
                //type
                if(format[formatIndex] == 'd'){
                    info.type = FormatType_d;
                    formatIndex++;
                }else if(format[formatIndex] == 'u'){
                    info.type = FormatType_u;
                    formatIndex++;
                }else if(format[formatIndex] == 'c'){
                    info.type = FormatType_c;
                    formatIndex++;
                }else if(format[formatIndex] == '['){
                    info.type = FormatType_charlist;
                    formatIndex++;
                    info.charlist.inverted = false;
                    if(format[formatIndex] == '^'){
                        info.charlist.inverted = true;
                        formatIndex++;
                    }
                    uint8 charlistLen = 0;
                    for(;format[formatIndex] != ']'; formatIndex++){
                        ASSERT(format[formatIndex] != '\0');
                        if(format[formatIndex+1] == '-'){
                            
                            if(charlistLen > 0){
                                info.charlist.charlistLengths[info.charlist.charlistCount] = charlistLen;
                                charlistLen = 0;
                            }
                            
                            if(format[formatIndex] >= 'A' && format[formatIndex] <= 'Z'){
                                info.charlist.capitalLetterRangeLow = format[formatIndex];                            
                            }else if(format[formatIndex] >= 'a' && format[formatIndex] <= 'z'){
                                info.charlist.smallLetterRangeLow = format[formatIndex];                            
                            }else if(format[formatIndex] >= 'a' && format[formatIndex] <= 'z'){
                                info.charlist.digitRangeLow = format[formatIndex];
                            }else{
                                ASSERT(!"fuck");
                            }
                            
                            formatIndex += 2;
                            
                            if(format[formatIndex] >= 'A' && format[formatIndex] <= 'Z'){
                                info.charlist.capitalLetterRangeHigh = format[formatIndex];                            
                                ASSERT(info.charlist.capitalLetterRangeHigh > info.charlist.capitalLetterRangeLow);
                            }else if(format[formatIndex] >= 'a' && format[formatIndex] <= 'z'){
                                info.charlist.smallLetterRangeHigh = format[formatIndex];
                                ASSERT(info.charlist.capitalLetterRangeHigh > info.charlist.capitalLetterRangeLow);
                            }else if(format[formatIndex] >= 'a' && format[formatIndex] <= 'z'){
                                info.charlist.digitRangeHigh = format[formatIndex];
                                ASSERT(info.charlist.capitalLetterRangeHigh > info.charlist.capitalLetterRangeLow);
                            }else{
                                ASSERT(!"fuck");
                            }
                            
                        }else{
                            
                            if(charlistLen == 0){
                                info.charlist.charlistCount++;
                                info.charlist.charlist[info.charlist.charlistCount-1] = &format[formatIndex];
                            }
                            info.charlist.charlistLengths[info.charlist.charlistCount-1]++;
                            charlistLen++;
                            
                        }
                    }
                    formatIndex++;//jumping over ']'
                    
                    
                }
                
                
                
                uint32 scannedChars = 0;
                
                switch(info.type){
                    case FormatType_d:
                    case FormatType_u:{
                        if(info.type == FormatType_u){
                            if(info.typeLength == FormatTypeSize_Default){
                                uint32 * targetVar = va_arg(ap, uint32 * );
                                uint8 maxDigits = 10;
                                if(info.maxlen != 0){
                                    maxDigits = info.maxlen;
                                }
                                scannedChars = scanUnumber(source + sourceIndex, (uint32 *) targetVar, maxDigits);
                            }else if (info.typeLength == FormatTypeSize_h){
                                uint16 * targetVar = va_arg(ap, uint16 * );
                                uint8 maxDigits = 5;
                                if(info.maxlen != 0){
                                    maxDigits = info.maxlen;
                                }
                                scannedChars = scanUnumber16(source + sourceIndex, (uint16 *) targetVar, maxDigits);
                            }else{
                                ASSERT(!"fuk");
                            }
                        }else if(info.type == FormatType_d){
                            if(info.typeLength == FormatTypeSize_Default){
                                int32 * targetVar = va_arg(ap, int32 * );
                                uint8 maxDigits = 10;
                                if(info.maxlen != 0){
                                    maxDigits = info.maxlen;
                                }
                                scannedChars = scanNumber(source + sourceIndex, (int32 *) targetVar, maxDigits);
                            }else if (info.typeLength == FormatTypeSize_h){
                                int16 * targetVar = va_arg(ap, int16 * );
                                uint8 maxDigits = 5;
                                if(info.maxlen != 0){
                                    maxDigits = info.maxlen;
                                }
                                scannedChars = scanNumber16(source + sourceIndex, (int16 *) targetVar, maxDigits);
                            }else{
                                ASSERT(!"fuk");
                            }
                        }
                        
                        if(scannedChars > 0){
                            successfullyScanned++;
                            sourceIndex += scannedChars;
                        }
                        
                    }break;
                    case FormatType_charlist:{
                        char * targetVar = va_arg(ap, char *);
                        bool first = true;
                        uint32 i = 0;
                        for(; sourceIndex < maxread && i < info.maxlen; sourceIndex++, i++){
                            if(first) first = false;
                            if(info.charlist.digitRangeLow != '\0'){
                                if(source[sourceIndex] >= info.charlist.digitRangeLow && source[sourceIndex] <= info.charlist.digitRangeHigh && !info.charlist.inverted){
                                    targetVar[i] = source[sourceIndex];
                                    continue;               
                                }else if((source[sourceIndex] < info.charlist.digitRangeLow && source[sourceIndex] > info.charlist.digitRangeHigh) && info.charlist.inverted){
                                    targetVar[i] = source[sourceIndex];
                                    continue;                            
                                }
                            }
                            if(info.charlist.capitalLetterRangeLow != '\0'){
                                if(source[sourceIndex] >= info.charlist.capitalLetterRangeLow && source[sourceIndex] <= info.charlist.capitalLetterRangeHigh && !info.charlist.inverted){
                                    targetVar[i] = source[sourceIndex];
                                    continue;               
                                }else if((source[sourceIndex] < info.charlist.capitalLetterRangeLow && source[sourceIndex] > info.charlist.capitalLetterRangeHigh) && info.charlist.inverted){
                                    targetVar[i] = source[sourceIndex];
                                    continue;                            
                                }
                            }
                            if(info.charlist.smallLetterRangeLow != '\0'){
                                if(source[sourceIndex] >= info.charlist.smallLetterRangeLow && source[sourceIndex] <= info.charlist.smallLetterRangeHigh && !info.charlist.inverted){
                                    targetVar[i] = source[sourceIndex];
                                    continue;               
                                }else if((source[sourceIndex] < info.charlist.smallLetterRangeLow || source[sourceIndex] > info.charlist.smallLetterRangeHigh) && info.charlist.inverted){
                                    targetVar[i] = source[sourceIndex];
                                    continue;                            
                                }                        
                            }
                            
                            bool found = false;
                            for(int charlistIndex = 0; charlistIndex < info.charlist.charlistCount; charlistIndex++){
                                for(int charIndex = 0; charIndex < info.charlist.charlistLengths[charlistIndex]; charIndex++){
                                    if(source[sourceIndex] == info.charlist.charlist[charlistIndex][charIndex] && !info.charlist.inverted){
                                        targetVar[i] = source[sourceIndex];
                                        found = true;
                                        break;
                                    }else if(source[sourceIndex] != info.charlist.charlist[charlistIndex][charIndex] && info.charlist.inverted){
                                        targetVar[i] = source[sourceIndex];
                                        found = true;
                                        break;
                                    }
                                    if(!found){
                                        break;    
                                    }
                                }
                                if(!found){
                                    break;    
                                }
                            }
                            if(!found){
                                i++;
                                break;    
                            }
                            
                        }
                        if(!first)
                            successfullyScanned++;
                        targetVar[i-1] = '\0';
                        
                    }break;
                    default:{
                        ASSERT(!"fuck");
                    }break;
                }
                
                
            }
        }else if(format[formatIndex] == source[sourceIndex]){
            //todo: whitespaces eating
            formatIndex++;
            sourceIndex++;
        }else{
            return 0; //not hardcoded string match
            //break;
        }
        
    }
    
    
    return successfullyScanned;
}


uint32 sprintf(char * target, const char * format, ...){
    va_list ap;    
    va_start(ap, format);
    uint32 successfullyPrinted = printFormatted(target, format, ap);
    va_end(ap);
    return successfullyPrinted;
}

uint32 sscanf(const char * target, const char * format, ...){
    va_list ap;    
    va_start(ap, format);
    uint32 successfullyScanned  = scanFormatted(target, format, ap);
    va_end(ap);
    return successfullyScanned;
}



#endif