//
// Created by Iason Katsaros on 9/18/2019.
// Student ID: 200089673
//

#include "cache.h"
#include <string>
#include <iomanip>
#include <cmath>
#include <iostream>
#define DEBUG_OUTPUT

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
    cache_address = nullptr;
    cache_sectored = nullptr;

    block_bits = 0;
    l1_index_bits = 0;
    l2_index_bits = 0;
    l1_index_addr = 0;
    l1_tag_addr = 0;
    l2_index_addr = 0;
    l2_tag_addr = 0;
    l2_sector_addr = 0;
    l2_sector_bits = 0;
    l2_selection_addr = 0;
    l2_selection_bits = 0;

    l1_reads = 0;
    l1_reads_miss = 0;
    l1_writes = 0;
    l1_writes_miss = 0;
    l1_write_backs = 0;
    l1_miss_rate = 0;
    memory_traffic = 0;

    l2_miss_rate = 0;
    l2_reads_miss = 0;
    l2_writes_miss = 0;
    l2_reads = 0;
    l2_writes = 0;
    l2_sector_miss = 0;
    l2_cache_miss = 0;
    l2_write_backs = 0;

    l2_write_sector_addr = 0;
    l2_write_index_addr = 0;
    l2_write_selection_addr = 0;
    l2_write_tag_addr = 0;

    l2_addr_check = 0;
    run_num = 0;
    input_address = 0;

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

    //for l2 cache, if we are using sectored cache we will use first statement
    //otherwise make a standard cache structure like before
    if ((l2_size == 0) && (l2_data_blocks > 1)) {
        cache_structure = nullptr;
        l1_length = l1_size/(blocksize*l1_assoc*l2_data_blocks);
        cache_address = new Cachesection[l1_length * l2_addr_tags];
        cache_sectored = new Cachesector[l1_length * l2_data_blocks];
    } else {
        l1_length = l1_size/(blocksize*l1_assoc);
        cache_structure = new Cacheway[l1_length * l1_assoc];
        cache_address = nullptr;
        cache_sectored = nullptr;
    }

    block_bits = 0;
    l1_index_bits = 0;
    l2_index_bits = 0;
    l1_index_addr = 0;
    l1_tag_addr = 0;
    l2_index_addr = 0;
    l2_tag_addr = 0;
    l2_sector_addr = 0;
    l2_sector_bits = 0;
    l2_selection_addr = 0;
    l2_selection_bits = 0;

    l1_reads = 0;
    l1_reads_miss = 0;
    l1_writes = 0;
    l1_writes_miss = 0;
    l1_write_backs = 0;
    l1_miss_rate = 0;
    memory_traffic = 0;

    l2_miss_rate = 0;
    l2_reads_miss = 0;
    l2_addr_check = 0;
    l2_writes_miss = 0;
    l2_reads = 0;
    l2_writes = 0;
    l2_sector_miss = 0;
    l2_cache_miss = 0;
    l2_write_backs = 0;

    l2_write_sector_addr = 0;
    l2_write_index_addr = 0;
    l2_write_selection_addr = 0;
    l2_write_tag_addr = 0;

    run_num = 0;
    input_address = 0;

    nextLevel = nullptr;
    if (cache_structure != nullptr) {
        Cache::lruInitializer();
    }
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
void Cache::cpuRequest(char mode, unsigned long address) {
    input_address = address;
    Cache::hexManipulator(address);
    run_num++;
#ifdef DEBUG_OUTPUT
    cout << "------------------------------------------------" << endl;
#endif
    if (mode == 'r') {
#ifdef DEBUG_OUTPUT
        cout << "# " << dec << run_num << ": read " << hex << address << endl;
        cout << "L1 read: " << hex << address << " (tag " << l1_tag_addr << dec << ", index " << l1_index_addr << ")" << endl;
#endif
        l1_reads++;
        Cache::readFromL1Address();
    } else {
#ifdef DEBUG_OUTPUT
        cout << "# " << dec << run_num << ": write " << hex << address << endl;
        cout << "L1 write: " << hex << address << " (tag " << l1_tag_addr << dec << ", index " << l1_index_addr << ")" << endl;
#endif
        l1_writes++;
        Cache::writeToL1Address();
    }
}
//handles what to do with cpu request

//Hex manipulation
void Cache::hexManipulator(unsigned long hex) {
    block_bits = Cache::getBlockOffset();
    l1_index_bits = Cache::getIndexBitSize();
    l1_index_addr = Cache::parseL1Index(hex);
    l1_tag_addr = Cache::parseL1Tag(hex);

    if (nextLevel != nullptr) {
        if (l2_assoc > 1) {
            l2_index_bits = nextLevel->Cache::getIndexBitSize();
            l2_index_addr = Cache::parseL2Index(hex);
            l2_tag_addr = Cache::parseL2Tag(hex);
        } else {
            l2_sector_bits = Cache::getSectorBitSize();
            l2_index_bits = nextLevel->Cache::getIndexBitSize();
            l2_selection_bits = Cache::getSelectionBitSize();
            l2_sector_addr = Cache::parseL2Sector(hex);
            l2_index_addr = Cache::parseL2SectorIndex(hex);
            l2_selection_addr = Cache::parseL2Selection(hex);
            l2_tag_addr = Cache::parseL2SectorTag(hex);
        }
    }
}
//Hex manipulation

//Parse Hex Value to get Index and Tag
unsigned long Cache::parseL2Sector(unsigned long hex) {
    return ((1 << l2_sector_bits) - 1) & (hex >> block_bits);
}

unsigned long Cache::parseL1Index(unsigned long hex) {
    return ((1 << l1_index_bits) - 1) & (hex >> block_bits);
}
unsigned long Cache::parseL2Index(unsigned long hex) {
    return ((1 << l2_index_bits) - 1) & (hex >> (block_bits));
}
unsigned long Cache::parseL2SectorIndex(unsigned long hex) {
    return ((1 << l2_index_bits) - 1) & (hex >> (block_bits + l2_sector_bits));
}

unsigned long Cache::parseL2Selection(unsigned long hex) {
    return ((1 << l2_selection_bits) - 1) & (hex >> (block_bits + l2_sector_bits + l2_index_bits));
}

unsigned long Cache::parseL1Tag(unsigned long hex) {
    return hex >> (block_bits + l1_index_bits);
}
unsigned long Cache::parseL2Tag(unsigned long hex) {
    return hex >> (block_bits + l2_index_bits);
}
unsigned long Cache::parseL2SectorTag(unsigned long hex) {
    return hex >> (block_bits + l2_sector_bits + l2_index_bits + l2_selection_bits);
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

//L1 Read Function Call
void Cache::readFromL1Address(void) {
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
#ifdef DEBUG_OUTPUT
        cout << "L1 hit" << endl;
#endif
        lru_max = cache_structure[hit_index].lru;
        for (unsigned int i = 0; i < l1_assoc; i++) {
            if (cache_structure[l1_index_addr + i * l1_length].lru < lru_max) {
                cache_structure[l1_index_addr + i * l1_length].lru += 1;
            }
        }
        cache_structure[hit_index].lru = 0;
    } else {
#ifdef DEBUG_OUTPUT
        cout << "L1 miss" << endl;
#endif
        l1_reads_miss++;

        for (unsigned int i = 0; i < l1_assoc; i++) {
            if (cache_structure[l1_index_addr + i * l1_length].lru != (l1_assoc - 1)) {
                cache_structure[l1_index_addr + i * l1_length].lru += 1;
            } else {
                if (cache_structure[l1_index_addr + i * l1_length].dirty == 'D') {
                    if (nextLevel != nullptr) {
                        if (l2_data_blocks < 2) {
                            rebuildNormalL2Index(l1_index_addr, cache_structure[l1_index_addr + i * l1_length].tag);
                        } else {
                            rebuildSectoredL2Index(l1_index_addr, cache_structure[l1_index_addr + i * l1_length].tag);
                        }
#ifdef DEBUG_OUTPUT
                        cout << "L2 write: " << hex << "(C0 " << l2_write_sector_addr << ", C1 " << l2_write_index_addr << ", C2 " << l2_write_selection_addr << ", C3 " << l2_write_tag_addr << ")" << endl;
#endif
                        writeToL2Address();
                    }
                    l1_write_backs++;
                    cache_structure[l1_index_addr + i * l1_length].dirty = 'N';
                }
                cache_structure[l1_index_addr + i * l1_length].tag = l1_tag_addr;
                cache_structure[l1_index_addr + i * l1_length].lru = 0;
            }
        }
        if (nextLevel != nullptr) {
#ifdef DEBUG_OUTPUT
            cout << "L2 read: " << hex << input_address << " (C0 " << l2_sector_addr << ", C1 " << l2_index_addr << ", C2 " << l2_selection_addr << ", C3 " << l2_tag_addr << ")" << endl;
#endif
            Cache::readFromL2Address();
        }
    }
#ifdef DEBUG_OUTPUT
    cout << "L1 update LRU" << endl;
#endif
}
//L1 Read Function Call

//L2 Read Function Call
void Cache::readFromL2Address(void) {
    l2_reads++;

    if (nextLevel->cache_address == nullptr) {
        //pretty much same as readFromL1Address honestly... Just with L2 information!
        bool hit = false;
        unsigned int hit_index = 0;
        unsigned int lru_max = 0;

        for (unsigned int i = 0; i < l2_assoc; i++) {
            Cacheway *cachePtrLoop = &nextLevel->cache_structure[l2_index_addr + i * nextLevel->l1_length];
            if (cachePtrLoop->tag == l2_tag_addr) {
                hit = true;
                hit_index = l2_index_addr + i * nextLevel->l1_length;
                break;
            }
        }

        if (hit) {
#ifdef DEBUG_OUTPUT
            cout << "L2 hit" << endl;
#endif
            lru_max = nextLevel->cache_structure[hit_index].lru;
            for (unsigned int i = 0; i < l2_assoc; i++) {
                Cacheway *cachePtrLoop = &nextLevel->cache_structure[l2_index_addr + i * nextLevel->l1_length];
                if (cachePtrLoop->lru < lru_max) {
                    cachePtrLoop->lru += 1;
                }
            }
            nextLevel->cache_structure[hit_index].lru = 0;
        } else {
#ifdef DEBUG_OUTPUT
            cout << "L2 miss" << endl;
#endif
            l2_reads_miss++;

            for (unsigned int i = 0; i < l2_assoc; i++) {
                Cacheway *cachePtrLoop = &nextLevel->cache_structure[l2_index_addr + i * nextLevel->l1_length];
                if (cachePtrLoop->lru != (l2_assoc - 1)) {
                    cachePtrLoop->lru += 1;
                } else {
                    if (cachePtrLoop->dirty == 'D') {
                        l2_write_backs++;
                        cachePtrLoop->dirty = 'N';
                    }
                    cachePtrLoop->tag = l2_tag_addr;
                    cachePtrLoop->lru = 0;
                }
            }
        }
    } else {
        //TODO this is the new function, have to account for it being slightly different
        //          because it has address array and sectored data array
        //this is how they look
        //cache_address = new Cachesection[l1_length * l2_addr_tags];
        //cache_sectored = new Cachesector[l1_length * l2_data_blocks];
        bool addr_hit = false;
        bool data_hit = false;
        Cachesector *sectPtr = &nextLevel->cache_sectored[l2_index_addr + l2_sector_addr * nextLevel->l1_length];
        Cachesection *addrPtr = &nextLevel->cache_address[l2_index_addr + l2_selection_addr * nextLevel->l1_length];

        if (addrPtr->tag == l2_tag_addr) {
            addr_hit = true;
        }
        if (addr_hit) {
            if ((sectPtr->tag == l2_tag_addr) && (sectPtr->select == l2_selection_addr)) {
                data_hit = true;
#ifdef DEBUG_OUTPUT
                cout << "L2 hit" << endl;
#endif
            }
        }
        if (!data_hit) {
#ifdef DEBUG_OUTPUT
            cout << "L2 miss" << endl;
#endif
            l2_reads_miss++;
            //check for miss type
            if ((sectPtr->tag == 0) || (sectPtr->valid = 'I')) {
                l2_sector_miss++;
            } else {
                l2_cache_miss++;
            }

            if (addr_hit) {
                sectPtr->tag = l2_tag_addr;
                sectPtr->select = l2_selection_addr;
                sectPtr->valid = 'V';
                if (sectPtr->dirty == 'D') {
                    l2_write_backs++;
                }
                sectPtr->dirty = 'N';
            } else {
                for (unsigned int i = 0; i < l2_data_blocks; i++) {
                    Cachesector *sectPtrLoop = &nextLevel->cache_sectored[l2_index_addr + i * nextLevel->l1_length];
                    if (sectPtrLoop->select == l2_selection_addr) {
                        if (sectPtrLoop->dirty == 'D') {
                            l2_write_backs++;
                            sectPtrLoop->dirty = 'N';
                        }
                        sectPtr->valid = 'I';
                    }
                }
                l2_addr_check++;
                addrPtr->tag = l2_tag_addr;
                sectPtr->tag = l2_tag_addr;
                sectPtr->select = l2_selection_addr;
                sectPtr->valid = 'V';
                if (sectPtr->dirty == 'D') {
                    l2_write_backs++;
                }
                sectPtr->dirty = 'N';
            }
        }
    }
#ifdef DEBUG_OUTPUT
    cout << "L2 update LRU" << endl;
#endif
    //Cache::printData();
}
//L2 Read Function Call

//L1 Write Function Call
void Cache::writeToL1Address(void) {
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
#ifdef DEBUG_OUTPUT
        cout << "L1 hit" << endl;
#endif
        lru_max = cache_structure[hit_index].lru;
        for (unsigned int i = 0; i < l1_assoc; i++) {
            if (cache_structure[l1_index_addr + i * l1_length].lru < lru_max) {
                cache_structure[l1_index_addr + i * l1_length].lru += 1;
            }
        }
        cache_structure[hit_index].lru = 0;
        cache_structure[hit_index].dirty = 'D';
    } else {
#ifdef DEBUG_OUTPUT
        cout << "L1 miss" << endl;
#endif
        l1_writes_miss++;

        for (unsigned int i = 0; i < l1_assoc; i++) {
            if (cache_structure[l1_index_addr + i * l1_length].lru != (l1_assoc - 1)) {
                cache_structure[l1_index_addr + i * l1_length].lru += 1;
            } else {
                if (cache_structure[l1_index_addr + i * l1_length].dirty == 'D') {
                    if (nextLevel != nullptr) {
                        if (l2_data_blocks < 2) {
                            rebuildNormalL2Index(l1_index_addr, cache_structure[l1_index_addr + i * l1_length].tag);
                        } else {
                            rebuildSectoredL2Index(l1_index_addr, cache_structure[l1_index_addr + i * l1_length].tag);
                        }
#ifdef DEBUG_OUTPUT
                        cout << "L2 write: " << hex << "(C0 " << l2_write_sector_addr << ", C1 " << l2_write_index_addr << ", C2 " << l2_write_selection_addr << ", C3 " << l2_write_tag_addr << ")" << endl;
#endif
                        writeToL2Address();
                    }
                    l1_write_backs++;
                }
                cache_structure[l1_index_addr + i * l1_length].tag = l1_tag_addr;
                cache_structure[l1_index_addr + i * l1_length].lru = 0;
                cache_structure[l1_index_addr + i * l1_length].dirty = 'D';
            }
        }
        if (nextLevel != nullptr) {
#ifdef DEBUG_OUTPUT
            cout << "L2 read: " << hex << input_address << " (C0 " << l2_sector_addr << ", C1 " << l2_index_addr << ", C2 " << l2_selection_addr << ", C3 " << l2_tag_addr << ")" << endl;
#endif
            Cache::readFromL2Address();
        }
    }
#ifdef DEBUG_OUTPUT
    cout << "L1 update LRU" << endl;
#endif
}
//L1 Write Function Call

//L2 Write Function Call
void Cache::writeToL2Address(void) {
    l2_writes++;

    if (nextLevel->cache_address == nullptr) {
        //pretty much same as readFromL1Address honestly... Just with L2 information!
        bool hit = false;
        unsigned int hit_index = 0;
        unsigned int lru_max = 0;

        for (unsigned int i = 0; i < l2_assoc; i++) {
            Cacheway *cachePtrLoop = &nextLevel->cache_structure[l2_index_addr + i * nextLevel->l1_length];
            if (cachePtrLoop->tag == l2_write_tag_addr) {
                hit = true;
                hit_index = l2_write_index_addr + i * nextLevel->l1_length;
                break;
            }
        }

        if (hit) {
#ifdef DEBUG_OUTPUT
            cout << "L2 hit" << endl;
#endif
            lru_max = nextLevel->cache_structure[hit_index].lru;
            for (unsigned int i = 0; i < l2_assoc; i++) {
                Cacheway *cachePtrLoop = &nextLevel->cache_structure[l2_index_addr + i * nextLevel->l1_length];
                if (cachePtrLoop->lru < lru_max) {
                    cachePtrLoop->lru += 1;
                }
            }
            nextLevel->cache_structure[hit_index].lru = 0;
            nextLevel->cache_structure[hit_index].dirty = 'D';
        } else {
#ifdef DEBUG_OUTPUT
            cout << "L2 miss" << endl;
#endif
            l2_writes_miss++;

            for (unsigned int i = 0; i < l2_assoc; i++) {
                Cacheway *cachePtrLoop = &nextLevel->cache_structure[l2_index_addr + i * nextLevel->l1_length];
                if (cachePtrLoop->lru != (l2_assoc - 1)) {
                    cachePtrLoop->lru += 1;
                } else {
                    if (cachePtrLoop->dirty == 'D') {
                        l2_write_backs++;
                    }
                    cachePtrLoop->tag = l2_write_tag_addr;
                    cachePtrLoop->lru = 0;
                    cachePtrLoop->dirty = 'D';
                }
            }
        }
    } else {
        //TODO this is the new function, have to account for it being slightly different
        //          because it has address array and sectored data array
        //this is how they look
        //cache_address = new Cachesection[l1_length * l2_addr_tags];
        //cache_sectored = new Cachesector[l1_length * l2_data_blocks];
        bool addr_hit = false;
        bool data_hit = false;
        Cachesector *sectPtr = &nextLevel->cache_sectored[l2_write_index_addr + l2_write_sector_addr * nextLevel->l1_length];
        Cachesection *addrPtr = &nextLevel->cache_address[l2_write_index_addr + l2_write_selection_addr * nextLevel->l1_length];

        if (addrPtr->tag == l2_write_tag_addr) {
            addr_hit = true;
        }
        if (addr_hit) {
            if ((sectPtr->tag == l2_write_tag_addr) && (sectPtr->select == l2_write_selection_addr)) {
                data_hit = true;
#ifdef DEBUG_OUTPUT
                cout << "L2 hit" << endl;
#endif
            }
        }
        if (!data_hit) {
#ifdef DEBUG_OUTPUT
            cout << "L2 miss" << endl;
#endif
            l2_writes_miss++;
            //check for miss type
            if ((sectPtr->tag == 0) || (sectPtr->valid = 'I')) {
                l2_sector_miss++;
            } else {
                l2_cache_miss++;
            }

            if (addr_hit) {
                sectPtr->tag = l2_write_tag_addr;
                sectPtr->select = l2_write_selection_addr;
                sectPtr->valid = 'V';
                if (sectPtr->dirty == 'D') {
                    l2_write_backs++;
                }
                sectPtr->dirty = 'D';
            } else {
                for (unsigned int i = 0; i < l2_data_blocks; i++) {
                    Cachesector *cachePtrLoop = &nextLevel->cache_sectored[l2_write_index_addr + i * nextLevel->l1_length];
                    if (cachePtrLoop->select == l2_write_selection_addr) {
                        if (cachePtrLoop->dirty == 'D') {
                            l2_write_backs++;
                            cachePtrLoop->dirty = 'D';
                        }
                        cachePtrLoop->valid = 'I';
                    }
                }
                addrPtr->tag = l2_write_tag_addr;
                //nextLevel->cache_structure[l2_write_index_addr + i * nextLevel->l1_length].tag == l2_write_tag_addr
                sectPtr->tag = l2_write_tag_addr;
                sectPtr->select = l2_write_selection_addr;
                sectPtr->valid = 'V';
                if (sectPtr->dirty == 'D') {
                    l2_write_backs++;
                }
                sectPtr->dirty = 'D';
            }
        }
    }
#ifdef DEBUG_OUTPUT
    cout << "L2 update LRU" << endl;
#endif
    //Cache::printData();
}
//L2 Write Function Call

//Rebuild addresses for writeback
void Cache::rebuildNormalL2Index(unsigned long index, unsigned long tag) {
    //rebuild so that we get index, tag for L2
    unsigned long address = (tag << l1_index_bits) | index;
    l2_write_index_addr = (((1 << l2_index_bits) - 1) & address);
    l2_write_tag_addr = (address >> l2_index_bits);
}
void Cache::rebuildSectoredL2Index(unsigned long index, unsigned long tag) {
    //rebuild so that we get sector, index, selection, tag for L2
    unsigned long address = (tag << l1_index_bits) | index;
    l2_write_sector_addr = (((1 << l2_sector_bits) - 1) & address);
    address = address >> l2_sector_bits;
    l2_write_index_addr = (((1 << l2_index_bits) - 1) & address);
    address = address >> l2_index_bits;
    l2_write_selection_addr = (((1 << l2_selection_bits) - 1) & address);
    l2_write_tag_addr = (address >> l2_selection_bits);

#ifdef DEBUG_OUTPUT
    cout << "L2 write: " << hex << "(C0 " << l2_write_sector_addr << ", C1 " << l2_write_index_addr << ", C2 " << l2_write_selection_addr << ", C3 " << l2_write_tag_addr << ")" << endl;
#endif
}
//Rebuild addresses for writeback

//Prints out desired values with formatting
void Cache::printData(void) {
    if (nextLevel != nullptr) {
        memory_traffic = l2_reads_miss + l2_writes_miss + l2_write_backs;
    } else {
        memory_traffic = l1_reads_miss + l1_writes_miss + l1_write_backs;
    }
    l1_miss_rate = double((l1_reads_miss + l1_writes_miss)) / double((l1_reads + l1_writes));
    l2_miss_rate = double(l2_reads_miss) / double(l2_reads);

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
    //L2 Regular Cache Tag Printout
    if ((nextLevel != nullptr) && (nextLevel->cache_sectored == nullptr)) {
        cout << endl;
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
    }
    //L2 Regular Cache Tag Printout
    //L2 Sectored Cache Tag Printout
    else if ((nextLevel != nullptr) && (nextLevel->cache_sectored != nullptr)) {
        cout << endl;
        cout << "===== L2 Address Array contents =====" << endl;
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
            for (unsigned int j = 0; j < nextLevel->l2_addr_tags; j++) {
                cout << hex << nextLevel->cache_address[i + j * nextLevel->l1_length].tag << "		";
            }
            cout << "|| " << endl;
        }

        cout << endl;
        cout << "===== L2 Data Array contents =====" << endl;
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

            for (unsigned int j = 0; j < l2_data_blocks; j++) {
                cout << hex << nextLevel->cache_sectored[i + j * nextLevel->l1_length].select << "," << nextLevel->cache_sectored[i + j * nextLevel->l1_length].valid << "," << nextLevel->cache_sectored[i + j * nextLevel->l1_length].dirty << "		";
            }
            cout << "|| " << endl;
        }
    }
    //L2 Sectored Cache Tag Printout

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

    if ((nextLevel != nullptr) && (nextLevel->cache_structure != nullptr)) {
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

//Cache section class constructor and sectored cache setter
Cachesection::Cachesection() {
    tag = 0;
}
//Cache section class constructor and sectored cache setter

//Cache sector class constructor
Cachesector::Cachesector() {
    tag = 0;
    select = 0;
    valid = 'I';
    dirty = 'N';
}
//Cache sector class constructor