//
// Created by Jason on 9/18/2019.
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
    int parseBlockOffset();
    int parseIndex();
    int parseTag();

    int block_size;
    int l1_size;
    int l1_assoc;
    int l1_length;
    int l2_size;
    int l2_assoc;
    int l2_data_blocks;
    int l2_addr_tags;

public:
    Cache(int bs, int l1s, int l1a, int l2s, int l2a, int l2db, int l2at);
    void read(String hex);
    void write(String hex);
};

class CacheWay {

};

#endif //ECE563_PROJECT1A_CACHE_H
