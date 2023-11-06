//==============================================================
// Author: Jean Pourroy, HPE, October 2023
// Originally Copyright © 2022 Intel Corporation
//
// SPDX-License-Identifier: MIT
// =============================================================

/**
 * Description:
 * This program serves as a benchmark to measure the performance of Intel's AMX technology.
 * It initializes two matrices in BF16 (Brain Floating Point) format and performs a dot product
 * operation on these matrices. The results are then stored in an FP32 matrix.
 * This benchmark is designed to be run on Intel processors that support AMX technology.
 *
 * Usage:
 * To run this benchmark, compile it using an AMX-compatible compiler and execute the resulting
 * binary. You can specify the CPU core for binding as an argument, otherwise, it defaults to core 0.
 *
 * Note:
 * The code for executing two tile sets is written but commented out for ease of use.
 */

#include <immintrin.h>
#include <sched.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>

// Corresponds to the FINAL number of operations (dpbf16ps) executed by the loop
#ifndef NB_OPERATION
#define NB_OPERATION 100000000  // This should last 4 seconds
#endif

// AMX CONFIG
#define ARCH_GET_XCOMP_PERM 0x1022
#define ARCH_REQ_XCOMP_PERM 0x1023
#define XFEATURE_XTILECFG 17
#define XFEATURE_XTILEDATA 18
#define TILE_SRC_1 2
#define TILE_SRC_2 3
#define TILE_SRC_3 5
#define TILE_SRC_4 6
#define TILE_RES_1 1
#define TILE_RES_2 4

// CPU INFO
#define MAX_ROWS 16   // Each Tile has 16 raws
#define ROW_SIZE 64   // Each Tile is 64 bytes
#define STRIDE 64     // Row major memory = size of a row
#define BF16_BYTES 2  // bfloat16 (brain floating point) occupying 16 bitsoccupying 16 bits
#define CPU_BIND 0    // By default we attach to core 0

// Benchmark configuration for BF16
#define NB_BF16_PER_LINE ROW_SIZE / BF16_BYTES               // Each Tile line can hold 32 FP16
#define NB_LINE_PER_TILE MAX_ROWS                            // We use all the 16 rows available
#define NB_BF16_PER_TILE NB_BF16_PER_LINE *NB_LINE_PER_TILE  // Each
#define OUT_NB_FP32 ROWS *ROWS                               // Square output matrice 16*16 elements
// Matrices equivalent
#define ROWS NB_LINE_PER_TILE
#define COL NB_BF16_PER_LINE

// Define tile config data structure
typedef struct __tile_config {
    uint8_t palette_id;      // 1 byte, representing the palette ID
    uint8_t start_row;       // 1 byte, representing the starting row number
    uint8_t reserved_0[14];  // 14 reserved bytes, value must be zero
    uint16_t colsb[16];      // Row size in bytes for 16 tiles = 32 bytes in total (2 bytes * 16),
    uint8_t rows[16];        // Number of rows for 16 tiles (only 8 tile configurations in our case)

} __tilecfg;

/* Initialize tile config */
static void init_tile_config(__tilecfg *tileinfo) {
    int i;
    tileinfo->palette_id = 1;
    tileinfo->start_row = 0;

    // Special tile 0
    tileinfo->colsb[0] = MAX_ROWS;
    tileinfo->rows[0] = MAX_ROWS;

    // For the 3 pallets we use
    // - MAX_ROWS = 16 lines
    // - Each line is ROW_SIZE = 64 bytes

    tileinfo->colsb[TILE_RES_1] = ROW_SIZE;
    tileinfo->rows[TILE_RES_1] = MAX_ROWS;
    tileinfo->colsb[TILE_SRC_1] = ROW_SIZE;
    tileinfo->rows[TILE_SRC_1] = MAX_ROWS;
    tileinfo->colsb[TILE_SRC_2] = ROW_SIZE;
    tileinfo->rows[TILE_SRC_2] = MAX_ROWS;

    // Here we initialize another tile triplet to facilitate testing in the main function loop
    // but they are not used in this version
    tileinfo->colsb[TILE_RES_2] = ROW_SIZE;
    tileinfo->rows[TILE_RES_2] = MAX_ROWS;
    tileinfo->colsb[TILE_SRC_3] = ROW_SIZE;
    tileinfo->rows[TILE_SRC_3] = MAX_ROWS;
    tileinfo->colsb[TILE_SRC_4] = ROW_SIZE;
    tileinfo->rows[TILE_SRC_4] = MAX_ROWS;

    _tile_loadconfig(tileinfo);
}

/* Set_tiledata_use() - Invoke syscall to set ARCH_SET_STATE_USE */
static bool set_tiledata_use() {
    if (syscall(SYS_arch_prctl, ARCH_REQ_XCOMP_PERM, XFEATURE_XTILEDATA)) {
        printf("\n Fail to do XFEATURE_XTILEDATA \n\n");
        return false;
    } else {
        printf("\n TILE DATA USE SET - OK \n\n");
        return true;
    }
    return true;
}

/**
 * Convert a single-precision floating-point number (FP32) to bfloat16 (BF16) format.
 * BF16 format retains the same 8-bit exponent from FP32 but truncates the mantissa
 * from 23 bits to the top 7 bits. This function achieves the conversion by taking
 * the most significant 16 bits of the FP32 representation.
 *
 * @param f Input FP32 value.
 * @return Corresponding BF16 value as a uint16_t.
 */
static uint16_t FP32_toBF16(float f) {
    union {
        float f;
        uint32_t u;
    } fu;
    fu.f = f;
    fu.u = (fu.u >> 16) & 0xffff;
    return (uint16_t)fu.u;
}

/**
 * Convert a number from bfloat16 (BF16) format to a single-precision floating-point
 * number (FP32).
 *
 * BF16 format retains the same 8-bit exponent from FP32 but uses only the top 7 bits
 * of the 23-bit mantissa. When converting to FP32, this function pads the less
 * significant bits of the mantissa with zeros.
 *
 * @param bf Input BF16 value as a uint16_t.
 * @return Corresponding FP32 value.
 */
static float BF16_to_FP32(uint16_t bf) {
    union {
        float f;
        uint32_t u;
    } fu;
    fu.u = (uint32_t)bf << 16;
    return fu.f;
}

/**
 * Initializes a buffer with BF16 (Brain Floating Point) values.
 *
 * This function takes an input float value, converts it to BF16 format,
 * and then initializes a buffer with the BF16 representation of the input value.
 *
 * @param src1   Pointer to the buffer to be initialized.
 * @param value  Float value to be converted and used for initialization.
 * @param size   Size of the buffer (number of elements).
 */
void init_bufferBF16(uint16_t *src1, float value, size_t size) {
    uint16_t bf16_value = FP32_toBF16(value);
    for (size_t i = 0; i < size; ++i) {
        src1[i] = bf16_value;
    }
}

/**
 * Initializes a buffer with FP32 (Single Precision Floating Point) values.
 *
 * This function takes an input float value and initializes a buffer 
 * with the same value repeated for the specified number of times.
 *
 * @param src    Pointer to the buffer to be initialized.
 * @param value  Float value to be used for initialization.
 * @param size   Size of the buffer (number of elements).
 */
void init_buffer_FP32(float *src, float value, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        src[i] = value;
    }
}


/**
 * Prints the contents of a buffer holding BF16 (Brain Floating Point) values.
 *
 * This function reads the BF16 values from the buffer, converts them to 
 * FP32 (Single Precision Floating Point) format for display, and then prints 
 * the values in a 2D matrix format based on the provided number of rows and columns.
 *
 * @param buf    Pointer to the buffer containing BF16 values.
 * @param rows   Number of rows in the 2D representation of the buffer.
 * @param colsb  Number of columns (in bytes) in the 2D representation of the buffer.
 */
static void print_buffer_BF16(uint16_t *buf, int32_t rows, int32_t colsb) {
    printf("print_buffer_BF16\n");
    printf("rows: %d\n", rows);
    printf("colsb: %d\n", colsb);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < colsb; j++) {
            printf("%3.2f ", BF16_to_FP32(buf[i * colsb + j]));
        }
        printf("\n");
    }
    printf("\n");
}


/**
 * Prints the contents of a buffer holding FP32 values.
 *
 * This function reads the FP32 values directly from the buffer and prints 
 * them in a 2D matrix format based on the provided number of rows and columns.
 *
 * @param buf    Pointer to the buffer containing FP32 values.
 * @param rows   Number of rows in the 2D representation of the buffer.
 * @param colsb  Number of columns (in bytes) in the 2D representation of the buffer.
 */
static void print_buffer_FP32(float *buf, int32_t rows, int32_t colsb) {
    printf("print_buffer_FP32\n");
    printf("rows: %d\n", rows);
    printf("colsb: %d\n", colsb);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < (colsb); j++) {
            printf("%f ", buf[i * colsb + j]);
        }
        printf("\n");
    }
    printf("\n");
}

int main(int argc, char **argv) {
    __tilecfg tile_data = {0};        // All fields set to 0: so even pallets and unused raw are 0
    uint16_t src1[NB_BF16_PER_TILE];  // 16rows x 64 bytes = 512 BF16 per tile
    uint16_t src2[NB_BF16_PER_TILE];  // 16rows x 64 bytes = 512 BF16 per tile
    float res[ROWS * ROWS];           // Square ouput matrice

    //    Let's bind the process to a particular core
    int cpu_bind = CPU_BIND;
    cpu_set_t mycpumask;
    CPU_ZERO(&mycpumask);

    if (argc >= 2) {
        char *ptr;
        int value = strtol(argv[1], &ptr, 10);
        // Checks if conversion was successful and if there are no remaining non-numeric characters
        if (*ptr == '\0') {
            cpu_bind = value;
        } else {
            printf("Error: The second argument is not a valid integer.\n");
        }
    }



    // Bind the process to a specific core
    CPU_SET(cpu_bind, &mycpumask);
    sched_setaffinity(0, sizeof(cpu_set_t), &mycpumask);
    sched_getaffinity(0, sizeof(cpu_set_t), &mycpumask);        // double-check
    for (int i = 0; i < sysconf(_SC_NPROCESSORS_CONF); i++) {
        if (CPU_ISSET(i, &mycpumask))
            printf("+ Running on CPU #%d\n", i);
    };

    // Request permission to linux kernel to run AMX
    if (!set_tiledata_use())
        exit(-1);

    // Load tile configuration
    init_tile_config(&tile_data);

    // Init src matrix buffers with data
    init_bufferBF16(src1, 0.01, NB_BF16_PER_TILE);
    init_bufferBF16(src2, 0.01, NB_BF16_PER_TILE);
    init_buffer_FP32(res, 0, OUT_NB_FP32);
    
    // print_buffer_BF16(src1, ROWS, COL);
    // print_buffer_BF16(src2, COL, ROWS);
    // print_buffer_FP32(res, ROWS, ROWS);

    // STRIDE = 64 bytes = size of each row
    _tile_loadd(TILE_SRC_1, src1, STRIDE);
    _tile_loadd(TILE_SRC_2, src2, STRIDE);
    _tile_loadd(TILE_RES_1, res, STRIDE);

    // Not used in this version : uncomment the corresponding line in the following loop
    _tile_loadd(TILE_SRC_3, src1, STRIDE);
    _tile_loadd(TILE_SRC_4, src2, STRIDE);
    _tile_loadd(TILE_RES_2, res, STRIDE);

    // Compute dot-product of bytes in tiles
    for (long long int i = 0; i < NB_OPERATION / 10; ++i) {
        _tile_dpbf16ps(TILE_RES_1, TILE_SRC_1, TILE_SRC_2);
        _tile_dpbf16ps(TILE_RES_1, TILE_SRC_1, TILE_SRC_2);
        _tile_dpbf16ps(TILE_RES_1, TILE_SRC_1, TILE_SRC_2);
        _tile_dpbf16ps(TILE_RES_1, TILE_SRC_1, TILE_SRC_2);
        _tile_dpbf16ps(TILE_RES_1, TILE_SRC_1, TILE_SRC_2);
        _tile_dpbf16ps(TILE_RES_1, TILE_SRC_1, TILE_SRC_2);
        _tile_dpbf16ps(TILE_RES_1, TILE_SRC_1, TILE_SRC_2);
        _tile_dpbf16ps(TILE_RES_1, TILE_SRC_1, TILE_SRC_2);
        _tile_dpbf16ps(TILE_RES_1, TILE_SRC_1, TILE_SRC_2);
        _tile_dpbf16ps(TILE_RES_1, TILE_SRC_1, TILE_SRC_2);
        // _tile_dpbf16ps(TILE_RES_2, TILE_SRC_3, TILE_SRC_4); 
    }

    // Store the tile data to memory
    _tile_stored(TILE_RES_1, res, STRIDE);
    _tile_stored(TILE_RES_2, res, STRIDE);
    // print_buffer_FP32(res, ROWS, ROWS);

    // Release the tile configuration to return to the init state,
    // which releases all storage it currently holds
    _tile_release();

    return 0;
}
