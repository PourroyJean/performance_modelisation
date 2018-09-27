//
// Created by Jean Pourroy on 09/04/2018.
//

#ifndef PERFORMANCE_MODELISATION_BM_PARAMETERS_H
#define PERFORMANCE_MODELISATION_BM_PARAMETERS_H

#include <string>
#include <iostream>
#include <unistd.h>
#include "ezOptionParser.hpp"
#include "bm_misc.h"

using namespace ez;
using namespace std;

#ifdef __linux__
#define THEINT  unsigned long long
#else
#define THEINT  uint64_t
#endif


enum BENCH_TYPE {
    READ = 0, WRITE
};

enum BENCH_MODE {
    NORMAL = 10, SPECIAL, INDEXED,
};

enum DISP_MODE {
    BEST = 100, AVERAGE, TWO, ALL
};

enum DISP_UNIT {
    NS = 1000, GB, CY
};

static std::map<int, std::string> mapParameter = {{READ,    "READ"},
                                                  {WRITE,   "WRITE"},
                                                  {NORMAL,  "NORMAL"},
                                                  {SPECIAL, "SPECIAL"},
                                                  {INDEXED, "INDEXED"},
                                                  {BEST,    "BEST"},
                                                  {AVERAGE, "AVERAGE"},
                                                  {TWO,     "TWO"},
                                                  {ALL,     "ALL"},
                                                  {NS,      "NS"},
                                                  {GB,      "GB"},
                                                  {CY,      "CY"},
};


#define UNROLL1    1
#define UNROLL2    2
#define UNROLL4    4
#define UNROLL8    8

class bm_parameters {
private:
    ezOptionParser opt;

    int getKey(string val);

    string getValue(int val);

public:
    int m_PID;
    int m_mode;
    int m_type;

    BM_DATA_TYPE (*m_BENCHMARK)(bm_parameters *p, THEINT max_index, int step, int repeat, THEINT ops_per_scan);
    int m_GHZ = 0;
    int m_UNROLL;//= UNROLL4;
    int m_VERBOSE = 1;
    int m_DISP;//= DISP_AVE;
    int m_unit;//= DISP_GB;
    int m_CACHE_LINE = 64;
    int m_CPU_AFF = 0;
    int m_MEM_AFF = 0;
    size_t m_MAT_SIZE = 100;        // size of the matrix in byte
    size_t m_MAT_NB_ELEM = -1;
    int m_MAX_OPS = 1;
    int m_MIN_STRIDE = sizeof(BM_DATA_TYPE);// = sizeof(THETYPE);
    int m_MAX_STRIDE = 1024 * 8;
    int m_MAX_MEASURES = 2;
    int m_MAT_OFFSET = 0;
    int m_NUM_INDEX = 1;
    double m_MIN_LOG10=3.0;
    double m_MAX_LOG10=8.0;
    double m_STEP_LOG10 = 0.1;
    bool m_is_huge_pages = false;
    bool m_is_log = false;
    bool m_is_annotate = true;
    std::string m_prefix = "_bench_";
    std::string m_log_file_name = "";
    std::string m_annotate_file_name = "";
    std::ofstream m_log_file;

    int init_arguments(int argc, const char *argv[]);

    void print_configuration();

    int parse_arguments(int argc, const char *argv[]);

    int setup_parser(int argc, const char *argv[]);

    virtual ~bm_parameters();


};


#endif //PERFORMANCE_MODELISATION_BM_PARAMETERS_H
