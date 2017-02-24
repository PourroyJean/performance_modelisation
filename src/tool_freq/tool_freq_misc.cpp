//
// Created by Jean Pourroy on 20/02/2017.
//

#include <cstdio>
#include "tool_freq_misc.h"


#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>

void usage() {
    fprintf(stderr,
            "Usage: ./tool_freq [-I INSTRUCTIONS ] [-W WIDTH] [-O operations] [-B BINDING] [-vh]\n");
}

template<typename Out>
void split(const std::string &s, char delim, Out result) {
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        *(result++) = item;
    }
}


std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}