//
// Created by Iason Katsaros on 9/18/2019.
// Student ID: 200089673
//

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include "cache.h"

int main(int argc, char **argv) {
    //Takes in original specs for the cache
    int block_size = atoi(argv[1]);
    int l1_size = atoi(argv[2]);
    int l1_assoc = atoi(argv[3]);
    int l2_size = atoi(argv[4]);
    int l2_assoc = atoi(argv[5]);
    int l2_data_blocks = atoi(argv[6]);
    int l2_addr_tags = atoi(argv[7]);
    char *trace_file = argv[8];

    //initializes the cache object
    Cache cache(block_size, l1_size, l1_assoc, l2_size, l2_assoc, l2_data_blocks, l2_addr_tags, trace_file);

    //Reads in trace file
    std::string data_segment;
    std::ifstream input(trace_file);
    while (getline(input, data_segment)) {
        cache.cpuRequest(data_segment.at(0), std::strtoul(data_segment.substr(2, data_segment.length()).c_str(), nullptr, 16));
    }
    cache.printData();

    //Signals the end of the program
    return 0;
}