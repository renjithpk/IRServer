/*

 */

#include <Arduino.h>
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "constant.h"
#include "iofeed.h"
#include "logger.h"
#include "IRutilX.h"
//#include "base64.hpp"

// unsigned int decode_base64(unsigned char input[], unsigned char output[]);
EServerState eNextState = Idle_State;
const uint32_t kBaudRate = 115200;
bool DEBUG = true;
IRDecodeBuff irDecodeBuf;

void configcallback(char *data, uint16_t len) {
  log_debug("configcallbackcallback, the config value is: ");
  log_debug(data);
  switch (data[0])
  {
  case toggle_dbg_cmd:
    DEBUG? DEBUG=false:DEBUG=true;
    DEBUG? log_info("debug enabled"):log_info("debug disabled");
    break;
  
  case ir_decode_cmd:
    log_info("chaging next state to ir_decode_State");
    irDecodeBuf.length = 0;
    eNextState = ir_decode_State;
  break;
  
  case ir_send_cmd:
    log_info("transmitting ircode");
    eNextState = Idle_State;
    for (size_t i = 0; i < irDecodeBuf.length; i++)
    {
      Serial.printf("%d ",((uint16_t *)irDecodeBuf.buffer)[i]);
    }
  break;
  
  default:
    log_info("Invalid command received...");
    break;
  }
}

void ircodecallback(char *data, uint16_t len) {
  eNextState = Idle_State;
  log_debug("ircodecallback, the config value is: ");
  log_debug(data);
  irDecodeBuf.addPartition((uint8_t *)data);
  if(irDecodeBuf.completed()) {
    log_info("Received complete data\n");
    for(int i =0; i < irDecodeBuf.length; i++) {
      Serial.printf("%d,",((uint16_t *) irDecodeBuf.buffer)[i]);
    }
  }
}

// This section of code runs only once at start-up.
void setup() {

  Serial.begin(kBaudRate, SERIAL_8N1);

  while (!Serial)  // Wait for the serial connection to be establised.
    delay(50);
  log_info("\nIRserver is now running.. ");
  WiFiManager wifiManager;
  wifiManager.autoConnect("IRServerAP");
  log_info("connected...yeey :)");

  IRSensor::setup();  
  IOFeed::enable_config_feed(configcallback);
  IOFeed::enable_ircode_feed(ircodecallback);

}

ECmd getcmd(){
  return ECmd::ir_decode_cmd;
}

void MQTT_connect();
// The repeating section of the code
void loop() {
  switch(eNextState){
    
    case Idle_State:{
      //log_debug("In Idle_State");
      IOFeed::connection_check();
      IOFeed::listen_packet(10000);
      // ECmd eCmd = getcmd();
      // if (eCmd == ECmd::ir_decode_cmd)
      // {
      //   eNextState = EServerState::ir_decode_State;
      // }
      break;
    }

    case ir_RCV_State:{

      break;
    }

    case ir_decode_State:{
      // log_info("Ready!!! Show IR command to decoder...");
      uint8_t buffer[200];
      IREncodeBuff irencodebuff;
      IRSensor sensor;
      // eNextState = Idle_State;
  
      if(sensor.recieve(irencodebuff)) {
        yield();

        log_debug("updating raw buff");
        if(!irencodebuff.updateRawBuff()) break;
        irencodebuff.dump();
        
        uint16_t length = 0;
        do {
          log_debug("getting base64 encoded partition");
          length = irencodebuff.getNextPartition(buffer);
          if(length){
            // log_debug(code);
            IOFeed::send_packets(buffer, length);
            yield();
          }
        }
        while(length);
        eNextState = Idle_State;
      }
      IOFeed::listen_packet(2000);
      yield();
    }

  }
}




