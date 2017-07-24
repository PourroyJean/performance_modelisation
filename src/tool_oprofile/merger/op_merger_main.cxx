#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <iomanip>

#include "op_objdump_line.h"
#include "op_oprofile_line.h"
#include "op_misc.h"

using namespace std;


int main(int argc, char *argv[]) {

    string line;
    if (argc != 3) {
        cerr << "command Object_file oprofile_file" << endl;
        return 1;
    }

    //Read both files
    read_object_file(argv[1]);
    read_oprofile_file(argv[2]);

// we have all data now ; we will reconstruct the profile in same order
    // we have to present the hot spots in same order as op2
    cout
            << "================================================================================================================"
            << endl;
    cout
            << "================================================================================================================"
            << endl;
    for (int line = 0; line < oprofile_file.size(); line++) {
        if (oprofile_file[line].type == 0) cout << "_0_ " << oprofile_file[line].str << endl;
        if (oprofile_file[line].type == 1) {
            vector<string> v{split(oprofile_file[line].str, ' ')};
            double cycles = stof(v[2]);
            if (cycles > 0.01)cout << "_1_ " << oprofile_file[line].str << endl;
        }
    }
    cout
            << "================================================================================================================"
            << endl;
    cout
            << "================================================================================================================"
            << endl;
    for (int line = 0; line < oprofile_file.size(); line++) {
        if (oprofile_file[line].type == 1) {
            vector<string> v{split(oprofile_file[line].str, ' ')};
            double cycles = stof(v[2]);
            ui64 add = stoullhexa(v[0]);
            // we dump the disassembly of all functions consuming more than 0.1% of total
            if (cycles > 0.1) {
                // we search the line in objdump
                int in_objdump = objdump_address[add];
                cout << "_2_ " << add << " " << in_objdump << " " << v[0] << " " << oprofile_file[line].str << endl;
                cout
                        << "----------------------------------------------------------------------------------------------------------------"
                        << endl;
                //cout << "_5_        87670        28368        28366 |        23005           41      400854:     c4 c2 a5 a8 cb          vfmadd213pd %ymm11,%ymm11,%ymm1
                cout
                        << "           SUM*4        SUM*3        SUM*2          CYCLES       INSTS      ADDRESS     code HEXA               disassembly"
                        << endl;
                cout
                        << "----------------------------------------------------------------------------------------------------------------"
                        << endl;
                if (in_objdump > 0)
                    for (int li = in_objdump - 1;; li++) {
                        int type = objdump_file[li].type;
                        ui64 ctr1 = objdump_file[li].ctr1;
                        ui64 ctr2 = objdump_file[li].ctr2;
                        string str = objdump_file[li].str;
                        int len = objdump_file[li].len_str;
                        ui64 myadd = objdump_file[li].address;
                        ui64 sum2 = ctr1;
                        sum2 += objdump_file[li + 1].ctr1;
                        ui64 sum3 = sum2;
                        sum3 += objdump_file[li + 2].ctr1;
                        ui64 sum4 = sum3;
                        sum4 += objdump_file[li + 3].ctr1;
                        if (len == 0)break;
                        if (ctr1 > 0)
                            if (ctr2 > 0)
                                cout << "_5_ " << std::setw(12) << sum4 << " "
                                     << std::setw(12) << sum3 << " "
                                     << std::setw(12) << sum2 << " | "
                                     << std::setw(12) << ctr1 << " "
                                     << std::setw(12) << ctr2 << "    " << str << endl;
                        if (len >= 32) { // we try to detect a loop
                            string instr = str.substr(32, 999);
                            vector<string> v{split(instr, ' ')};
                            if (instr[0] == 'j')
                                if (v[0].compare("jmpq") != 0) {
                                    ui64 add = stoullhexa(v[1]);
                                    ui64 diff = myadd - add;
                                    if (diff < 100000ull) { // backward loop
                                        // we will sum the cycles and instructions of the loop
                                        ui64 sumcy = 0;
                                        ui64 sumin = 0;
                                        int count = 0;
                                        for (int li2 = li;; li2--) {
                                            ui64 ctr1 = objdump_file[li2].ctr1;
                                            ui64 ctr2 = objdump_file[li2].ctr2;
                                            sumcy += ctr1;
                                            sumin += ctr2;
                                            count++;
                                            ui64 addt = objdump_file[li2].address;
                                            if (addt == add)break;
                                        }
                                        double IPC = double(sumin) / double(sumcy);
                                        double cyL = double(count) / IPC;
                                        cout
                                                << "----------------------------------------------------------------------------------------------------------------"
                                                << endl;
                                        cout << "_7_ LOOP from " << std::hex << myadd << " to " << std::hex << add
                                             << " size= " << std::dec << diff << " sum(cycles)= "
                                             << sumcy << " sum(inst)= " << sumin << " #inst= " << count << " IPC= "
                                             << IPC << " cycles/LOOP= " << cyL << endl;
                                        cout
                                                << "----------------------------------------------------------------------------------------------------------------"
                                                << endl;
                                    }
                                }
                        }
                    }
            }
        }
    }
    cout
            << "================================================================================================================"
            << endl;
    cout
            << "================================================================================================================"
            << endl;

}


