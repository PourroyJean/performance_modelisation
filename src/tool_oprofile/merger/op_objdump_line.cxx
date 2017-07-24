//
// Created by Jean Pourroy on 21/07/2017.
//

#include <iostream>
#include <misc.h>
#include "op_objdump_line.h"
#include "op_misc.h"


int objdump_line::line_ctr = 0;
int objdump_line::last_func = 0;

//TODO deplacer dehors
vector<objdump_line> objdump_file;
std::unordered_map<ui64, int> objdump_address;

objdump_line::objdump_line(string line) {
    ctr1 = 0;
    ctr2 = 0;
    target = 0;
    line_ctr++;
    str = line;


    len_str = line.length();

    if (len_str < 0) return;


    if (line[0] == '0')
        Extract_Function_Name(line);

    if (line[0] == ' ') {
// this line is an instruction assembly
        // instruction
        address = get_address();
        line_fname = last_func;
        objdump_address[address] =
                line_ctr;
        if (len_str >= 32) {
            string instr = line.substr(32, 999);
            vector<string> v{split(instr, ' ')};
            if (instr[0] == 'j')
                if (v[0].compare("jmpq") != 0) {
                    ui64 add = stoullhexa(v[1]);
                    int in_objdump = objdump_address[add];
                    objdump_file[in_objdump].
                            target = 1;
//cout << v[0] << " " << v[1] << endl;
                }
        }
//cout << "_2_ " << std::dec << len_str << " " << line_ctr << " " << last_func << " " <<
//line << " " << address << " " <<  std::hex << address << endl;
        return;
    }
}

void objdump_line::Extract_Function_Name(string line) {
    int inf = -1; // char poz of first '<'
    int sup = -1; // char poz of first '>'

    for (int i = 0; i < len_str; i++) {
        if (line[i] == '<')
            inf = i;
        if (line[i] == '>')
            sup = i;
    }
    if (inf == 17) { // txt name
        string fname;
        fname = line.substr(inf + 1, sup - inf - 1);
        // function name
        last_func = line_ctr;
        DEBUG_PRINT(string("_JEAN_FCT_NAME_ ") + string ("fdsfd"));
    }
    return;

}


ui64 objdump_line::get_address() {
    ui64 add = 0;
    for (int i = 2;; i++) {
        char c = str[i];
        if (c == ':')break;
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
    return (add);
}

