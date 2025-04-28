#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <iostream>
#include <sys/time.h>
#include <sys/shm.h>        // Large parge
#include <unistd.h>
#include <iterator>
#include <iomanip>

#include "misc.h"
#include "dml_misc.h"
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

double total_time_max_stride = 0.0;
double total_last_stride_loops = 0;

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

        double total_last_stride = total_time_max_stride * 1000.0 / total_last_stride_loops;
        double last_stride_bandwidth = my_parameters->m_CACHE_LINE / total_last_stride;

        printf("%20s    %-10s \n", "Name", my_parameters->m_prefix.c_str());
        printf("%20s    %-10f \n", "Total micros", end_time - start_time);
        printf("%20s    %-10f \n", "Total Loops", total_loops);
        printf("%20s    %-10f ns/loop\n", "Performance", tot);
        printf("%20s    %-10f GB/s\n", "Bandwidth", band);
        printf("%20s    %-10f GB/s\n", "Last Stride Bandwidth", last_stride_bandwidth);
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

    uint64_t current_max_index = p->m_START_SIZE;
    bool first_iteration = true;

    while (true) { 

        // Ensure current_max_index is valid before proceeding
        if (!first_iteration && current_max_index <= p->m_START_SIZE && p->m_START_SIZE != p->m_END_SIZE) {
            // Avoid getting stuck if step factor is too small or start size is 1
            current_max_index = p->m_START_SIZE + 1; // Force minimum progress
        }

        // Have we gone past the end size?
        if (current_max_index > p->m_END_SIZE) {
            break;
        }

        max_index = current_max_index; // Use the current loop size

        // Sanity check: Check against the allocated buffer size
        if (max_index > p->m_MAT_NB_ELEM) {
            if (mpi_rank == 0 && p->m_VERBOSE > 0) {
                printf("INFO: Stopping benchmark loop: Requested size %lu exceeds allocated elements %lu\n", max_index, p->m_MAT_NB_ELEM);
            }
            break;
        }

        size_t curr_dateset_size_bytes = max_index * sizeof(DML_DATA_TYPE); // Size of the subset in bytes

        if (mpi_rank == 0) {
            string size_str = convert_size(curr_dateset_size_bytes);
            printf("_ %-10s %10s : ", p->m_prefix.c_str(), size_str.c_str());
            LOG_MPI(log_temporal, to_string(curr_dateset_size_bytes) + ","); // Log byte size
            ANNOTATE(string("Size = " + size_str).c_str(), "blue"); // YAMB annotation
        }

        for (int stride_bytes : p->m_STRIDE_LIST) {

            // Check if stride is valid for the current data type size
            if (stride_bytes % sizeof(DML_DATA_TYPE) != 0) {
                if (mpi_rank == 0 && p->m_VERBOSE > 0)
                    printf("Warning: Stride %d bytes not multiple of element size %lu bytes. Skipping.\n", stride_bytes, sizeof(DML_DATA_TYPE));
                // Print placeholder and continue
                if (mpi_rank == 0) {
                    int cols = (p->m_DISP == DISP_MODE::ALL) ? 3 : ((p->m_DISP == DISP_MODE::TWO) ? 2 : 1);
                    for (int i = 0; i < cols; ++i) {
                        printf("%11s", "N/A");
                        LOG_MPI(log_temporal, "N/A");
                        if (i < cols - 1 || stride_bytes != p->m_STRIDE_LIST.back()) LOG_MPI(log_temporal, ",");
                    }
                }
                continue; // Skip to next stride
            }
            int stride_elems = stride_bytes / sizeof(DML_DATA_TYPE); // Stride in elements

            // Reset measures for this stride
            stride_best_measure = BIG_VAL;
            stride_worst_measure = 0.0;
            stride_sum_measures = 0.0;

            if (mpi_rank == 0) {
                string proc_count_str = (mpi_size > 1) ? to_string(mpi_size) : "1";
                ANNOTATE(string("Stride(B): " + to_string(stride_bytes) + " np: " + proc_count_str).c_str(), "red");
            }

            // Calculate ops per scan and repeats
            if (stride_elems == 0) stride_elems = 1; // Avoid division by zero if stride somehow became 0
            uint64_t nb_step_per_scan = max_index / stride_elems; // Number of stride steps to cover the array

            if (nb_step_per_scan >= MIN_OPS_PER_SCAN) {
                // Calculate repeat factor to reach target ops (p->m_MAX_OPS)
                int repeat = p->m_MAX_OPS / nb_step_per_scan;
                if (repeat < 5) { // Ensure minimum repeats for stability
                    repeat = 5;
                }

                // --- Measurement Loop ---
                for (int measure = 0; measure < p->m_MAX_MEASURES; measure++) {

                    MPI_BARRIER // Sync before measurement
                    time_start = get_micros(); // Time on each rank

                    // Call the selected benchmark function
                    nb_effective_op = p->m_BENCHMARK(p, stride_elems, repeat, nb_step_per_scan);

                    time_stop = get_micros(); // Time on each rank
                    MPI_BARRIER // Sync after measurement

                    // Accumulate time
                    double measure_total_time = (time_stop - time_start) * 1000.0;
                    if (measure_total_time < stride_best_measure)
                        stride_best_measure = measure_total_time;
                    if (measure_total_time > stride_worst_measure)
                        stride_worst_measure = measure_total_time;
                    stride_sum_measures += measure_total_time;

                    // Accumulate total loops/ops
                    total_loops += nb_effective_op;
                    if (stride_bytes == p->m_MAX_STRIDE) {
                        total_time_max_stride += time_stop - time_start;
                        total_last_stride_loops += nb_effective_op;
                    }
                } // End Measurement Loop

            } else { // Not enough steps for a reliable measurement
                nb_effective_op = BIG_VAL;
            }

            // --- Process and Print Results for this Stride (Rank 0) ---
            if (mpi_rank == 0) {
                if (nb_effective_op < BIG_VAL) {
                    // Print the best measure
                    ns_per_op = stride_best_measure / nb_effective_op;
                    double gb = p->m_CACHE_LINE / ns_per_op;
                    if (p->m_DISP == DISP_UNIT::GB) ns_per_op = gb;
                    if (p->m_DISP == DISP_UNIT::CY) ns_per_op *= p->m_GHZ;
                    if ((p->m_DISP == DISP_MODE::BEST || p->m_DISP == DISP_MODE::ALL || p->m_DISP == DISP_MODE::TWO)) {
                        stringstream ss;
                        ss << fixed << setprecision(2) << ns_per_op;
                        float a;
                        ss >> a;
                        sprintf(res_str, "%11.2f", a);
                        printf("%s", res_str);
                        LOG_MPI(log_temporal, ss.str());
                    }

                    // Print the worst measure
                    ns_per_op = stride_worst_measure / nb_effective_op;
                    gb = p->m_CACHE_LINE / ns_per_op;
                    if (p->m_DISP == DISP_UNIT::GB) ns_per_op = gb;
                    if (p->m_DISP == DISP_UNIT::CY) ns_per_op *= p->m_GHZ;
                    if (p->m_DISP == DISP_MODE::ALL) {
                        stringstream ss;
                        ss << fixed << setprecision(2) << ns_per_op;
                        float a;
                        ss >> a;
                        sprintf(res_str, "%11.2f", a);
                        printf("%s", res_str);
                        LOG_MPI(log_temporal, ss.str());
                    }

                    // Print the average measure
                    ns_per_op = stride_sum_measures / nb_effective_op / p->m_MAX_MEASURES;
                    gb = p->m_CACHE_LINE / ns_per_op;
                    if (p->m_unit == DISP_UNIT::GB) ns_per_op = gb;
                    if (p->m_unit == DISP_UNIT::CY) ns_per_op *= p->m_GHZ;
                    if (p->m_DISP == DISP_MODE::ALL || p->m_DISP == DISP_MODE::TWO || p->m_DISP == DISP_MODE::AVERAGE) {
                        stringstream ss;
                        ss << fixed << setprecision(2) << ns_per_op;
                        float a;
                        ss >> a;
                        sprintf(res_str, "%11.2f", a);
                        printf("%s", res_str);
                        LOG_MPI(log_temporal, ss.str());
                    }
                } else {
                    sprintf(res_str, "%11s", "-");
                    printf("%s", res_str);
                    LOG_MPI(log_temporal, "0");
                }
            } 

        } 
        if (mpi_rank == 0) {
            printf("\n");
            LOG_MPI(log_temporal, "\n");
        }

        first_iteration = false; // Mark that we've completed the first iteration

        if (current_max_index == p->m_END_SIZE) {
            break;
        }

        uint64_t next_max_index = 0;
        if (p->m_SIZE_STEP_FACTOR > 1.0) {
            next_max_index = static_cast<uint64_t>(static_cast<double>(current_max_index) * p->m_SIZE_STEP_FACTOR);
        } else {
            if (current_max_index == p->m_START_SIZE && p->m_START_SIZE != p->m_END_SIZE) {
                next_max_index = p->m_END_SIZE;
            } else {
                break;
            }
        }

        if (next_max_index <= current_max_index) {
            next_max_index = current_max_index + 1;
        }

        if (next_max_index > p->m_END_SIZE && current_max_index < p->m_END_SIZE) {
            next_max_index = p->m_END_SIZE;
        }

        current_max_index = next_max_index;

    }

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

