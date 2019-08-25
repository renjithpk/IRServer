#ifndef IRRECV_H_
#define IRRECV_H_
#include <stdint.h>
#include <stdio.h>
#include <string.h>


const uint16_t kStateSizeMax = 53;
// Results returned from the decoder
class decode_results {
 public:
  // decode_type_t decode_type;  // NEC, SONY, RC5, UNKNOWN
  // value, address, & command are all mutually exclusive with state.
  // i.e. They MUST NOT be used at the same time as state, so we can use a union
  // structure to save us a handful of valuable bytes of memory.
  union {
    struct {
      uint64_t value;    // Decoded value
      uint32_t address;  // Decoded device address.
      uint32_t command;  // Decoded command.
    };
    uint8_t state[kStateSizeMax];  // Multi-byte results.
  };
  uint16_t bits;              // Number of bits in decoded value
  volatile uint16_t *rawbuf;  // Raw intervals in .5 us ticks
  uint16_t rawlen;            // Number of records in rawbuf.
  bool overflow;
  bool repeat;  // Is the result a repeat code?
};


void yield(){}

class IRrecv {
 public:
  // IRrecv(const uint16_t recvpin, const uint16_t bufsize,
  //                 const uint8_t timeout,
  //                 const bool save_buffer,
  //                 const uint8_t timer_num){}
  IRrecv(int,int,int,bool){}
  void setUnknownThreshold(int){}
  
  void enableIRIn(){}
  int decode(decode_results *){
    return 1;
  }
};
char * resultToHumanReadableBasic(void *){ return "test";}


uint16_t* resultToRawArray(const decode_results * const decode) {
  uint16_t *result = new uint16_t[decode->rawlen];
  if (result != NULL) {  // The memory was allocated successfully.
    // Convert the decode data.
    uint16_t pos = 0;
    for (uint16_t i = 0; i < decode->rawlen; i++) {
      result[i] = decode->rawbuf[i];
    }
  }
  return result;
}

#endif // !irutil_mock.h