//
// Created by Iason Katsaros on 9/18/2019.
// Student ID: 200089673
//

#include "cache.h"
#include <stdlib.h>
#include <string>
using namespace std;

//Cache Constructor
Cache::Cache(int bs, int l1s, int l1a, int l2s, int l2a, int l2db, int l2at) {
    blocksize = bs;
    l1_size = l1s;
    l1_assoc = l1a;
    l1_length = l1s/(bs*l1a);
    l2_size = l2s;
    l2_assoc = l2a;
    l2_data_blocks = l2db;
    l2_addr_tags = l2at;

    l1_reads = 0;
    l1_reads_miss = 0;
    l1_writes = 0;
    l1_writes_miss = 0;
    l1_write_backs = 0;
}
//Cache Constructor

//Read Function Call
void Cache::read(unsigned long hex) {
    block_bits = Cache::getBlockOffset();
    index_bits = Cache::getIndexSize();
    block_offset = Cache:::parseBlockOffset(hex)
    index = Cache::parseIndex(hex);
    tag = Cache::parseTag(hex);
}
//Read Function Call

//Write Function Call
void Cache::write(unsigned long hex) {
    block_bits = Cache::getBlockOffset();
    index_bits = Cache::getIndexSize();
    block_offset = Cache:::parseBlockOffset(hex)
    index = Cache::parseIndex(hex);
    tag = Cache::parseTag(hex);
}
//Write Function Call

//Parse Hex Value to get Block Offset, Index, and Tag
unsigned long Cache::parseBlockOffset(unsigned long hex) {
    return ((1 << block_bits) - 1) & hex;
}
unsigned long Cache::parseIndex(unsigned long hex) {
    return ((1 << index_bits) - 1) & (hex >> block_bits);
}
unsigned long Cache::parseTag(unsigned long hex) {
    return hex >> (block_bits + index_bits);
}
//Parse Hex Value to get Block Offset, Index, and Tag

//Getters
int Cache::getBlockSize() {
    return blocksize;
}

int Cache::getL1Size() {
    return l1_size;
}

int Cache::getL1Assoc() {
    return l1_assoc;
}

int Cache::getBlockOffset() {
    return log2(Cache::getBlockSize());
}

int Cache::getIndexSize() {
    return log2(l1_length);
}

int Cache::getTagSize() {
    return 32 - (Cache::getIndexSize() + Cache::getBlockOffset());
}
//Getters

//Prints out desired values with formatting
void Cache::print() {

}
//Prints out desired values with formatting