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
// $(grep -v DML bench_keccakf_quad.cpp|md5sum) should print 65f55efd3ffe6064e9ff9776f34db6c8

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
typedef unsigned long long u64;

#define vsize 4

typedef u64 vector_t __attribute__ ((vector_size(8*vsize)));

typedef union vector_u
{
  	vector_t 	vec;
  	u64    		u64s[vsize];
} vector;

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

static vector_u vkeccakf_rndc[24];

void prep_vkeccakf_rndc()
{
        for(int i=0;i<24;i++){
                for(int j=0;j<vsize;j++)
                        vkeccakf_rndc[i].u64s[j]=keccakf_rndc[i];
        }
}

static void keccakf(vector_u sts[25])
{
        int round;
        vector_t t,t1,t2, bc0,bc1,bc2,bc3,bc4;
        vector_t st0=sts[0].vec;
        vector_t st1=sts[1].vec;
        vector_t st2=sts[2].vec;
        vector_t st3=sts[3].vec;
        vector_t st4=sts[4].vec;
        vector_t st5=sts[5].vec;
        vector_t st6=sts[6].vec;
        vector_t st7=sts[7].vec;
        vector_t st8=sts[8].vec;
        vector_t st9=sts[9].vec;
        vector_t st10=sts[10].vec;
        vector_t st11=sts[11].vec;
        vector_t st12=sts[12].vec;
        vector_t st13=sts[13].vec;
        vector_t st14=sts[14].vec;
        vector_t st15=sts[15].vec;
        vector_t st16=sts[16].vec;
        vector_t st17=sts[17].vec;
        vector_t st18=sts[18].vec;
        vector_t st19=sts[19].vec;
        vector_t st20=sts[20].vec;
        vector_t st21=sts[21].vec;
        vector_t st22=sts[22].vec;
        vector_t st23=sts[23].vec;
        vector_t st24=sts[24].vec;

        for (round = 0; round < KECCAK_ROUNDS; round++) {

	        bc0=st0^st5^st10^st15^st20;
                bc1=st1^st6^st11^st16^st21;
                bc2=st2^st7^st12^st17^st22;
                bc3=st3^st8^st13^st18^st23;
                bc4=st4^st9^st14^st19^st24;
                t=bc4^((bc1<<1)|(bc1>>63));st0^=t;st5^=t;st10^=t;st15^=t;st20^=t;
                t=bc0^((bc2<<1)|(bc2>>63));st1^=t;st6^=t;st11^=t;st16^=t;st21^=t;
                t=bc1^((bc3<<1)|(bc3>>63));st2^=t;st7^=t;st12^=t;st17^=t;st22^=t;
                t=bc2^((bc4<<1)|(bc4>>63));st3^=t;st8^=t;st13^=t;st18^=t;st23^=t;
                t=bc3^((bc0<<1)|(bc0>>63));st4^=t;st9^=t;st14^=t;st19^=t;st24^=t;

                t1=st1;
                t2=st10 ; st10=ROTL64(t1,1 );
                t1=st7  ; st7 =ROTL64(t2,3 );
                t2=st11 ; st11=ROTL64(t1,6 );
                t1=st17 ; st17=ROTL64(t2,10);
                t2=st18 ; st18=ROTL64(t1,15);
                t1=st3  ; st3 =ROTL64(t2,21);
                t2=st5  ; st5 =ROTL64(t1,28);
                t1=st16 ; st16=ROTL64(t2,36);
                t2=st8  ; st8 =ROTL64(t1,45);
                t1=st21 ; st21=ROTL64(t2,55);
                t2=st24 ; st24=ROTL64(t1,2 );
                t1=st4  ; st4 =ROTL64(t2,14);
                t2=st15 ; st15=ROTL64(t1,27);
                t1=st23 ; st23=ROTL64(t2,41);
                t2=st19 ; st19=ROTL64(t1,56);
                t1=st13 ; st13=ROTL64(t2,8 );
                t2=st12 ; st12=ROTL64(t1,25);
                t1=st2  ; st2 =ROTL64(t2,43);
                t2=st20 ; st20=ROTL64(t1,62);
                t1=st14 ; st14=ROTL64(t2,18);
                t2=st22 ; st22=ROTL64(t1,39);
                t1=st9  ; st9 =ROTL64(t2,61);
                t2=st6  ; st6 =ROTL64(t1,20);
                t1=st1  ; st1 =ROTL64(t2,44);

                bc0=st0;bc1=st1;bc2=st2;bc3=st3;bc4=st4;
                st0^=~bc1&bc2;st1^=~bc2&bc3;st2^=~bc3&bc4;st3^=~bc4&bc0;st4^=~bc0&bc1;
                bc0=st5;bc1=st6;bc2=st7;bc3=st8;bc4=st9;
                st5^=~bc1&bc2;st6^=~bc2&bc3;st7^=~bc3&bc4;st8^=~bc4&bc0;st9^=~bc0&bc1;
                bc0=st10;bc1=st11;bc2=st12;bc3=st13;bc4=st14;
                st10^=~bc1&bc2;st11^=~bc2&bc3;st12^=~bc3&bc4;st13^=~bc4&bc0;st14^=~bc0&bc1;
                bc0=st15;bc1=st16;bc2=st17;bc3=st18;bc4=st19;
                st15^=~bc1&bc2;st16^=~bc2&bc3;st17^=~bc3&bc4;st18^=~bc4&bc0;st19^=~bc0&bc1;
                bc0=st20;bc1=st21;bc2=st22;bc3=st23;bc4=st24;
                st20^=~bc1&bc2;st21^=~bc2&bc3;st22^=~bc3&bc4;st23^=~bc4&bc0;st24^=~bc0&bc1;

                st0 ^= vkeccakf_rndc[round].vec;

        }

        sts[0].vec=st0;
        sts[1].vec=st1;
        sts[2].vec=st2;
        sts[3].vec=st3;
        sts[4].vec=st4;
        sts[5].vec=st5;
        sts[6].vec=st6;
        sts[7].vec=st7;
        sts[8].vec=st8;
        sts[9].vec=st9;
        sts[10].vec=st10;
        sts[11].vec=st11;
        sts[12].vec=st12;
        sts[13].vec=st13;
        sts[14].vec=st14;
        sts[15].vec=st15;
        sts[16].vec=st16;
        sts[17].vec=st17;
        sts[18].vec=st18;
        sts[19].vec=st19;
        sts[20].vec=st20;
        sts[21].vec=st21;
        sts[22].vec=st22;
        sts[23].vec=st23;
        sts[24].vec=st24;
}


static vector_u scramble(vector keys, int loops) 
{
  	u64 st[25] = {
		0x6170727463692e6bull,0x68406570632e6d6full,0x3335203539383937ull,0x3436333333383732ull,0x3931313736203339ull,
		0x350a323839303437ull,0x3730313834363020ull,0x2039363838323330ull,0x3131303737362039ull,0x3638313533203832ull,
		0x3930383334343036ull,0x3332373120323335ull,0x2020340a31383131ull,0x7463692e6b656469ull,0x70632e6d6f310a31ull,
		0x3539383937323338ull,0x3333383732203930ull,0x3736203339393937ull,0x3839303437343920ull,0x3834363020323636ull,
		0x3838323330383435ull,0x37373620390a2032ull,0x3533203832333236ull,0x3334343036353935ull,0x3120323335393530ull
  	};

	vector_u sts[25];
  	for(int i=0;i<25;i++) 
		for(int j=0;j<vsize;j++) sts[i].u64s[j]=st[i]^keys.u64s[j];

	for(int l=0;l<loops;l++){
  		keccakf(sts);
		vector tmp=sts[0] ; for(int i=0;i<24;i++) sts[i]=sts[i+1] ; sts[24]=tmp;  // rotate sts
	}

	vector_u sum;
	for(int j=0;j<vsize;j++) sum.u64s[j]=0;
  	for(int i=0;i<25;i++) 
		for(int j=0;j<vsize;j++) sum.u64s[j]^=sts[i].u64s[j];
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

	prep_vkeccakf_rndc();

	u64 sum=0;
	double micros=dml_micros();
	// all calls to scramble() are independant
	// you can execute them in any order
	assert(((lkey-fkey)%vsize)==0);
	for(u64 key=fkey;key<lkey;key+=vsize){
		double micros=dml_micros();
		vector_u keys;
		for(int j=0;j<vsize;j++)
			keys.u64s[j]=key+j;
		vector_u tsum = scramble(keys,loops);
		micros-=dml_micros();
		for(int j=0;j<vsize;j++){
			sum^=tsum.u64s[j];
			printf("_S_ %10llu %16llx %16llx %12.6lf\n",key+j,tsum.u64s[j],sum,-micros/1000000.0);
		}
		fflush(stdout);
	}
	micros-=dml_micros();
        double secs=-micros/1000000.0;
        double Mks=loops*(lkey-fkey)/1000000.0/secs;
        printf("_F_ fkey= %6llu lkey= %6llu loops= %6lld SUM= %llx seconds= %1.6lf   Mks= %1.6lf\n",fkey,lkey,loops,sum,secs,Mks);
}
