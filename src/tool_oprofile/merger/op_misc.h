//
// Created by Jean Pourroy on 21/07/2017.
//

#ifndef PERFORMANCE_MODELISATION_OP_MISC_H
#define PERFORMANCE_MODELISATION_OP_MISC_H

#include <string>
#include <vector>
#include <misc.h>




using namespace std;


const vector<string> split(const std::string & s, const char& c);

const vector<string> my_split(const std::string &s, const char &c);


ui64 stoullhexa(string str);


void read_oprofile_file(const char* fname);

void read_object_file(const char* fname);

void read_file(const char *fname);
//{
//    ifstream fobj(fname);
//    assert(fobj.is_open());
//    while (1) {
//        string line;
//        getline(fobj, line);
//        if (fobj.eof())
//            break;
//        assert(!fobj.fail() && !fobj.bad());
//        objdump_file.push_back(line);
//    }
//    fobj.close();
//}


#endif //PERFORMANCE_MODELISATION_OP_MISC_H
