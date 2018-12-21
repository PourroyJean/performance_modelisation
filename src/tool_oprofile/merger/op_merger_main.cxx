#include <string>
#include <iostream>
#include <vector>
#include <iomanip>
#include <sstream>

#include "op_misc.h"
#include "InputFile.h"
#include "Line_Objdump.h"
#include "Line_Oprofile.h"


using namespace std;


int main(int argc, char *argv[]) {

    string line;
    if (argc != 3) {
        cerr << "command Object_file oprofile_file" << endl;
        return 1;
    }

    InputFile<Line_Objdump> *FILE_OBJ = new InputFile<Line_Objdump>(argv[1]);
    Line_Objdump::setFILE_OBJ(FILE_OBJ);
    FILE_OBJ->analysis();

    InputFile<Line_Oprofile> *FILE_OPR = new InputFile<Line_Oprofile>(argv[2]);
    Line_Oprofile::setFILE_OPR(FILE_OPR);
    FILE_OPR->analysis();

    //Debugging: print both files content
//    cout << *FILE_OBJ << endl;
//    cout << *FILE_OPR << endl;


    // we have all data now ; we will reconstruct the profile in same order
    Line_Oprofile::print_resume();

    //OPROFILE is already sorted by % of the execution time
    for (Line_Oprofile *current_line : FILE_OPR->get_lines()) {

        uint64_t memory_address = current_line->get_memory_address();
        int in_objdump = Line_Objdump::objdump_address[memory_address];

        // we dump the disassembly of all functions consuming more than 0.1% of total
        if (current_line->get_line_type() != Line::LINE_TYPE::FUNCTION
            || current_line->get_event_cpu_clk_percentage() <= 0.1
            || in_objdump <= 0) {
            continue;
        }

        cout << endl
             << "====================================================================================================================================================\n"
             << "_FUNCTION_ANALYSIS_ from the app name (" << current_line->get_application_name()
             << ") hot spot from the symbole name (" << current_line->get_symbole_name() << ")" << " which takes "
             << current_line->get_event_cpu_clk_percentage() << "% of the profiling" << endl
             << "====================================================================================================================================================\n"
             << "           SUM*4        SUM*3        SUM*2          CYCLES       INSTS     ADDRESS    ASSEMBLY                         \n"
             << "----------------------------------------------------------------------------------------------------------------------------------------------------"
             << endl;

        //Easier to manipulate
        const vector<Line_Objdump *> *objdump_file = &Line_Objdump::getFILE_OBJ()->get_lines_vector();
        //Boolean only used for display purpose
        bool loop_detected = false;

        for (int li = in_objdump - 1;; li++) {
            if (objdump_file->at(li)->get_original_line().size() == 0) {
                break;
            }

            //Make some values local
            ui64 event_cpu_clk = objdump_file->at(li)->get_event_cpu_clk();
            ui64 event_inst_retired = objdump_file->at(li)->get_event_inst_retired();
            string str = objdump_file->at(li)->get_original_line();
            ui64 myadd = objdump_file->at(li)->get_address();
            string instr = objdump_file->at(li)->get_assembly_instruction();


            //Sum 2 3 4
            //We try to sum instructions by pack of 2, 3, 4 to detect super-scalar execution pattern
            ui64 sum2 = event_cpu_clk + objdump_file->at(li + 1)->get_event_cpu_clk();
            ui64 sum3 = sum2 + objdump_file->at(li + 2)->get_event_cpu_clk();
            ui64 sum4 = sum3 + objdump_file->at(li + 3)->get_event_cpu_clk();

            //So we have all the necessary information for the display
            if (event_cpu_clk > 0 && event_inst_retired > 0) {
                cout << "_5_ "
                     << std::setw(12) << sum4 << " "
                     << std::setw(12) << sum3 << " "
                     << std::setw(12) << sum2 << " | "
                     << std::setw(12) << event_cpu_clk << " "
                     << std::setw(12) << event_inst_retired << "    "
                     << std::setw(8) << myadd << "    "
                     << std::setw(15) << instr
                     << endl;
            }


            //We try to detect a loop:
            //  (1) Split the assembly to get the instrution (first word)
            //  (2) Detect if its a jump (begin by 'j'
            //  (3) But not a JMPQ one (return jump for function)
            std::istringstream iss(instr);
            std::vector<std::string> v_split(std::istream_iterator<std::string> { iss },
            std::istream_iterator<std::string>());
            if (instr.size() >= 0 && instr[0] == 'j' && v_split[0] != "jmpq") {

                ui64 jump_add = stoullhexa(v_split[1]);

                ui64 diff = myadd - jump_add;
                if (diff < 100000ull) { // backward loop
                    if (!loop_detected) {
                        cout
                                << "----------------------------------------------------------------------------------------------------------------\n";
                    }
                    loop_detected = true;


                    // For en entire loop (from the source to the target jump address
                    // we will sum the cycles and instructions of the loop
                    ui64 sumcy = 0;
                    ui64 sumin = 0;
                    int count = 0;
                    for (int li2 = li;; li2--) {
                        ui64 event_cpu_clk = objdump_file->at(li2)->get_event_cpu_clk();
                        ui64 event_inst_retired = objdump_file->at(li2)->get_event_inst_retired();
                        sumcy += event_cpu_clk;
                        sumin += event_inst_retired;
                        count++;
                        ui64 addt = objdump_file->at(li2)->get_address();
                        if (addt == jump_add)break;
                    }
                    double IPC = double(sumin) / double(sumcy);
                    double cyL = double(count) / IPC;

                    //TODO use setw for this part
                    cout << "_7_ LOOP from " << std::hex << myadd << " to " << std::hex << jump_add
                         << " size= " << std::dec << diff << " sum(cycles)= "
                         << sumcy << " sum(inst)= " << sumin << " #inst= " << count << " IPC= "
                         << IPC << " cycles/LOOP= " << cyL << endl;
                }


            }

        }
        if (loop_detected) {
            cout
                    << "----------------------------------------------------------------------------------------------------------------\n";
            loop_detected = false;
            cout << endl;
        }


    }

    cout << "End ..." << endl;
}

