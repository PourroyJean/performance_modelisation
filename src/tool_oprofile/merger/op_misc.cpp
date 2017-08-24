//
// Created by Jean Pourroy on 21/07/2017.
//

#include <assert.h>
#include "op_misc.h"
#include "op_oprofile_line.h"
#include "op_objdump_line.h"
#include <fstream>


ui64 stoullhexa(string str) {
    ui64 add = 0;
    for (int i = 0;; i++) {
        char c = str[i];
        if ((c >= '0') and (c <= '9')) {
            add *= 16;
            add += (c - '0');
            continue;
        }
        if ((c >= 'a') and (c <= 'f')) {
            add *= 16;
            add += (c - 'a' + 10);
            continue;
        }
        if ((c >= 'A') and (c <= 'F')) {
            add *= 16;
            add += (c - 'A' + 10);
            continue;
        }
        break;
    }
    return (add);
}


const vector<string> split(const string &s, const char &c) {
    string buff{""};
    vector<string> v;

    for (auto n:s) {
        if (n != c) buff += n;
        else if (n == c && buff != "") {
            v.push_back(buff);
            buff = "";
        }
    }
    if (buff != "") v.push_back(buff);
    return v;
}


void read_oprofile_file(const char *fname) {
    ifstream fop2(fname);
    assert(fop2.is_open());
    while (1) {
        string line;
        getline(fop2, line);
        if (fop2.eof())
            break;
        assert(!fop2.fail() && !fop2.bad());
        oprofile_file.push_back(line);
    }
    fop2.close();
}

void read_object_file(const char *fname) {
    ifstream fobj(fname);
    assert(fobj.is_open());
    while (1) {
        string line;
        getline(fobj, line);
        if (fobj.eof())
            break;
        assert(!fobj.fail() && !fobj.bad());
        objdump_file.push_back(line);
    }
    fobj.close();
}
