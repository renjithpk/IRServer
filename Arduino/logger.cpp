#include "logger.h"

void log_debug(char * message) {
  if(DEBUG){
    Serial.println(message);
  }
}
void log_info(char * message) {
  Serial.println(message);
}
void log_debug(String message) {
  if(DEBUG){
    Serial.println(message);
  }
}
void log_info(String message) {
  Serial.println(message);
}