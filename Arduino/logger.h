#ifndef _logger_h_
#define _logger_h_
#include <Arduino.h>

extern bool DEBUG;
void log_debug(char * message);
void log_info(char * message);
void log_debug(String message);
void log_info(String message);

#endif // !_logger_h_