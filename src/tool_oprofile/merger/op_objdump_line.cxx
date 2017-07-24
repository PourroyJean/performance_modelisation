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
    line_ctr++; //Number of the line in the txt file
    str = line;


    len_str = line.length();

    //Current line is empty
    if (len_str < 0)
        return;

    //Current line is a function header
    if (line[0] == '0') {
        Extract_Function_Name(line);
        DEBUG << ("New function retired " + to_string(line_ctr) + "\n");

        return;
    }

    //Current line is an assembly line
    if (line[0] == ' ') {
        //The first chunk is the instruction's address
        address = get_address();
        DEBUG << ("ADDRESS IS: " + to_string(address) + "  -  ");

        //line_fname is the line number of the assembly's function
        line_fname = last_func;
        DEBUG << "line_fname is:  " + to_string(line_fname) + "\n";
        objdump_address[address] = line_ctr;

        //Check if the line is not a comment or something else that is not an assembly instruction
        if (len_str < 32)
            return;

        //The assembly instruction is located after the 32th char:
        //      Example:   [0]... [32] jmpq   *0x201a8a(%rip)        # 602020 <_GLOBAL_OFFSET_TABLE_+0x20>
        string ass_instr = line.substr(32, 999);
        vector<string> v {split(ass_instr, ' ')};


        //The instruction is a JUMP
        if (ass_instr[0] == 'j') {

        //Here can be implmented all the jump comparison
            if (v[0].compare("jmpq") != 0) {
                ui64 add = stoullhexa(v[1]);
                int in_objdump = objdump_address[add];
                objdump_file[in_objdump].target = 1;
                DEBUG << "JUMP FOUND " <<  v[0] << " " << v[1] << endl;
            }

        }

        return;
    }
}

void objdump_line::Extract_Function_Name(string line) {
    int inf = -1; // char poz of first '<'
    int sup = -1; // char poz of first '>'

    //Get the position of the .... <function_name> ....
    for (int i = 0; i < len_str; i++) {
        if (line[i] == '<')
            inf = i;
        if (line[i] == '>')
            sup = i;
    }

    //The function name is between inf and sup
    if (inf == 17) { // txt name
        string fname;
        fname = line.substr(inf + 1, sup - inf - 1);
        last_func = line_ctr;
        DEBUG << "Fct_name: " + fname + " Last func" + to_string(last_func) + " Adress " + to_string(address) + "\n";
    }
    return;

}


//When a line is starting by ' '. We go through le line until we find ':'
ui64 objdump_line::get_address() {
    ui64 address = 0;
    for (int i = 2;; i++) {
        //We go char by char
        char c = str[i];

        //Did we find the end of the address ?
        if (c == ':' || c == 0)
            break;

        if ((c >= '0') and (c <= '9')) {
            address *= 16;
            address += (c - '0');
        }
        if ((c >= 'a') and (c <= 'f')) {
            address *= 16;
            address += (c - 'a' + 10);
        }
        if ((c >= 'A') and (c <= 'F')) {
            address *= 16;
            address += (c - 'A' + 10);
        }
    }
    return (address);
}

