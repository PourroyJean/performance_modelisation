//
// Created by Jean Pourroy on 21/07/2017.
//

#include "op_oprofile_line.h"
#include "op_misc.h"
#include "op_objdump_line.h"
#include <string.h>
#include <iostream>
#include <iomanip>


int oprofile_line::line_ctr = 0;
int oprofile_line::last_func = 0;

//TODO garder dans le main
// OBJECT FILE lines
vector<oprofile_line> oprofile_file;

// map address of instructions of OBJECT FILE to line inside oprofile_file
// used for branch instruction to recover the line of the branch
std::unordered_map<ui64,int> oprofile_address;


oprofile_line::oprofile_line(string line) {
    ctr1 = 0;
    ctr2 = 0;
    line_ctr++;
    string fname;
    str = line;
    len_str = line.length();
    type = 0; // nothing usefull
    if (len_str > 0) { // function name
        if (line[0] == '0') {
            last_func = line_ctr;
            type = 1;
//cout << "_1_ " << line << endl;
            return;
        }
        if (line[0] == ' ') { // instruction
            get_address_and_ctrs();

            line_fname = last_func;
            oprofile_address[address] =
                    line_ctr;
//cout << "_2_ " << std::dec << len_str << " " << line_ctr << " " << last_func << " "
//<< line << " " << address << " " <<  std::hex << address << " " << std::dec
//<< ctr1 << " " << ctr2 << endl;
            type = 2;
            return;
        }
    }
}

void oprofile_line::get_address_and_ctrs() {
    ui64 add = 0;
    for (int i = 2;; i++) {
        char c = str[i];
        if (c == ' ')break;
        if (c == 0)break; // should not occur
        if ((c >= '0') and (c <= '9')) {
            add *= 16;
            add += (c - '0');
        }
        if ((c >= 'a') and (c <= 'f')) {
            add *= 16;
            add += (c - 'a' + 10);
        }
        if ((c >= 'A') and (c <= 'F')) {
            add *= 16;
            add += (c - 'A' + 10);
        }
    }
    address = add;
    vector <string> v{split(str, ' ')};
    ctr1 = stol(v[1]);
    ctr2 = stol(v[3]);
    int in_objdump = objdump_address[add] - 1;
    //cout << "_4_ ";for(auto n:v) cout << n << " "; cout << ctr1 << " " << ctr2 << " " << in_objdump << " "  << endl;
    if (in_objdump > 0) {
        objdump_file[in_objdump].ctr1 = ctr1;
        objdump_file[in_objdump].ctr2 = ctr2;
        //cout << "_3_ ";for(auto n:v) cout << n << " "; cout << ctr1 << " " << ctr2 << " " << in_objdump << " " << objdump_file[in_objdump].str << endl;
    }
}

