//
// Created by Jason on 9/18/2019.
//

#include "cache.h"
using namespace std;

//Cache Constructor
Cache::Cache(int bs, int l1s, int l1a, int l2s, int l2a, int l2db, int l2at) {
    block_size = bs;
    l1_size = l1s;
    l1_assoc = l1a;
    l1_length = l1s/(bs*l1a);
    l2_size = l2s;
    l2_assoc = l2a;
    l2_data_blocks = l2db;
    l2_addr_tags = l2at;
}
//Cache Constructor

//Read Function Call
void Cache::read(String hex) {

}
//Read Function Call

//Write Function Call
void Cache::write(String hex) {

}
//Write Function Call

//Parse Hex Value to get Block Offset, Index, and Tag
int Cache::parseBlockOffset() {

}
int Cache::parseIndex() {

}
int Cache::parseTag() {

}
//Parse Hex Value to get Block Offset, Index, and Tag

//Getters
int Cache::getBlockSize() {
    return block_size;
}

int Cache::getL1Size() {
    return l1_size;
}

int Cache::getL1Assoc() {
    return l1_assoc;
}

int Cache::getBlockOffset() {
    return log2(block_size);
}

int Cache::getIndexSize() {
    return log2(l1_length);
}

int Cache::getTagSize() {
    return 32 - (Cache::getIndexSize() + Cache::getBlockOffset());
}
//Getters
