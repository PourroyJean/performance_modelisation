//
// Created by Jean Pourroy on 19/12/2018.
//

#ifndef PERFORMANCE_MODELISATION_PAP_LINE_H
#define PERFORMANCE_MODELISATION_PAP_LINE_H

#include <string>
#include <inttypes.h>   //TODO use cintypes
#include <iostream>
#include <iomanip>


class Line {

protected:

    const std::string m_original_line;       // Contains the original line (with spaces, etc...)
    int m_line_number;                      // Its number in the file from 0 .. N
    std::string m_symbole_name;             // Related symbole of the line (only make sens for INSTRUCTION and FUNCTION)
    uint64_t m_event_cpu_clk;               // First event gathered: number of cycles
    double m_event_cpu_clk_percentage;      // Its corresponding percentage
    uint64_t m_event_inst_retired;          // Second event gathered: number of instructions executed
    uint64_t m_memory_address;              // Memory address of the instruction
    int m_line_type;                        // Type of the instruction: NORMAL, FUNCTION, INSTRUCTION, EMPTY

public:

    //Construct a Line from the original line
    Line(const std::string &m_original_line);

    enum LINE_TYPE {
        NORMAL = 0, FUNCTION = 1, INSTRUCTION = 2, EMPTY = 3
    };

    static std::string type_to_str(int type);

    friend std::ostream &operator<<(std::ostream &sortie, Line &l);


    // ------ GETTERs  AND  SETTERS ----
    virtual void analyse_current_line() = 0;

    const std::string &get_original_line() const;

    uint64_t get_event_cpu_clk() const;

    void set_event_cpu_clk(uint64_t m_event_cpu_clk);

    double get_event_cpu_clk_percentage() const;

    void set_event_cpu_clk_percentage(double m_event_cpu_clk_percentage);

    uint64_t get_event_inst_retired() const;

    void set_event_inst_retired(uint64_t m_event_inst_retired);

    uint64_t get_memory_address() const;

    void set_memory_address(uint64_t m_memory_address);

    const std::string &get_symbole_name() const;

    void set_symbole_name(const std::string &m_symbole_name);

    void set_original_line(const std::string &m_original_line);

    int get_line_number() const;

    void set_line_number(int m_line_number);

    int get_line_type() const;

    void set_line_type(int m_line_type);

};

#endif //PERFORMANCE_MODELISATION_PAP_LINE_H
