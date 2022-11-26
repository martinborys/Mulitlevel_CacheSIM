#include "YOURCODEHERE.h"

/**********************************************************************
    Function    : lg2pow2
    Description : this help funciton for you to calculate the bit number
                  this function is not allowed to modify
    Input       : pow2 - for example, pow2 is 16
    Output      : retval - in this example, retval is 4
***********************************************************************/
unsigned int lg2pow2(uint64_t pow2){
  unsigned int retval=0;
  while(pow2 != 1 && retval < 64){
    pow2 = pow2 >> 1;
    ++retval;
  }
  return retval;
}

void  setSizesOffsetsAndMaskFields(cache* acache, unsigned int size, unsigned int assoc, unsigned int blocksize){
  unsigned int localVAbits=8*sizeof(uint64_t*);
  if (localVAbits!=64){
    fprintf(stderr,"Running non-portable code on unsupported platform, terminating. Please use designated machines.\n");
    exit(-1);
  }

  // YOUR CODE GOES HERE

}


unsigned long long getindex(cache* acache, unsigned long long address){
  return 0; //Replace with correct value
}

unsigned long long gettag(cache* acache, unsigned long long address){
  return 0; //Replace with correct value
}

void writeback(cache* acache, unsigned int index, unsigned int oldestway){

  // YOUR CODE GOES HERE

}

void fill(cache * acache, unsigned int index, unsigned int oldestway, unsigned long long address){

  // YOUR CODE GOES HERE

}
