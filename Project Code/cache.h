//
// Created by Iason Katsaros on 9/18/2019.
// Student ID: 200089673
//

#ifndef ECE563_PROJECT1A_CACHE_H
#define ECE563_PROJECT1A_CACHE_H

class Cachesector {
public:
    Cachesector(void);

    unsigned long tag;
    unsigned int select;
    char valid;
    char dirty;
};

class Cachesection {
public:
    Cachesection(void);

    unsigned long tag;
};

class Cacheway {
public:
    Cacheway(void);

    unsigned long tag;
    unsigned int lru;
    char dirty;
};

class Cache {
private:
    void addressBlockInitializer(void);
    void dataBlockInitializer(void);

    void hexManipulator(unsigned long hex);
    void readFromL1Address(void);
    void writeToL1Address(void);
    void readFromL2Address(void);
    void writeToL2Address(void);
    void sortData(void);

    unsigned int getBlockOffset(void);
    unsigned int getSectorBitSize(void);
    unsigned int getIndexBitSize(void);
    unsigned int getSelectionBitSize(void);
    unsigned long parseL2Sector(unsigned long hex);
    unsigned long parseL1Index(unsigned long hex);
    unsigned long parseL2Index(unsigned long hex);
    unsigned long parseL2SectorIndex(unsigned long hex);
    unsigned long parseL2Selection(unsigned long hex);
    unsigned long parseL1Tag(unsigned long hex);
    unsigned long parseL2Tag(unsigned long hex);
    unsigned long parseL2SectorTag(unsigned long hex);

    void rebuildNormalL2Index(unsigned long index, unsigned long tag);
    void rebuildSectoredL2Index(unsigned long index, unsigned long tag);

    unsigned int blocksize;
    unsigned int l1_size;
    unsigned int l1_assoc;
    unsigned int l1_length;
    unsigned int l2_size;
    unsigned int l2_assoc;
    unsigned int l2_data_blocks;
    unsigned int l2_addr_tags;
    char *trace_file;

    Cacheway *cache_structure;
    Cache *nextLevel;
    Cachesection *cache_address;
    Cachesector *cache_sectored;

    unsigned int l1_reads;
    unsigned int l1_reads_miss;
    unsigned int l1_writes;
    unsigned int l1_writes_miss;
    unsigned int l1_write_backs;
    double l1_miss_rate; //should be (r_miss + w_miss)/(reads + writes)
    unsigned int memory_traffic; //should match r_miss + w_miss + write_backs

    double l2_miss_rate;
    unsigned int l2_reads_miss;
    unsigned int l2_writes_miss;
    unsigned int l2_reads;
    unsigned int l2_writes;
    unsigned int l2_sector_miss;
    unsigned int l2_cache_miss;
    unsigned int l2_write_backs;

    unsigned int block_bits;
    unsigned int l2_sector_bits;
    unsigned int l1_index_bits;
    unsigned int l2_index_bits;
    unsigned int l2_selection_bits;
    unsigned long l2_sector_addr;
    unsigned long l1_index_addr;
    unsigned long l2_index_addr;
    unsigned long l2_selection_addr;
    unsigned long l1_tag_addr;
    unsigned long l2_tag_addr;

    unsigned long l2_write_sector_addr;
    unsigned long l2_write_index_addr;
    unsigned long l2_write_selection_addr;
    unsigned long l2_write_tag_addr;

public:
    Cache(void);
    Cache(int bs, int l1s, int l1a, int l2s, int l2a, int l2db, int l2at, char *file);

    void nextLevelInitializer(void);
    void lruInitializer(void);
    void cpuRequest(char mode, unsigned long hex);
    void printData(void);
};

#endif //ECE563_PROJECT1A_CACHE_H
