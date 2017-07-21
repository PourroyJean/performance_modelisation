//
// Created by Jean Pourroy on 21/07/2017.
//

#ifndef PERFORMANCE_MODELISATION_OPROFILE_LINE_H
#define PERFORMANCE_MODELISATION_OPROFILE_LINE_H


#include <string>
#include <vector>
#include <unordered_map>



typedef uint64_t ui64;
typedef uint32_t ui32;

using namespace std;

class oprofile_line {
public:
    oprofile_line(string);

    string getstr() { return str; };

    void get_address_and_ctrs();

    string str;
    int type;  // 1=func : 2=inst
private:
    ui64 ctr1;
    ui64 ctr2;
    ui64 address;
    int cur_line;
    int len_str;
    int line_fname;
    static int line_ctr;
    static int last_func;
};


//TODO garder dans le main
// OBJECT FILE lines
extern vector<oprofile_line> oprofile_file;

// map address of instructions of OBJECT FILE to line inside oprofile_file
// used for branch instruction to recover the line of the branch
extern std::unordered_map<ui64,int> oprofile_address;


#endif //PERFORMANCE_MODELISATION_OPROFILE_LINE_H
