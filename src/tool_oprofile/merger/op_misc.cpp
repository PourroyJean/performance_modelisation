//
// Created by Jean Pourroy on 21/07/2017.
//

#include <assert.h>
#include "op_misc.h"
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

const vector<string> my_split(const string &s, const char &c) {
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


