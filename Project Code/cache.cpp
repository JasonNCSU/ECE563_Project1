//
// Created by Iason Katsaros on 9/18/2019.
// Student ID: 200089673
//

#include "cache.h"
#include <stdlib.h>
#include <string>
using namespace std;

//CACHE Constructor
CACHE::cache(int bs, int l1s, int l1a, int l2s, int l2a, int l2db, int l2at, char *file) {
    blocksize = bs;
    l1_size = l1s;
    l1_assoc = l1a;
    l1_length = l1s/(bs*l1a);
    l2_size = l2s;
    l2_assoc = l2a;
    l2_data_blocks = l2db;
    l2_addr_tags = l2at;
    trace_file = file;

    cache_structure = new CACHEWAY[l1_size * l1_assoc];

    l1_reads = 0;
    l1_reads_miss = 0;
    l1_writes = 0;
    l1_writes_miss = 0;
    l1_write_backs = 0;

    trace_size = 0;
    trace_filename = "";
    data_segment = "";
    data = '';

    nextLevel = nullptr;

    CACHE::start();
}
//CACHE Constructor

//Action function, controls what to do
void CACHE::start() {
    //Gives the cache structure lru initial values required
    CACHE::lruInitializer();

    //convert trace_file from char array to String
    trace_size = sizeof(trace_file) / sizeof(char);
    for (i = 0; i < trace_size; i++) {
        trace_filename += trace_file[i];
    }

    //Reads in trace file
    string data_segment = "";
    ifstream input(trace_filename);
    while (!input.eof()) {
        input >> data;

        //When end of line is reached, we send the hex value to read or write functions
        if (data == '\n') {
            CACHE::cpuRequest(data_segment.at(0), strtoul(data_segment.substr(2, data_segment.length() - 2), nullptr, 10));
            data_segment = "";
        } else {
            input >> data;
            data_segment += data;
        }
    }
};
//Action function, controls what to do

//Initializes cache data structure
void CACHE::lruInitializer() {
    for (int i = 0; i < l1_size; i++) {
        for (int j = 0; j < l1_assoc; j++) {
            cache_structure[i + j*l1_size].lru = j;
        }
    }
}
//Initializes cache data structure

//handles what to do with cpu request
void CACHE::cpuRequest(char mode, unsigned long hex) {
    CACHE::hexManipulator(hex);
    bool temp;
    if (mode == 'r') {
        //TODO read data path
        temp = CACHE::readFromAddress(index);
    } else {
        //TODO write data path
        temp = CACHE::writeToAddress(index);
    }
    if (!temp) {
        cout << "FAILED";
    }
}
//handles what to do with cpu request

//Hex manipulation
void CACHE::hexManipulator(unsigned long hex) {
    block_bits = CACHE::getBlockOffset();
    index_bits = CACHE::getIndexBitSize();
    block_offset = CACHE:::parseBlockOffset(hex)
    index = CACHE::parseIndex(hex);
    tag = CACHE::parseTag(hex);
}
//Hex manipulation

//Read Function Call
bool CACHE::readFromAddress(unsigned long add) {

}
//Read Function Call

//Write Function Call
bool CACHE::writeToAddress(unsigned long add) {

}
//Write Function Call

//Parse Hex Value to get Block Offset, Index, and Tag
unsigned long CACHE::parseBlockOffset(unsigned long hex) {
    return ((1 << block_bits) - 1) & hex;
}
unsigned long CACHE::parseIndex(unsigned long hex) {
    return ((1 << index_bits) - 1) & (hex >> block_bits);
}
unsigned long CACHE::parseTag(unsigned long hex) {
    return hex >> (block_bits + index_bits);
}
//Parse Hex Value to get Block Offset, Index, and Tag

//Getters
unsigned int CACHE::getBlockSize() {
    return blocksize;
}

unsigned int CACHE::getL1Size() {
    return l1_size;
}

unsigned int CACHE::getL1Assoc() {
    return l1_assoc;
}

unsigned int CACHE::getBlockOffset() {
    return log2(CACHE::getBlockSize());
}

unsigned int CACHE::getIndexBitSize() {
    return log2(l1_length);
}

unsigned int CACHE::getTagSize() {
    return 32 - (CACHE::getIndexSize() + CACHE::getBlockOffset());
}
//Getters

//Prints out desired values with formatting
void CACHE::print() {

}
//Prints out desired values with formatting


//=========================================================================================
//Cache way class constructor
CACHEWAY::cacheway() {
    tag = 0;
    lru = 0;
    dirty = 0;
}
//Cache way class constructor