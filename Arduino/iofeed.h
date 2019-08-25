#ifndef __iofeed_h__
#define __iofeed_h__
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "mqtt_secrets.h"
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883



class IOFeed : public Adafruit_MQTT_Client {    
    public:
    IOFeed(Client *client):Adafruit_MQTT_Client(client, AIO_SERVER, AIO_SERVERPORT, IO_USERNAME, IO_USERNAME, IO_KEY) {
    }

    void connect_chk_m();
    static void enable_config_feed(SubscribeCallbackBufferType cb);
    static void enable_ircode_feed(SubscribeCallbackBufferType cb);
    static void connection_check();
    static void listen_packet(int16_t timeout);
    static void heartbeat();
    static void send_packets(uint8_t * code, int16_t length);
};

#endif // !__iofeed_h__