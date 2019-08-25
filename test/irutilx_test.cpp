#define IRAC_H_
#define IRUTILS_H_
#define Arduino_h
#include "IRrecv_mock.h"
#include "logger_mock.h"

//#include "../irserver/base64x.cpp"
#include "../irserver/IRutilX.cpp"
#include "../irserver/base64new.cpp"
// #include "../irserver/cencode.c"
//#include "../irserver/base64x.h" 

bool DEBUG = false;
char test_ip_data_byte[] =  {0,1, 2, 3, 4, 5, 6, 7, 8, 9,
                            10,11,12,13,14,15,16,17,18,19,
                            20,21,22,23,24,25,26,27,28,29,
                            30,31,32,33,34,35,36,37,38,39,
                            40,41,42,43,44,45,46,47,48,49,
                            50,51,52,53,54,55,56,57,58,59};
uint16_t test_ip_data1[] = {0,1,2,3,4,5,6,7,8,9,
                            10,11,12,13,14,15,16,17,18,19,
                            20,21,22,23,24};

uint16_t test_ip_data2[] = {123, 3534, 121, 454,  576};

uint16_t test_ip_data3[] = {100,101,102,103,104,105,106,107,108,109,
                            200,201,202,203,204,205,206,207,208,209,
                            300,301,302,303,304,305,306,307,308,309,
                            400,401};



#include <gtest/gtest.h>
bool compareArray(uint16_t * act, uint16_t * exp, uint16_t len){
  for(int i =0; i< len; i++) {
    if(exp[i] != act[i]){
      printf("FAILED: Array not same at index %d\n", i);
      printf("Actual: \n");
      for(int i =0; i< len; i++) {
        printf("%d,",act[i]);
      }
      printf("\nExpected: \n");
      for(int i =0; i< len; i++) {
        printf("%d,",exp[i]);
      }
      exit(1);
    }
  }
  printf("\nPASSED\n");
  printf("Actual: \n");
  uint16_t *act16 = (uint16_t*)act;
  uint16_t *exp16 = (uint16_t*)exp;
  // for(int i =0; i< len/2; i++) {
  //   printf("%d,",act16[i]);
  // }
  // printf("\nExpected: \n");
  // for(int i =0; i< len/2; i++) {
  //   printf("%d,",exp16[i]);
  // }
  return true;
} 

bool compareArray(char * act, char * exp, uint16_t len){
  for(int i =0; i< len; i++) {
    if(exp[i] != act[i]){
      printf("FAILED: Array not same at index %d\n", i);
      printf("Actual: \n");
      for(int i =0; i< len; i++) {
        printf("%d,",act[i]);
      }
      printf("\nExpected: \n");
      for(int i =0; i< len; i++) {
        printf("%d,",exp[i]);
      }
      exit(1);
    }
  }
  printf("\nPASSED\n");
  
  //printf("Actual: \n");
  // for(int i =0; i< len; i++) {
  //   printf("%d,",act[i]);
  // }
  // printf("\nExpected: \n");
  // for(int i =0; i< len; i++) {
  //   printf("%d,",exp[i]);
  // }
  return true;
} 



int test_1(char * testdata, int testdatalen){

  char buf[300];
  char result[50];
  int  len_e = b64_encode((char* )test_ip_data_byte, buf, testdatalen);
  int len_d = b64_decode(buf, result, len_e);
  return compareArray( result,test_ip_data_byte,  len_d);
}

void printArray(uint16_t * array, int len){
  printf("\n");
  for(int i = 0 ; i < len; i++) {
    printf("%d  ", array[i]);
  }
  printf("\n");
}

int test_2(uint16_t * testdata, int testdatalen) { 

  printf("\nStaring %d\n\n\n", testdatalen);
  IREncodeBuff irencodebuff;
  irencodebuff.rawbuf = testdata;
  irencodebuff.rawlen = testdatalen;
  irencodebuff.updateRawBuff();
  uint8_t byteTempBuff[10][PART_LENGTH*4]={0};
  int i =0;
  for(int len = 1 ; len > 0; i++) {
    len = irencodebuff.getNextPartition(byteTempBuff[i]);
    printf((char *)byteTempBuff[i]);
    printf("\n");
  }



  //////// Decoding 
  uint16_t decodedRawData[75];
  IRDecodeBuff irbuff;
  if(irbuff.completed()){
    printf("FAILED, should not be completed 1");
    exit(1);
  }

  i = 0;
  while (!irbuff.completed())
  {
    irbuff.addPartition(byteTempBuff[i++]);
    if(i > (MAX_BUFF_LENGTH/PART_LENGTH)) {
      printf("FAILED, should be completed, something wrong");
      exit(1);
    }
  }
  
  // TEST verification 1 
  printf(" CASE 1 : check resulted length :");
  if(testdatalen == irbuff.count){ 
    printf("PASSED\n");
  }
  else{
    printf("FAILED, legth of decoded string not matching with original %d != %d", testdatalen, irbuff.count);
    exit(1);
  }
  // TEST verification 2
  printf(" CASE 1 : check result is matching with actual : ");
  compareArray(testdata,(uint16_t *)irbuff.buffer, testdatalen);


  return 1;
}

int test_3(){
  IRDecodeBuff irbuff;
  irbuff.addPartition((uint8_t*)"DgABACUCwAC4AL0ALgK2ACsCwQAoAo8A5QA8Aq8AvwA=");
  irbuff.addPartition((uint8_t*)"DgAAACwAAQD4AmUBwAAqArwAMAKwADQCkADlAI4AXAK3ACoCwQArAq8AxAA0ArYAugAoLPACbgGuAD0CvwAiAsIA");
  if (!irbuff.completed()){
    printf("FAILED: test_3");
    return 0;
  }
  printf("SUCCESS: test_3");
  printArray((uint16_t *)irbuff.buffer,irbuff.length);

  irbuff.addPartition((uint8_t*)"CwABACYCxgCtAL4ALAK9AC8CswAuAscArgA3Ar4AswA=\n");
  irbuff.addPartition((uint8_t*)"CwAAACwAAQD0AmoBwQAlAsYAIwK9AC8CvACzAMEAJgK9ACwCvwAtApkA2gAwArcAvQAjLO0CcgHGACQCugAyArwA\n");
  if(!irbuff.completed()){
    printf("FAILED: test_3");
    return 0;   
  }
  printf("SUCCESS: test_3");
  printArray((uint16_t *)irbuff.buffer,irbuff.length);
  return 1;
}

int main(int argc, char **argv) {
    if(! test_1((char *)test_ip_data_byte, sizeof(test_ip_data_byte)*sizeof(test_ip_data_byte[0]))) return 1;
    if(! test_1((char *)test_ip_data3, sizeof(test_ip_data3)*sizeof(test_ip_data3[0]))) return 1;
    //if(test_2(test_ip_data1, sizeof(test_ip_data1)/sizeof(test_ip_data1[0]))) return 1;
    // if(test1(test_ip_data2, sizeof(test_ip_data2)/sizeof(test_ip_data1[0])) ) return 1;
    if(!test_2(test_ip_data3, sizeof(test_ip_data3)/sizeof(test_ip_data1[0]))) return 1;
    if(!test_3()) return 1;
}