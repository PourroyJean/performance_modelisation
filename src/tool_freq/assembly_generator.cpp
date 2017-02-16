#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>
#include <sched.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <iostream>
#include <map>


using namespace std;

//Used to write in files
//2 file pointers: one for SOURCE, one for HEADER file
#define PC(s) fprintf(FPC, s);
#define PH(s) fprintf(FPH, s);
FILE * FPC;
FILE * FPH;




void init_generator(){
    FPC = fopen("assembly_generated.c", "w+");
    FPH = fopen("assembly_generated.h", "w+");
}

int main(int argc, char **argv) {

    init_generator();

    std::map<int,std::string> mymap;

    return 0;

}