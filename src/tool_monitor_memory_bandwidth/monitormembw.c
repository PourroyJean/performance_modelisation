#include "utils.h"

#define NBUFS 1100
#define SAMPLING_INTERVAL_US 1000

#define MAX_NCHAN 16 /* the max number of possible channels */
int nchan = 0; /* will be set to the number of active channels */
int bus = -1; /* will be set to the bus to use */
int chan_dev[MAX_NCHAN]; /* devices corresponding to the MAX_NCHAN possible channels */
int chan_func[MAX_NCHAN]; /* functions corresponding to the MAX_NCHAN possible channels */
double membw[NBUFS][2][MAX_NCHAN];
unsigned long old_ev[2][MAX_NCHAN];
unsigned long ev[2][MAX_NCHAN];
double time = 0;
double old_time = 0;
double times[NBUFS];

void map_channels();

void init_counters();

void stop_counters();

void read_counters(int ibuf);

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* MAIN
/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
int main(int argc, char *argv[]) {
    register int ibuf, jbuf;
    register int chan;
    register useconds_t delay;
    register double start = 0., now;
    register double timeflush;

    global_init();

    map_channels();
    if ((nchan == 0) && (mymapping_mchbar == 0UL)) {
        printf("Could not identify a way to measure the memory bandwidth, aborting.\n");
        return 1;
    }
    init_counters();

    if (argv[1]) {
        start = atof(argv[1]); // start is always passed as a plain number of seconds
        if (start < 0) {
            stop_counters();
            return 0;
        }
    }

    now = mytimestamp();
    if (start < now) {
        start = 1. +
                (useconds_t) (now); // let's at least align on a new plain second, to remain synchronized with any other similar instance
    }
    delay = (useconds_t) (1000000. * (start - now));
    usleep(delay);

    time = mytimestamp() - start;
    old_time = time;
    read_counters(-1);

    while (1) {
        ibuf = 0;
        timeflush = 1. + (double) (int) (time);
        DEBUG("Will flush next data at t=%f...\n", timeflush);
        do {
            delay = (useconds_t) (SAMPLING_INTERVAL_US -
                                  ((useconds_t) (1000000. * (mytimestamp() - start))) % (SAMPLING_INTERVAL_US));
            usleep(delay);
            time = mytimestamp() - start;
            times[ibuf] = time;
            read_counters(ibuf);
            old_time = time;
            ibuf++;
        } while ((ibuf < NBUFS) && (time <= timeflush));



        for (jbuf = 0; jbuf < ibuf; jbuf++) {
            printf("t= %.6f", times[jbuf]);
            if (nchan == 0) {
                printf(" rtot= %.1f wtot= %.1f", membw[jbuf][0][0], membw[jbuf][1][0]);
            } else {
                for (chan = 0; chan < nchan; chan++) {
                    printf(" r%d= %.1f", chan, membw[jbuf][0][chan]);
                }
                for (chan = 0; chan < nchan; chan++) {
                    printf(" w%d= %.1f", chan, membw[jbuf][1][chan]);
                }
            }
            printf("\n");
        }
        DEBUG("Have flushed %d samples at t=%f.\n", ibuf, time);
        fflush(stdout);
    }

    return 0;
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* init counters of corresponding processor                        */
/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
void init_counters() {
    int chan;
    if (systype == KNL) { /* KNL */
        for (chan = 0; chan < nchan; chan++) {
            // FREEZE COUNTERS
            PCIDATA8(bus, chan_dev[chan], chan_func[chan], 0xb31) = 0x1;
            MSYNC;
            // RESET COUNTERS
            PCIDATA8(bus, chan_dev[chan], chan_func[chan], 0xb30) = 0x2;
            MSYNC;
            // ZERO COUNTERS
            //                #0
            PCIDATA32(bus, chan_dev[chan], chan_func[chan], 0xb04) = 0x0;
            PCIDATA32(bus, chan_dev[chan], chan_func[chan], 0xb00) = 0x0;
            //                #1
            PCIDATA32(bus, chan_dev[chan], chan_func[chan], 0xb0c) = 0x0;
            PCIDATA32(bus, chan_dev[chan], chan_func[chan], 0xb08) = 0x0;
            // RESET MONITORING CTRL REGISTERS
            PCIDATA8(bus, chan_dev[chan], chan_func[chan], 0xb30) = 0x1;
            MSYNC;
            // CONFIGURE COUNTER #0 FOR CAS.Reads (event:0x03 umask:b00000001), RESET AND REENABLE IT
            PCIDATA32(bus, chan_dev[chan], chan_func[chan], 0xb20) = 0x400103;
            // CONFIGURE COUNTER #1 FOR CAS.Writes (event:0x03 umask:b00000010), RESET AND REENABLE IT
            PCIDATA32(bus, chan_dev[chan], chan_func[chan], 0xb24) = 0x400203;
            MSYNC;
            // RELEASE COUNTERS
            PCIDATA8(bus, chan_dev[chan], chan_func[chan], 0xb31) = 0x0;
            MSYNC;
        }
    } else if ((systype == SDB) || (systype == HSW) || (systype == SKL)) { /* SDB/IVB/HSW/BDW/SKL */
        for (chan = 0; chan < nchan; chan++) {
            // FREEZE COUNTERS
            PCIDATA8(bus, chan_dev[chan], chan_func[chan], 0xf5) = 0x1;
            MSYNC;
            // RESET COUNTERS
            PCIDATA8(bus, chan_dev[chan], chan_func[chan], 0xf4) = 0x2;
            MSYNC;
            // ZERO COUNTERS
            //                #0
            PCIDATA32(bus, chan_dev[chan], chan_func[chan], 0xa4) = 0x0;
            PCIDATA32(bus, chan_dev[chan], chan_func[chan], 0xa0) = 0x0;
            //                #1
            PCIDATA32(bus, chan_dev[chan], chan_func[chan], 0xac) = 0x0;
            PCIDATA32(bus, chan_dev[chan], chan_func[chan], 0xa8) = 0x0;
            // RESET MONITORING CTRL REGISTERS
            PCIDATA8(bus, chan_dev[chan], chan_func[chan], 0xf4) = 0x1;
            MSYNC;
            // CONFIGURE COUNTER #0 FOR CAS_COUNT.RD (event:0x04 umask:b00000011), RESET AND REENABLE IT
            PCIDATA32(bus, chan_dev[chan], chan_func[chan], 0xd8) = 0x420304;
            // CONFIGURE COUNTER #1 FOR CAS_COUNT.WR (event:0x04 umask:b00001100), RESET AND REENABLE IT
            PCIDATA32(bus, chan_dev[chan], chan_func[chan], 0xdc) = 0x420c04;
            MSYNC;
            // RELEASE COUNTERS
            PCIDATA8(bus, chan_dev[chan], chan_func[chan], 0xf5) = 0x0;
            MSYNC;
        }
    }
    return;
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* stop/disable counters of corresponding processor                */
/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
void stop_counters() {
    int chan;
    if (systype == KNL) { /* KNL */
        for (chan = 0; chan < nchan; chan++) {
            // FREEZE COUNTERS
            PCIDATA8(bus, chan_dev[chan], chan_func[chan], 0xb31) = 0x1;
            MSYNC;
            // RESET COUNTERS
            PCIDATA8(bus, chan_dev[chan], chan_func[chan], 0xb30) = 0x2;
            MSYNC;
            // ZERO COUNTERS
            //                #0
            PCIDATA32(bus, chan_dev[chan], chan_func[chan], 0xb04) = 0x0;
            PCIDATA32(bus, chan_dev[chan], chan_func[chan], 0xb00) = 0x0;
            //                #1
            PCIDATA32(bus, chan_dev[chan], chan_func[chan], 0xb0c) = 0x0;
            PCIDATA32(bus, chan_dev[chan], chan_func[chan], 0xb08) = 0x0;
            // RESET MONITORING CTRL REGISTERS
            PCIDATA8(bus, chan_dev[chan], chan_func[chan], 0xb30) = 0x1;
            MSYNC;
            // UNCONFIGURE AND DISABLE COUNTER #0
            PCIDATA32(bus, chan_dev[chan], chan_func[chan], 0xb20) = 0x0;
            // UNCONFIGURE AND DISABLE COUNTER #1
            PCIDATA32(bus, chan_dev[chan], chan_func[chan], 0xb24) = 0x0;
            MSYNC;
        }
    } else if ((systype == SDB) || (systype == HSW) || (systype == SKL)) { /* SDB/IVB/HSW/BDW/SKL */
        for (chan = 0; chan < nchan; chan++) {
            // FREEZE COUNTERS
            PCIDATA8(bus, chan_dev[chan], chan_func[chan], 0xf5) = 0x1;
            MSYNC;
            // RESET COUNTERS
            PCIDATA8(bus, chan_dev[chan], chan_func[chan], 0xf4) = 0x2;
            MSYNC;
            // ZERO COUNTERS
            //                #0
            PCIDATA32(bus, chan_dev[chan], chan_func[chan], 0xa4) = 0x0;
            PCIDATA32(bus, chan_dev[chan], chan_func[chan], 0xa0) = 0x0;
            //                #1
            PCIDATA32(bus, chan_dev[chan], chan_func[chan], 0xac) = 0x0;
            PCIDATA32(bus, chan_dev[chan], chan_func[chan], 0xa8) = 0x0;
            // RESET MONITORING CTRL REGISTERS
            PCIDATA8(bus1, chan_dev[chan], chan_func[chan], 0xf4) = 0x1;
            MSYNC;
            // UNCONFIGURE AND DISABLE COUNTER #0
            PCIDATA32(bus, chan_dev[chan], chan_func[chan], 0xd8) = 0x0;
            // UNCONFIGURE AND DISABLE COUNTER #1
            PCIDATA32(bus, chan_dev[chan], chan_func[chan], 0xdc) = 0x0;
            MSYNC;
        }
    }
    return;
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* read counters of corresponding processor                        */
/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
void read_counters(int ibuf) {
    register uint64_t val0, val1;
    register uint64_t delta0, delta1;
    register int chan;
    if (systype == SOC) { /* SOC */
        ev[0][0] = *(uint32_t *) (mymapping_mchbar + 0x5050); /* reads, 32-bit counter */
        ev[1][0] = *(uint32_t *) (mymapping_mchbar + 0x5054); /* writes, 32-bit counter */
        if (ibuf >= 0) {
            membw[ibuf][0][0] = 0.000064 * ((uint32_t) (ev[0][0] - old_ev[0][0])) / (time - old_time);
            membw[ibuf][1][0] = 0.000064 * ((uint32_t) (ev[1][0] - old_ev[1][0])) / (time - old_time);
        }
        old_ev[0][0] = ev[0][0];
        old_ev[1][0] = ev[1][0];
    } else if (systype == KNL) { /* KNL */
        for (chan = 0; chan < nchan; chan++) {
            // FREEZE COUNTERS
            PCIDATA8(bus, chan_dev[chan], chan_func[chan], 0xb31) = 0x1;
            MSYNC;
            // READ COUNTER #0
            val0 = ((uint64_t) PCIDATA32(bus, chan_dev[chan], chan_func[chan], 0xb04) & 0x0000ffff) << 32;
            val0 |= (uint64_t) PCIDATA32(bus, chan_dev[chan], chan_func[chan], 0xb00);
            // READ COUNTER #1
            val1 = ((uint64_t) PCIDATA32(bus, chan_dev[chan], chan_func[chan], 0xb0c) & 0x0000ffff) << 32;
            val1 |= (uint64_t) PCIDATA32(bus, chan_dev[chan], chan_func[chan], 0xb08);
            // RELEASE COUNTERS
            PCIDATA8(bus, chan_dev[chan], chan_func[chan], 0xb31) = 0x0;
            MSYNC;
            // PROCESS DATA
            ev[0][chan] = val0;
            ev[1][chan] = val1;
            delta0 = (ev[0][chan] - old_ev[0][chan]) & 0x0000ffffffffffff;
            delta1 = (ev[1][chan] - old_ev[1][chan]) & 0x0000ffffffffffff;
            old_ev[0][chan] = ev[0][chan];
            old_ev[1][chan] = ev[1][chan];
            if (ibuf >= 0) {
                membw[ibuf][0][chan] = 0.000064 * (double) delta0 / (time - old_time);
                membw[ibuf][1][chan] = 0.000064 * (double) delta1 / (time - old_time);
            }
        }
    } else if ((systype == SDB) || (systype == HSW) || (systype == SKL)) { /* SDB/IVB/HSW/BDW/SKL */
        for (chan = 0; chan < nchan; chan++) {
            // FREEZE COUNTERS
            PCIDATA8(bus, chan_dev[chan], chan_func[chan], 0xf5) = 0x1;
            MSYNC;
            // READ COUNTER #0
            val0 = ((uint64_t) PCIDATA32(bus, chan_dev[chan], chan_func[chan], 0xa4) & 0x0000ffff) << 32;
            val0 |= (uint64_t) PCIDATA32(bus, chan_dev[chan], chan_func[chan], 0xa0);
            // READ COUNTER #1
            val1 = ((uint64_t) PCIDATA32(bus, chan_dev[chan], chan_func[chan], 0xac) & 0x0000ffff) << 32;
            val1 |= (uint64_t) PCIDATA32(bus, chan_dev[chan], chan_func[chan], 0xa8);
            // RELEASE COUNTERS
            PCIDATA8(bus, chan_dev[chan], chan_func[chan], 0xf5) = 0x0;
            MSYNC;
            // PROCESS DATA
            ev[0][chan] = val0;
            ev[1][chan] = val1;
            delta0 = (ev[0][chan] - old_ev[0][chan]) & 0x0000ffffffffffff;
            delta1 = (ev[1][chan] - old_ev[1][chan]) & 0x0000ffffffffffff;
            old_ev[0][chan] = ev[0][chan];
            old_ev[1][chan] = ev[1][chan];
            if (ibuf >= 0) {
                membw[ibuf][0][chan] = 0.000064 * (double) delta0 / (time - old_time);
                membw[ibuf][1][chan] = 0.000064 * (double) delta1 / (time - old_time);
            }
        }
    }
    return;
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* retrieve corresponding devices and functions for all channels   */
/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
void map_channels() {
    int i, j, nb, active;
    uint32_t val32;

    // Bus1 Devices 0xf and 0x1d corresponds to SDB/IVB ...
    val32 = PCIDATA32(bus1, 0x0f, 0x0, 0x0);
    DEBUG("Got id %x for B1 D0xf F0 0x0\n", val32);
    if ((val32 & 0xffffff) == 0xA88086) { /* SDB/IVB IMC0 */
        DEBUG("Discovered IVB IMC0 (F15)\n");
        systype = SDB;
        bus = bus1;
        // we deliberately assume that the first 2 channels are 'active', even if not populated...
        nchan = 0;
        chan_dev[nchan] = 0x10;
        chan_func[nchan] = 4;
        chan_dev[nchan + 1] = 0x10;
        chan_func[nchan + 1] = 5;
        nchan += 2;
        nb = 0;
        for (i = 4; i < 6; i++) {
            for (j = 0x80; j < 0x89; j = j + 0x4) {
                nb += 0x1 & ((PCIDATA32(bus1, 0x0f, i, j)) >> 14);
            }
        }
        DEBUG("Channels 3&4 of IMC0 have %d DIMMs\n", nb);
        if (nb > 0) {
            chan_dev[nchan] = 0x10;
            chan_func[nchan] = 0;
            chan_dev[nchan + 1] = 0x10;
            chan_func[nchan + 1] = 1;
            nchan += 2;
        }
        // Is there a second IMC?
        val32 = PCIDATA32(bus1, 0x1d, 0x0, 0x0);
        DEBUG("Got id %x for B1 D0x1d F0 0x0\n", val32);
        if ((val32 & 0xffffff) == 0x688086) { /* SDB/IVB IMC1 */
            DEBUG("Discovered IVB IMC1 (F29)\n");
            chan_dev[nchan] = 0x1e;
            chan_func[nchan] = 4;
            chan_dev[nchan + 1] = 0x1e;
            chan_func[nchan + 1] = 5;
            nchan += 2;
            nb = 0;
            for (i = 4; i < 6; i++) {
                for (j = 0x80; j < 0x89; j = j + 0x4) {
                    nb += 0x1 & ((PCIDATA32(bus1, 0x1d, i, j)) >> 14);
                }
            }
            DEBUG("Channels 3&4 of IMC1 have %d DIMMs\n", nb);
            if (nb > 0) {
                chan_dev[nchan] = 0x1e;
                chan_func[nchan] = 0;
                chan_dev[nchan + 1] = 0x1e;
                chan_func[nchan + 1] = 1;
                nchan += 2;
            }
        }
    }

    // Bus1 Devices 0x13,0x16 correspond to HSW ...
    val32 = PCIDATA32(bus1, 0x13, 0x0, 0x0);
    DEBUG("Got id %x for B1 D0x13 F0 0x0\n", val32);
    if ((val32 & 0xffffff) == 0xA88086) { /* HSW */
        DEBUG("Discovered HSW IMC0\n");
        systype = HSW;
        bus = bus1;
        nchan = 0;
        val32 = PCIDATA32(bus1, 0x14, 0x0, 0x0);
        DEBUG("Got id %x for B1 D0x14 F0 0x0\n", val32);
        if ((val32 & 0xffffff) == 0xB08086) { /* HSW */
            DEBUG("Discovered HSW IMC0 (F20)\n");
            // we deliberately assume that the first 2 channels are 'active', even if not populated...
            chan_dev[nchan] = 0x14;
            chan_func[nchan] = 0;
            chan_dev[nchan + 1] = 0x14;
            chan_func[nchan + 1] = 1;
            nchan += 2;
        }
        val32 = PCIDATA32(bus1, 0x15, 0x0, 0x0);
        DEBUG("Got id %x for B1 D0x15 F0 0x0\n", val32);
        if ((val32 & 0xffffff) == 0xB48086) { /* HSW */
            DEBUG("Discovered HSW IMC0 (F21)\n");
            nb = 0;
            active = 0;
            for (i = 4; i < 6; i++) {
                for (j = 0x80; j < 0x89; j = j + 0x4) {
                    val32 = (uint32_t) PCIDATA16(bus1, 0x13, i, j);
                    if (val32 != 0xffff) {
                        nb += 0x1 & (val32 >> 14);
                        active++;
                    }
                }
            }
            DEBUG("Channels 3&4 of IMC0 are %s and have a total of %d DIMMs\n", (active > 0) ? "active" : "inactive",
                  nb);
            if (active > 0) {
                chan_dev[nchan] = 0x15;
                chan_func[nchan] = 0;
                chan_dev[nchan + 1] = 0x15;
                chan_func[nchan + 1] = 1;
                nchan += 2;
            }
        }
        // Is there a second IMC?
        val32 = PCIDATA32(bus1, 0x16, 0x0, 0x0);
        DEBUG("Got id %x for B1 D0x16 F0 0x0\n", val32);
        if ((val32 & 0xffffff) == 0x688086) { /* HSW */
            DEBUG("Discovered HSW IMC1\n");
            val32 = PCIDATA32(bus1, 0x17, 0x0, 0x0);
            DEBUG("Got id %x for B1 D0x17 F0 0x0\n", val32);
            if ((val32 & 0xffffff) == 0xD08086) { /* HSW */
                DEBUG("Discovered HSW IMC1 (F23)\n");
                // we cannot deliberately assume that the first 2 channels are 'active', as 6- or 8-core BDW do have that IMC1 while all 4 channels are managed by IMC0...
                nb = 0;
                active = 0;
                for (i = 2; i < 4; i++) {
                    for (j = 0x80; j < 0x89; j = j + 0x4) {
                        val32 = PCIDATA16(bus1, 0x16, i, j);
                        if (val32 != 0xffff) {
                            nb += 0x1 & (val32 >> 14);
                            active++;
                        }
                    }
                }
                DEBUG("Channels 1&2 of IMC1 are %s and have a total of %d DIMMs\n",
                      (active > 0) ? "active" : "inactive", nb);
                if (active > 0) {
                    chan_dev[nchan] = 0x17;
                    chan_func[nchan] = 0;
                    chan_dev[nchan + 1] = 0x17;
                    chan_func[nchan + 1] = 1;
                    nchan += 2;
                }
            }
            val32 = PCIDATA32(bus1, 0x18, 0x0, 0x0);
            DEBUG("Got id %x for B1 D0x18 F0 0x0\n", val32);
            if ((val32 & 0xffffff) == 0xD48086) { /* HSW */
                DEBUG("Discovered HSW IMC1 (F24)\n");
                nb = 0;
                active = 0;
                for (i = 4; i < 6; i++) {
                    for (j = 0x80; j < 0x89; j = j + 0x4) {
                        val32 = PCIDATA16(bus1, 0x16, i, j);
                        if (val32 != 0xffff) {
                            nb += 0x1 & (val32 >> 14);
                            active++;
                        }
                    }
                }
                DEBUG("Channels 3&4 of IMC1 are %s and have a total of %d DIMMs\n",
                      (active > 0) ? "active" : "inactive", nb);
                if (active > 0) {
                    chan_dev[nchan] = 0x18;
                    chan_func[nchan] = 0;
                    chan_dev[nchan + 1] = 0x18;
                    chan_func[nchan + 1] = 1;
                    nchan += 2;
                }
            }
        }
    }

    // Bus2 Devices 0x8,0x9 correspond to SKL ...
    val32 = PCIDATA32(bus2, 0x8, 0x0, 0x0);
    DEBUG("Got id %x for B2 D0x8 F0 0x0\n", val32);
    if ((val32 & 0xffffff) == 0x668086) { /* SKL */
        DEBUG("Discovered SKL IMC0\n");
        systype = SKL;
        bus = bus2;
        nchan = 0;
        val32 = PCIDATA32(bus2, 0xa, 0x0, 0x0);
        DEBUG("Got id %x for B2 D0x0A F0 0x0\n", val32);
        if ((val32 & 0xf0ffff) == 0x408086) { /* SKL */
            DEBUG("Discovered SKL IMC0 (F10)\n");
            // we deliberately assume that all 3 channels are 'active', even if not populated...
            chan_dev[nchan] = 0xa;
            chan_func[nchan] = 2;
            chan_dev[nchan + 1] = 0xa;
            chan_func[nchan + 1] = 6;
            chan_dev[nchan + 2] = 0xb;
            chan_func[nchan + 2] = 2;
            nchan += 3;
        }
        // The second IMC should always be there, but let's nevertheless check...
        val32 = PCIDATA32(bus2, 0x9, 0x0, 0x0);
        DEBUG("Got id %x for B2 D0x9 F0 0x0\n", val32);
        if ((val32 & 0xffffff) == 0x668086) { /* SKL */
            DEBUG("Discovered SKL IMC1\n");
            val32 = PCIDATA32(bus2, 0xc, 0x0, 0x0);
            DEBUG("Got id %x for B2 D0x0C F0 0x0\n", val32);
            if ((val32 & 0xf0ffff) == 0x408086) { /* SKL */
                DEBUG("Discovered SKL IMC1 (F12)\n");
                // we deliberately assume that all 3 channels are 'active', even if not populated...
                chan_dev[nchan] = 0xc;
                chan_func[nchan] = 2;
                chan_dev[nchan + 1] = 0xc;
                chan_func[nchan + 1] = 6;
                chan_dev[nchan + 2] = 0xd;
                chan_func[nchan + 2] = 2;
                nchan += 3;
            }
        }
    }

    // Bus2 Devices 0x8,0x9 correspond to KNL ...
    val32 = PCIDATA32(bus2, 0x8, 0x0, 0x0);
    DEBUG("Got id %x for B2 D0x8 F0 0x0\n", val32);
    if ((val32 & 0xfff0ffff) == 0x78408086) { /* KNL */
        DEBUG("Discovered KNL IMC0\n");
        systype = KNL;
        bus = bus2;
        nchan = 0;
        chan_dev[nchan] = 0x8;
        chan_func[nchan] = 2;
        chan_dev[nchan + 1] = 0x8;
        chan_func[nchan + 1] = 3;
        chan_dev[nchan + 2] = 0x8;
        chan_func[nchan + 2] = 4;
        nchan += 3;
    }
    val32 = PCIDATA32(bus2, 0x9, 0x0, 0x0);
    DEBUG("Got id %x for B2 D0x9 F0 0x0\n", val32);
    if ((val32 & 0xfff0ffff) == 0x78408086) { /* KNL */
        DEBUG("Discovered KNL IMC1\n");
        chan_dev[nchan] = 0x9;
        chan_func[nchan] = 2;
        chan_dev[nchan + 1] = 0x9;
        chan_func[nchan + 1] = 3;
        chan_dev[nchan + 2] = 0x9;
        chan_func[nchan + 2] = 4;
        nchan += 3;
    }

    return;
}

