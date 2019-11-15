//
// Created by Jean Pourroy on 21/07/2017.
//

#ifndef PERFORMANCE_MODELISATION_OP_MISC_H
#define PERFORMANCE_MODELISATION_OP_MISC_H

#include <string>
#include <vector>
#include <misc.h>

#include "../../common/AnyOption/anyoption.h"



using namespace std;


const vector<string> split(const std::string & s, const char& c);

const vector<string> my_split(const std::string &s, const char &c);


ui64 stoullhexa(string str);

void parse_argument(int argc, char *argv[], AnyOption *opt);

bool to_bool(std::string str);


#endif //PERFORMANCE_MODELISATION_OP_MISC_H
