//
// Created by Iason Katsaros on 9/18/2019.
// Student ID: 200089673
//

#ifndef ECE563_PROJECT1A_MAIN_H
#define ECE563_PROJECT1A_MAIN_H

#include "cache.h"
#include <string>

class main {
private:
    int block_size;
    int l1_size;
    int l1_assoc;
    int l2_size;
    int l2_assoc;
    int l2_data_blocks;
    int l2_addr_tags;
    char *trace_file;

    Cache cache;

    std::string trace_filename;
    std::string data_segment;
    unsigned int trace_size;
    char data;
};


#endif //ECE563_PROJECT1A_MAIN_H
