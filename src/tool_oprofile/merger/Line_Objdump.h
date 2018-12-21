//
// Created by Jean Pourroy on 19/12/2018.
//

#ifndef PERFORMANCE_MODELISATION_PAP_LINE_OBJDUMP_H
#define PERFORMANCE_MODELISATION_PAP_LINE_OBJDUMP_H

#include <unordered_map>
#include "string"
#include "Line.h"
#include "InputFile.h"



//-----------------------------------------------------------------
// Class used to represent the objdump file output
// 0000000000401d79 <myBench>:
// 401d79:	c5 f3 58 d0          	vaddsd %xmm0,%xmm1,%xmm2
// 401d8d:	83 e8 01             	sub    $0x1,%eax
// 401d92:	48 89 85 50 ff ff ff 	mov    %rax,-0xb0(%rbp)
//-----------------------------------------------------------------

class Line_Objdump : public Line {
protected:
    static int LINE_COUNTER;                    // Count the current number of line read
    static std::string LAST_SYMB;               // Name of the last symbol fetched
    string m_assembly_instruction;              // vaddsd %xmm0,%xmm1,%xmm2
    static InputFile<Line_Objdump> *FILE_OBJ;   // Save all the lines in a vector implemented with the InputFile class

public:

    Line_Objdump(std::string line);

    virtual void analyse_current_line() override;

    uint64_t get_address(); //TODO already done in Oprofile class


    // map address of instructions of OBJECT FILE to line inside objdump_file
    // used for branch instruction to recover the line of the branch
    static std::unordered_map<uint64_t, int> objdump_address;

    friend ostream &operator<<(ostream &cout, const Line_Objdump L);

    // ------ GETTERS  AND  SETTERS ----

    static InputFile<Line_Objdump> *getFILE_OBJ();

    static void setFILE_OBJ(InputFile<Line_Objdump> *FILE_OBJ);

    const std::string &get_assembly_instruction() const;

    void set_assembly_instruction(const std::string &m_assembly_instruction);

    static const unordered_map<uint64_t, int> &getObjdump_address();

    static void setObjdump_address(const unordered_map<uint64_t, int> &objdump_address);
};


#endif //PERFORMANCE_MODELISATION_PAP_LINE_OBJDUMP_H
