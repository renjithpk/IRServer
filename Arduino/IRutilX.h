#ifndef _irutil_h_
#define _irutil_h_
#include <IRrecv.h>
#include <IRremoteESP8266.h>
#include <IRac.h>
#include <IRutils.h>
#include "logger.h"
//#include "base64x.h"
#include "base64new.h"
//#include "cdecode.h"

#define PART_LENGTH 30 //EVEN number
#define MAX_BUFF_LENGTH (PART_LENGTH*20) // 300

class IRDecodeBuff
{
public:
    uint16_t buffer[MAX_BUFF_LENGTH/PART_LENGTH][PART_LENGTH];
    uint16_t type;
    uint16_t count;
    uint16_t length;
    IRDecodeBuff(uint16_t aType=0):type(aType),count(0),length(0){}
    bool copyCode(uint16_t * const inbuf, uint16_t len);
    bool addPartition(uint8_t *byteTempBuff);
    bool completed();
};

class IREncodeBuff:public decode_results
{
public:
    uint16_t index;
    static uint16_t type;
    uint16_t readOffset;
    uint16_t *finalBuffer;
    // decode_results results;
public:
    IREncodeBuff();

    uint16_t getNextPartition(uint8_t *outByteBuffer);
    void dump();
    uint16_t updateRawBuff();
};

class IRSensor
{
    static IRrecv irrecv;
public:
    bool recieve(IREncodeBuff &ebuff);
    static void setup();
    std::string convertToString(const decode_results * const results);
};



#endif // !_irutil_h_