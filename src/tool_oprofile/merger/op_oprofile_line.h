//
// Created by Jean Pourroy on 21/07/2017.
//

#ifndef PERFORMANCE_MODELISATION_OPROFILE_LINE_H
#define PERFORMANCE_MODELISATION_OPROFILE_LINE_H


#include <string>
#include <vector>
#include <unordered_map>
#include <misc.h>


using namespace std;


class oprofile_line {
public:
    oprofile_line(string);

    string line_original_string;
    string m_function_name; //Only if its a function
    string m_binary_name;


    void get_event_counters();

    void get_address();

    enum class Type {
        NO, FUNC, INST
    };
    Type type;                              //0=NO,  1=FUNC : 2=INST
    ui64 memory_address;                    //Memory address of the instruction

    ui64 event_cpu_clk;                     //Clock cycles when not halted
    double event_cpu_clk_percentage;          //Clock cycles when not halted
    ui64 event_inst_retired;                //number of instructions retired

private:
    int cur_line;
    int len_str;                            //Size of the line
    int line_fname;
    static int static_line_counter;         //Current line number in the file
    static int static_line_of_last_func;    //Line number of the last function we found
};


//Oprofile file is converted in a vector of oprofile_line
extern vector<oprofile_line> oprofile_file;

// map address of instructions of OBJECT FILE to line inside oprofile_file
// used for branch instruction to recover the line of the branch
extern std::unordered_map<ui64, int> oprofile_address;


#endif //PERFORMANCE_MODELISATION_OPROFILE_LINE_H
