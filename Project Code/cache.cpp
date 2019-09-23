//
// Created by Iason Katsaros on 9/18/2019.
// Student ID: 200089673
//

#include "cache.h"
#include <string>
#include <iomanip>
#include <cmath>
#include <iostream>

//Cache Constructor we don't want
Cache::Cache(void) {

}
//Cache Constructor we don't want

//Cache Constructor we want
Cache::Cache(int bs, int l1s, int l1a, int l2s, int l2a, int l2db, int l2at, char *file) {
    blocksize = bs;
    l1_size = l1s;
    l1_assoc = l1a;
    l1_length = l1s/(bs*l1a);
    l2_size = l2s;
    l2_assoc = l2a;
    l2_data_blocks = l2db;
    l2_addr_tags = l2at;
    trace_file = file;

    cache_structure = new Cacheway[l1_length * l1_assoc];

    block_bits = 0;
    index_bits = 0;
    index_addr = 0;
    tag_addr = 0;

    l1_reads = 0;
    l1_reads_miss = 0;
    l1_writes = 0;
    l1_writes_miss = 0;
    l1_write_backs = 0;
    l1_miss_rate = 0;
    l1_memory_traffic = 0;

    nextLevel = nullptr;

    Cache::lruInitializer();
}
//Cache Constructor we want

//Initializes cache data structure
void Cache::lruInitializer(void) {
    for (unsigned int i = 0; i < l1_length; i++) {
        for (unsigned int j = 0; j < l1_assoc; j++) {
            cache_structure[i + j * l1_length].lru = j;
        }
    }
}
//Initializes cache data structure

//handles what to do with cpu request
void Cache::cpuRequest(char mode, unsigned long hex) {
    Cache::hexManipulator(hex);
    bool temp;
    if (mode == 'r') {
        l1_reads++;
        temp = Cache::readFromAddress();
    } else {
        l1_writes++;
        temp = Cache::writeToAddress();
    }
    if (!temp) {
        std::cout << "FAILED";
    }
}
//handles what to do with cpu request

//Hex manipulation
void Cache::hexManipulator(unsigned long hex) {
    block_bits = Cache::getBlockOffset();
    index_bits = Cache::getIndexBitSize();
    index_addr = Cache::parseIndex(hex);
    tag_addr = Cache::parseTag(hex);
}
//Hex manipulation

//Read Function Call
bool Cache::readFromAddress(void) {
    bool hit = false;
    unsigned int hit_index = 0;
    unsigned int lru_max = 0;

    for (unsigned int i = 0; i < l1_assoc; i++) {
        if (cache_structure[index_addr + i * l1_length].tag == tag_addr) {
            hit = true;
            hit_index = index_addr + i * l1_length;
            break;
        }
    }

    if (hit) {
        lru_max = cache_structure[hit_index].lru;
        for (unsigned int i = 0; i < l1_assoc; i++) {
            if (cache_structure[index_addr + i * l1_length].lru < lru_max) {
                cache_structure[index_addr + i * l1_length].lru += 1;
            }
        }
        cache_structure[hit_index].lru = 0;
    } else {
        l1_reads_miss++;

        for (unsigned int i = 0; i < l1_assoc; i++) {
            if (cache_structure[index_addr + i * l1_length].lru != (l1_assoc - 1)) {
                cache_structure[index_addr + i * l1_length].lru += 1;
            } else {
                if (cache_structure[index_addr + i * l1_length].dirty == 'D') {
                    l1_write_backs++;
                    cache_structure[index_addr + i * l1_length].dirty = 'N';
                }
                cache_structure[index_addr + i * l1_length].tag = tag_addr;
                cache_structure[index_addr + i * l1_length].lru = 0;
            }
        }
    }
    return true;
}
//Read Function Call

//Write Function Call
bool Cache::writeToAddress(void) {
    bool hit = false;
    unsigned int hit_index = 0;
    unsigned int lru_max = 0;

    for (unsigned int i = 0; i < l1_assoc; i++) {
        if (cache_structure[index_addr + i * l1_length].tag == tag_addr) {
            hit = true;
            hit_index = index_addr + i * l1_length;
            break;
        }
    }

    if (hit) {
        lru_max = cache_structure[hit_index].lru;
        for (unsigned int i = 0; i < l1_assoc; i++) {
            if (cache_structure[index_addr + i * l1_length].lru < lru_max) {
                cache_structure[index_addr + i * l1_length].lru += 1;
            }
        }
        cache_structure[hit_index].lru = 0;
        cache_structure[hit_index].dirty = 'D';
    } else {
        l1_writes_miss++;

        for (unsigned int i = 0; i < l1_assoc; i++) {
            if (cache_structure[index_addr + i * l1_length].lru != (l1_assoc - 1)) {
                cache_structure[index_addr + i * l1_length].lru += 1;
            } else {
                if (cache_structure[index_addr + i * l1_length].dirty == 'D') {
                    l1_write_backs++;
                }
                cache_structure[index_addr + i * l1_length].tag = tag_addr;
                cache_structure[index_addr + i * l1_length].lru = 0;
                cache_structure[index_addr + i * l1_length].dirty = 'D';
            }
        }
    }
    return true;
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
unsigned int Cache::getBlockSize(void) {
    return blocksize;
}

unsigned int Cache::getBlockOffset(void) {
    return log2(Cache::getBlockSize());
}

unsigned int Cache::getIndexBitSize(void) {
    return log2(l1_length);
}
//Getters

//Prints out desired values with formatting
void Cache::printData(void) {
    Cache::sortData();

    l1_memory_traffic = l1_reads_miss + l1_writes_miss + l1_write_backs;
    l1_miss_rate = double((l1_reads_miss + l1_writes_miss)) / double((l1_reads + l1_writes));

    //Header to printout
    std::cout << "  ===== Simulator configuration =====" << std::endl;
    std::cout << "  BLOCKSIZE:                        " << blocksize << std::endl;
    std::cout << "  L1_SIZE:                          " << l1_size << std::endl;
    std::cout << "  L1_ASSOC:                         " << l1_assoc << std::endl;
    std::cout << "  L2_SIZE:                          " << l2_size << std::endl;
    std::cout << "  L2_ASSOC:                         " << l2_assoc << std::endl;
    std::cout << "  L2_DATA_BLOCKS:                   " << l2_data_blocks << std::endl;
    std::cout << "  L2_ADDRESS_TAGS:                  " << l2_addr_tags << std::endl;
    std::cout << "  trace_file:                       " << trace_file << std::endl << std::endl;
    //Header to printout

    //Tag Printout
    std::cout << "===== L1 contents =====" << std::endl;
    for (unsigned int i = 0; i < l1_length; i++) {
        std::cout << std::dec << "set " << i << ":";
        if (i > 999) {
            std::cout << std::left << std::setfill(' ') << std::setw(3) << " ";
        } else if (i > 99) {
            std::cout << std::left << std::setfill(' ') << std::setw(4) << " ";
        } else if (i > 9) {
            std::cout << std::left << std::setfill(' ') << std::setw(1) << " ";
        } else {
            std::cout << std::left << std::setfill(' ') << std::setw(2) << " ";
        }
        for (unsigned int j = 0; j < l1_assoc; j++) {
            std::cout << std::hex << cache_structure[i + j * l1_length].tag << " " << cache_structure[i + j * l1_length].dirty << " ||  ";
        }
        std::cout << std::endl;
    }
    //Tag Printout

    //Footer to printout
    std::cout << std::dec << std::endl;
    std::cout << "===== Simulation Results =====" << std::endl;
    std::cout << "a. number of L1 reads:             " << l1_reads << std::endl;
    std::cout << "b. number of L1 read misses:		    " << l1_reads_miss << std::endl;
    std::cout << "c. number of L1 writes:			" << l1_writes << std::endl;
    std::cout << "d. number of L1 write misses:		" << l1_writes_miss << std::endl;
    std::cout << "e. L1 miss rate:			" << std::setprecision(4) << std::fixed << l1_miss_rate << std::endl;
    std::cout << "f. number of writebacks from L1 memory:	" << l1_write_backs << std::endl;
    std::cout << "g. total memory traffic:		" << l1_memory_traffic << std::endl;
    //Footer to printout
}
//Prints out desired values with formatting

void Cache::sortData(void) {
    unsigned int min_idx;
    unsigned long temp_tag = 0;
    unsigned int temp_lru = 0;
    char temp_dirty = ' ';

    for (unsigned int i = 0; i < l1_length; i++) {
        for (unsigned int j = 0; j < l1_assoc - 1; j++) {
            min_idx = j;
            for (unsigned int k = j + 1; k < l1_assoc; k++) {
                if (cache_structure[i + k * l1_length].lru < cache_structure[i + min_idx * l1_length].lru) {
                    min_idx = k;
                }
            }
            //swap here!
            //swap the tag and dirty bit as well
            //tag is unsigned long
            temp_tag = cache_structure[i + j * l1_length].tag;
            cache_structure[i + j * l1_length].tag = cache_structure[i + min_idx * l1_length].tag;
            cache_structure[i + min_idx * l1_length].tag = temp_tag;
            //lru is unsigned int
            temp_lru = cache_structure[i + j * l1_length].lru;
            cache_structure[i + j * l1_length].lru = cache_structure[i + min_idx * l1_length].lru;
            cache_structure[i + min_idx * l1_length].lru = temp_lru;
            //dirty is char
            temp_dirty = cache_structure[i + j * l1_length].dirty;
            cache_structure[i + j * l1_length].dirty = cache_structure[i + min_idx * l1_length].dirty;
            cache_structure[i + min_idx * l1_length].dirty = temp_dirty;
        }
    }
}

//=========================================================================================
//Cache way class constructor
Cacheway::Cacheway() {
    tag = 0;
    lru = 0;
    dirty = 'N';
}
//Cache way class constructor