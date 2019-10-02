//
// Created by Iason Katsaros on 9/18/2019.
// Student ID: 200089673
//

#ifndef ECE563_PROJECT1A_CACHE_H
#define ECE563_PROJECT1A_CACHE_H

class Cacheway {
public:
    Cacheway(void);

    unsigned long tag;
    unsigned int lru;
    char dirty;
};

class Cache {
private:
    void lruInitializer(void);
    void hexManipulator(unsigned long hex);
    bool readFromAddress(void);
    bool writeToAddress(void);
    void sortData(void);

    unsigned int getBlockSize(void);
    unsigned int getBlockOffset(void);
    unsigned int getIndexBitSize(void);
    unsigned long parseIndex(unsigned long hex);
    unsigned long parseTag(unsigned long hex);

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

    unsigned int l1_reads;
    unsigned int l1_reads_miss;
    unsigned int l1_writes;
    unsigned int l1_writes_miss;
    unsigned int l1_write_backs;
    double l1_miss_rate; //should be (r_miss + w_miss)/(reads + writes)
    unsigned int l1_memory_traffic; //should match r_miss + w_miss + write_backs

    unsigned int block_bits;
    unsigned int index_bits;
    unsigned long index_addr;
    unsigned long tag_addr;

    Cache *nextLevel;
public:
    Cache(void);
    Cache(int bs, int l1s, int l1a, int l2s, int l2a, int l2db, int l2at, char *file);
    void cpuRequest(char mode, unsigned long hex);
    void printData(void);
};

#endif //ECE563_PROJECT1A_CACHE_H
