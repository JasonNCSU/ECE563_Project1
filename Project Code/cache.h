//
// Created by Iason Katsaros on 9/18/2019.
// Student ID: 200089673
//

#ifndef ECE563_PROJECT1A_CACHE_H
#define ECE563_PROJECT1A_CACHE_H


class CACHE {
private:
    void start();
    void lruInitializer();
    void cpuRequest(char mode, unsigned long hex);
    void hexManipulator(unsigned long hex);
    bool readFromAddress();
    bool writeToAddress();

    unsigned int getBlockSize();
    unsigned int getL1Size();
    unsigned int getL1Assoc();
    unsigned int getBlockOffset();
    unsigned int getIndexBitSize();
    unsigned int getTagSize();
    unsigned long parseBlockOffset(unsigned long hex);
    unsigned long parseIndex(unsigned long hex);
    unsigned long parseTag(unsigned long hex);
    void printData();

    unsigned int blocksize;
    unsigned int l1_size;
    unsigned int l1_assoc;
    unsigned int l1_length;
    unsigned int l2_size;
    unsigned int l2_assoc;
    unsigned int l2_data_blocks;
    unsigned int l2_addr_tags;
    char *trace_file;
    CACHEWAY *cache_structure;

    unsigned int l1_reads;
    unsigned int l1_reads_miss;
    unsigned int l1_writes;
    unsigned int l1_writes_miss;
    unsigned int l1_write_backs;
    unsigned int l1_miss_rate; //should be (r_miss + w_miss)/(reads + writes)
    unsigned int l1_memory_traffic; //should match r_miss + w_miss + write_backs

    unsigned int block_bits;
    unsigned int index_bits;
    unsigned long block_offset;
    unsigned long index;
    unsigned long tag;

    CACHE *nextLevel;
public:
    cache();
    cache(int bs, int l1s, int l1a, int l2s, int l2a, int l2db, int l2at);
};

class CACHEWAY {
public:
    cacheway();

    unsigned long tag;
    unsigned int lru;
    char dirty;
};

#endif //ECE563_PROJECT1A_CACHE_H
