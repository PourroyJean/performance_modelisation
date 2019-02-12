//
// Created by Jean Pourroy on 19/12/2018.
//

#include "Line.h"

using namespace std;

Line::Line(const string &original_line) : m_original_line(original_line) {
    m_line_number = 0;
    m_event_cpu_clk = 0;
    m_event_cpu_clk_percentage = 0;
    m_event_inst_retired = 0;
    m_memory_address = 0;
    m_symbole_name = "no_name";
}


string Line::type_to_str(int type) {
    if (LINE_TYPE::NORMAL == type) {
        return "NORMAL";
    } else if (LINE_TYPE::FUNCTION == type) {
        return "FUNCTION";
    } else if (LINE_TYPE::INSTRUCTION == type) {
        return "INSTRUCTION";
    } else if (LINE_TYPE::EMPTY == type) {
        return "EMPTY";
    } else {
        return "error";
    }
}


std::ostream &operator<<(std::ostream &cout, Line &l) {
    cout << setw(5) << l.m_line_number
         << setw(13) << l.type_to_str(l.m_line_type)
         << setw(31) << l.m_symbole_name.substr(0, 20)
         << setw(21) << l.m_memory_address
         << setw(8) << l.m_event_cpu_clk
         << setw(8) << l.m_event_cpu_clk_percentage
         << setw(8) << l.m_event_inst_retired;
    return cout;
}


// ---------------------------------
// ---------------------------------
// ------ GETTERs  AND  SETTERS ----
// ---------------------------------
// ---------------------------------

const string &Line::get_original_line() const {
    return m_original_line;
}

uint64_t Line::get_event_cpu_clk() const {
    return m_event_cpu_clk;
}

void Line::set_event_cpu_clk(uint64_t m_event_cpu_clk) {
    Line::m_event_cpu_clk = m_event_cpu_clk;
}

double Line::get_event_cpu_clk_percentage() const {
    return m_event_cpu_clk_percentage;
}

void Line::set_event_cpu_clk_percentage(double m_event_cpu_clk_percentage) {
    Line::m_event_cpu_clk_percentage = m_event_cpu_clk_percentage;
}

uint64_t Line::get_event_inst_retired() const {
    return m_event_inst_retired;
}

void Line::set_event_inst_retired(uint64_t m_event_inst_retired) {
    Line::m_event_inst_retired = m_event_inst_retired;
}

uint64_t Line::get_memory_address() const {
    return m_memory_address;
}

void Line::set_memory_address(uint64_t m_memory_address) {
    Line::m_memory_address = m_memory_address;
}

const string &Line::get_symbole_name() const {
    return m_symbole_name;
}

int Line::get_line_number() const {
    return m_line_number;
}

void Line::set_line_number(int m_line_number) {
    Line::m_line_number = m_line_number;
}

void Line::set_symbole_name(const string &m_symbole_name) {
    Line::m_symbole_name = m_symbole_name;
}

int Line::get_line_type() const {
    return m_line_type;
}

void Line::set_line_type(int m_line_type) {
    Line::m_line_type = m_line_type;
}



