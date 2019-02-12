#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <ctype.h>
#include <stdint.h>
#include <assert.h>

typedef uint64_t ui64;
typedef uint32_t ui32;
int shmid=-2;
#define ADDR (0x0UL)


static void data_alloc_spe(ui64 **data, int nrow) {
    void *p;
    size_t len=(ui64)nrow*sizeof(ui64);
    len >>=24;len++;len<<=24; //align 16MB
    shmid=shmget(IPC_PRIVATE,len,SHM_HUGETLB|IPC_CREAT|SHM_R|SHM_W);
    printf ("shmid = %d", shmid);
    assert(shmid>=0);
    p=(ui64*)shmat(shmid,(void*)ADDR,SHM_RND);
    *data = (ui64 *)p;
    assert(*data!=(void*)(-1ULL));
}


int main(int argc, char *argv[]) {
    printf ("coucou\n");
    ui64 *data;

    data_alloc_spe(&data, 10);

}


