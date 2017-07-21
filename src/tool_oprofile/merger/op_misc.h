//
// Created by Jean Pourroy on 21/07/2017.
//

#ifndef PERFORMANCE_MODELISATION_OP_MISC_H
#define PERFORMANCE_MODELISATION_OP_MISC_H

#include <string>
#include <vector>


using namespace std;

typedef uint64_t ui64;
typedef uint32_t ui32;

const vector<string> split(const std::string & s, const char& c);


ui64 stoullhexa(string str);


void read_oprofile_file(const char* fname);

void read_object_file(const char* fname);

#endif //PERFORMANCE_MODELISATION_OP_MISC_H
