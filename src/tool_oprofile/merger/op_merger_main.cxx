#include <string>
#include <iostream>
#include <vector>
#include <iterator>
#include <unordered_map>
#include <iomanip>

#include "op_objdump_line.h"
#include "op_oprofile_line.h"
#include "op_misc.h"
#include "InputFile.h"
#include "Line_Objdump.h"
#include "Line_Oprofile.h"


using namespace std;


void update_objdump_counters() {
    for (std::vector<oprofile_line>::iterator it = oprofile_file.begin(); it != oprofile_file.end(); ++it) {
        if (it->type == oprofile_line::Type::INST) {
            /* std::cout << *it; ... */
            int in_objdump = objdump_address[it->memory_address] - 1;

            if (in_objdump > 0) {
                objdump_file[in_objdump].event_cpu_clk = it->event_cpu_clk;
                objdump_file[in_objdump].event_inst_retired = it->event_inst_retired;
                //cout << "_3_ ";for(auto n:v) cout << n << " "; cout << event_cpu_clk << " " << event_inst_retired << " " << in_objdump << " " << objdump_file[in_objdump].str << endl;
            }
        }
    }
}


void oprofile_print_resume() {
    cout << "======================== OPROFILE FILE WITH > 0.1 CYCLES ======================================" << endl
         << "===============================================================================================" << endl;

    oprofile_line *current_line = NULL;
    for (int line = 0; line < oprofile_file.size(); line++) {
        current_line = &oprofile_file[line];

        if (current_line->type == oprofile_line::Type::NO) {
            cout << "_0_ " << current_line->line_original_string << endl;
        }
        if (current_line->type == oprofile_line::Type::FUNC) {
            if (current_line->event_cpu_clk > 0.01)
                cout << "_1_ " << current_line->line_original_string << endl;
        }
    }
    cout << "==============================================================================================="
         << "==============================================================================================="
         << endl << endl;
}

int main(int argc, char *argv[]) {

    string line;
    if (argc != 3) {
        cerr << "command Object_file oprofile_file" << endl;
        return 1;
    }


    //Read both files
    read_object_file(argv[1]);
    read_oprofile_file(argv[2]);

    InputFile<Line_Objdump> FILE_OBJ(argv[1]);
    InputFile<Line_Oprofile> FILE_OPR(argv[2]);


    cout << FILE_OBJ << endl;
//    cout << FILE_OPR << endl;

    exit(-1);

    cout << " BEFORE " << endl;
    for (std::vector<oprofile_line>::iterator it = oprofile_file.begin(); it != oprofile_file.end(); ++it) {
        if (it->type == oprofile_line::Type::INST) {
            cout << "OP2 INSTRUCTION " << it->line_original_string << endl;
        }
    }

    for (std::vector<oprofile_line>::iterator it = oprofile_file.begin(); it != oprofile_file.end(); ++it) {
        if (it->type == oprofile_line::Type::FUNC) {
            cout << " OP2 FUNCTION     " << it->line_original_string << endl;
        }
    }

    for (std::vector<objdump_line>::iterator it = objdump_file.begin(); it != objdump_file.end(); ++it) {
        cout << " OBJ " << it->line_original_string << endl;
    }


    update_objdump_counters();






    // we have all data now ; we will reconstruct the profile in same order
    oprofile_print_resume();

    cout << endl << endl;

//    cout << oprofile_file[22].line_original_string << endl;
//    cout << oprofile_file[22].event_cpu_clk  << endl;
//    cout << oprofile_file[22].event_inst_retired  << endl;
//    cout << oprofile_file[22].memory_address  << endl;


    //OPROFILE is already sorted by % of the execution time
    for (int line = 0; line < oprofile_file.size(); line++) {
        oprofile_line *current_line = &oprofile_file[line];

        vector<string> v{split(current_line->line_original_string, ' ')};
        ui64 memory_address = current_line->memory_address;
        int in_objdump = objdump_address[memory_address];



        // we dump the disassembly of all functions consuming more than 0.1% of total
        if (current_line->type != oprofile_line::Type::FUNC
            || current_line->event_cpu_clk_percentage <= 0.1
            || in_objdump <= 0) {
            continue;
        }





//        cout << "_2_ " << memory_address << " " << in_objdump << " " << v[0] << " " << current_line->line_original_string << endl;
        cout << endl
             << "====================================================================================================================================================\n"
             << "_FUNCTION_ANALYSIS_ from the app name (" << current_line->m_binary_name
             << ") hot spot from the symbole name ( " << current_line->m_function_name << " )" << endl
             << "====================================================================================================================================================\n"
             << "           SUM*4        SUM*3        SUM*2          CYCLES       INSTS      ADDRESS     code HEXA               disassembly                         \n"
             << "----------------------------------------------------------------------------------------------------------------------------------------------------"
             << endl;

        for (int li = in_objdump - 1;; li++) {
            if (objdump_file[li].len_str == 0) {
                break;
            }

//            cout << "NANOU " <<  objdump_file[li].str << endl;

            //Make some values local
            int type = objdump_file[li].type;
            ui64 event_cpu_clk = objdump_file[li].event_cpu_clk;
            ui64 event_inst_retired = objdump_file[li].event_inst_retired;
            string str = objdump_file[li].line_original_string;
            ui64 myadd = objdump_file[li].address;

            //Sum 2 3 4
            ui64 sum2 = event_cpu_clk;
            sum2 += objdump_file[li + 1].event_cpu_clk;
            ui64 sum3 = sum2;
            sum3 += objdump_file[li + 2].event_cpu_clk;
            ui64 sum4 = sum3;
            sum4 += objdump_file[li + 3].event_cpu_clk;

            if (event_cpu_clk > 0 && event_inst_retired > 0) {
                cout << "_5_ "
                     << std::setw(12) << sum4 << " "
                     << std::setw(12) << sum3 << " "
                     << std::setw(12) << sum2 << " | "
                     << std::setw(12) << event_cpu_clk << " "
                     << std::setw(12) << event_inst_retired << "    "
                     << str << endl;
            }
//            cout << " AFFICHE\n";

            if (objdump_file[li].len_str >= 32) { // we try to detect a loop
                string instr = str.substr(32, 999);
                vector<string> v{split(instr, ' ')};


//                cout  << "JANNOU " << instr[0]<< instr[1]<< instr[2]<< instr[3] << ' ' << v[0] << endl;


                //Detect if its a jump but not a JMPQ one.
                if (instr[0] == 'j' && v[0].compare("jmpq") != 0) {

                    ui64 add = stoullhexa(v[1]);


                    ui64 diff = myadd - add;
                    if (diff < 100000ull) { // backward loop
                        // we will sum the cycles and instructions of the loop
                        ui64 sumcy = 0;
                        ui64 sumin = 0;
                        int count = 0;
                        for (int li2 = li;; li2--) {
                            ui64 event_cpu_clk = objdump_file[li2].event_cpu_clk;
                            ui64 event_inst_retired = objdump_file[li2].event_inst_retired;
                            sumcy += event_cpu_clk;
                            sumin += event_inst_retired;
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

    cout
            << "================================================================================================================"
            << endl
            << "================================================================================================================"
            << endl;


}














/*
  for (int line = 0; line < oprofile_file.size(); line++) {
        if (oprofile_file[line].type == oprofile_line::Type::FUNC) {
            vector<string> v{split(oprofile_file[line].line_original_string, ' ')};
            double cycles = stof(v[2]);
            ui64 add = stoullhexa(v[0]);
            // we dump the disassembly of all functions consuming more than 0.1% of total
            if (cycles > 0.1) {
                // we search the line in objdump
                int in_objdump = objdump_address[add];
                cout << "_2_ " << add << " " << in_objdump << " " << v[0] << " " << oprofile_file[line].line_original_string << endl;
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
                        ui64 ctr1 = objdump_file[li].event_cpu_clk;
                        ui64 ctr2 = objdump_file[li].event_inst_retired;
                        string str = objdump_file[li].str;
                        int len = objdump_file[li].len_str;
                        ui64 myadd = objdump_file[li].address;
                        ui64 sum2 = ctr1;
                        sum2 += objdump_file[li + 1].event_cpu_clk;
                        ui64 sum3 = sum2;
                        sum3 += objdump_file[li + 2].event_cpu_clk;
                        ui64 sum4 = sum3;
                        sum4 += objdump_file[li + 3].event_cpu_clk;
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
                                            ui64 ctr1 = objdump_file[li2].event_cpu_clk;
                                            ui64 ctr2 = objdump_file[li2].event_inst_retired;
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

 */

