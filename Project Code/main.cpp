//
// Created by Jason on 9/18/2019.
//

#include "main.h"
#include <stdlib>
#include <stdio.h>
#include <iostream>
#include <fstream>
using namespace std;

int main(int argc, char *argv[]) {
    //Takes in original specs for the cache
    int block_size = atoi(argv[1]);
    int l1_size = atoi(argv[2]);
    int l1_assoc = atoi(argv[3]);
    int l2_size = atoi(argv[4]);
    int l2_assoc = atoi(argv[5]);
    int l2_data_blocks = atoi(argv[6]);
    int l2_addr_tags = atoi(argv[7]);
    char *trace_file = argv[8];

    //Creates the cache object
    Cache myCache = new Cache(block_size, l1_size, l1_assoc, 0, 0, 0, 0);

    //convert trace_file from char array to String
    int trace_size = sizeof(trace_file) / sizeof(char);
    string trace_filename = "";
    for (i = 0; i < trace_size; i++) {
        trace_filename += trace_file[i];
    }

    //TODO: trace file contains r/w + [tag, index, BO]. Read in trace_file and issue read/write commands
    //Reads in trace file
    char data = '';
    string data_segment = "";
    ifstream input(trace_filename);
    while (!input.eof()) {
        input >> data;

        if (data == '\n') {
            //TODO: determine read/write stuff
            if (data_segment.at(0) == 'r') {
                //TODO read logic
                myCachce.read(data_segment.substr(2, data_segment.length() - 2));
            } else {
                //TODO write logic
                myCachce.write(data_segment.substr(2, data_segment.length() - 2));
            }
            data_segment = "";
        } else {
            input >> data;
            data_segment += data;
        }


//        int char_count = 0;
//        if (char_count == 10) {
//            char_count = 0;
//            //TODO: determine read/write stuff
//            if (data_segment.at(0) == 'r') {
//                //TODO read logic
//                myCachce.read(data_segment.substr(2, 8));
//            } else {
//                //TODO write logic
//                myCachce.write(data_segment.substr(2, 8));
//            }
//            data_segment = "";
//        }
//
//        char_count++;
    }
}