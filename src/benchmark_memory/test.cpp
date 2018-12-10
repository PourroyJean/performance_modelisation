/*
20110505 rsz Created.
*/
#include <stdio.h>
#include <string>
#include "ezOptionParser.hpp"

using namespace ez;
using namespace std;

#define STRINGIFY(s) #s

#define xstr(s) str(s)
#define str(s) #s

#define UNROLL1    1
#define UNROLL2    2
#define UNROLL4    4
#define UNROLL8    8

#ifdef _DML_INT64
#ifdef __linux__
#define THETYPE unsigned long long
#else
#define THETYPE uint64_t
#endif
#else
#define THETYPE double
#endif

int main(int argc, const char *argv[]) {
    ezOptionParser opt;

    opt.overview = "Full demo of all the features.";
    opt.syntax = "full [OPTIONS]";
    opt.example = "full -h\n\n";
    opt.footer = "full v0.1.4 Copyright (C) 2011 Remik Ziemlinski\nThis program is free and without warranty.\n";

//    int mypid;
//    int moderw;
//    int GHZ = 0;
//    int UNROLL = UNROLL4;
//    int VERBOSE = 1;
//    int DISP = DISP_AVE;            /* mode de DISPLAY                                                */
//    int NSGBCY = DISP_GB;            /* mode in ns or GB						   */
//    int CACHE_LINE = 128;            /* length of cache line      					   */
//    THEINT CPU_AFF = 0;                /* CPU ou le processeur doit tourner                              */
//    THEINT MEM_AFF = 0;                /* MEM ou le processeur doit tourner                              */
//    THEINT MAT_SIZE = 100;                /* size of matrice in Meg 					   */
//    THEINT MAX_OPS = 1;                /* max number of operations per function call                     */
//    int MIN_STRIDE = sizeof(THETYPE);    /* smallest stride size used ( number of THETYPEs )                */
//    int MAX_STRIDE = 1024 * 8;        /* greatest stride size used ( number of THETYPEs )                */
//    int MAX_MEASURES = 2;            /* number of time the measure is taken we print only the best     */
//    THEINT MAT_OFFSET = 0;                /* THETYPE offset added to mat ( which is forced to Meg boundary ) */
//    int NUM_INDEX = 1;                /* number of indexes used with MODE_READIND                       */
//    int MAKE_UNAME = 1;                /* flag to not do uname when using special tools like aries 0=NO  */
//    double MIN_LOG10 = 3.0;            /* log10 of minimal  vector size used				   */
//    double MAX_LOG10 = 8.0;            /* log10 of maximum  vector size used				   */
//    double STEP_LOG10 = 0.1;            /* log10 of STEP for vector size used				   */
//#define MIN_OPS_PER_SCAN 128        /* we do not measure if less MIN_OPS_PER SCAN operations per loop */

//  init_uint64_parm(argc, argv, &MAT_SIZE, "MAT_SIZE");
    opt.add(
            "100", // Default.
            0, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "Matrix size in meg", // Help description.
//            "-s",     // Flag token.
            "--matrixsize" // Flag token.
    );

//    init_uint64_parm(argc, argv, &MAT_OFFSET, "MAT_OFFSET");
    opt.add(
            "0", // Default.
            0, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "THETYPE offset added to mat ( which is forced to Meg boundary )", // Help description.
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
            "max number of operations per function call", // Help description.
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
            to_string(sizeof(THETYPE)).c_str(), // Default.
            0, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "smallest stride size used ( number of THETYPEs )", // Help description.
            "--minstride" // Flag token.
    );

//    init_int_parm(argc, argv, &MAX_STRIDE, "MAX_STRIDE");
    opt.add(
            to_string(1024 * 8).c_str(), // Default.
            0, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "biggest  stride size used ( number of THETYPEs )", // Help description.
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
            "4", // Default.
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
            "--unroll" // Flag token.
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
            " log10 of minimal  vector size used", // Help description.
            "--minlog" // Flag token.
    );

//    init_double_parm(argc, argv, &MAX_LOG10, "MAX_LOG10");
    opt.add(
            "8.0", // Default.
            0, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            " log10 of maximal  vector size used", // Help description.
            "--maxlog" // Flag token.
    );

//    init_double_parm(argc, argv, &STEP_LOG10, "STEP_LOG10");
    opt.add(
            "0.1", // Default.
            0, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            " log10 of STEP for vector size used", // Help description.
            "--steplog" // Flag token.
    );

//    init_int_with_str_parm(argc, argv, &MODE, "MODE", "READ", MODE_READ);
    opt.add(
            "0.1", // Default.
            0, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            " log10 of STEP for vector size used", // Help description.
            "--steplog" // Flag token.
    );
//    if (MODE == MODE_READ)moderw = MODER;
//    init_int_with_str_parm(argc, argv, &MODE, "MODE", "READSPE", MODE_READSPE);
//    if (MODE == MODE_READSPE)moderw = MODER;
//    init_int_with_str_parm(argc, argv, &MODE, "MODE", "READIND", MODE_READIND);
//    if (MODE == MODE_READIND)moderw = MODER;
//    init_int_with_str_parm(argc, argv, &MODE, "MODE", "WRITE", MODE_WRITE);
//    if (MODE == MODE_WRITE)moderw = MODEW;
//    init_int_with_str_parm(argc, argv, &MODE, "MODE", "WRITESPE", MODE_WRITESPE);
//    if (MODE == MODE_WRITESPE)moderw = MODEW;
//    init_int_with_str_parm(argc, argv, &MODE, "MODE", "WRITEIND", MODE_WRITEIND);
//    if (MODE == MODE_WRITEIND)moderw = MODEW;
//    init_int_with_str_parm(argc, argv, &DISP, "DISP", "BEST", DISP_BEST);
//    init_int_with_str_parm(argc, argv, &DISP, "DISP", "AVE", DISP_AVE);
//    init_int_with_str_parm(argc, argv, &DISP, "DISP", "TWO", DISP_TWO);
//    init_int_with_str_parm(argc, argv, &DISP, "DISP", "ALL", DISP_ALL);
//    init_int_with_str_parm(argc, argv, &NSGBCY, "DISP", "NS", DISP_NS);
//    init_int_with_str_parm(argc, argv, &NSGBCY, "DISP", "GB", DISP_GB);
//    init_int_with_str_parm(argc, argv, &NSGBCY, "DISP", "CY", DISP_CY);
//



    opt.add(
            "", // Default.
            0, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "Print this usage message in one of three different layouts. The choices are:\n0 - aligned (default)\n1 - interleaved\n2 - staggered", // Help description.
            "-h",     // Flag token.
            "-help", // Flag token.
            "--help", // Flag token.
            "--usage" // Flag token.
    );


    opt.parse(argc, argv);

    if (opt.isSet("-h")) {
        std::string usage;
        int layout;
        opt.get("-h")->getInt(layout);
        switch (layout) {
            case 0:
                opt.getUsage(usage, 80, ezOptionParser::ALIGN);
                break;
            case 1:
                opt.getUsage(usage, 80, ezOptionParser::INTERLEAVE);
                break;
            case 2:
                opt.getUsage(usage, 80, ezOptionParser::STAGGER);
                break;
        }
        std::cout << usage;
        return 1;
    }


//    if (opt.isSet("-i")) {
//        // Import one or more files that use # as comment char.
//        std::vector< std::vector<std::string> > files;
//        opt.get("-i")->getMultiStrings(files);
//
//        for(int j=0; j < files.size(); ++j)
//            if (! opt.importFile(files[j][0].c_str(), '#'))
//                std::cerr << "ERROR: Failed to open file " << files[j][0] << std::endl;
//    }
//
//    if (opt.isSet("+i")) {
//        // Import one or more files that use # as comment char.
//        std::vector< std::vector<std::string> > files;
//        opt.get("+i")->getMultiStrings(files);
//
//        if(!files.empty()) {
//            std::string file = files[files.size()-1][0];
//            opt.resetArgs();
//            if (! opt.importFile(file.c_str(), '#'))
//                std::cerr << "ERROR: Failed to open file " << file << std::endl;
//        }
//    }
//
//    if (opt.isSet("-e")) {
//        std::string outfile;
//        opt.get("-e")->getString(outfile);
//        opt.exportFile(outfile.c_str(), false);
//    }
//
//    if (opt.isSet("+e")) {
//        std::string outfile;
//        opt.get("+e")->getString(outfile);
//        opt.exportFile(outfile.c_str(), true);
//    }
//
//    if (!opt.isSet("+d")) {
//        std::string pretty;
//        opt.prettyPrint(pretty);
//        std::cout << pretty;
//    }

//    cout << "--------- FLOAT LIST----------\n";
//    std::string pretty;
//    opt.prettyPrint(pretty);
//    std::cout << pretty;
//    cout << "--------- FLOAT LIST----------\n";
//

    if (!opt.isSet("-h")) {
        int size;
        opt.get("--maxstride")->getInt(size);
        std::cout << "my size : " << size << endl;
    }

    return 0;
}