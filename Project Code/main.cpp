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
    Cache myCache = new Cache(block_size, l1_size, l1_assoc, 0, 0, 0, 0);

    //convert trace_file from char array to String
    trace_size = sizeof(trace_file) / sizeof(char);
    string trace_filename = "";
    for (i = 0; i < trace_size; i++) {
        trace_filename += trace_file[i];
    }

    //Reads in trace file
    data = '';
    string data_segment = "";
    ifstream input(trace_filename);
    while (!input.eof()) {
        input >> data;

        //When end of line is reached, we send the hex value to read or write functions
        if (data == '\n') {
            if (data_segment.at(0) == 'r') {
                myCachce.read(strtoul(data_segment.substr(2, data_segment.length() - 2), nullptr, 10));
            } else {
                myCachce.write(strtoul(data_segment.substr(2, data_segment.length() - 2), nullptr, 10));
            }
            data_segment = "";
        } else {
            input >> data;
            data_segment += data;
        }
    }
    //TODO create the print() function that writes out values in the format specified by document
    myCache.print();

    //Signals the end of the program
    return 0;
}