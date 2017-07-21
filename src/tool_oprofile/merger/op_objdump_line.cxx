//
// Created by Jean Pourroy on 21/07/2017.
//

#include "op_objdump_line.h"
#include "op_misc.h"


int objdump_line::line_ctr = 0;
int objdump_line::last_func = 0;

//TODO deplacer dehors
// OBJECT FILE lines
vector<objdump_line> objdump_file;
// map address of instructions of OBJECT FILE to line inside objdump_file
// used for branch instruction to recover the line of the branch
std::unordered_map<ui64,int> objdump_address;

objdump_line::objdump_line(string line) {
    ctr1 = 0;
    ctr2 = 0;
    target = 0;
    line_ctr++;
    string fname;
    str = line;
    len_str = line.length();
    int type = 0; // nothing usefull
    int inf = -1; // char poz of first '<'
    int sup = -1; // char poz of first '>'
    if (len_str > 0) {
        if (line[0] == '0') {
// this line is a function name
            type = 1;
            for (
                    int i = 0;
                    i < len_str;
                    i++) {
                if (line[i] == '<')
                    inf = i;
                if (line[i] == '>')
                    sup = i;
            }
            if (inf == 17) { // txt name
                fname = line.substr(inf + 1, sup - inf - 1);
                type = 1; // function name
                last_func = line_ctr;
//cout << "_1_ " << " " << inf << " " << sup << " " << fname << endl;
            }
            return;
        }
        if (line[0] == ' ') {
// this line is an instruction assembly
            type = 2; // instruction
            address = get_address();
            line_fname = last_func;
            objdump_address[address] =
                    line_ctr;
            if (len_str >= 32) {
                string instr = line.substr(32, 999);
                vector <string> v{split(instr, ' ')};
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
}


ui64 objdump_line::get_address()
{
    ui64 add=0;
    for(int i=2;;i++){
        char c=str[i];
        if(c==':')break;
        if(c==0)break; // should not occur
        if((c>='0')and(c<='9')){add*=16;add+=(c-'0');}
        if((c>='a')and(c<='f')){add*=16;add+=(c-'a'+10);}
        if((c>='A')and(c<='F')){add*=16;add+=(c-'A'+10);}
    }
    return(add);
}

