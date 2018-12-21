//
// Created by Jean Pourroy on 19/12/2018.
//

#include "Line_Objdump.h"
#include "Line.h"
#include "op_misc.h"
#include <iomanip>

int Line_Objdump::LINE_COUNTER = 0;                     //Start to read at line 0
string Line_Objdump::LAST_SYMB = "";
InputFile<Line_Objdump> *Line_Objdump::FILE_OBJ = NULL; // Done in the main function
std::unordered_map<uint64_t, int> Line_Objdump::objdump_address;


Line_Objdump::Line_Objdump(string line) : Line(line) {
    m_line_number = Line_Objdump::LINE_COUNTER;
    Line_Objdump::LINE_COUNTER++;

    //Search for the type of the line
    m_line_type = LINE_TYPE::EMPTY; //for the moment
    if (m_original_line.size() > 0) {
        analyse_current_line();
    }
}

uint64_t Line_Objdump::get_address() {
    uint64_t add = 0;
    for (int i = 2;; i++) {
        char c = m_original_line[i];
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


void Line_Objdump::analyse_current_line() {

    // -------- -------- ---------
    // -------- FUNCTION ---------
    // -------- -------- ---------
    //0000000000401690 <_init>:
    if (m_original_line[0] == '0') {
        // this line is a function name
        m_line_type = LINE_TYPE::FUNCTION;

        int inf = -1; // char poz of first '<'
        int sup = -1; // char poz of first '>'
        for (int i = 0; i < m_original_line.size(); i++) {
            if (m_original_line[i] == '<')
                inf = i;
            if (m_original_line[i] == '>')
                sup = i;
        }
        if (inf == 17) { // txt name
            m_symbole_name = m_original_line.substr(inf + 1, sup - inf - 1);
            LAST_SYMB = m_symbole_name;
        }
    }

        // ------ ----------- ---------
        // ------ INSTRUCTION ---------
        // ------ ----------- ---------
        //  401694:	48 8b 05 5d 29 20 00 	mov    0x20295d(%rip),%rax
    else if (m_original_line[0] == ' ') {
        // this m_original_line is an instruction assembly
        m_line_type = LINE_TYPE::INSTRUCTION;
        m_memory_address = get_address();
        m_symbole_name = LAST_SYMB;
        objdump_address[m_memory_address] = LINE_COUNTER;


        if (m_original_line.size() >= 32) {
            string instr = m_original_line.substr(32, 999);
            m_assembly_instruction = instr;
            vector<string> v{my_split(instr, ' ')};
            if (instr[0] == 'j')
                if (v[0].compare("jmpq") != 0) {
                    uint64_t add = stoullhexa(v[1]);
                    int in_objdump = objdump_address[add];
                }
        }
    } else {
        m_line_type = LINE_TYPE::NORMAL;
        m_symbole_name = "";
    }
}


ostream &operator<<(ostream &cout, Line_Objdump L) {
    if (!(Line::LINE_TYPE::EMPTY == L.m_line_type)) {
        cout << *static_cast<Line * >(&L); //Base class cout
        cout << left << setw(101) << L.m_assembly_instruction.substr(0, 100);
    } else {
        cout << endl;
    }
    return cout;
}

// ------ GETTERS  AND  SETTERS ----

const string &Line_Objdump::get_assembly_instruction() const {
    return m_assembly_instruction;
}

void Line_Objdump::set_assembly_instruction(const string &m_assembly_instruction) {
    Line_Objdump::m_assembly_instruction = m_assembly_instruction;
}

const unordered_map<uint64_t, int> &Line_Objdump::getObjdump_address() {
    return objdump_address;
}

void Line_Objdump::setObjdump_address(const unordered_map<uint64_t, int> &objdump_address) {
    Line_Objdump::objdump_address = objdump_address;
}

InputFile<Line_Objdump> *Line_Objdump::getFILE_OBJ() {
    return FILE_OBJ;
}

void Line_Objdump::setFILE_OBJ(InputFile<Line_Objdump> *FILE_OBJ) {
    Line_Objdump::FILE_OBJ = FILE_OBJ;
}