//
// Created by Iason Katsaros on 9/18/2019.
// Student ID: 200089673
//

#include "cache.h"
#include <string>
#include <iomanip>
#include <cmath>
#include <iostream>

using namespace std;

//Cache Constructor we don't want
Cache::Cache(void) {
    blocksize = 0;
    l1_size = 0;
    l1_assoc = 0;
    l1_length = 0;
    l2_size = 0;
    l2_assoc = 0;
    l2_data_blocks = 0;
    l2_addr_tags = 0;
    trace_file = nullptr;

    cache_structure = nullptr;

    block_bits = 0;
    index_bits = 0;
    l1_index_addr = 0;
    l1_tag_addr = 0;
    l2_index_addr = 0;
    l2_tag_addr = 0;

    l1_reads = 0;
    l1_reads_miss = 0;
    l1_writes = 0;
    l1_writes_miss = 0;
    l1_write_backs = 0;
    l1_miss_rate = 0;
    memory_traffic = 0;

    nextLevel = nullptr;
}
//Cache Constructor we don't want

//Cache Constructor we want
Cache::Cache(int bs, int l1s, int l1a, int l2s, int l2a, int l2db, int l2at, char *file) {
    blocksize = bs;
    l1_size = l1s;
    l1_assoc = l1a;
    l2_size = l2s;
    l2_assoc = l2a;
    l2_data_blocks = l2db;
    l2_addr_tags = l2at;
    trace_file = file;

    if ((l2_size == 0) && (l2_data_blocks != 0)) {
        l1_length = (l1s*l2_addr_tags)/(bs*l1a*l2_data_blocks);
        cache_structure = new Cacheway[l1_length * l1_assoc * l2_addr_tags];
    } else {
        l1_length = l1s/(bs*l1a);
        cache_structure = new Cacheway[l1_length * l1_assoc];
    }

    block_bits = 0;
    index_bits = 0;
    l1_index_addr = 0;
    l1_tag_addr = 0;
    l2_index_addr = 0;
    l2_tag_addr = 0;

    l1_reads = 0;
    l1_reads_miss = 0;
    l1_writes = 0;
    l1_writes_miss = 0;
    l1_write_backs = 0;
    l1_miss_rate = 0;
    memory_traffic = 0;

    nextLevel = nullptr;
    Cache::lruInitializer();
    if (l2_size != 0) {
        Cache::nextLevelInitializer();
    }
}
//Cache Constructor we want

void Cache::nextLevelInitializer(void) {
    nextLevel = new Cache(blocksize, l2_size, l2_assoc, 0, 0, l2_data_blocks, l2_addr_tags, trace_file);
}

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
        cout << "FAILED";
    }
}
//handles what to do with cpu request

//Hex manipulation
void Cache::hexManipulator(unsigned long hex) {
    block_bits = Cache::getBlockOffset();
    index_bits = Cache::getIndexBitSize();
    l1_index_addr = Cache::parseL1Index(hex);
    l1_tag_addr = Cache::parseL1Tag(hex);

    if (nextLevel != nullptr) {
        l2_sector_bits = Cache::getSectorBitSize();
        l2_selection_bits = Cache::getSelectionBitSize();
        l2_sector_addr = Cache::parseL2Sector(hex);
        l2_index_addr = Cache::parseL2Index(hex);
        l2_selection_addr = Cache::parseL2Selection(hex);
        l2_tag_addr = Cache::parseL2Tag(hex);
    }
}
//Hex manipulation

//Read Function Call
bool Cache::readFromAddress(void) {
    bool hit = false;
    unsigned int hit_index = 0;
    unsigned int lru_max = 0;

    for (unsigned int i = 0; i < l1_assoc; i++) {
        if (cache_structure[l1_index_addr + i * l1_length].tag == l1_tag_addr) {
            hit = true;
            hit_index = l1_index_addr + i * l1_length;
            break;
        }
    }

    if (hit) {
        lru_max = cache_structure[hit_index].lru;
        for (unsigned int i = 0; i < l1_assoc; i++) {
            if (cache_structure[l1_index_addr + i * l1_length].lru < lru_max) {
                cache_structure[l1_index_addr + i * l1_length].lru += 1;
            }
        }
        cache_structure[hit_index].lru = 0;
    } else {
        l1_reads_miss++;

        for (unsigned int i = 0; i < l1_assoc; i++) {
            if (cache_structure[l1_index_addr + i * l1_length].lru != (l1_assoc - 1)) {
                cache_structure[l1_index_addr + i * l1_length].lru += 1;
            } else {
                if (cache_structure[l1_index_addr + i * l1_length].dirty == 'D') {
                    if (nextLevel != nullptr) {
                        //take the above dirty block and send it back to L2!
                    }
                    l1_write_backs++;
                    cache_structure[l1_index_addr + i * l1_length].dirty = 'N';
                }
                cache_structure[l1_index_addr + i * l1_length].tag = l1_tag_addr;
                cache_structure[l1_index_addr + i * l1_length].lru = 0;
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
        if (cache_structure[l1_index_addr + i * l1_length].tag == l1_tag_addr) {
            hit = true;
            hit_index = l1_index_addr + i * l1_length;
            break;
        }
    }

    if (hit) {
        lru_max = cache_structure[hit_index].lru;
        for (unsigned int i = 0; i < l1_assoc; i++) {
            if (cache_structure[l1_index_addr + i * l1_length].lru < lru_max) {
                cache_structure[l1_index_addr + i * l1_length].lru += 1;
            }
        }
        cache_structure[hit_index].lru = 0;
        cache_structure[hit_index].dirty = 'D';
    } else {
        l1_writes_miss++;

        for (unsigned int i = 0; i < l1_assoc; i++) {
            if (cache_structure[l1_index_addr + i * l1_length].lru != (l1_assoc - 1)) {
                cache_structure[l1_index_addr + i * l1_length].lru += 1;
            } else {
                if (cache_structure[l1_index_addr + i * l1_length].dirty == 'D') {
                    if (nextLevel != nullptr) {
                        //take the above dirty block and send it back to L2!
                    }
                    l1_write_backs++;
                }
                cache_structure[l1_index_addr + i * l1_length].tag = l1_tag_addr;
                cache_structure[l1_index_addr + i * l1_length].lru = 0;
                cache_structure[l1_index_addr + i * l1_length].dirty = 'D';
            }
        }
    }
    return true;
}
//Write Function Call

//Parse Hex Value to get Index and Tag
unsigned long Cache::parseL2Sector(unsigned long hex) {
    return ((1 << l2_sector_bits) - 1) & (hex >> block_bits);
}

unsigned long Cache::parseL1Index(unsigned long hex) {
    return ((1 << index_bits) - 1) & (hex >> block_bits);
}
unsigned long Cache::parseL2Index(unsigned long hex) {
    return ((1 << index_bits) - 1) & (hex >> (block_bits + l2_sector_bits));
}

unsigned long Cache::parseL2Selection(unsigned long hex) {
    return ((1 << l2_selection_bits) - 1) & (hex >> (block_bits + l2_sector_bits + index_bits));
}

unsigned long Cache::parseL1Tag(unsigned long hex) {
    return hex >> (block_bits + index_bits);
}
unsigned long Cache::parseL2Tag(unsigned long hex) {
    return hex >> (block_bits + l2_sector_bits + index_bits + l2_selection_bits);
}
//Parse Hex Value to get Block Offset, Index, and Tag

//Getters
unsigned int Cache::getBlockOffset(void) {
    return log2(blocksize);
}

unsigned int Cache::getSectorBitSize(void) {
    return log2(l2_data_blocks);
}

unsigned int Cache::getIndexBitSize(void) {
    return log2(l1_length);
}

unsigned int Cache::getSelectionBitSize(void) {
    return log2(l2_addr_tags);
}
//Getters

//Prints out desired values with formatting
void Cache::printData(void) {
    Cache::sortData();
    if (nextLevel != nullptr) {
        memory_traffic = 0;
    } else {
        memory_traffic = l1_reads_miss + l1_writes_miss + l1_write_backs;
    }
    l1_miss_rate = double((l1_reads_miss + l1_writes_miss)) / double((l1_reads + l1_writes));
    l2_miss_rate = double((l2_reads_miss + l2_writes_miss)) / double((l2_reads + l2_writes));

    //Header to printout
    cout << "  ===== Simulator configuration =====" << endl;
    cout << "  BLOCKSIZE:                        " << blocksize << endl;
    cout << "  L1_SIZE:                          " << l1_size << endl;
    cout << "  L1_ASSOC:                         " << l1_assoc << endl;
    cout << "  L2_SIZE:                          " << l2_size << endl;
    cout << "  L2_ASSOC:                         " << l2_assoc << endl;
    cout << "  L2_DATA_BLOCKS:                   " << l2_data_blocks << endl;
    cout << "  L2_ADDRESS_TAGS:                  " << l2_addr_tags << endl;
    cout << "  trace_file:                       " << trace_file << endl << endl;
    //Header to printout

    //L1 Tag Printout
    cout << "===== L1 contents =====" << endl;
    for (unsigned int i = 0; i < l1_length; i++) {
        cout << dec << "set " << i << ":";
        if (i > 999) {
            cout << left << setfill(' ') << setw(3) << " ";
        } else if (i > 99) {
            cout << left << setfill(' ') << setw(4) << " ";
        } else if (i > 9) {
            cout << left << setfill(' ') << setw(1) << " ";
        } else {
            cout << left << setfill(' ') << setw(2) << " ";
        }
        for (unsigned int j = 0; j < l1_assoc; j++) {
            cout << hex << cache_structure[i + j * l1_length].tag << " " << cache_structure[i + j * l1_length].dirty << " ||  ";
        }
        cout << endl;
    }
    //L1 Tag Printout

    if (nextLevel != nullptr) {
        //L2 Tag Printout
        cout << "===== L2 contents =====" << endl;
        for (unsigned int i = 0; i < nextLevel->l1_length; i++) {
            cout << dec << "set " << i << ":";
            if (i > 999) {
                cout << left << setfill(' ') << setw(3) << " ";
            } else if (i > 99) {
                cout << left << setfill(' ') << setw(4) << " ";
            } else if (i > 9) {
                cout << left << setfill(' ') << setw(1) << " ";
            } else {
                cout << left << setfill(' ') << setw(2) << " ";
            }
            for (unsigned int j = 0; j < nextLevel->l1_assoc; j++) {
                cout << hex << nextLevel->cache_structure[i + j * nextLevel->l1_length].tag << " " << nextLevel->cache_structure[i + j * nextLevel->l1_length].dirty << " ||  ";
            }
            cout << endl;
        }
        //L2 Tag Printout
    }

    //Footer to printout
    cout << dec << endl;
    cout << "===== Simulation Results =====" << endl;
    cout << "a. number of L1 reads:             " << l1_reads << endl;
    cout << "b. number of L1 read misses:		    " << l1_reads_miss << endl;
    cout << "c. number of L1 writes:			" << l1_writes << endl;
    cout << "d. number of L1 write misses:		" << l1_writes_miss << endl;
    cout << "e. L1 miss rate:			" << setprecision(4) << fixed << l1_miss_rate << endl;
    cout << "f. number of writebacks from L1 memory:	" << l1_write_backs << endl;
    if (nextLevel == nullptr) {
        cout << "g. total memory traffic:              " << memory_traffic << endl;
    } else {
        cout << "g. number of L2 reads:			" << l2_reads << endl;
        cout << "h. number of L2 read misses:		 " << l2_reads_miss << endl;
        cout << "i. number of L2 writes:			 " << l2_writes << endl;
        cout << "j. number of L2 write misses:		 " << l2_writes_miss << endl;

        if (l2_data_blocks != 1) {
            cout << "k. number of L2 sector misses: 		 " << l2_sector_miss << endl;
            cout << "l. number of L2 cache block misses: 	 " << l2_cache_miss << endl;
            cout << "m. L2 miss rate:			 " << setprecision(4) << fixed << l2_miss_rate << endl;
            cout << "n. number of writebacks from L2 memory:	 " << l2_write_backs << endl;
            cout << "o. total memory traffic:		" << memory_traffic << endl;
        } else {
            cout << "k. L2 miss rate:                 " << setprecision(4) << fixed << l2_miss_rate << endl;
            cout << "l. number of writebacks from L2 memory:       " << l2_write_backs << endl;
            cout << "m. total memory traffic:              " << memory_traffic << endl;
        }
    }
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

    if (nextLevel != nullptr) {
        for (unsigned int i = 0; i < nextLevel->l1_length; i++) {
            for (unsigned int j = 0; j < nextLevel->l1_assoc - 1; j++) {
                min_idx = j;
                for (unsigned int k = j + 1; k < nextLevel->l1_assoc; k++) {
                    if (nextLevel->cache_structure[i + k * nextLevel->l1_length].lru < nextLevel->cache_structure[i + min_idx * nextLevel->l1_length].lru) {
                        min_idx = k;
                    }
                }
                //swap here!
                //swap the tag and dirty bit as well
                //tag is unsigned long
                temp_tag = nextLevel->cache_structure[i + j * nextLevel->l1_length].tag;
                nextLevel->cache_structure[i + j * nextLevel->l1_length].tag = nextLevel->cache_structure[i + min_idx * nextLevel->l1_length].tag;
                nextLevel->cache_structure[i + min_idx * nextLevel->l1_length].tag = temp_tag;
                //lru is unsigned int
                temp_lru = nextLevel->cache_structure[i + j * nextLevel->l1_length].lru;
                nextLevel->cache_structure[i + j * nextLevel->l1_length].lru = nextLevel->cache_structure[i + min_idx * nextLevel->l1_length].lru;
                nextLevel->cache_structure[i + min_idx * nextLevel->l1_length].lru = temp_lru;
                //dirty is char
                temp_dirty = nextLevel->cache_structure[i + j * nextLevel->l1_length].dirty;
                nextLevel->cache_structure[i + j * nextLevel->l1_length].dirty = nextLevel->cache_structure[i + min_idx * nextLevel->l1_length].dirty;
                nextLevel->cache_structure[i + min_idx * nextLevel->l1_length].dirty = temp_dirty;
            }
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

Cachesector::Cachesector() {
    tag = 0;
    select = 0;
    valid = 0;
    dirty = 'N';
}