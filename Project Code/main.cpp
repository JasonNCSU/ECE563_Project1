//
// Created by Iason Katsaros on 9/18/2019.
// Student ID: 200089673
//

#include "main.h"
#include <stdlib>
#include <stdio>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

int main(int argc, char *argv[]) {
    //Takes in original specs for the cache
    block_size = atoi(argv[1]);
    l1_size = atoi(argv[2]);
    l1_assoc = atoi(argv[3]);
    l2_size = atoi(argv[4]);
    l2_assoc = atoi(argv[5]);
    l2_data_blocks = atoi(argv[6]);
    l2_addr_tags = atoi(argv[7]);
    *trace_file = argv[8];

    //Creates the cache object
    CACHE cache = new CACHE(block_size, l1_size, l1_assoc, 0, 0, 0, 0, &trace_file);

    //Signals the end of the program
    return 0;
}