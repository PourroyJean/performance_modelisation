//
// Created by Jean Pourroy on 09/04/2018.
//

#include "ezOptionParser.hpp"
#include "bm_parameters.h"
#include <iostream>
#include <unistd.h>
#include <assert.h>
#include "misc.h"
#include "bm_misc.h"
#include <algorithm>


using namespace ez;
using namespace std;

#ifdef _DML_INT64
#ifdef __linux__
#define THETYPE unsigned long long
#else
#define THETYPE uint64_t
#endif
#else
#define THETYPE double
#endif



void bm_parameters::print_configuration () {
    printf("%20s    %-10s \n", "m_type", getValue(m_type).c_str());
    printf("%20s    %-10s \n", "m_mode", getValue(m_mode).c_str());
    printf("%20s    %-10d \n", "m_MAT_SIZE", m_MAT_SIZE);
    printf("%20s    %-10d \n", "m_MAT_NB_ELEM", m_MAT_NB_ELEM);
    printf("%20s    %-10d \n", "m_UNROLL", m_UNROLL);
    printf("%20s    %-10s \n", "m_unit", getValue(m_unit).c_str());
    printf("%20s    %-10s \n", "m_display", getValue(m_DISP).c_str());
    printf("%20s    %-10d \n", "m_PID", m_PID);
}
int bm_parameters::init_arguments(int argc, const char *argv[]) {
    //Setup the command to parse
    setup_parser (argc, argv);

    //Parse and check the argument validity
    parse_arguments(argc, argv);
}

int bm_parameters::setup_parser(int argc, const char *argv[]) {

//    DEBUG << "ARGUMENT PARSING\n";

    opt.overview = "The unrolled version expect multiply of the unroll factor since this is a performance code we do not care loosing the last elements of the normal loop.";
    opt.syntax = "full [OPTIONS]";
    opt.example = "full -h\n\n";
    opt.footer = "dmlmem v0.1.4 Copyright (C) 2018 Jean Pourroy \nThis program is free and without warranty.\n";

    opt.add(
            "_bench_", // Default.
            0, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "string that will be displayed on each line of measures to thelp remember the name of the experiment", // Help description.
            "--prefix" // Flag token.
    );

    opt.add(
            "100", // Default.
            0, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "Matrix size in meg", // Help description.
            "--matrixsize" // Flag token.
    );

//    init_uint64_parm(argc, argv, &MAT_OFFSET, "MAT_OFFSET");
    opt.add(
            "0", // Default.
            0, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "THETYPE offset added to mat ( which is forced to Meg boundary )number of THETYPEs you want to offset \n"
                    "We are normally aligned on megabyte boundaries then also on cache line boundary ", // Help description.
            "--matrixoffset" // Flag token.
    );

//    init_uint64_parm(argc, argv, &CPU_AFF, "CPU_AFF");
    opt.add(
            "0", // Default.
            0, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "bind the process to a specific cpu", // Help description.
            "--cpuaff" // Flag token.
    );


//    init_uint64_parm(argc, argv, &MEM_AFF, "MEM_AFF");
    opt.add(
            "0", // Default.
            0, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "memory affinity", // Help description.
            "--memaff" // Flag token.
    );


//    init_uint64_parm(argc, argv, &MAX_OPS, "MAX_OPS");
    opt.add(
            "1", // Default.
            0, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "maximum number of operation per loop\n"
                    "The reason is to limit CPU for large buffer when there is no need to loop too much\n"
                    " because all accesses are out of cache", // Help description.
            "--maxops" // Flag token.
    );

//    init_int_parm(argc, argv, &VERBOSE, "VERBOSE");
    opt.add(
            "1", // Default.
            0, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "verbose of the execution", // Help description.
            "--verbose" // Flag token.
    );

//    init_int_parm(argc, argv, &GHZ, "GHZ");
    opt.add(
            "0", // Default.
            0, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "TODO", // Help description.
            "--ghz" // Flag token.
    );

//    init_int_parm(argc, argv, &MIN_STRIDE, "MIN_STRIDE");
    opt.add(
            to_string(sizeof(BM_DATA_TYPE)).c_str(), // Default.
            0, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "smallest stride size used ( number of THETYPEs )\n"
                    "if  (MIN_STRIDE==MAX_STRIDE) you have a constant stride", // Help description.
            "--minstride" // Flag token.
    );

//    init_int_parm(argc, argv, &MAX_STRIDE, "MAX_STRIDE");
    opt.add(
            to_string(1024 * 8).c_str(), // Default.
            0, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "biggest  stride size used ( number of THETYPEs )\n"
                    "if  (MIN_STRIDE==MAX_STRIDE) you have a constant stride", // Help description.
            "--maxstride" // Flag token.
    );

//    init_int_parm(argc, argv, &MAX_MEASURES, "MAX_MEASURES");
    opt.add(
            "2", // Default.
            0, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "number of time the measure is taken we print only the best", // Help description.
            "--maxmeasures" // Flag token.
    );


//    init_int_parm(argc, argv, &UNROLL, "UNROLL");
    opt.add(
            "1", // Default.
            0, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            " UNROLL can be 1 2 4 8 16 but not for all MODEs", // Help description.
            "--unroll" // Flag token.
    );

//    init_int_parm(argc, argv, &NUM_INDEX, "NUM_INDEX");
    opt.add(
            "1", // Default.
            0, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "number of indexes used with MODE_READIND", // Help description.
            "--index" // Flag token.
    );


//    init_int_parm(argc, argv, &CACHE_LINE, "CACHE_LINE");
    opt.add(
            "128", // Default.
            0, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "Length of a cache line", // Help description.
            "--cacheline" // Flag token.
    );

//    init_double_parm(argc, argv, &MIN_LOG10, "MIN_LOG10");
    opt.add(
            "3.0", // Default.
            0, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "log10 of minimal  vector size used", // Help description.
            "--minlog" // Flag token.
    );

//    init_double_parm(argc, argv, &MAX_LOG10, "MAX_LOG10");
    opt.add(
            "8.0", // Default.
            0, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "log10 of maximal  vector size used", // Help description.
            "--maxlog" // Flag token.
    );


//    init_int_with_str_parm(argc, argv, &MODE, "MODE", "READ", MODE_READ);
    opt.add(
            "0.1", // Default.
            0, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "step used to increase the length of the buffer\n"
                    "combined with (MIN_STRIDE==MAX_STRIDE) you can profile caracteristic behaviour", // Help description.
            "--steplog" // Flag token.
    );

    opt.add(
            getValue(DISP_MODE::AVERAGE).c_str(), // Default.
            0, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "Display the BEST, the AVERAGE of the WORST MEASURE or ALL\n"
                    "BEST is a good indicator of peak but average will give better indication of reality", // Help description.
            "--display" // Flag token.
    );

    opt.add(
            getValue(DISP_UNIT::GB).c_str(), // Default.
            0, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "result units (ns, gb or cycle)", // Help description.
            "--unit" // Flag token.
    );


    //    init_int_with_str_parm(argc, argv, &DISP, "DISP", "BEST", DISP_BEST);
    //    init_int_with_str_parm(argc, argv, &DISP, "DISP", "AVE", DISP_AVE);
    //    init_int_with_str_parm(argc, argv, &DISP, "DISP", "TWO", DISP_TWO);
    //    init_int_with_str_parm(argc, argv, &DISP, "DISP", "ALL", DISP_ALL);
    opt.add(
            getValue(BENCH_MODE::SPECIAL).c_str(), // Default.
            0, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "Mode of the benchmark: read or write of different type\n"
                    "  {READ, WRITE}    : classical algorythm , with UNROLL level from 1 to 8\n"
                    "  {READ, WRITE}SPE : unrolling from 2 to 16 with sum reduction\n"
                    "  {READ, WRITE}IND : unrolling from 2 to 16 with sum reduction with indexing", // Help description.
            "--mode" // Flag token.
    );

    opt.add(
            getValue(BENCH_TYPE::WRITE).c_str(), // Default.
            0, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "Mode of the benchmark: read or write\n"
                    "  {READ, WRITE}\n",
            "--type" // Flag token.
    );

    opt.add(
            "1", // Default.
            0, // Required?
            0, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "Print this usage message", // Help description.
            "-h"    // Flag token.
    );

}

int bm_parameters::parse_arguments(int argc, const char *argv[]) {

    DEBUG << " - PARSING -\n";
    opt.parse(argc, argv);


    if (opt.isSet("-h")) {
        std::string usage;
        opt.getUsage(usage, 120, ezOptionParser::INTERLEAVE);
        std::cout << usage;
        return 0;
    }


    string tmp;

    m_PID = getpid();

    opt.get("--prefix")->getString(m_prefix);

    opt.get("--mode")->getString(tmp);
    std::transform(tmp.begin(), tmp.end(),tmp.begin(), ::toupper);
    m_mode = getKey(tmp);
    if (m_mode == -1 || (m_mode != BENCH_MODE::NORMAL && m_mode != BENCH_MODE::SPECIAL && m_mode != BENCH_MODE::INDEXED)){
        cout << "Error check the mode value (" << tmp << " " << m_mode << ") : can be NORMAL, SPECIAL, INDEXED\n";
        exit(EXIT_FAILURE);
    }

    opt.get("--type")->getString(tmp);
    std::transform(tmp.begin(), tmp.end(),tmp.begin(), ::toupper);
    m_type = getKey(tmp);
    if (m_type == -1 || (m_type != BENCH_TYPE::WRITE && m_type != BENCH_TYPE::READ)){
        cout << "Error check the type value (" << tmp << ") : can be READ or WRITE\n";
        exit(EXIT_FAILURE);
    }


    opt.get("--unroll")->getInt(m_UNROLL);
    if (!((m_UNROLL == UNROLL1) || (m_UNROLL == UNROLL2) || (m_UNROLL == UNROLL4) || (m_UNROLL == UNROLL8))) {
        cout << "Error: please check the unroll argument: " << m_UNROLL << ": can be 1, 2, 4, 8\n";
        exit(EXIT_FAILURE);
    };

    opt.get("--verbose")->getInt(m_VERBOSE);


    opt.get("--display")->getString(tmp);
    std::transform(tmp.begin(), tmp.end(),tmp.begin(), ::toupper);
    m_DISP = getKey(tmp);
    if (m_DISP == -1 || (m_DISP != DISP_MODE::AVERAGE && m_DISP != DISP_MODE::BEST && m_DISP != DISP_MODE::ALL &&m_DISP != DISP_MODE::TWO)){
        cout << "Error check the display value (" << tmp << "): can be AVERAGE BEST ALL TWO\n";
        exit(EXIT_FAILURE);
    }


    opt.get("--ghz")->getInt(m_GHZ);
    if (! (0.0 <= m_GHZ && m_GHZ <= 10.0)){
        cout << "Error: please check the ghz argument: " << m_GHZ << ": should be between 0.1 and 10\n";
        exit(EXIT_FAILURE);
    }

    opt.get("--unit")->getString(tmp);  //ns cy gb
    std::transform(tmp.begin(), tmp.end(),tmp.begin(), ::toupper);
    m_unit = getKey(tmp);
    if (m_unit == -1 || (m_unit != DISP_UNIT::CY && m_unit != DISP_UNIT::GB &&m_unit != DISP_UNIT::NS)){
        cout << "Error check the unit value (" << tmp << ")\n";
        exit(EXIT_FAILURE);
    }
    else if (m_unit == DISP_UNIT::CY && m_GHZ == 0){
        cout << "Error: if you want to display the result in cycle, please provide the frequency of your processor with --ghz\n";
        exit(EXIT_FAILURE);
    }


        opt.get("--cacheline")->getInt(m_CACHE_LINE);
    if (!((m_CACHE_LINE == 64) || (m_CACHE_LINE == 128))) {
        cout << "Error: please check the cache line size: " << m_CACHE_LINE << "\n";
        exit(EXIT_FAILURE);
    };

    opt.get("--cpuaff")->getInt(m_CPU_AFF);

    opt.get("--memaff")->getInt(m_MEM_AFF);

    opt.get("--matrixsize")->getInt(m_MAT_SIZE);
    m_MAT_SIZE += 1; //TODO WHY ?
    m_MAT_SIZE *= (1024 * 1024);
    if (!(m_MAT_SIZE >= 1024 * 1024)) {
        cout << "Error: please check the size of your matrix\n";
        exit(EXIT_FAILURE);
    };
    m_MAT_NB_ELEM = m_MAT_SIZE / (sizeof(BM_DATA_TYPE));


    opt.get("--maxops")->getInt(m_MAX_OPS);
    m_MAX_OPS *= (1024 * 1024); //TODO why ?


    opt.get("--minstride")->getInt(m_MIN_STRIDE);
    m_MIN_STRIDE/=sizeof(BM_DATA_TYPE);

    opt.get("--maxstride")->getInt(m_MAX_STRIDE);
    m_MAX_STRIDE/=sizeof(BM_DATA_TYPE);

    opt.get("--maxmeasures")->getInt(m_MAX_MEASURES);

    opt.get("--matrixoffset")->getInt(m_MAT_OFFSET);

    opt.get("--index")->getInt(m_NUM_INDEX);
    if (!(m_NUM_INDEX == 1 || m_NUM_INDEX == 2 || m_NUM_INDEX == 4)) {
        cout << "Error: please check the size of your matrix\n";
        exit(EXIT_FAILURE);
    };

    opt.get("--minlog")->getDouble(m_MIN_LOG10);

    opt.get("--maxlog")->getDouble(m_MAX_LOG10);

    opt.get("--steplog")->getDouble(m_STEP_LOG10);


    string pretty;
    opt.configPrint(pretty);
    string line;
    std::istringstream f(pretty);
    while (std::getline(f, line)) {
        DEBUG << line << std::endl;
    }
}


int bm_parameters::getKey(string val) {
    for (map<int, string>::iterator it = mapParameter.begin(); it != mapParameter.end(); ++it) {
        if (it->second == val) return it->first;
    }
    return -1;
}
string bm_parameters::getValue(int key) {
    for (map<int, string>::iterator it = mapParameter.begin(); it != mapParameter.end(); ++it) {
        if (it->first == key) return it->second;
    }
    cout << "Error : getValue (" << key << ")\n";
    exit(EXIT_FAILURE);
}