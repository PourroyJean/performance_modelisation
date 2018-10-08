#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <iostream>
#include <sys/time.h>
#include <misc.h>
#include "bm_misc.h"
#include <sys/shm.h>        // Large parge
#include <unistd.h>


#include <sys/shm.h>
#include <iomanip>

#include "misc.h"
#include "bm_parameters.h"
#include "bm_benchmark.h"
#include "code_annotation.h"


using namespace std;

BM_DATA_TYPE *data_alloc_spe(size_t size);

void init_mat(bm_parameters *p);

int work(bm_parameters *p);

double work_part2(bm_parameters *p, uint64_t max_index, int step);

double get_micros();

double total_loops = 0.01; //TODO metre a 0

BM_DATA_TYPE *mat;  //THE matrix :)
bool WITH_MPI = false;
#define ADDR (0x0UL)


int shmid = -2;
int mpi_rank = 0;
int mpi_size = 1;

std::stringstream black_hole;


#ifdef  COMPILED_WITH_MPI
#include <mpi.h>
#else
#define MESS "NO MPI"
#endif


#define LOG(string, message)  if( p->m_is_log ) { string += message; }
#define ANNOTATE(ping, color)  if( p->m_is_annotate ) { yamb_annotate_set_event(ping, color); }

//#define SCREEN_OUTPUT false
#define SCREEN_OUTPUT true


int main(int argc, const char *argv[]) {
    fclose(stderr);
    double tot, band;
    double start_time, end_time = 0.0;

//    cout.rdbuf(NULL);


    char **m_argv = (char **) argv;
//    strdup(*argv);

#ifdef  COMPILED_WITH_MPI
    if ( ! is_Launched_with_mpirun()){
        cout << "This program was compiled with MPI, you should use mpirun to launch it\n";
        return 1;
    }
#endif

    //MPI INIT
#ifdef  COMPILED_WITH_MPI
    MPI_Init(&argc, &m_argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
    DEBUG << "Hello world from processor "  << mpi_rank << "out of " << mpi_size << "processors\n";
#endif


    bm_parameters *my_parameters;

    //Parse argument, initiliaze the argument structure and print the configuration
    COUT_MPI << "\n ------------- CONFIGURATION --------------\n";
    my_parameters = new bm_parameters();
    my_parameters->init_arguments(argc, argv);
    if (mpi_rank == 0) {
        cout << "\n-- The following configuration will be used for the benchmark:\n";
        my_parameters->print_configuration();
    }
    //Matrice initialisation
    init_mat(my_parameters);

    COUT_MPI << "\n ---------------- BENCHMARK ----------------\n";

    MPI_BARRIER
    start_time = get_micros();
    work(my_parameters);
    end_time = get_micros();
    MPI_BARRIER

    if (mpi_rank == 0) {
        cout << "\n ---------------- RESULTS ----------------\n";
        tot = (end_time - start_time) * 1000.0 / total_loops;
        band = my_parameters->m_CACHE_LINE / tot;

        printf("%20s    %-10s \n", "Name", my_parameters->m_prefix.c_str());
        printf("%20s    %-10f \n", "Total micros", end_time - start_time);
        printf("%20s    %-10f \n", "Total Loops", total_loops);
        printf("%20s    %-10f ns/loop\n", "Performance", tot);
        printf("%20s    %-10f GB/s\n", "Bandwidth", band);
    }

    //Release the memory:
    if (my_parameters->m_is_huge_pages) {
        shmctl(shmid, IPC_RMID, NULL);
    } else {
        free(mat);
    }

    MPI_FINALIZE


    return (0);
}

int work(bm_parameters *p) {
    //Some init
    double log_max_index, time1, time2, ns_per_op, num_ops, best_measure, worst_measure, sum_measures;
    uint64_t max_index;
    int measure;
    string log_temporal = "";
    string big_log = "";
    char res_str[100];



    //Print header
    if (mpi_rank == 0) {
        printf("_ %s Stride  S   ->", p->m_prefix.c_str());
        LOG(log_temporal, "-1,");

        for (int stride : p->m_STRIDE_LIST) {
            char res_str[10];

            if (p->m_DISP == DISP_MODE::AVERAGE) sprintf(res_str, "%11d", stride);
            if (p->m_DISP == DISP_MODE::BEST) sprintf(res_str, "%11d", stride);
            if (p->m_DISP == DISP_MODE::TWO) sprintf(res_str, "%11d%11d", stride, stride);
            if (p->m_DISP == DISP_MODE::ALL) sprintf(res_str, "%11d%11d%11d", stride, stride, stride);
            COUT << res_str;
            if (p->m_is_log) {
                string s(res_str);
                s.erase(remove(s.begin(), s.end(), ' '), s.end());
                LOG(log_temporal, s);
                if (stride != p->m_MAX_STRIDE) {
                    LOG(log_temporal, ",");
                }
            }
        }
        printf("\n");
        LOG(log_temporal, '\n');

        printf("_ %s Value       ->", p->m_prefix.c_str());
        for (int stride : p->m_STRIDE_LIST) {
            if (p->m_DISP == DISP_MODE::AVERAGE) printf("%11s", "AVERAGE");
            if (p->m_DISP == DISP_MODE::BEST) printf("%11s", "BEST");
            if (p->m_DISP == DISP_MODE::TWO) printf("%11s%11s", "BEST", "AVERAGE");
            if (p->m_DISP == DISP_MODE::ALL) printf("%11s%11s%11s", "BEST", "WORST", "AVERAGE");
        }
        printf("\n");
    }


    //Work
    for (log_max_index = p->m_MIN_LOG10; log_max_index < p->m_MAX_LOG10 + .0000001; log_max_index += p->m_STEP_LOG10) {
        max_index = (THEINT) (double) (exp(log_max_index * LOG10) + 0.5);
        THEINT istride = max_index * sizeof(BM_DATA_TYPE) / 1024;

        if (max_index > p->m_MAT_NB_ELEM) {
            break;
        }

        if (mpi_rank == 0) {
            printf("_ %s K = %10s", p->m_prefix.c_str(), convert_size(istride * 1024).c_str());
            LOG(log_temporal, to_string(istride * 1024) + ",");
            ANNOTATE(string("K = " + convert_size(istride * 1024)).c_str(), "blue");
        }

        for (int stride : p->m_STRIDE_LIST) {
            if (stride != p->m_MIN_STRIDE) {
                LOG(log_temporal, ",");
            }
            double gb;
            best_measure = BIG_VAL;
            worst_measure = 0;
            sum_measures = 0.0;
            if (mpi_rank == 0) {
                ANNOTATE(string("Stride : " + to_string(stride)).c_str(), "red");
            }
            for (measure = 0; measure < p->m_MAX_MEASURES; measure++) {
                double t;

//                printf ("_work_ p->m_MIN_LOG10   %f \n", p->m_MIN_LOG10);
//                printf ("_work_ p->m_MAX_LOG10   %f \n", p->m_MAX_LOG10);
//                printf ("_work_ p->m_STEP_LOG10  %f \n", p->m_STEP_LOG10);
//                printf ("_work_ max_index        %d \n", max_index);
//                printf ("_work_ step             %d \n", step);
//                printf ("_work_ p->MIN_STRIDE    %d \n", p->m_MIN_STRIDE);


                int repeat;
                int stride_nb_elem = stride / sizeof(BM_DATA_TYPE);
                THEINT ops_per_scan = max_index / stride_nb_elem;
                if (ops_per_scan < MIN_OPS_PER_SCAN) {
                    num_ops = BIG_VAL;
                } else {
                    repeat = p->m_MAX_OPS / ops_per_scan;
                    if (repeat == 0)repeat = 1;

                    // --- BENCHMARK MEASURE ---
//                    MPI_BARRIER
                    time1 = get_micros();
                    num_ops = p->m_BENCHMARK(p, max_index, stride_nb_elem, repeat, ops_per_scan);
                    time2 = get_micros();
//                    MPI_BARRIER

                    t = (time2 - time1) * 1000.0;
                    if (num_ops != 0)
                        total_loops += num_ops;
                    if (t < best_measure)
                        best_measure = t;
                    if (t > worst_measure)
                        worst_measure = t;
                    sum_measures += t;
                }
            }

            if (mpi_rank != 0)
                continue;

            if (num_ops < BIG_VAL) {


                //Print the best measure
                ns_per_op = best_measure / num_ops;
                gb = p->m_CACHE_LINE;
                gb /= ns_per_op;
                if (p->m_DISP == DISP_UNIT::GB)ns_per_op = gb;
                if (p->m_DISP == DISP_UNIT::CY)ns_per_op *= p->m_GHZ;
                if ((p->m_DISP == DISP_MODE::BEST || p->m_DISP == DISP_MODE::ALL || p->m_DISP == DISP_MODE::TWO)) {
                    stringstream ss;
                    ss << fixed << setprecision(2) << ns_per_op;
                    float a;
                    ss >> a;
                    sprintf(res_str, "%11.2f", a);
                    COUT << res_str;
                    LOG(log_temporal, ss.str());
                }

                //Print the worst measure
                ns_per_op = worst_measure / num_ops;
                gb = p->m_CACHE_LINE;
                gb /= ns_per_op;
                if (p->m_DISP == DISP_UNIT::GB)ns_per_op = gb;
                if (p->m_DISP == DISP_UNIT::CY)ns_per_op *= p->m_GHZ;
                if (p->m_DISP == DISP_MODE::ALL) {
                    stringstream ss;
                    ss << fixed << setprecision(2) << ns_per_op;
                    float a;
                    ss >> a;
                    sprintf(res_str, "%11.2f", a);
                    COUT << res_str;
                    LOG(log_temporal, ss.str());
                }

                //Print the average measure
                ns_per_op = sum_measures / num_ops / p->m_MAX_MEASURES;
                gb = p->m_CACHE_LINE;
                gb /= ns_per_op;
                if (p->m_unit == DISP_UNIT::GB) {
                    ns_per_op = gb;
                }
                if (p->m_unit == DISP_UNIT::CY) {
                    ns_per_op *= p->m_GHZ;
                }
                if (p->m_DISP == DISP_MODE::ALL || p->m_DISP == DISP_MODE::TWO || p->m_DISP == DISP_MODE::AVERAGE) {
                    stringstream ss;
                    ss << fixed << setprecision(2) << ns_per_op;
                    float a;
                    ss >> a;
                    sprintf(res_str, "%11.2f", a);
                    COUT << res_str;
                    LOG(log_temporal, ss.str());

                }
            } else {
                sprintf(res_str, "%11s", "-");
                COUT << res_str;
                LOG(log_temporal, "0");
            }
        }

        COUT_MPI << endl << flush;
        LOG(log_temporal, "\n");
        LOG(big_log, log_temporal);
        log_temporal = "";
    }
    //Write to the file only at the end of the benchmark: performance matter
    //Move this line above to be able to stop the benchmark while being able to get the output file
    p->m_log_file << big_log << flush;


}


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//
//   we use the content of the data to count the number of memory operations
//   then for read count 1 per operation
//   then for write count 2 per operation    read+write
//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
BM_DATA_TYPE *data_alloc_spe(size_t size) {
    BM_DATA_TYPE **data;
    void *p;
//    len >>= 24;
//    len++;
//    len <<= 24; //align 16MB
    shmid = shmget(IPC_PRIVATE, size, SHM_HUGETLB | IPC_CREAT | SHM_R | SHM_W);
    cout << "\n SHMID  " << shmid << endl;
    assert(shmid >= 0);
    p = (ui64 *) shmat(shmid, (void *) ADDR, SHM_RND);
    *data = (BM_DATA_TYPE *) p;
    assert(*data != (void *) (-1ULL));
    return *data;
}

void init_mat(bm_parameters *p) {
    int lp;
    char tmpc[1024];
    unsigned int m2;
    BM_DATA_TYPE *mat_for_free;


//    set_affinity(MEM_AFF); //TODO

    if (p->m_is_huge_pages) {
        mat_for_free = (BM_DATA_TYPE *) data_alloc_spe(p->m_MAT_SIZE);
    } else {
        mat_for_free = (BM_DATA_TYPE *) malloc(p->m_MAT_SIZE);
    }

    assert(mat_for_free != NULL);


    mat = mat_for_free;
    //TODO what's that?
    mat += (p->m_MAT_OFFSET / sizeof(double));

    /* store 1 to count the read accesses */
    /* store 2 to count the read + write accesses */
    BM_DATA_TYPE init = (p->m_type == BENCH_TYPE::READ) ? 1.0 : 2.0;
    for (lp = 0; lp < p->m_MAT_NB_ELEM; lp++) {
        mat[lp] = init;
    }


    // need to set CPU affinity after having writen in memory
//    set_affinity(CPU_AFF); //TODO
}

double get_micros() {
    static struct timezone tz;
    static struct timeval tv;
    gettimeofday(&tv, &tz);
    return ((tv.tv_sec * 1000000.0) + tv.tv_usec);
}

