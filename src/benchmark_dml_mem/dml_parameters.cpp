//
// Created by Jean Pourroy on 09/04/2018.
//

#include "ezOptionParser.hpp"
#include "dml_parameters.h"
#include <iostream>
#include <unistd.h>
#include <assert.h>
#include "misc.h"
#include "dml_misc.h"
#include <algorithm>
#include <cmath>
#include "dml_benchmark.h"
#include "dml_misc.h"


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

#include <string>
#include <code_annotation.h>

#ifndef DEBUG_MPI
#define DEBUG_MPI cout
#endif

#ifdef YAMB_ENABLED
extern string YAMB_ANNOTATE_LOG_FILE;
#endif

void Dml_parameters::print_configuration() {

    uint64_t actual_max_elements = std::min<uint64_t>(m_END_SIZE, m_MAT_NB_ELEM);
    if (m_START_SIZE > m_MAT_NB_ELEM) {
        if (mpi_rank == 0)
            printf("  WARNING: Start size (%lu) exceeds allocated elements (%lu).\n", m_START_SIZE, m_MAT_NB_ELEM);
        actual_max_elements = m_START_SIZE; // To prevent negative range display if start > alloc
    }
    if (m_END_SIZE > m_MAT_NB_ELEM && mpi_rank == 0) {
        printf("  INFO: Requested end size (%lu) capped at allocated elements (%lu).\n", m_END_SIZE, m_MAT_NB_ELEM);
    }


    size_t subdataset_min_size_bytes = m_START_SIZE * sizeof(DML_DATA_TYPE);
    size_t subdataset_max_size_bytes = actual_max_elements * sizeof(DML_DATA_TYPE);


    printf("  %-25s    %-10s \n", "Benchmark type", getValue(m_type).c_str());
    printf("  %-25s    %-10s \n", "Benchmark mode", getValue(m_mode).c_str());
    printf("  %-25s    %-10s \n", "Total Matrix Alloc Size", convert_size(m_MAT_SIZE).c_str());
    printf("  %-25s    %-10lu\n", "Total Matrix Elements", m_MAT_NB_ELEM);
    printf("  %-25s    %-10d \n", "Number of processes", mpi_size);
    printf("  %-25s    %-10s \n", "Memory page size", m_is_huge_pages ? "Huge Pages (2 MiB)" : "Default (4 KiB)");
    printf("  %-25s    %-10d \n", "Number of manual unroll", m_UNROLL);
    printf("  %-25s    %-10s \n", "Measure are displayed in ", getValue(m_unit).c_str());
    printf("  %-25s    %-10s \n", "Measure represents the", getValue(m_DISP).c_str());
    printf("  %-25s    %-10d \n", "Cache line size", m_CACHE_LINE);
    printf("  %-25s    %-10s \n", "Stride range in byte",
    string(to_string(m_STRIDE_LIST.empty() ? 0 : m_STRIDE_LIST.front()) + " - " +
        to_string(m_STRIDE_LIST.empty() ? 0 : m_STRIDE_LIST.back()) + " mode " +
        getValue(m_STRIDE_MODE).c_str()).c_str());
    printf("  %-25s    %lu - %lu \n", "Element range", m_START_SIZE, actual_max_elements);
    printf("  %-25s    %.2f \n", "Size Step Factor", m_SIZE_STEP_FACTOR);
    printf("  %-25s    %-10s \n", "Memory range Tested", string((convert_size(subdataset_min_size_bytes)) + " - " + (convert_size(subdataset_max_size_bytes))).c_str());
    printf("  %-25s    %-10s \n", "Save output ", m_is_log ? ("yes in : " + m_log_file_name).c_str() : "no output file");
    printf("  %-25s    %-10s \n", "Annotation file for YAMB", m_is_annotate ? ("yes in : " + m_annotate_file_name).c_str() : "no");


    cout << endl;

    if (m_mode == BENCH_MODE::NORMAL) {
        cout << "Normal mode: \n";
        cout << "    [X] [ ] [ ] [ ] [X] [ ] [ ] [ ] [X]  ....   [ ]\n";
        cout << "     |               |                           | \n";
        cout << "     \\-- Stride S ---/                           | \n";
        cout << "     \\------------------ Size K -----------------/      \n";
        cout << "\n";
    }
}

int Dml_parameters::init_arguments(int argc, const char *argv[]) {
    //Setup the command to parse
    setup_parser(argc, argv);

    //Parse and check the argument validity
    parse_arguments(argc, argv);

    //Select the correct benchmark
    //----- READ BENCHMARK -----
    if (m_type == BENCH_TYPE::READ) {
        if (m_mode == BENCH_MODE::NORMAL) {
            if (m_UNROLL == UNROLL1) {
                m_BENCHMARK = sum_read_unroll1;
            } else if (m_UNROLL == UNROLL2) {
                m_BENCHMARK = sum_read_unroll2;
            } else if (m_UNROLL == UNROLL4) {
                m_BENCHMARK = sum_read_unroll4;
            } else if (m_UNROLL == UNROLL8) {
                m_BENCHMARK = sum_read_unroll8;
            } else if (m_UNROLL == UNROLL16) {
                cout << "\nERROR: Not yet implemented\n";
                exit(-1);
            } else if (m_UNROLL == UNROLL64) {
                cout << "\nERROR: Not yet implemented\n";
                exit(-1);
            }
        }
        if (m_mode == BENCH_MODE::SPECIAL) {
            if (m_UNROLL == UNROLL1) {
                m_BENCHMARK = sum_read_unroll1; //without unrolling it is the same as normal mode
            } else if (m_UNROLL == UNROLL2) {
                m_BENCHMARK = sum_readspe_unroll2;
            } else if (m_UNROLL == UNROLL4) {
                m_BENCHMARK = sum_readspe_unroll4;
            } else if (m_UNROLL == UNROLL8) {
                m_BENCHMARK = sum_readspe_unroll8;
            } else if (m_UNROLL == UNROLL16) {
                m_BENCHMARK = sum_readspe_unroll16;
            } else if (m_UNROLL == UNROLL32) {
                m_BENCHMARK = sum_readspe_unroll32;
            } else if (m_UNROLL == UNROLL64) {
                m_BENCHMARK = sum_readspe_unroll64;
            }
        }

        if (m_mode == BENCH_MODE::INDEXED) {
            if (m_UNROLL == UNROLL1) {
                cout << "\nERROR: Not yet implemented\n";
                exit(-1);
            } else if (m_UNROLL == UNROLL2) {
                m_BENCHMARK = sum_readind_unroll2;
            } else if (m_UNROLL == UNROLL4) {
                m_BENCHMARK = sum_readind_unroll4;
            } else if (m_UNROLL == UNROLL8) {
                m_BENCHMARK = sum_readind_unroll8;
            } else if (m_UNROLL == UNROLL16) {
                cout << "\nERROR: Not yet implemented\n";
                exit(-1);
            } else if (m_UNROLL == UNROLL64) {
                cout << "\nERROR: Not yet implemented\n";
                exit(-1);
            }
        }
    }
        //----- WRITE BENCHMARK -----
    else if (m_type == BENCH_TYPE::WRITE) {
        if (m_mode == BENCH_MODE::NORMAL) {
            if (m_UNROLL == UNROLL1) {
                m_BENCHMARK = sum_write_unroll1;
            } else if (m_UNROLL == UNROLL2) {
                m_BENCHMARK = sum_write_unroll2;
            } else if (m_UNROLL == UNROLL4) {
                m_BENCHMARK = sum_write_unroll4;
            } else if (m_UNROLL == UNROLL8) {
                m_BENCHMARK = sum_write_unroll8;
            } else if (m_UNROLL == UNROLL16) {
                cout << "\nERROR: Not yet implemented\n";
                exit(-1);
            } else if (m_UNROLL == UNROLL64) {
                cout << "\nERROR: Not yet implemented\n";
                exit(-1);
            }
        }
        if (m_mode == BENCH_MODE::SPECIAL) {
            if (m_UNROLL == UNROLL1) {
                m_BENCHMARK = sum_write_unroll1;
            } else if (m_UNROLL == UNROLL2) {
                m_BENCHMARK = sum_writespe_unroll2;
            } else if (m_UNROLL == UNROLL4) {
                m_BENCHMARK = sum_writespe_unroll4;
            } else if (m_UNROLL == UNROLL8) {
                m_BENCHMARK = sum_writespe_unroll8;
            } else if (m_UNROLL == UNROLL16) {
                cout << "\nERROR: Not yet implemented\n";
                exit(-1);
            } else if (m_UNROLL == UNROLL64) {
                cout << "\nERROR: Not yet implemented\n";
                exit(-1);
            }
        }

        if (m_mode == BENCH_MODE::INDEXED) {
            if (m_UNROLL == UNROLL1) {
                m_BENCHMARK = sum_write_unroll1;
            } else if (m_UNROLL == UNROLL2) {
                m_BENCHMARK = sum_writeind_unroll2;
            } else if (m_UNROLL == UNROLL4) {
                m_BENCHMARK = sum_writeind_unroll4;
            } else if (m_UNROLL == UNROLL8) {
                m_BENCHMARK = sum_writeind_unroll8;
            } else if (m_UNROLL == UNROLL16) {
                cout << "\nERROR: Not yet implemented\n";
                exit(-1);
            } else if (m_UNROLL == UNROLL64) {
                cout << "\nERROR: Not yet implemented\n";
                exit(-1);
            }
        }
    }

    return 0;
}

int Dml_parameters::setup_parser(int argc, const char *argv[]) {

    opt.overview = "The unrolled version expect multiply of the unroll factor since this is a performance code we do not care loosing the last elements of the normal loop.";
    opt.syntax = "full [OPTIONS]";
    opt.example = "full -h\n\n";
    opt.footer = "dmlmem_orig v0.1.4 Copyright (C) 2018 Jean Pourroy \nThis program is free and without warranty.\n";
    string default_name = "_bench_";
    opt.add(
            default_name.c_str(), // Default.
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
            "Matrix size in Mib", // Help description.
            "--matrixsize" // Flag token.
    );

    opt.add(
            "0", // Default.
            0, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "THETYPE offset added to mat ( which is forced to Meg boundary )number of THETYPEs you want to offset \n"
                    "We are normally aligned on megabyte boundaries then also on cache line boundary ", // Help description.
            "--matrixoffset" // Flag token.
    );

    opt.add(
            "0", // Default.
            0, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "bind the process to a specific cpu", // Help description.
            "--cpuaff" // Flag token.
    );


    opt.add(
            "0", // Default.
            0, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "memory affinity", // Help description.
            "--memaff" // Flag token.
    );


    opt.add(
            "1048576", // Default 1024*1024.
            0, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "maximum number of operation per loop\n"
                    "The reason is to limit CPU for large buffer when there is no need to loop too much\n"
                    " because all accesses are out of cache", // Help description.
            "--maxops" // Flag token.
    );

    opt.add(
            "1", // Default.
            0, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "verbose of the execution", // Help description.
            "--verbose" // Flag token.
    );

    opt.add(
            "0", // Default.
            0, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "CPU frequency", // Help description.
            "--ghz" // Flag token.
    );

    opt.add(
            to_string(sizeof(DML_DATA_TYPE)).c_str(), // Default.
            0, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "smallest stride size used in byte\n"
                    "if  (MIN_STRIDE==MAX_STRIDE) you have a constant stride", // Help description.
            "--minstride" // Flag token.
    );

    opt.add(
            to_string(1024 * 8).c_str(), // Default.
            0, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "biggest stride size used in byte\n"
                    "if  (MIN_STRIDE==MAX_STRIDE) you have a constant stride", // Help description.
            "--maxstride" // Flag token.
    );

    opt.add(
            "0", // Default.
            0, // Required?
            1, // Number of args expected.
            ',', // Delimiter if expecting multiple args.
            "only one stride measured", // Help description.
            "--stride" // Flag token.
    );

    opt.add(
            getValue(BENCH_STRIDE::ODD).c_str(), // Default.
            1, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "Odd or even stride", // Help description.
            "--stridemode" // Flag token.
    );


    opt.add(
            "2", // Default.
            0, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "number of time the measure is taken", // Help description.
            "--measure" // Flag token.
    );


    opt.add(
            "1", // Default.
            0, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            " UNROLL can be 1 2 4 8 16 but not for all MODEs", // Help description.
            "--unroll" // Flag token.
    );

    opt.add(
            "1", // Default.
            0, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "number of indexes used with MODE_READIND", // Help description.
            "--index" // Flag token.
    );


    opt.add(
            "64", // Default.
            0, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "Length of a cache line", // Help description.
            "--cacheline" // Flag token.
    );
    // Default values for matrix size parameters
    std::string start_size_default = to_string(m_START_SIZE);
    std::string end_size_default = to_string(m_END_SIZE);
    std::string size_step_factor_default = to_string(m_SIZE_STEP_FACTOR);

    // Add option for starting matrix size
    opt.add(
        start_size_default.c_str(), // Default value
        0, // Not required
        1, // Number of arguments expected
        0, // No delimiter
        "Starting matrix size (number of elements)", // Help description
        "--startsize" // Flag token
    );

    // Add option for ending matrix size
    opt.add(
        end_size_default.c_str(), // Default value
        0, // Not required
        1, // Number of arguments expected
        0, // No delimiter
        "Ending matrix size (number of elements)", // Help description
        "--endsize" // Flag token
    );

    // Add option for benchmarking a single matrix size
    opt.add(
        "0", // Default: 0 means use start/end range
        0, // Not required
        1, // Number of arguments expected
        0, // No delimiter
        "Benchmark a single matrix size (number of elements). Overrides --startsize and --endsize.", // Help description
        "--size" // Flag token
    );

    // Add option for size step factor
    opt.add(
        size_step_factor_default.c_str(), // Default value
        0, // Not required
        1, // Number of arguments expected
        0, // No delimiter
        "Multiplicative factor for increasing size between steps (e.g., 1.2 for 20% increase)", // Help description
        "--sizestepfactor" // Flag token
    );

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


    opt.add(
            getValue(BENCH_MODE::NORMAL).c_str(), // Default.
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
            getValue(BENCH_TYPE::READ).c_str(), // Default.
            0, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "Mode of the benchmark: read or write\n"
                    "  {READ, WRITE}\n",
            "--type" // Flag token.
    );


    opt.add(
            "false", // Default.
            0, // Required?
            0, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "Use huge pages", // Help description.
            "--hugepages"    // Flag token.
    );


    opt.add(
            "", // Default.
            1, // Required?
            2, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "Log output value in a log file", // Help description.
            "--output"    // Flag token.
    );

    opt.add(
            "", // Default.
            1, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "Annotate the YAMB memory bandwidth graph", // Help description.
            "--annotate"    // Flag token.
    );


    opt.add(
            "1", // Default.
            0, // Required?
            0, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "Print this usage message", // Help description.
            "-h"    // Flag token.
    );

    return 0;
}

int Dml_parameters::parse_arguments(int argc, const char *argv[]) {

    DEBUG_MPI << " - PARSING -\n";
    opt.parse(argc, argv);


    if (opt.isSet("-h")) {
        std::string usage;
        opt.getUsage(usage, 120, ezOptionParser::INTERLEAVE);
        std::cout << usage;
        exit(0);
    }


    string tmp;
    int itmp;
    double dtmp;
    unsigned long long ultmp; // For parsing sizes

    m_PID = getpid();

    opt.get("--prefix")->getString(m_prefix);

    opt.get("--mode")->getString(tmp);
    std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::toupper);
    m_mode = getKey(tmp);
    if (m_mode == -1 ||
        (m_mode != BENCH_MODE::NORMAL && m_mode != BENCH_MODE::SPECIAL && m_mode != BENCH_MODE::INDEXED)) {
        cout << "Error check the mode value (" << tmp << " " << m_mode << ") : can be NORMAL, SPECIAL, INDEXED\n";
        exit(EXIT_FAILURE);
    }

    opt.get("--type")->getString(tmp);
    std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::toupper);
    m_type = getKey(tmp);
    if (m_type == -1 || (m_type != BENCH_TYPE::WRITE && m_type != BENCH_TYPE::READ)) {
        cout << "Error check the type value (" << tmp << ") : can be READ or WRITE\n";
        exit(EXIT_FAILURE);
    }


    opt.get("--unroll")->getInt(m_UNROLL);
    if (!((m_UNROLL == UNROLL1) || (m_UNROLL == UNROLL2) || (m_UNROLL == UNROLL4) || (m_UNROLL == UNROLL8) ||
          (m_UNROLL == UNROLL16) || (m_UNROLL == UNROLL32) || (m_UNROLL == UNROLL64))) {
        cout << "Error: please check the unroll argument: " << m_UNROLL << ": can be 1, 2, 4, 8, 16, 32, 64 \n";
        exit(EXIT_FAILURE);
    };

    opt.get("--verbose")->getInt(m_VERBOSE);


    opt.get("--display")->getString(tmp);
    std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::toupper);
    m_DISP = getKey(tmp);
    if (m_DISP == -1 || (m_DISP != DISP_MODE::AVERAGE && m_DISP != DISP_MODE::BEST && m_DISP != DISP_MODE::ALL &&
                         m_DISP != DISP_MODE::TWO)) {
        cout << "Error check the display value (" << tmp << "): can be AVERAGE BEST ALL TWO\n";
        exit(EXIT_FAILURE);
    }


    opt.get("--ghz")->getInt(m_GHZ);
    if (!(0.0 <= m_GHZ && m_GHZ <= 10.0)) {
        cout << "Error: please check the ghz argument: " << m_GHZ << ": should be between 0.1 and 10\n";
        exit(EXIT_FAILURE);
    }

    opt.get("--unit")->getString(tmp);  //ns cy gb
    std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::toupper);
    m_unit = getKey(tmp);
    if (m_unit == -1 || (m_unit != DISP_UNIT::CY && m_unit != DISP_UNIT::GB && m_unit != DISP_UNIT::NS)) {
        cout << "Error check the unit value (" << tmp << ")\n";
        exit(EXIT_FAILURE);
    } else if (m_unit == DISP_UNIT::CY && m_GHZ == 0) {
        cout
                << "Error: if you want to display the result in cycle, please provide the frequency of your processor with --ghz\n";
        exit(EXIT_FAILURE);
    }


    opt.get("--cacheline")->getInt(m_CACHE_LINE);
    if (!((m_CACHE_LINE == 64) || (m_CACHE_LINE == 128))) {
        cout << "Error: please check the cache line size: " << m_CACHE_LINE << "\n";
        exit(EXIT_FAILURE);
    };

    opt.get("--cpuaff")->getInt(m_CPU_AFF);

    opt.get("--memaff")->getInt(m_MEM_AFF);

    double size;
    opt.get("--matrixsize")->getDouble(size);

    m_MAT_SIZE = size*(1024 * 1024); // The size is stored in byte : 1 MiB  == 1 * 1024 * 1024 byte
    if (!(m_MAT_SIZE >=  1024)) {
        cout << "Error: please check the size of your matrix (" << m_MAT_SIZE << ")\n";
        exit(EXIT_FAILURE);
    };
    m_MAT_NB_ELEM = size_t(size_t(m_MAT_SIZE) / (sizeof(DML_DATA_TYPE)));


    opt.get("--maxops")->getInt(m_MAX_OPS);


    opt.get("--minstride")->getInt(m_MIN_STRIDE);
    m_MIN_STRIDE /= sizeof(DML_DATA_TYPE);
    if (m_MIN_STRIDE < 1) {
        cout << "Error: please check the size of the minimum stride (" << m_MIN_STRIDE << " byte)\n";
        exit(EXIT_FAILURE);
    }

    opt.get("--maxstride")->getInt(m_MAX_STRIDE);
    m_MAX_STRIDE /= sizeof(DML_DATA_TYPE);


    if (opt.isSet("--stride")) {
        vector<vector<int>> v;
        opt.get("--stride")->getMultiInts(v);

        if (v.size() == 0) {
            cout << "Error: please enter a stride in byte separated by ',' character\n";
            exit(EXIT_FAILURE);

        }
        m_STRIDE_LIST = v.at(0);
        sort(m_STRIDE_LIST.begin(), m_STRIDE_LIST.end());

        if (m_STRIDE_LIST.at(0) < 8) {
            cout << "Error: please enter a stride in byte (must be >= 8)\n";
            exit(EXIT_FAILURE);
        }
        m_MIN_STRIDE = m_STRIDE_LIST[0];
        m_MAX_STRIDE = m_STRIDE_LIST[m_STRIDE_LIST.size() - 1];
    }


    opt.get("--stridemode")->getString(tmp);
    std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::toupper);
    m_STRIDE_MODE = getKey(tmp);
    if (m_STRIDE_MODE == -1 ||
        (m_STRIDE_MODE != BENCH_STRIDE::EVEN && m_STRIDE_MODE != BENCH_STRIDE::ODD)) {
        cout << "Error check the stride mode value (" << tmp << " " << m_STRIDE_MODE << ") : can be ODD or EVEN\n";
        exit(EXIT_FAILURE);
    }

    if (!opt.isSet("--stride")) {
        //Generate the stride list
        for (int step = m_MIN_STRIDE; step <= m_MAX_STRIDE; step = ((step * 2))) {
            //Stride de step element
            //converti en byte
            //décallage si impaire
            m_STRIDE_LIST.push_back((step * sizeof(DML_DATA_TYPE)) + ((m_STRIDE_MODE == BENCH_STRIDE::ODD) ? 0 : +7));
        }
        m_MIN_STRIDE = m_STRIDE_LIST[0];
        m_MAX_STRIDE = m_STRIDE_LIST[m_STRIDE_LIST.size() - 1];
    }


    opt.get("--measure")->getInt(m_MAX_MEASURES);

    opt.get("--matrixoffset")->getInt(m_MAT_OFFSET);

    opt.get("--index")->getInt(m_NUM_INDEX);
    if (!(m_NUM_INDEX == 1 || m_NUM_INDEX == 2 || m_NUM_INDEX == 4)) {
        cout << "Error: please check the size of your matrix\n";
        exit(EXIT_FAILURE);
    };
    double parsed_start_mb, parsed_end_mb, parsed_size_mb;

    opt.get("--startsize")->getDouble(parsed_start_mb);
    opt.get("--endsize")->getDouble(parsed_end_mb);
    opt.get("--sizestepfactor")->getDouble(m_SIZE_STEP_FACTOR);

    m_START_SIZE = static_cast<uint64_t>((parsed_start_mb * 1024.0 * 1024.0) / sizeof(DML_DATA_TYPE));
    m_END_SIZE = static_cast<uint64_t>((parsed_end_mb * 1024.0 * 1024.0) / sizeof(DML_DATA_TYPE));

    opt.get("--size")->getDouble(parsed_size_mb);
    if (parsed_size_mb > 0) {
        m_START_SIZE = static_cast<uint64_t>((parsed_size_mb * 1024.0 * 1024.0) / sizeof(DML_DATA_TYPE));
        m_END_SIZE = m_START_SIZE;
        DEBUG_MPI << "Info: --size specified, setting range to [" << m_START_SIZE << ", " << m_END_SIZE << "]\n";
    }

    if (m_START_SIZE == 0) {
        cerr << "Warning: --startsize is 0. Adjusting to 1 element.\n";
        m_START_SIZE = 1;
    }
    if (m_END_SIZE < m_START_SIZE) {
        cerr << "Error: --endsize (" << m_END_SIZE << ") cannot be less than --startsize (" << m_START_SIZE << ").\n";
        exit(EXIT_FAILURE);
    }
    if (m_SIZE_STEP_FACTOR <= 1.0) {
        if (m_START_SIZE != m_END_SIZE) {
            cerr << "Warning: --sizestepfactor (" << m_SIZE_STEP_FACTOR << ") is <= 1.0. Only start and end sizes will be tested.\n";
        }
    }
    if (m_START_SIZE > m_MAT_NB_ELEM) {
        cerr << "Warning: --startsize (" << m_START_SIZE << ") exceeds total allocated elements (" << m_MAT_NB_ELEM << "). Benchmark might not run.\n";
    }
    if (m_END_SIZE > m_MAT_NB_ELEM) {
        cerr << "Info: --endsize (" << m_END_SIZE << ") exceeds total allocated elements (" << m_MAT_NB_ELEM << "). Range will be capped.\n";
    }

    if (opt.isSet("--hugepages")) {
        m_is_huge_pages = true;
    } else
        m_is_huge_pages = false;


    if (opt.isSet("--output")) {
        m_is_log = true;
        opt.get("--output")->getString(m_log_file_name);
        //Check if the file name was given in argument, and is is not the next option (begin with --)
        if (m_log_file_name == "" || !strncmp(m_log_file_name.c_str(), string("--").c_str(), string("--").size())) {
            cout << "Error: please write the name of the output file\n";
            exit(EXIT_FAILURE);
        }
    } else
        m_is_log = false;


    if (opt.isSet("--annotate")) {
        #ifdef YAMB_ENABLED
            m_is_annotate = true;
            opt.get("--annotate")->getString(m_annotate_file_name);
            //Check if the file name was given in argument, and is is not the next option (begin with --)
            if (m_annotate_file_name.empty() ||
                !strncmp(m_annotate_file_name.c_str(), string("--").c_str(), string("--").size())) {
                cout << "Error: please write the name of the annotate file\n";
                exit(EXIT_FAILURE);
            }
            YAMB_ANNOTATE_LOG_FILE = m_annotate_file_name;
        #endif
    } else {
        m_is_annotate = false;
    }


    #ifdef OP_DEBUG //TODO CA MARCHE PAS
    string pretty;
    opt.configPrint(pretty);
    string line;
    std::istringstream f(pretty);
    while (std::getline(f, line)) {
        DEBUG_MPI << line << std::endl;
    }
    #endif
    return 0;
}


int Dml_parameters::getKey(string val) {
    for (map<int, string>::iterator it = mapParameter.begin(); it != mapParameter.end(); ++it) {
        if (it->second == val) return it->first;
    }
    return -1;
}

string Dml_parameters::getValue(int key) {
    for (map<int, string>::iterator it = mapParameter.begin(); it != mapParameter.end(); ++it) {
        if (it->first == key) return it->second;
    }
    cout << "Error : getValue (" << key << ")\n";
    exit(EXIT_FAILURE);
}

Dml_parameters::Dml_parameters() : m_STRIDE_LIST() {

}

Dml_parameters::~Dml_parameters() {
    if (m_log_file.is_open()) {
        m_log_file.close();
    }
}


