//
// Created by Iason Katsaros on 9/18/2019.
// Student ID: 200089673
//

#ifndef ECE563_PROJECT1A_CACHE_H
#define ECE563_PROJECT1A_CACHE_H


class Cache {
private:
    int getBlockSize();
    int getL1Size();
    int getL1Assoc();
    int getBlockOffset();
    int getIndexSize();
    int getTagSize();
    unsigned long parseBlockOffset(unsigned long hex);
    unsigned long parseIndex(unsigned long hex);
    unsigned long parseTag(unsigned long hex);

    int blocksize;
    int l1_size;
    int l1_assoc;
    int l1_length;
    int l2_size;
    int l2_assoc;
    int l2_data_blocks;
    int l2_addr_tags;

    unsigned int l1_reads;
    unsigned int l1_reads_miss;
    unsigned int l1_writes;
    unsigned int l1_writes_miss;
    unsigned int l1_write_backs;
    unsigned int l1_miss_rate; //(r_miss + w_miss)/(reads + writes)
    unsigned int l1_memory_traffic; //should match r_miss + w_miss + write_backs

    int block_bits;
    int index_bits;
    unsigned long block_offset;
    unsigned long index;
    unsigned long tag;

public:
    Cache(int bs, int l1s, int l1a, int l2s, int l2a, int l2db, int l2at);
    void read(unsigned long hex);
    void write(unsigned long hex);
    void print();
};

class CacheWay {

};

#endif //ECE563_PROJECT1A_CACHE_H
