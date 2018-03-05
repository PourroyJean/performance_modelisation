//
// Created by Jean Pourroy on 02/03/2018.
//



#include <stdio.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <inttypes.h>
#include <sys/time.h>
#include <sys/io.h>
#include <sys/mman.h>
#include <cpuid.h>
#include <sys/resource.h>
#include <fcntl.h>

#include <errno.h>

#define PRINT_ERROR \
	do { \
		fprintf(stderr, "Error at line %d, file %s (%d) [%s]\n", \
		__LINE__, __FILE__, errno, strerror(errno)); exit(1); \
	} while(0)

#define MAXBUFLENGTH 65536
void map_pci_mmconfig_addr() {
    char *mybuf, *mystr;
    int i = 0;
    FILE *mystream;
    uint64_t myval = 0;

    if (mystream = fopen("/sys/devices/pci0000:3a/0000:3a:0a.6/resource", "r")) {
        mybuf = malloc(MAXBUFLENGTH);
        if (fread(mybuf, 1, MAXBUFLENGTH, mystream) > 0) {
            mystr = strtok(mybuf, "\n");
            myval = strtol(mystr, NULL, 16);
            printf ("%s %lf\n", mystr, myval);
            while (mystr) {
                if (strstr(mystr, "PCI MMCONFIG 0000")) {
                    myval = strtol(mystr, NULL, 16);
                    break;
                }
                mystr = strtok(NULL, "\n");
            }
        }
        free(mybuf);
        fclose(mystream);
//        mymapping_pciext = mmap(0, 0x10000000, PROT_READ | PROT_WRITE, MAP_SHARED, fdmem, myval);
//        DEBUG("PCI MMCONFIG 0000 address is %lx\n", myval);
//        DEBUG("PCI MMCONFIG 0000 mapped at %lx\n", mymapping_pciext);
    }
    else
        printf ("ERror");

    return;
}


int main (){
//    map_pci_mmconfig_addr();
    int fd;
    off_t target = 10;
    if((fd = open("/sys/devices/pci0000:3a/0000:3a:0a.6/config", O_RDWR | O_SYNC)) == -1) PRINT_ERROR;
    printf("Target offset is 0x%x, page size is %ld\n", (int) target, sysconf(_SC_PAGE_SIZE));
    fflush(stdout);

//    fd = open("/sys/devices/pci0000:3a/0000:3a:0a.6/config", O_RDWR | O_SYNC);
    void * ptr = mmap(0, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    printf("PCI BAR0 0x0000 = 0x%4x\n",  *((unsigned short *) ptr));

    printf ("ok");
}