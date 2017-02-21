//
// Created by Jean Pourroy on 20/02/2017.
//

#include <iostream>
#include "tool_freq_generators.h"
#include "tool_freq_misc.h"



//Write to Cpp or Header file
#define WC(s) fprintf(P_FPC, s "\n");
#define WH(s) fprintf(P_FPH, s "\n");

void Tool_freq_generators::generate_source(){
    WC("#include \"assembly_generated.h\"");
    WC("#include <string.h>");
    WC("#include <stdio.h>");
    WC("#include <iostream>");
    WC("using namespace std;");
    WC("int main (){");
    WC("cout << \"coucou\";");
    WC("}")
    WC("")

}
void Tool_freq_generators::generate_header(){
    WH("#ifndef ASSEMBLY_GENERATED_H")
    WH("#define ASSEMBLY_GENERATED_H")
    WH("void assembly_function ();")
    WH("#endif //ASSEMBLY_GENERATED_H")


}

string Tool_freq_generators::generate_assembly(Tool_freq_parameters *t) {
    DEBUG_PRINT("Generating assembly...\n");

    generate_source();
    generate_header();



    return "";
}

Tool_freq_generators::Tool_freq_generators() {
    P_FPC = fopen("assembly_generated.cpp", "w+");
    P_FPH = fopen("assembly_generated.h", "w+");

}

Tool_freq_generators::~Tool_freq_generators() {
    fclose(P_FPC);
    fclose(P_FPH);
}
