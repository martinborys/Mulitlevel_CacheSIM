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
  acache->numways = assoc; // numways is the same as set association
  acache->blocksize = blocksize;
  acache->numsets = (size/blocksize)/assoc; // # of sets is # of blocks / # of ways
  acache->numBitsForBlockOffset = lg2pow2(blocksize) - byteOffset; // # of bits for block indexing minus bits for byte offset
  acache->numBitsForIndex = lg2pow2((size/blocksize)/assoc); 
  acache->VAImask = ((size/blocksize)/assoc)-1;

  int bits = (addressSize - acache->numBitsForBlockOffset - acache->numBitsForIndex - byteOffset); // 2^x function
  unsigned long long power = 1; // this loop takes input x and returns 2^x like a reverse log
  for(int i=0; i<bits; i++){
    power *= 2;
  }
  power -= 1;
  acache->VATmask = power;
}

unsigned long long getindex(cache* acache, unsigned long long address){
  int byteOffset = 3;
  unsigned long long index;
  index = (acache->VAImask  << (acache->numBitsForBlockOffset + byteOffset)) & address; // shift mask by # of bits for block and byte offset
  index = index >> (acache->numBitsForBlockOffset + byteOffset); // shift back by same amount
  return index;
}

unsigned long long gettag(cache* acache, unsigned long long address){
  int byteOffset = 3;
  unsigned long long tag;
  tag = (acache->VATmask  << (acache->numBitsForIndex + acache->numBitsForBlockOffset + byteOffset))& address; // shift mask by # of bits for block and byte offset
  tag = tag >> (acache->numBitsForIndex + acache->numBitsForBlockOffset + byteOffset); // shift back by same amount
  return tag; //Replace with correct value
}

void writeback(cache* acache, unsigned int index, unsigned int oldestway){

  // YOUR CODE GOES HERE
  int numBitsByteOffset = 3;
  int words = acache->blocksize/8; // find # of words based on block size and word size
  unsigned long long address; // find tag value based on index, oldestway and shift by # bits for index, block offset, byte offset
  unsigned long long tag = (acache->sets[index].blocks[oldestway].tag) << (acache->numBitsForIndex + acache->numBitsForBlockOffset + numBitsByteOffset); 
  address = tag | (index << (acache->numBitsForBlockOffset + numBitsByteOffset)); // Bitwise OR address with index shifted by byte and block offset
  for(int i = 0; i < words; i++){ // set value for each word in block and store
    unsigned long long value = acache->sets[index].blocks[oldestway].datawords[i];
    StoreWord(acache->nextcache, address | (i << numBitsByteOffset), value);
  }
}

void fill(cache * acache, unsigned int index, unsigned int oldestway, unsigned long long address){

  // YOUR CODE GOES HERE
  int numBitsByteOffset = 3;
  int words = acache->blocksize/8; // find # of words based on block size and word size
  unsigned long long startAddress = address >> (acache->numBitsForBlockOffset + numBitsByteOffset); // shift address by # of bits for block and byte offset
  startAddress = startAddress << (acache->numBitsForBlockOffset + numBitsByteOffset); // shift back to obtain address at the start of the block.
  for(int i = 0; i < words; i++){ // load value for each word in block and store at given index + oldestway
    unsigned long long value = LoadWord(acache->nextcache, startAddress + (i << numBitsByteOffset));
    acache->sets[index].blocks[oldestway].datawords[i] = value;
  }
}

/*
For this project, I used many print statements to test my code. I began by using the provided debugging print statement for the first function.
I used the equations in the slides to find the correct values to put into acache. When I printed out the values, they matched the ones provided
in the Discussion post. From there, I implemented gettag and getindex. I printed the value of the address to confirm that it was not shifted.
I did the same for the VAI and VAT mask. By printing out the Bitwise OR result of the two, I was able to confirm that the functions were
working as intended.

For the write and fill functions, I referenced the canvas discussions, where I learned that I can read/write specific datawords in memory by
referencing them using the index and oldestway (acache->sets[index].blocks[oldestway].datawords[i]). With this information, I printed out the
address provided in the fill function, and compared it to the address I had crafted in writeback. They were similar enough in value that I
determined that I'm getting reasonable value for the address in writeback. I was able to run the makefile with no segmentation faults to confirm
that I am not accessing any illegal locations in memory. From there, it was simple enought to iterate through the block. I shifted my iterator
"i," by the amount of numBitsByteOffset so that I was accessing data at the word granularity. That allowed me to freely load and store each block.
*/