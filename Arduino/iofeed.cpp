#include "iofeed.h"
#include "logger.h"
#include "constant.h"
/************************* Adafruit.io Setup *********************************/
extern bool DEBUG;
WiFiClient client;
// Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, IO_USERNAME, IO_USERNAME, IO_KEY);
IOFeed mqtt(&client);
Adafruit_MQTT_Subscribe config_fd = Adafruit_MQTT_Subscribe(&mqtt, IO_USERNAME "/feeds/irsever.config", MQTT_QOS_1);
Adafruit_MQTT_Subscribe code_fd = Adafruit_MQTT_Subscribe(&mqtt, IO_USERNAME "/feeds/irsever.code", MQTT_QOS_1);
Adafruit_MQTT_Publish decoded_fd = Adafruit_MQTT_Publish(&mqtt, IO_USERNAME "/feeds/irsever.decoded", MQTT_QOS_1);



void IOFeed::connect_chk_m() {
    int8_t ret;
    // Stop if already connected.
    if (connected()) {
        return;
    }

    log_info("Connecting to MQTT... ");

    uint8_t retries = 3;
    while ((ret = connect()) != 0) { // connect will return 0 for connected
        log_info(connectErrorString(ret));
        log_info("Retrying MQTT connection in 10 seconds...");
        disconnect();
        delay(10000);  // wait 10 seconds
        retries--;
        if (retries == 0) {
            // basically die and wait for WDT to reset me
            while (1);
        }
    }
    log_info("MQTT Connected!");
}

void IOFeed::enable_config_feed(SubscribeCallbackBufferType cb) {
    //Adafruit_MQTT_Subscribe slider = Adafruit_MQTT_Subscribe(&mqtt, IO_USERNAME "/feeds/irsever.config", MQTT_QOS_1);
    config_fd.setCallback(cb);
    ::mqtt.subscribe(&config_fd);
}

void IOFeed::enable_ircode_feed(SubscribeCallbackBufferType cb) {
    //Adafruit_MQTT_Subscribe slider = Adafruit_MQTT_Subscribe(&mqtt, IO_USERNAME "/feeds/irsever.config", MQTT_QOS_1);
    code_fd.setCallback(cb);
    ::mqtt.subscribe(&code_fd);
}

void IOFeed::connection_check(){
    mqtt.connect_chk_m();
}

void IOFeed::listen_packet(int16_t timeout){
    mqtt.processPackets(timeout);
}

void IOFeed::send_packets(uint8_t * code, int16_t length){
    // const int16_t maxlen = 100;
    // for (size_t i = 0; i < length; i=i+maxlen)
    // {
    //     int16_t send_len = (i+maxlen) < length? maxlen : (length - i);
    //     if(DEBUG) Serial.printf("sending %d bytes : %s", send_len, &(code[i]) );
    //     decoded_fd.publish(&(code[i]), send_len);   
    // }
    decoded_fd.publish(code, length); 
}

void IOFeed::heartbeat(){
    if(! mqtt.ping()) {
        mqtt.disconnect();
    }
}

/*
#include <Arduino.h>
#include "base64.hpp"


const char input[] = "[true, 42, \"apple\"]";

void setup() {
  Serial.begin(115200);
  while (!Serial);

uint16_t binary[] = {1480, 754,  354, 1130,  324, 1172,  380, 1090,  388, 366,  286, 1196,  350, 1146,  362, 1108,  384, 360,  1044, 456,  326};

uint16_t binary_op[100];
uint16_t length = sizeof(binary);
for (size_t i = 0; i < length*2; i++)
{
  Serial.printf(" %d ", ((unsigned char *)binary_op)[i]);
}
unsigned char base64[200];

unsigned int base64_length = encode_base64((unsigned char *)binary, length*2, base64);

Serial.printf("generate length %d, actual %d\n", base64_length, length*2); // Prints "8"
Serial.printf((char *) base64); // Prints "hfR1zrLD"

unsigned int binary_length = decode_base64(base64, (unsigned char *)binary_op);
for (size_t i = 0; i < binary_length/2; i++)
{
  Serial.printf(" %d ", binary_op[i]);
}

 */