//lxr.free-electrons.com/source/crypto/sha3_generic.c#L24
/*
 * Cryptographic API.
 *
 * SHA-3, as specified in
 * http://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.202.pdf
 *
 * SHA-3 code by Jeff Garzik <jeff@garzik.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)•
 * any later version.
 *
 */

// simple benchmark created by patrick.demichel@hpe.com to exercise the keccakf kernel and optimize it on wide range of hardwares
// based on http://lxr.free-electrons.com/source/crypto/sha3_generic.c#L24 
// We make it as simple as possible to minimize the effort of recoding for the many architectures and features we plan to test
// In real codes we will spend 99% in this section
// Then is will be a good demonstrator of the potential architectures and required features to deliver a good performance for such code patterns
// $(grep -v DML bench_keccakf_ref.cpp|md5sum) should print 3689add9d6181f7ffc3fa7ea4d8cfd05

#include <stdio.h>
#include <stdlib.h>
typedef unsigned long long u64;

#define KECCAK_ROUNDS 24

#define ROTL64(x, y) (((x) << (y)) | ((x) >> (64 - (y))))

static const u64 keccakf_rndc[24] = {
        0x0000000000000001ULL, 0x0000000000008082ULL, 0x800000000000808aULL,
        0x8000000080008000ULL, 0x000000000000808bULL, 0x0000000080000001ULL,
        0x8000000080008081ULL, 0x8000000000008009ULL, 0x000000000000008aULL,
        0x0000000000000088ULL, 0x0000000080008009ULL, 0x000000008000000aULL,
        0x000000008000808bULL, 0x800000000000008bULL, 0x8000000000008089ULL,
        0x8000000000008003ULL, 0x8000000000008002ULL, 0x8000000000000080ULL,
        0x000000000000800aULL, 0x800000008000000aULL, 0x8000000080008081ULL,
        0x8000000000008080ULL, 0x0000000080000001ULL, 0x8000000080008008ULL
};

static const int keccakf_rotc[24] = {
        1,  3,  6,  10, 15, 21, 28, 36, 45, 55, 2,  14,
        27, 41, 56, 8,  25, 43, 62, 18, 39, 61, 20, 44
};

static const int keccakf_piln[24] = {
        10, 7,  11, 17, 18, 3, 5,  16, 8,  21, 24, 4,
        15, 23, 19, 13, 12, 2, 20, 14, 22, 9,  6,  1
};

/* update the state with given number of rounds */
// NO changes at all in keccakf

static void keccakf(u64 st[25])
{
        int i, j, round;
        u64 t, bc[5];

        for (round = 0; round < KECCAK_ROUNDS; round++) {

                /* Theta */
                for (i = 0; i < 5; i++)
                        bc[i] = st[i] ^ st[i + 5] ^ st[i + 10] ^ st[i + 15]
                                ^ st[i + 20];

                for (i = 0; i < 5; i++) {
                        t = bc[(i + 4) % 5] ^ ROTL64(bc[(i + 1) % 5], 1);
                        for (j = 0; j < 25; j += 5)
                                st[j + i] ^= t;
                }

                /* Rho Pi */
                t = st[1];
                for (i = 0; i < 24; i++) {
                        j = keccakf_piln[i];
                        bc[0] = st[j];
                        st[j] = ROTL64(t, keccakf_rotc[i]);
                        t = bc[0];
                }

                /* Chi */
                for (j = 0; j < 25; j += 5) {
                        for (i = 0; i < 5; i++)
                                bc[i] = st[j + i];
                        for (i = 0; i < 5; i++)
                                st[j + i] ^= (~bc[(i + 1) % 5]) &
                                             bc[(i + 2) % 5];
                }

                /* Iota */
                st[0] ^= keccakf_rndc[round];
        }
}

static u64 scramble(u64 key, int loops) 
{
  	u64 st[25] = {
		0x6170727463692e6bull,0x68406570632e6d6full,0x3335203539383937ull,0x3436333333383732ull,0x3931313736203339ull,
		0x350a323839303437ull,0x3730313834363020ull,0x2039363838323330ull,0x3131303737362039ull,0x3638313533203832ull,
		0x3930383334343036ull,0x3332373120323335ull,0x2020340a31383131ull,0x7463692e6b656469ull,0x70632e6d6f310a31ull,
		0x3539383937323338ull,0x3333383732203930ull,0x3736203339393937ull,0x3839303437343920ull,0x3834363020323636ull,
		0x3838323330383435ull,0x37373620390a2032ull,0x3533203832333236ull,0x3334343036353935ull,0x3120323335393530ull
  	};

  	for(int i=0;i<25;i++) st[i] ^= key;

	for(int l=0;l<loops;l++){
  		keccakf(st);
		u64 tmp=st[0];for(int i=0;i<24;i++) st[i] = st[i+1];st[24]=tmp;  // rotate st
	}

	u64 sum=0;
  	for(int i=0;i<25;i++) sum ^= st[i] ;
  	return sum;
}

#include <sys/time.h>
double dml_micros()
{
        static struct timeval  tv;
        gettimeofday(&tv,0);
        return((tv.tv_sec*1000000.0)+tv.tv_usec);
}


int main(int argc,char **argv)
{
  	const u64 fkey  = argc > 1 ? atoi(argv[1]) : 0;      // first key
  	const u64 lkey  = argc > 2 ? atoi(argv[2]) : 1<<10;  // last  key
  	const u64 loops = argc > 3 ? atoi(argv[3]) : 1<<20;  // #loops
	u64 sum=0;
	double micros=dml_micros();
	// all calls to scramble() are independant
	// you can execute them in any order
	for(u64 key=fkey;key<lkey;key++){
		double micros=dml_micros();
		u64    tsum = scramble(key,loops);
		sum ^= tsum;
		micros-=dml_micros();
		printf("_S_ %10llu %16llx %16llx %12.6lf\n",key,tsum,sum,-micros/1000000.0);
		fflush(stdout);
	}
	micros-=dml_micros();
	double secs=-micros/1000000.0;
	double Mks=loops*(lkey-fkey)/1000000.0/secs;
	printf("_F_ fkey= %6llu lkey= %6llu loops= %6lld SUM= %llx seconds= %1.6lf   Mks= %1.6lf\n",fkey,lkey,loops,sum,secs,Mks);
}

