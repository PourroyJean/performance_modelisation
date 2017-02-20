#include <getopt.h>
#include <cstdlib>
#include "tool_freq_parameters.h"
#include <string.h>
#include <iostream>
#include "tool_freq_misc.h"


Tool_freq_parameters::Tool_freq_parameters() {
    P_SIMD = SCALAR;
    P_WIDTH = 64;
    P_OPERATIONS = "aaaa";
    P_BIND = -1;
    P_DEPENDENCY = false;
    P_VERBOSE = false;
    P_HELP = false;
};


void Tool_freq_parameters::parse_arguments(int argc, char **argv) {
    const char *const short_opts = "I:W:jkBvh";
    const option long_opts[] = {
            {"instruction", required_argument, nullptr, 'I'},
            {"width",       required_argument, nullptr, 'W'},
            {"operations",  required_argument, nullptr, 'O'},
            {"bind",        required_argument, nullptr, 'B'},
            {"dependency",  required_argument, nullptr, 'D'},
            {"verbose",     no_argument,       nullptr, 'v'},
            {"help",        no_argument,       nullptr, 'h'},
            {nullptr, 0,                       nullptr, 0}
    };

    char option;


    while ((option = getopt_long(argc, argv, short_opts, long_opts, nullptr)) != -1) {
        int ioptarg;
        switch (option) {
            case 'I':
                if (!strcmp(optarg, mapParameter[SCALAR].c_str())) {
                    this->P_SIMD = SCALAR;
                } else if (!strcmp(optarg, mapParameter[SSE].c_str())) {
                    this->P_SIMD = SSE;
                } else if (!strcmp(optarg, mapParameter[AVX].c_str())) {
                    this->P_SIMD = AVX;
                } else {
                    printf("/!\\ WRONG PARALLELISM OPTION: %s\n", optarg);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'W':
                ioptarg = atoi(optarg);
                if (ioptarg == 64 || ioptarg == 128 || ioptarg == 256 || ioptarg == 512) {
                    this->P_WIDTH = ioptarg;
                } else {
                    printf("/!\\ WRONG WIDTH OPTION: %s\n", optarg);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'O':
                P_OPERATIONS = optarg;
                break;
            case 'B':
                if ((atoi(optarg) >= 0) && (4096 >= atoi(optarg))) {
                    this->P_BIND = atoi(optarg);
                } else {
                    printf("/!\\ WRONG BINDING OPTION: %s\n", optarg);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'D':
                P_DEPENDENCY = atoi(optarg);

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
            case 'h':
                this->P_HELP = true;
                break;
            default:
                usage();
                exit(EXIT_FAILURE);
        }
    }

}


void Tool_freq_parameters::check_operations() {
    cout << P_OPERATIONS;
    for (char &c : P_OPERATIONS) {
        if (c != 'a' && c != 'A' && c != 'f' && c != 'F' && c != 'm' && c != 'M'){
            cout << "/!\\ WRONG OPERATIONS " << c << endl;
        }
    }
}


void Tool_freq_parameters::parameter_summary() {
    cout << "\t -I (instructions)     " << mapParameter[this->P_SIMD] << endl;
    cout << "\t -W (width)            " << this->P_WIDTH << endl;
    cout << "\t -O <operationsl list> " << this->P_OPERATIONS << endl;
    cout << "\t -B (core binding)     " << this->P_BIND << endl;
    cout << "\t -D (op dependency)    " << std::boolalpha << this->P_DEPENDENCY << endl;

}


void Tool_freq_parameters::help() {
    usage();
    cout << " " << endl;
    cout << "This tool should be launched with the following parameters ([] = default):\n";
    cout << "\t -I,--instruction    [SCALAR], SSE, AVX" << endl;
    cout << "\t -W, --width         [64] 128 256 512" << endl;
    cout << "\t -O                  [ADD] MUL FMA" << endl;
    cout << "\t -B,--binding        [0] 1 2 ... NbCore\n";
    cout << "\t -h,--help\n";

}

void Tool_freq_parameters::check_arguments() {
    if (this->P_HELP) {
        this->help();
        exit(0);
    }
    if (this->P_SIMD == SCALAR && this->P_WIDTH != 64) {
        printf("/!\\ SCALAR should be use with 64 bits register\n");
        exit(EXIT_FAILURE);
    }
    if (this->P_VERBOSE) {
        printf("The frequency tool was launched with:\n");
        this->parameter_summary();
    };

    check_operations();

}