//
// Created by Jean Pourroy on 21/07/2017.
//

#include "op_oprofile_line.h"
#include "op_misc.h"
#include "op_objdump_line.h"
#include <string.h>
#include <iostream>
#include <iomanip>


int oprofile_line::static_line_counter = 0;
int oprofile_line::static_line_of_last_func = 0;

// OBJECT FILE lines
vector<oprofile_line> oprofile_file;

// map address of instructions of OBJECT FILE to line inside oprofile_file
// used for branch instruction to recover the line of the branch
std::unordered_map<ui64, int> oprofile_address;


oprofile_line::oprofile_line(string line) {

    //Initialisation
    event_cpu_clk = 0;
    event_inst_retired = 0;
    static_line_counter++;
    line_original_string = line;

    if (len_str = line.length() <= 0)
        return;

    //If line begins with '0' then its a function summary line
    if (line[0] == '0') {
        type = Type::FUNC;
        static_line_of_last_func = static_line_counter;
    }

        //If line begins with ' ' then its an instruction line counter
    else if (line[0] == ' ') { // instruction
        type = Type::INST;
        line_fname = static_line_of_last_func;
        oprofile_address[memory_address] = static_line_counter;
    }

        //Or this line is not useful for the profile
    else {
        type = Type::NO;
        return;
    }

    //If we reach this point then the line is an instruction on a function
    get_address();
    get_event_counters();


    DEBUG << "_OP_CONSTR_ "
          << (type == Type::FUNC ? " FUNC " : " INST ") << " "
          << static_line_counter << " "
          << '(' << static_line_of_last_func << ')' << " "
          << memory_address << " 0x" << std::hex << memory_address << " " << std::dec
          << " Event 1,2: " << event_cpu_clk << " " << event_inst_retired << endl;
//          << line_original_string << endl << endl;


}

void oprofile_line::get_address() {
    ui64 add = 0;
    for (int i = 2;; i++) {
        char c = line_original_string[i];
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
    memory_address = add;
}

void oprofile_line::get_event_counters() {
    vector<string> v{split(line_original_string, ' ')};
    event_cpu_clk               = stol(v[1]);
    event_cpu_clk_percentage    = stof(v[2]);
    event_inst_retired          = stol(v[3]);
//    cout << "JANNOU " << event_cpu_clk << endl;
}

