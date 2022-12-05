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
  int byteOffset = 3;
  int addressSize = 64;
  acache->numways = assoc;
  acache->blocksize = blocksize;
  acache->numsets = (size/blocksize)/assoc;
  acache->numBitsForBlockOffset = lg2pow2(blocksize) - byteOffset;
  acache->numBitsForIndex = lg2pow2((size/blocksize)/assoc);
  acache->VAImask = ((size/blocksize)/assoc)-1;

  int bits = (addressSize - acache->numBitsForBlockOffset - acache->numBitsForIndex - byteOffset);
  unsigned long long power = 1;
  for(int i=0; i<bits; i++){
    power *= 2;
  }
  power -= 1;
  acache->VATmask = power;
  //printf("%u %u %u %u %u %llx %llx\n", acache->numways, acache->blocksize, acache->numsets, acache->numBitsForBlockOffset, acache->numBitsForIndex, acache->VAImask, acache->VATmask);
}


unsigned long long getindex(cache* acache, unsigned long long address){
  int byteOffset = 3;
  unsigned long long index;
  index = (acache->VAImask  << (acache->numBitsForBlockOffset + byteOffset)) & address;
  index = index >> (acache->numBitsForBlockOffset + byteOffset);
  return index; //Replace with correct value
}

unsigned long long gettag(cache* acache, unsigned long long address){
  int byteOffset = 3;
  unsigned long long tag;
  tag = (acache->VATmask  << (acache->numBitsForIndex + acache->numBitsForBlockOffset + byteOffset))& address;
  tag = tag >> (acache->numBitsForIndex + acache->numBitsForBlockOffset + byteOffset);
  return tag; //Replace with correct value
}

void writeback(cache* acache, unsigned int index, unsigned int oldestway){

  // YOUR CODE GOES HERE
  int numBitsByteOffset = 3;
  int words = acache->blocksize/8;
  unsigned long long address;
  unsigned long long tag = (acache->sets[index].blocks[oldestway].tag) << (acache->numBitsForIndex + acache->numBitsForBlockOffset + numBitsByteOffset);
  address = tag | (index << (acache->numBitsForBlockOffset + numBitsByteOffset));
  for(int i = 0; i < words; i++){
    unsigned long long value = acache->sets[index].blocks[oldestway].datawords[i];
    StoreWord(acache->nextcache, address | (i << numBitsByteOffset), value);
  }
}

void fill(cache * acache, unsigned int index, unsigned int oldestway, unsigned long long address){

  // YOUR CODE GOES HERE
  int numBitsByteOffset = 3;
  int words = acache->blocksize/8;
  unsigned long long startAddress = address >> (acache->numBitsForBlockOffset + numBitsByteOffset);
  startAddress = startAddress << (acache->numBitsForBlockOffset + numBitsByteOffset);
  for(int i = 0; i < words; i++){
    unsigned long long value = LoadWord(acache->nextcache, startAddress + (i << numBitsByteOffset));
    acache->sets[index].blocks[oldestway].datawords[i] = value;
  }
}
