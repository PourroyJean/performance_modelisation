//#include"includeStudent.h"
#include <inttypes.h>
#include <stdio.h>
#include "misc.h"


typedef uint64_t ui64;

//TODO calibre K pour que ca fasse une seconde (a 0.01 pres)
//On fait la boucle avec dml + TIC TOC
// * 110^9 = freq op gg

/**
 *
 * @param argc coucou
 * @param argv jannou
 * @return
 */
int main(int argc, char *argv[]) {
    TIC
    ui64 deb = dml_cycles();
    int k = 305000000;
    int res, i, a,b,c,d,e = 1;
    for (i = 0; i < k; ++i) {
        a = b++ + c++;
        d = a + e++;
        res += d;
    }
    ui64 fin = dml_cycles();
    TOC("Boucle: ")
    printf("interval: %" PRIu64 "\n", fin-deb);
    return 0;
}