//
// Created by Jean Pourroy on 19/12/2018.
//

#ifndef PERFORMANCE_MODELISATION_PAP_LINE_H
#define PERFORMANCE_MODELISATION_PAP_LINE_H

#include <string>
#include <inttypes.h>
#include <iostream>
#include <iomanip>

using namespace std;

class Line {

protected:

//    const string &m_original_line; TODO
    string m_original_line;
    uint64_t m_event_cpu_clk;
    uint64_t m_event_cpu_clk_percentage;
    uint64_t m_event_inst_retired;
    uint64_t m_memory_address;
    string m_symbole_name;
    int m_line_type;

public:

    Line(const string &m_original_line);

    const string &get_original_line() const;

    uint64_t get_event_cpu_clk() const;

    void setM_event_cpu_clk(uint64_t m_event_cpu_clk);

    uint64_t get_event_cpu_clk_percentage() const;

    void setM_event_cpu_clk_percentage(uint64_t m_event_cpu_clk_percentage);

    uint64_t get_event_inst_retired() const;

    void setM_event_inst_retired(uint64_t m_event_inst_retired);

    uint64_t get_memory_address() const;

    void setM_memory_address(uint64_t m_memory_address);

    const string &get_symbole_name() const;

    void setM_symbole_name(const string &m_symbole_name);

    string type_to_str(int type) const;

//    friend std::ostream &operator<<(std::ostream &sortie, const Line &l) {

    virtual void print(std::ostream &sortie) const {
        sortie << setw(13) << this->type_to_str(this->m_line_type)
               << setw(8) << this->m_memory_address
               << setw(13) << this->m_symbole_name.substr(0, 12)
               << setw(8) << this->m_event_cpu_clk
               << setw(5) << this->m_event_cpu_clk_percentage
               << setw(8) << this->m_event_inst_retired;
        return;
    }


    enum LINE_TYPE {
        NORMAL = 0, FUNCTION = 1, INSTRUCTION = 2
    };


    virtual void analyse_current_line() = 0;
};

std::ostream &operator<<(std::ostream &sortie, const Line &l);

#endif //PERFORMANCE_MODELISATION_PAP_LINE_H
