//
// Created by Iason Katsaros on 9/18/2019.
// Student ID: 200089673
//

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include "cache.h"

using namespace std;

int main(int argc, char **argv) {
    //Takes in original specs for the cache
    int block_size = strtol(argv[1], nullptr, 10);
    int l1_size = strtol(argv[2], nullptr, 10);
    int l1_assoc = strtol(argv[3], nullptr, 10);
    int l2_size = strtol(argv[4], nullptr, 10);
    int l2_assoc = strtol(argv[5], nullptr, 10);
    int l2_data_blocks = strtol(argv[6], nullptr, 10);
    int l2_addr_tags = strtol(argv[7], nullptr, 10);
    char *trace_file = argv[8];

    //initializes the cache object
    Cache cache(block_size, l1_size, l1_assoc, l2_size, l2_assoc, l2_data_blocks, l2_addr_tags, trace_file);

    //Reads in trace file
    string data_segment;
    ifstream input(trace_file);
    while (getline(input, data_segment)) {
        cache.cpuRequest(data_segment.at(0), strtoul(data_segment.substr(2, data_segment.length()).c_str(), nullptr, 16));
    }
    cache.printData();

    //Signals the end of the program
    return 0;
}