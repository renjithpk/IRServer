#ifndef _logger_h_
#define _logger_h_
#include <iostream>
#include <stdarg.h>

extern bool DEBUG;

void log_debug(char * message){ if(DEBUG) printf(message);}
void log_info(char * message){ printf(message);}
void log_debug(std::string message){ if(DEBUG) printf(message.c_str());}
void log_info(std::string message){ printf(message.c_str());}
void log_debugf(const char* format, ...)
{
    if(DEBUG){
        va_list argptr;
        va_start(argptr, format);
        vprintf(format, argptr);
        va_end(argptr);
    }
}
void log_infof(const char* format, ...)
{
    va_list argptr;
    va_start(argptr, format);
    vprintf(format, argptr);
    va_end(argptr);
}
#endif // !_logger_h_