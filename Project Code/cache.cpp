//
// Created by Iason Katsaros on 9/18/2019.
// Student ID: 200089673
//

#include "cache.h"
#include <stdlib.h>
#include <string>
#include <iomanip>
using namespace std;

//CACHE Constructor we don't want
CACHE::cache() {

}
//CACHE Constructor we don't want

//CACHE Constructor we want
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

    nextLevel = nullptr;

    CACHE::lruInitializer();
}
//CACHE Constructor we want

//Initializes cache data structure
void CACHE::lruInitializer() {
    for (int i = 0; i < l1_size; i++) {
        for (int j = 0; j < l1_assoc; j++) {
            cache_structure[i + j * l1_length].lru = j;
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
        l1_reads++;
        temp = CACHE::readFromAddress();
    } else {
        //TODO write data path
        l1_writes++;
        temp = CACHE::writeToAddress();
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
bool CACHE::readFromAddress() {
    bool hit = false;
    unsigned int hit_index = 0;
    unsigned int lru_max = 0;

    for (int i = 0; i < l1_assoc; i++) {
        if (cache_structure[index + i * l1_length].tag == tag) {
            hit = true;
            hit_index = index + i * l1_length;
            break;
        }
    }

    if (hit) {
        lru_max = cache_structure[hit_index].lru;
        for (int i = 0; i < l1_assoc; i++) {
            if (cache_structure[index + i * l1_length].lru < lru_max) {
                cache_structure[index + i * l1_length].lru += 1;
            }
        }
        cache_structure[hit_index].lru = 0;
    } else {
        l1_reads_miss++;

        for (int i = 0; i < l1_assoc; i++) {
            if (cache_structure[index + i * l1_length].lru != (l1_assoc - 1)) {
                cache_structure[index + i * l1_length].lru += 1;
            } else {
                if (cache_structure[index + i * l1_length].dirty == 'D') {
                    l1_write_backs++;
                    cache_structure[index + i * l1_length].dirty = 'N';
                }
                cache_structure[index + i * l1_length].tag = tag;
                cache_structure[index + i * l1_length].lru = 0;
            }
        }
    }
}
//Read Function Call

//Write Function Call
bool CACHE::writeToAddress() {
    bool hit = false;
    unsigned int hit_index = 0;
    unsigned int lru_max = 0;

    for (int i = 0; i < l1_assoc; i++) {
        if (cache_structure[index + i * l1_length].tag == tag) {
            hit = true;
            hit_index = index + i * l1_length;
            break;
        }
    }

    if (hit) {
        lru_max = cache_structure[hit_index].lru;
        for (int i = 0; i < l1_assoc; i++) {
            if (cache_structure[index + i * l1_length].lru < lru_max) {
                cache_structure[index + i * l1_length].lru += 1;
            }
        }
        cache_structure[hit_index].lru = 0;
        cache_structure[hit_index].dirty = 'D';
    } else {
        l1_writes_miss++;

        for (int i = 0; i < l1_assoc; i++) {
            if (cache_structure[index + i * l1_length].lru != (l1_assoc - 1)) {
                cache_structure[index + i * l1_length].lru += 1;
            } else {
                if (cache_structure[index + i * l1_length].dirty == 'D') {
                    l1_write_backs++;
                }
                cache_structure[index + i * l1_length].tag = tag;
                cache_structure[index + i * l1_length].lru = 0;
                cache_structure[index + i * l1_length].dirty = 'D';
            }
        }
    }
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
    l1_memory_traffic = l1_reads_miss + l1_writes_miss + l1_write_backs;
    l1_miss_rate = (l1_reads_miss + l1_writes_miss) / (l1_reads + l1_writes);
    l1_avg_access_time = l1_hit_rate + (l1_miss_rate * l1_miss_penalty);

    //Header to printout
    cout << "===== Simulator configuration =====";
    cout << "BLOCKSIZE:                        " + blocksize;
    cout << "L1_SIZE:                          " + l1_size;
    cout << "L1_ASSOC:                         " + l1_assoc;
    cout << "L2_SIZE:                          " + l2_size;
    cout << "L2_ASSOC:                         " + l2_assoc;
    cout << "L2_DATA_BLOCKS:                   " + l2_data_blocks;
    cout << "L2_ADDRESS_TAGS:                  " + l2_addr_tags;
    cout << "trace_file:                       " + trace_file + "\n";
    //Header to printout

    //Tag Printout
    std::stringstream sstream;
    string tag_layout = "";
    cout << "===== L1 contents =====";
    for (int i = 0; i < l1_length; i++) {
        tag_layout = "set " + i + ":";
        for (int j = 0; j < l1_assoc; j++) {
            sstream << std::hex << cache_structure[i + j*l1_length].tag;
            tag_layout += "  " + sstream.str() + " " + cache_structure[i + j*l1_length].dirty + " ||";
        }
        cout << tag_layout;
    }
    //Tag Printout

    //Footer to printout
    cout << "===== Simulation Results =====";
    cout << "a. number of L1 reads:" + l1_reads;
    cout << "b. number of L1 read misses:		" + l1_reads_miss;
    cout << "c. number of L1 writes:			" + l1_writes;
    cout << "d. number of L1 write misses:		" + l1_writes_miss;
    cout << "e. L1 miss rate:			" + l1_miss_rate;
    cout << "f. number of writebacks from L1 memory:	" + l1_write_backs;\
    cout << "g. total memory traffic:		" + l1_reads_miss + l1_writes_miss + l1_write_backs;
    //Footer to printout
}
//Prints out desired values with formatting


//=========================================================================================
//Cache way class constructor
CACHEWAY::cacheway() {
    tag = 0;
    lru = 0;
    dirty = 'N';
}
//Cache way class constructor