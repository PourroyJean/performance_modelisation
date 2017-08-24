//
// Created by Jean Pourroy on 21/07/2017.
//

#ifndef PERFORMANCE_MODELISATION_OP_OBJDUMP_LINE_H
#define PERFORMANCE_MODELISATION_OP_OBJDUMP_LINE_H


#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

typedef uint64_t ui64;
typedef uint32_t ui32;

class objdump_line {
public:
    objdump_line (string s);

    string getstr() { return line_original_string; };

    ui64 get_address();



    string line_original_string;
    ui64 event_cpu_clk;
    ui64 event_inst_retired;
    ui64 address;
    int type;  // 1=func : 2=inst
    int len_str;
    static int line_ctr;
    static int last_func;




private:
    int cur_line;
    int line_fname;
    int target; // branch target 0:not target 1=target

    void Extract_Function_Name (string line);
};


// OBJECT FILE: each line represented by a objdump_line
extern vector<objdump_line> objdump_file;

// map address of instructions of OBJECT FILE to line inside objdump_file
// used for branch instruction to recover the line of the branch
// [MEMORY_ADDRESS]  --> LINE_NUMBER
extern std::unordered_map<ui64,int> objdump_address;





#endif //PERFORMANCE_MODELISATION_OP_OBJDUMP_LINE_H
