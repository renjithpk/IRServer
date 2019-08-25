#include "IRutilX.h"

#define K_RECEIVE_PIN 14
#define K_CAPTURE_BUFFER_SIZE 1024
#define K_TIMEOUT 50

// // Use turn on the save buffer feature for more complete capture coverage.
// IRrecv irrecv(K_RECEIVE_PIN, K_CAPTURE_BUFFER_SIZE, K_TIMEOUT, true);
// decode_results results;  // Somewhere to store the results
IRrecv IRSensor::irrecv(K_RECEIVE_PIN, K_CAPTURE_BUFFER_SIZE, K_TIMEOUT, true);

uint16_t IREncodeBuff::type = 0;

void IRSensor::setup() {
    // Ignore messages with less than minimum on or off pulses.
  irrecv.setUnknownThreshold(12);
  irrecv.enableIRIn();  // Start the receiver
}


bool IRSensor::recieve(IREncodeBuff &results) {
  log_debug(".");
  if (irrecv.decode(&results)) {
    printf("received ir code, length:  %d", results.rawlen);
    yield();
    // Check if we got an IR message tha was to big for our capture buffer.
    if (results.overflow)
      log_info(
          "WARNING: IR code is too big for buffer (>= %d). "
          "This result shouldn't be trusted until this is resolved. "
          "Edit & increase K_CAPTURE_BUFFER_SIZE.\n"/*K_CAPTURE_BUFFER_SIZE*/);
    // Display the basic output of what we found.
    // log_debug(resultToHumanReadableBasic(&results));
    yield();
    return true;
  }
  return false;
}

// String IRSensor::convertToString(const decode_results * const results) {
//   String output = "";
  
//   // Reserve some space for the string to reduce heap fragmentation.
//   output.reserve(1536);  // 1.5KB should cover most cases.
//   // array size
//   output += F("[");  // Start declaration

//   // Dump data
//   for (uint16_t i = 1; i < results->rawlen; i++) {
//     uint32_t usecs;
//     for (usecs = results->rawbuf[i] * kRawTick; usecs > UINT16_MAX;
//          usecs -= UINT16_MAX) {
//       output += uint64ToString(UINT16_MAX);
//       if (i % 2)
//         output += F(", 0,  ");
//       else
//         output += F(",  0, ");
//     }
//     output += uint64ToString(usecs, 10);
//     if (i < results->rawlen - 1)
//       output += F(", ");            // ',' not needed on the last one
//     if (i % 2 == 0) output += ' ';  // Extra if it was even.
//   }

//   // End declaration
//   output += F("]");
//   return output;
// }




bool IRDecodeBuff::completed(){
  if(type && length && (length == count)){
    return true;
  }
  else{
    return false;
  } 
}

bool IRDecodeBuff::copyCode(uint16_t * const inbuf, uint16_t len){
  if(len > PART_LENGTH + 3) {
    
    //Serial.printf("length %u exceeds partition length %u\n", len, PART_LENGTH);
    log_info("length exceeds partition length\n");

    return false;
  }
  if(!type) {
    type = inbuf[0];
  }
  else if (inbuf[1] > MAX_BUFF_LENGTH/PART_LENGTH)
  {
    log_info("index of input mesage exceeded partition list size");
    return false;
  }else if (type != inbuf[0])
  {
    log_info("New type mesage, resetting counter");
    count = 0;
    type = inbuf[0];
  }
  //Serial.printf("Index %u first value %u, len %u\n",inbuf[1],inbuf[2], len-2);

  uint16_t index = inbuf[1];
  uint16_t *spos = NULL;
  if (index == 0) {
    length = inbuf[2];
    spos = &inbuf[3];
    count += len-3;
  }
  else {
    spos = &inbuf[2];
    count += len-2;
  }

  // length = 
  // memcpy(buffer[index], &inbuf[2], (len-2)*2);
  memcpy(buffer[index], spos, (len-2)*2);

  return true;
}

bool IRDecodeBuff::addPartition(uint8_t *encodedByteBuf) {
  uint16_t decodedRawData[100];
  log_info("Adding partition: ");
  //uint16_t outByteLength = decode_base64(encodedByteBuf, (uint8_t *) decodedRawData);

  uint16_t outByteLength = b64_decode((char *)encodedByteBuf, (char *) decodedRawData, strlen((char *)encodedByteBuf));
  if(!outByteLength){
    return false;
  }


  uint16_t outLength = outByteLength/2;
  // log_debug("\n decoded array : ");
  // for(int i =0; i< outLength; i++) {
  //   log_debugf("%d, ", decodedRawData[i]);
  // }
  // log_debug("\n");
  copyCode(decodedRawData, outLength);
  // for(int i =0; i< outLength; i++) {
  //   log_infof("%u, ", decodedRawData[i]);
  // }
  log_info("\n");
  return true;
}


IREncodeBuff::IREncodeBuff(){
    type++;
    index = 0;
    readOffset = 0;
    finalBuffer = NULL;
}

void IREncodeBuff::dump(){
  if(!finalBuffer){
    log_info("ERROR: finalBuffer");
    return;
  }
  log_info("\n");
  for(uint16_t i = 0 ; i < rawlen; i++) {
    printf("%d  ", finalBuffer[i]);
  }
  log_info("\n");
}

uint16_t IREncodeBuff::updateRawBuff() {
    if(finalBuffer){
      log_debug("freeing finalBuffer");
      delete finalBuffer;
      finalBuffer = NULL;
    }
    log_debug("get raw array");
    if(rawlen < 5){
      log_debug("ERROR: rawlen less than 5, doesn't make sense");
      return 0;
    }
    finalBuffer = resultToRawArray(this);
    if(!finalBuffer){
      log_debug("ERROR: finalBuffer null ");
      return 0;
    }
    return rawlen;
}

uint16_t IREncodeBuff::getNextPartition(uint8_t *outByteBuffer) {
  
  uint16_t remaining = rawlen - readOffset;
  if(!remaining) {
    log_info("Reached end of partition\n");
    return 0;
  }
  if(!finalBuffer) {
    log_info("updateRawBuff before getNextPartition");
    return 0;
  }

  uint16_t  inBuffer[PART_LENGTH + 3];
  // [ type, index, length, data....]
  inBuffer[0] = type;
  inBuffer[1] = index;
  uint8_t  startPos = 2;
  uint8_t headerSize = 2;
  if (index == 0 ){
    inBuffer[2] = rawlen;
    startPos = 3;
    headerSize = 3;
  }

  uint16_t partLength = remaining > PART_LENGTH? PART_LENGTH : remaining;
  //        dest          src           len
  // memcpy(&inBuffer[2], (uint16_t * )&rawbuf[readOffset],  partLength*2);
  memcpy((char *)&inBuffer[startPos], (char * )&finalBuffer[readOffset],  partLength*2); //TODO
  readOffset += partLength;
  index ++;


  
  uint16_t outByteLength = b64_encode((char *)inBuffer, (char *)outByteBuffer,  (partLength + headerSize) * 2);

  //Serial.printf("\n%u  - %s  -  total/in/out %u/%u/%u, offset %u\n",  index, outByteBuffer, rawlen,  partLength * 2) + 4, outByteLength, readOffset);
  printf("\n%u  - %s  -  bytes total/code/in/out %d/%d/%f/%d, offset %u\n",  index, outByteBuffer, rawlen, partLength,  (partLength + headerSize), outByteLength/2, readOffset);
  if(!outByteLength){
    printf("Returned 0 length from base64 encoder");
    return false;
  }
  return outByteLength;
}


