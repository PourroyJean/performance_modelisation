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
#include "dml_misc.h"
#include <sys/shm.h>        // Large parge
#include <unistd.h>
#include <iterator>


#include <sys/shm.h>
#include <iomanip>

#include "misc.h"
#include "dml_parameters.h"
#include "dml_benchmark.h"
#include "code_annotation.h"

#include <sched.h>


using namespace std;

void data_alloc_spe(size_t size);

void init_mat(Dml_parameters *p);

int work(Dml_parameters *p);

double work_part2(Dml_parameters *p, uint64_t max_index, int step);

double get_micros();

double total_loops = 0;

DML_DATA_TYPE *mat;  //THE matrix :)
bool WITH_MPI = false;


int shmid = -2;
int mpi_rank = 0;
int mpi_size = 1;
bool is_I_LOG = false;

extern std::stringstream black_hole;


#ifdef  COMPILED_WITH_MPI
#include <mpi.h>
#else
#define MESS "NO MPI"
#endif


int main(int argc, const char *argv[]) {

#ifdef  COMPILED_WITH_MPI
    char **m_argv = (char **) argv;
    MPI_Init(&argc, &m_argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
    DEBUG << "Hello world from processor "  << mpi_rank << "out of " << mpi_size << "processors\n" << flush;
#endif

    //Parse argument, initialize the argument structure and print the configuration
    COUT_MPI << "\n ------------- CONFIGURATION --------------\n";
    Dml_parameters *my_parameters = new Dml_parameters();
    my_parameters->init_arguments(argc, argv);
    if (mpi_rank == 0) {
        cout << "\n-- The following configuration will be used for the benchmark:\n";
        my_parameters->print_configuration();
    }
    init_mat(my_parameters);

    is_I_LOG = (mpi_rank == 0 && my_parameters->m_is_log);
    if (is_I_LOG) {
        my_parameters->m_log_file.open(my_parameters->m_log_file_name, std::ios_base::binary);
        my_parameters->m_log_file.clear();
        stringstream ss;
        std::copy(argv + 1, argv + argc, ostream_iterator<const char *>(ss, " "));
        my_parameters->m_log_file << "#This was launch with : " << ss.str() << endl << flush;

    }


    COUT_MPI << "\n ---------------- BENCHMARK ----------------\n";
    MPI_BARRIER
    double start_time = get_micros();
    work(my_parameters);
    double end_time = get_micros();
    MPI_BARRIER


    if (mpi_rank == 0) {
        cout << "\n ---------------- RESULTS ----------------\n";
        double tot = (end_time - start_time) * 1000.0 / total_loops;
        double band = my_parameters->m_CACHE_LINE / tot;

        printf("%20s    %-10s \n", "Name", my_parameters->m_prefix.c_str());
        printf("%20s    %-10f \n", "Total micros", end_time - start_time);
        printf("%20s    %-10f \n", "Total Loops", total_loops);
        printf("%20s    %-10f ns/loop\n", "Performance", tot);
        printf("%20s    %-10f GB/s\n", "Bandwidth", band);
    }

    //Release the memory:
    if (my_parameters->m_is_huge_pages) {
        shmctl(shmid, IPC_RMID, NULL);
        if (shmdt((const void *) mat) != 0) {
            COUT << "HUGE_PAGE: Detach failure\n";
            shmctl(shmid, IPC_RMID, NULL);
            exit(4);
        }
        shmctl(shmid, IPC_RMID, NULL);
    } else {
        free(mat);
    }
    if (is_I_LOG) {
        my_parameters->m_log_file.close();
    }


    MPI_FINALIZE


    return (0);
}

int work(Dml_parameters *p) {
    //Some init
    double log_max_index, time_start, time_stop, ns_per_op, nb_effective_op, stride_best_measure, stride_worst_measure, stride_sum_measures;
    uint64_t max_index;
    int measure;
    string log_temporal, big_log = "";
    char res_str[1000];


    //Print header
    if (mpi_rank == 0) {
        printf("_ %s Stride  S   ->", p->m_prefix.c_str());
        LOG_MPI(log_temporal, "-1,");

        for (int stride : p->m_STRIDE_LIST) {
            char res_str[100];

            //Print the size of the stride for the current column
            if (p->m_DISP == DISP_MODE::AVERAGE) sprintf(res_str, "%11d", stride);
            if (p->m_DISP == DISP_MODE::BEST) sprintf(res_str, "%11d", stride);
            if (p->m_DISP == DISP_MODE::TWO) sprintf(res_str, "%11d%11d", stride, stride);
            if (p->m_DISP == DISP_MODE::ALL) sprintf(res_str, "%11d%11d%11d", stride, stride, stride);

            COUT << res_str;
            if (p->m_is_log) {
                string s(res_str);
                s.erase(remove(s.begin(), s.end(), ' '), s.end());
                LOG_MPI(log_temporal, s);
                if (stride != p->m_MAX_STRIDE) {
                    LOG_MPI(log_temporal, ",");
                }
            }
        }
        printf("\n");
        LOG_MPI(log_temporal, '\n');

        printf("_ %s Value       ->", p->m_prefix.c_str());
        for (int stride : p->m_STRIDE_LIST) {
            if (p->m_DISP == DISP_MODE::AVERAGE) printf("%11s", "AVERAGE");
            if (p->m_DISP == DISP_MODE::BEST) printf("%11s", "BEST");
            if (p->m_DISP == DISP_MODE::TWO) printf("%11s%11s", "BEST", "AVERAGE");
            if (p->m_DISP == DISP_MODE::ALL) printf("%11s%11s%11s", "BEST", "WORST", "AVERAGE");
        }
        printf("\n");
    }

    //Work: Each loop = 1 sub data set to bench
    for (log_max_index = p->m_MIN_LOG10; log_max_index < p->m_MAX_LOG10 + .0000001; log_max_index += p->m_STEP_LOG10) {
        max_index = (THEINT) (double) (exp(log_max_index * LOG10) + 0.5); //Index of the last element we read/write
        THEINT curr_dateset_size = max_index * sizeof(DML_DATA_TYPE);     //Size of the subset in byte

//        DEBUG << "max index " << max_index << endl; //1000
//        DEBUG << "curr_dateset_size   " << curr_dateset_size << "(" << convert_size(curr_dateset_size) << ")" << endl;   //

        //Sanity check: enough element ?
        if (max_index > p->m_MAT_NB_ELEM) break;


        if (mpi_rank == 0) {
            printf("_ %s K = %10s", p->m_prefix.c_str(), convert_size(curr_dateset_size).c_str());
            LOG_MPI(log_temporal, to_string(curr_dateset_size) + ",");
            ANNOTATE(string("K = " + convert_size(curr_dateset_size)).c_str(), "blue");
        }

        //Each loop = 1 stride to bench
        for (int stride : p->m_STRIDE_LIST) {
            if (stride != p->m_MIN_STRIDE) {
                LOG_MPI(log_temporal, ",");
            }
            double gb;
            stride_best_measure = BIG_VAL;
            stride_worst_measure = 0;
            stride_sum_measures = 0.0;

            //Only rank 0 annotate YAMB
            if (mpi_rank == 0) {
                string s = "1";
                if (mpi_size > 1) {
                    s = to_string(mpi_size);
                }
                ANNOTATE(string("Stride : " + to_string(stride) + " nb_proc(" + s + ")").c_str(), "red");
            }


            //Calculate the number of measure / repeat
            int stride_size_nb_elem = stride / sizeof(DML_DATA_TYPE);     //i.e. [stride  = 32]   / [double = 8]        = 4 elem per stride
            THEINT nb_step_per_scan = max_index / stride_size_nb_elem;    //i.e. [dateset = 1024] / [4 elem per stride] = 256 ops

            //Only measure if there is enough number of elements
            if (nb_step_per_scan >= MIN_OPS_PER_SCAN) {

                //We repeat the measure several times to take advantage of locality
                int repeat = p->m_MAX_OPS / nb_step_per_scan;
                if (repeat < 5) {
                    repeat = 5;
                }

                // --- BENCHMARK MEASURE : each loop = 1 measure --
                for (measure = 0; measure < p->m_MAX_MEASURES; measure++) {

//                    MPI_BARRIER
                    time_start = get_micros();
                    nb_effective_op = p->m_BENCHMARK(p, stride_size_nb_elem, repeat, nb_step_per_scan);
                    time_stop = get_micros();
//                    MPI_BARRIER

                    // Accumulate nb operation
                    if (nb_effective_op != 0)
                        total_loops += nb_effective_op;

                    // Accumulate time
                    double measure_total_time = (time_stop - time_start) * 1000.0;
                    if (measure_total_time < stride_best_measure)
                        stride_best_measure = measure_total_time;
                    if (measure_total_time > stride_worst_measure)
                        stride_worst_measure = measure_total_time;
                    stride_sum_measures += measure_total_time;
                }
            } else {
                nb_effective_op = BIG_VAL;
            }

            if (mpi_rank != 0)
                continue;

            //If the benchmark was executed
            if (nb_effective_op < BIG_VAL) {

                //Print the best measure
                ns_per_op = stride_best_measure / nb_effective_op;
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
                    LOG_MPI(log_temporal, ss.str());
                }

                //Print the worst measure
                ns_per_op = stride_worst_measure / nb_effective_op;
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
                    LOG_MPI(log_temporal, ss.str());
                }

                //Print the average measure
                ns_per_op = stride_sum_measures / nb_effective_op / p->m_MAX_MEASURES;
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
                    LOG_MPI(log_temporal, ss.str());

                }
            } else {
                sprintf(res_str, "%11s", "-");
                COUT << res_str;
                LOG_MPI(log_temporal, "0");
            }
        }

        COUT_MPI << endl << flush;
        LOG_MPI(log_temporal, "\n");
        LOG_MPI(big_log, log_temporal);
        log_temporal = "";
    }
    //Write to the file only at the end of the benchmark: performance matter
    //Move this line above to be able to stop the benchmark while being able to get the output file
    if (is_I_LOG) {
        p->m_log_file << big_log << flush;
    }
    return 0;
}


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//
//   we use the content of the data to count the number of memory operations
//   READ  count 1 per operation
//   WRITE count 2 per operation (read+write, not always true !)
//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

// Thanks to Linus Torvald : https://github.com/torvalds/linux/blob/master/tools/testing/selftests/vm/hugepage-shm.c
#ifdef __ia64__
#define ADDR (void *)(0x8000000000000000UL)
#define SHMAT_FLAGS (SHM_RND)
#define FLAGS  SHM_HUGETLB | IPC_CREAT | IPC_EXCL | SHM_R | SHM_W
#else
#define ADDR (void *)(0x0UL)
#define SHMAT_FLAGS (0)
#define  FLAGS IPC_CREAT | IPC_EXCL | SHM_R | SHM_W
#endif


void data_alloc_spe(size_t size) {
    DML_DATA_TYPE **data;
    MPI_BARRIER

    cout << flush;
    shmid = shmget(IPC_PRIVATE, size, FLAGS);

    if (shmid == -1) {
        DEBUG << "ERROR on shmget " << std::strerror(errno) << '\n';
        exit(1);
    }


#ifdef  COMPILED_WITH_MPI
    //Est ce que tout le monde a son shmid
    int local = 1;
    if (shmid == -1){
        local =0;
    }
    int global = 0;
    MPI_Allreduce(&local, &global, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    if(global == mpi_size){
//        COUT_MPI << "Huge Pages OK\n";
    }
    else{
        cout << "\n Error shmget\n";
        shmctl(shmid, IPC_RMID, NULL);
    }
#endif

    //shmat return 0xfffff if failed, equal to (char *) -1
    mat = (DML_DATA_TYPE *) shmat(shmid, ADDR, SHMAT_FLAGS);
    if ((char *) mat == (char *) -1) {
        DEBUG << "Shared memory attach failure\n";
        shmctl(shmid, IPC_RMID, NULL);
        exit(2);
    }

}

void init_mat(Dml_parameters *p) {
    int lp;
    char tmpc[1024];
    unsigned int m2;
    DML_DATA_TYPE *mat_for_free;


//    set_affinity(MEM_AFF); //TODO

    if (p->m_is_huge_pages) {
        data_alloc_spe(p->m_MAT_SIZE);
    } else {
        mat = (DML_DATA_TYPE *) malloc(p->m_MAT_SIZE);
    }
    mat_for_free = mat;
    assert(mat_for_free != NULL);


    mat = mat_for_free;
    //TODO what's that?
    mat += (p->m_MAT_OFFSET / sizeof(double));

    /* store 1 to count the read accesses */
    /* store 2 to count the read + write accesses */
    DML_DATA_TYPE init = (p->m_type == BENCH_TYPE::READ) ? 1.0 : 2.0;
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

