#include <getopt.h>
#include <cstdlib>
#include "tool_freq_misc.h"
#include <string.h>
#include <iostream>


Tool_freq_parameters::Tool_freq_parameters() {
    P_SIMD = SCALAR;
    P_OPERATION_TYPE = ADD;
    P_OPERATION_NB = 1;
    P_BIND = 0;
    P_WIDTH = 64;
    P_VERBOSE = false;
    P_HELP = false;

//    mapParameter[SCALAR] =  "SCALAR";

};


void Tool_freq_parameters::parse_arguments(int argc, char **argv) {
    char option;


    while ((option = getopt(argc, argv, "P:B:O:N:W:vh")) != -1) {
        int ioptarg;
//        char * dasa =  (char *) (string) mapParameter[SCALAR];
        switch (option) {
            case 'P':
                if (!strcmp(optarg, mapParameter[SCALAR].c_str())) {
                    this->P_SIMD = distance(mapParameter.begin(), mapParameter.find(AVX));
                }
                break;
// else if (!strcmp(optarg, mapParameter[SSE])) {
//                    this->P_SIMD = SSE;
//                } else if (!strcmp(optarg, mapParameter[AVX])) {
//                    this->P_SIMD = AVX;
//                } else {
//                    printf("/!\\ WRONG PARALLELISM OPTION: %s\n", optarg);
//                    exit(EXIT_FAILURE);
//                }
//                break;
//            case 'B':
//                if ((atoi(optarg) >= 0) && (4096 >= atoi(optarg))) {
//                    this->P_BIND = atoi(optarg);
//                } else {
//                    printf("/!\\ WRONG BINDING OPTION: %s\n", optarg);
//                    exit(EXIT_FAILURE);
//                }
//                break;
//            case 'W':
//                ioptarg = atoi(optarg);
//                if (ioptarg == 64 || ioptarg == 128 || ioptarg == 256 || ioptarg == 512) {
//                    this->P_WIDTH = ioptarg;
//                } else {
//                    printf("/!\\ WRONG WIDTH OPTION: %s\n", optarg);
//                    exit(EXIT_FAILURE);
//                }
//                break;
//            case 'N':
//                ioptarg = atoi(optarg);
//                if (ioptarg == 1 || ioptarg == 2 || ioptarg == 3) {
//                    this->P_OPERATION_NB = ioptarg;
//                } else {
//                    printf("/!\\ WRONG NUMBER OF OPERATION OPTION: %s\n", optarg);
//                    exit(EXIT_FAILURE);
//                }
//                break;
            case 'v':
                this->P_VERBOSE = true;
                break;
//            case 'O':
//                if (!strcmp(optarg, mapParameter[ADD])) {
//                    this->P_OPERATION_TYPE = ADD;
//                } else if (!strcmp(optarg, mapParameter[MUL])) {
//                    this->P_OPERATION_TYPE = MUL;
//                } else if (!strcmp(optarg, mapParameter[FMA])) {
//                    this->P_OPERATION_TYPE = FMA;
//                } else {
//                    printf("/!\\ WRONG OPERATION OPTION: %s\n", optarg);
//                    exit(EXIT_FAILURE);
//
//                }
//                break;
//            case 'h':
//                this->P_HELP = true;
//                break;
            default:
//                usage();
                exit(EXIT_FAILURE);
        }
    }


}


void Tool_freq_parameters::parameter_summary(){
    cout << "\t -P (parallelism)    " << this->P_SIMD;
    cout << "\t -W (width)          " << this->P_WIDTH;
    cout << "\t -O (type operation) " << this->P_OPERATION_TYPE;
    cout << "\t -N (nb operation)   " << this->P_OPERATION_NB;
    cout << "\t -B (core binding)   " << this->P_BIND;
}


void Tool_freq_parameters::check_arguments() {
    if (this->P_SIMD == SCALAR && this->P_WIDTH != 64) {
        printf("/!\\ SCALAR should be use with 64 bits register\n");
        exit(EXIT_FAILURE);
    }
    if (this->P_SIMD == SCALAR && this->P_OPERATION_TYPE == FMA) {
        printf("/!\\ SCALAR should not be use with FMA operation\n");
        exit(EXIT_FAILURE);
    }

}