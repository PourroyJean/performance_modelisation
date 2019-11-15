//
// Created by Jean Pourroy on 19/12/2018.
//

#include <sstream>
#include "Line_Oprofile.h"
#include "op_misc.h"
#include "Line_Objdump.h"


int Line_Oprofile::LINE_COUNTER = 0;                                //Start at 0
int Line_Oprofile::LINE_LAST_FCT = -1;                              //Start with incorrect index
InputFile<Line_Oprofile> *Line_Oprofile::FILE_OPR = NULL;           //The file is set in the main function
std::unordered_map<uint64_t, int> Line_Oprofile::oprofile_address;


// CONSTRUCTOR //
Line_Oprofile::Line_Oprofile(const string &m_original_line) : Line(m_original_line) {
    m_line_number = Line_Oprofile::LINE_COUNTER;
    Line_Oprofile::LINE_COUNTER++;

    //Search for the type of the line
    m_line_type = LINE_TYPE::EMPTY; //for the moment
    if (m_original_line.size() > 0) {
        analyse_current_line();
    }

}

/*
vma       samples  %       samples  %       app name  symbol name
00402961  20       0.6553  23       0.3100  assembly  aloop
  00402961 6        30.0000  2         8.6957
  00402964 3        15.0000  3        13.0435
 */
void Line_Oprofile::analyse_current_line() {

    // -------- -------- ---------
    // -------- FUNCTION ---------
    // -------- -------- ---------
    if (m_original_line[0] == '0') {
        m_line_type = Line::LINE_TYPE::FUNCTION;
        Line_Oprofile::LINE_LAST_FCT = m_line_number;

        //get symbole name (aloop)
        vector<string> v{my_split(m_original_line, ' ')};
        m_application_name = v[5];
        m_symbole_name = v[6];

        extract_address();
        extract_counters();

    }

        // ------ ----------- ---------
        // ------ INSTRUCTION ---------
        // ------ ----------- ---------
    else if (m_original_line[0] == ' ') {
        m_line_type = Line::LINE_TYPE::INSTRUCTION;
        extract_address();
        extract_counters();
        m_application_name = to_string(m_function_line);

        //Update the objdump counters
        int in_objdump = Line_Objdump::objdump_address[m_memory_address] - 1;
        if (in_objdump > 0) {
            Line_Objdump::getFILE_OBJ()->get_lines()[in_objdump]->set_event_cpu_clk(m_event_cpu_clk);
            Line_Objdump::getFILE_OBJ()->get_lines()[in_objdump]->set_event_inst_retired(m_event_inst_retired);
        }

        //Update the map
        oprofile_address[m_memory_address] = Line_Oprofile::LINE_COUNTER;

        //If a function was found before, the instruction belongs to it
        //We give to the instruction: the function line, its symbole name and its application name
        if (Line_Oprofile::LINE_LAST_FCT >= 0) {
            m_function_line = Line_Oprofile::LINE_LAST_FCT;
            m_symbole_name = Line_Oprofile::FILE_OPR->get_lines_vector()[m_function_line]->get_symbole_name();
            m_application_name = Line_Oprofile::FILE_OPR->get_lines_vector()[m_function_line]->m_application_name;
        }
    } else {
        m_line_type = Line::LINE_TYPE::NORMAL;
    }

}

//Example:
//vma      samples  %        samples  %        app name                 symbol name
//00401d79 2501     81.9463  6991     94.2310  assembly                 myBench
void Line_Oprofile::extract_counters() {
    vector<string> v{my_split(m_original_line, ' ')};
    m_event_cpu_clk = stol(v[1]);
    m_event_cpu_clk_percentage = stod(v[2]);
    m_event_inst_retired = stol(v[3]);
}

void Line_Oprofile::extract_address() {
    vector<string> v{my_split(m_original_line, ' ')};
    std::stringstream stream;
    uint64_t add;
    stream << v[0];
    stream >> std::hex >> add;
    //Is it a /no-vmlinux address ?
    if (add > 46744072464018238) {
        add = 0;
    }
    m_memory_address = add;
}

std::ostream &operator<<(std::ostream &cout, Line_Oprofile &L) {
    cout << *static_cast<Line * >(&L); //Base class cout
    cout << setw(15) << L.m_application_name;
    return cout;
}

void Line_Oprofile::print_resume() {
    cout
            << "============== OPROFILE SYMBOLE NAME  WITH MORE THAN 1 CYCLES ================="
            << endl
            << "===============================================================================\n"
            << endl;


    for (Line_Oprofile *current_line : FILE_OPR->get_lines()) {
        if (current_line->m_line_type == Line::LINE_TYPE::NORMAL) {
            DEBUG << "_0_";
            cout << current_line->m_original_line << endl;
        } else if (current_line->m_line_type == Line::LINE_TYPE::FUNCTION) {
            if (current_line->m_event_cpu_clk > 0.1) {
                DEBUG << "_1_";
                cout << current_line->m_original_line << endl;
            }
        }
    }
    cout
            << "===============================================================================\n"
            << endl;

}


// ------ GETTERS  AND  SETTERS ----

const string &Line_Oprofile::get_application_name() const {
    return m_application_name;
}

void Line_Oprofile::set_application_name(const string &m_application_name) {
    Line_Oprofile::m_application_name = m_application_name;
}

InputFile<Line_Oprofile> *Line_Oprofile::getFILE_OPR() {
    return FILE_OPR;
}

void Line_Oprofile::setFILE_OPR(InputFile<Line_Oprofile> *FILE_OPR) {
    Line_Oprofile::FILE_OPR = FILE_OPR;
}


