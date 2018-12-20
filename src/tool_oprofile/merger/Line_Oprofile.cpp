//
// Created by Jean Pourroy on 19/12/2018.
//

#include "Line_Oprofile.h"
#include "op_misc.h"
#include "Line_Objdump.h"


int Line_Oprofile::LINE_COUNTER = 0;
int Line_Oprofile::LAST_FCT_CTR = 0;
InputFile<Line_Oprofile> *Line_Oprofile::FILE_OPR = NULL;

std::unordered_map<uint64_t, int> Line_Oprofile::oprofile_address;


Line_Oprofile::Line_Oprofile(const string &m_original_line) : Line(m_original_line) {
    Line_Oprofile::LINE_COUNTER++;
    m_function_line;
    //Search for the type of the line
    m_line_type = LINE_TYPE::NORMAL; //for the moment
    if (m_original_line.size() > 0) {
        analyse_current_line();
    }

}

/*
00402961 20        0.6553  23        0.3100  assembly                 aloop
  00402961 6        30.0000  2         8.6957
  00402964 3        15.0000  3        13.0435
 */
void Line_Oprofile::analyse_current_line() {
    if (m_original_line[0] == '0') {
        m_line_type = Line::LINE_TYPE::FUNCTION;
        Line_Oprofile::LAST_FCT_CTR = Line_Oprofile::LINE_COUNTER;
    } else if (m_original_line[0] == ' ') { // instruction
        m_line_type = Line::LINE_TYPE::INSTRUCTION;
        get_address_and_ctrs();
        oprofile_address[m_memory_address] = Line_Oprofile::LINE_COUNTER;
    }

    m_function_line = Line_Oprofile::LAST_FCT_CTR;
}

void Line_Oprofile::get_address_and_ctrs() {
    //TODO refactor this part
    uint64_t add = 0;
    for (int i = 2;; i++) {
        char c = m_original_line[i];
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

    m_memory_address = add;
    vector<string> v{my_split(m_original_line, ' ')};
    m_event_cpu_clk = stol(v[1]);
    m_event_inst_retired = stol(v[3]);
    int in_objdump = Line_Objdump::objdump_address[add] - 1;
    if (in_objdump > 0) {
        Line_Objdump::FILE_OBJ->m_lines[in_objdump]->setM_event_cpu_clk(m_event_cpu_clk);
        Line_Objdump::FILE_OBJ->m_lines[in_objdump]->setM_event_inst_retired(m_event_inst_retired);
    }
}

void Line_Oprofile::print(std::ostream &sortie) const {
    Line::print(cout); //print the base class
    cout << left << setw(60) << this->m_function_line;
}
