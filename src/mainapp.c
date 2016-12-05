//#include"includeStudent.h"
#include <inttypes.h>
#include <stdio.h>
#include "misc.h"


typedef uint64_t ui64;




/**
 *
 * @param argc coucou
 * @param argv jannou
 * @return
 */
int main(int argc, char *argv[]) {
//    affiche_age();
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
    TOC
//    printf("Debut: %" PRIu64 "\n FIN: %" PRIu64 "\n interval: %" PRIu64 "\n Res = %d\n", deb, fin, fin-deb, res);
    printf("interval: %" PRIu64 "\n", fin-deb);
    return 0;
}
//TODO faire une boucle qui dure 1 seconde a la mano
//getime of day avant apres
//dml_cycle vant apres
//cycle/temps = freq