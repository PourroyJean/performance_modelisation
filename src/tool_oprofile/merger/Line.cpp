//
// Created by Jean Pourroy on 19/12/2018.
//

#include "Line.h"


Line::Line(const string &original_line) : m_original_line(original_line) {
//    cout << "Construction d'une LINE" << original_line;
    m_event_cpu_clk = 0;
    m_event_cpu_clk_percentage = 0;
    m_event_inst_retired = 0;


}


string Line::type_to_str(int type) const {
    if (LINE_TYPE::NORMAL == type) {
        return "NORMAL";
    } else if (LINE_TYPE::FUNCTION == type) {
        return "FUNCTION";
    } else if (LINE_TYPE::INSTRUCTION == type) {
        return "INSTRUCTION";
    } else {
        return "error";
    }
}


std::ostream &operator<<(std::ostream &sortie, const Line &l) {
    l.print(sortie); //print the base class
    return sortie;
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

void Line::setM_event_cpu_clk(uint64_t m_event_cpu_clk) {
    Line::m_event_cpu_clk = m_event_cpu_clk;
}

uint64_t Line::get_event_cpu_clk_percentage() const {
    return m_event_cpu_clk_percentage;
}

void Line::setM_event_cpu_clk_percentage(uint64_t m_event_cpu_clk_percentage) {
    Line::m_event_cpu_clk_percentage = m_event_cpu_clk_percentage;
}

uint64_t Line::get_event_inst_retired() const {
    return m_event_inst_retired;
}

void Line::setM_event_inst_retired(uint64_t m_event_inst_retired) {
    Line::m_event_inst_retired = m_event_inst_retired;
}

uint64_t Line::get_memory_address() const {
    return m_memory_address;
}

void Line::setM_memory_address(uint64_t m_memory_address) {
    Line::m_memory_address = m_memory_address;
}

const string &Line::get_symbole_name() const {
    return m_symbole_name;
}

void Line::setM_symbole_name(const string &m_symbole_name) {
    Line::m_symbole_name = m_symbole_name;
}




