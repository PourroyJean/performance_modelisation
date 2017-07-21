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

    string getstr() { return str; };

    ui64 get_address();

    string str;
    ui64 ctr1;
    ui64 ctr2;
    ui64 address;
    int type;  // 1=func : 2=inst
    int len_str;
    static int line_ctr;
    static int last_func;




private:
    int cur_line;
    int line_fname;
    int target; // branch target 0:not target 1=target
};

//TODO deplacer dehors
// OBJECT FILE lines
extern vector<objdump_line> objdump_file;
// map address of instructions of OBJECT FILE to line inside objdump_file
// used for branch instruction to recover the line of the branch
extern std::unordered_map<ui64,int> objdump_address;





#endif //PERFORMANCE_MODELISATION_OP_OBJDUMP_LINE_H
