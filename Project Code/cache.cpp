//
// Created by Jason on 9/18/2019.
//

#include "cache.h"
using namespace std;

//Cache constructor
Cache::Cache(int bs, int l1s, int l1a, int l2s, int l2a, int l2db, int l2at) {
    block_size = bs;
    l1_size = l1s;
    l1_assoc = l1a;
    l2_size = l2s;
    l2_assoc = l2a;
    l2_data_blocks = l2db;
    l2_addr_tags = l2at;
}

//Getters
int getBlockSize() {
    return block_size;
}

int getL1Size() {
    return l1_size;
}

int getL1Assoc() {
    return l1_assoc;
}

int getBlockOffset() {
    return log2(block_size);
}

int getIndex() {
    return log2(l1_size);
}

int getTag() {
    return 32-(getIndex() + getBlockOffset());
}