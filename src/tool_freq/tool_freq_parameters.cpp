#include <getopt.h>
#include <cstdlib>
#include "tool_freq_parameters.h"
#include <string.h>
#include <sstream>      // std::istringstream
#include <iostream>
#include "tool_freq_misc.h"


Tool_freq_parameters::Tool_freq_parameters() {
    P_SIMD = PARAM_SIMD;
    P_WIDTH = PARAM_WIDTH;
    P_OPERATIONS = PARAM_OPERATIONS;
    P_BIND = PARAM_BIND;
    P_DEPENDENCY = PARAM_DEPENDENCY;
    P_PRECISION = PARAM_PRECISION;
    P_VERBOSE = PARAM_VERBOSE;
    P_HELP =  false;
    P_LOOP_SIZE = PARAM_LOOP_SIZE;
};


void Tool_freq_parameters::parse_arguments(int argc, char **argv) {
    const char *const short_opts = "I:W:O:B:D:P:L:vh";
    const option long_opts[] = {
            {"instruction", required_argument, nullptr, 'I'},
            {"width",       required_argument, nullptr, 'W'},
            {"operations",  required_argument, nullptr, 'O'},
            {"bind",        required_argument, nullptr, 'B'},
            {"dependency",  required_argument, nullptr, 'D'},
            {"precision",   required_argument, nullptr, 'P'},
            {"loopsize",    required_argument, nullptr, 'L'},
            {"verbose",     no_argument,       nullptr, 'v'},
            {"help",        no_argument,       nullptr, 'h'},
            {nullptr, 0,                       nullptr, 0}
    };

    char option;
    string tmp_str;
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
                tmp_str = optarg;
                if (!tmp_str.compare("true") || !tmp_str.compare("false")){
                    bool b;
                    istringstream(optarg) >> std::boolalpha >> b;
                    P_DEPENDENCY = b;
                }
                else{
                    printf("/!\\ WRONG DEPENDENCY OPTION: %s\n", optarg);
                    exit(EXIT_FAILURE);
                }

                break;
            case 'P':
                tmp_str = optarg;
                if (!tmp_str.compare("single") || !tmp_str.compare("double")) {
                    this->P_PRECISION = optarg;
                } else {
                    printf("/!\\ WRONG PRECISION OPTION: %s\n", optarg);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'L':
                ioptarg = atoi(optarg);
                if (ioptarg > 0  && ioptarg < 10000000) {
                    this->P_LOOP_SIZE = ioptarg;
                } else {
                    printf("/!\\ WRONG LOOP SIZE OPTION OPTION: %s\n", optarg);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'v':
                this->P_VERBOSE = true;
                break;
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
    for (char &c : P_OPERATIONS) {
        if (c != 'a' && c != 'A' && c != 'f' && c != 'F' && c != 'm' && c != 'M') {
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
    cout << "\t -P (op precision)     " << this->P_PRECISION << endl;
    cout << "\t -L (loop size)        " << this->P_LOOP_SIZE<< endl;

}


void Tool_freq_parameters::help() {
    usage();
    cout << " " << endl;
    cout << "This tool should be launched with the following parameters ([] = default):\n";
    cout << "\t -I,--instruction    [SCALAR], SSE, AVX" << endl;
    cout << "\t -W, --width         [64] 128 256 512" << endl;
    cout << "\t -O                  [ADD] MUL FMA" << endl;
    cout << "\t -B,--binding        [0] 1 2 ... NbCore\n";
    cout << "\t -P,--precision      single [double]\n";
    cout << "\t -L,--loopsize       [200] \n";
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