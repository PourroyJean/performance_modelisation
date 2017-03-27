#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define BUF_SIZE 200
#define NB_LINE_TAIL 40000
#define MAX_LINE_OUTPUT 410

typedef struct core_file_and_info{
    uint32_t socket_number;
    uint32_t core_number;
    FILE *data;
    char *last_line;                // 100 char are allocated in open_files()
} core_info;



uint32_t nb_core, nb_socket;

core_info* open_files(char *file_name[], int number_of_file){
    nb_core = number_of_file;
    core_info *cores = (core_info*) malloc(sizeof (core_info) * nb_core);

    for (int i = 0; i<nb_core; i++){   // Opening files
        // Assuming the file name looks like "turbostats.x.xxx"

        uint32_t length = strlen(file_name[i]);
        cores[i].socket_number = file_name[i][length - 5]-'0';
        cores[i].core_number = ((file_name[i][length - 3]-'0') * 100)  + ((file_name[i][length - 2]-'0') * 10) + (file_name[i][length - 1]-'0');

        char cmd[200];
        sprintf(cmd, "tail -n %d %s", NB_LINE_TAIL, file_name[i]);
        //printf("opening pipe with for the core #%d of the socket #%d with the cmd \"%s\" \n", cores[i].core_number, cores[i].socket_number, cmd);
        cores[i].data = (FILE*) popen(cmd, "r");
        if (cores[i].data == NULL)
            perror("Failed while opening a pipe");
        cores[i].last_line = (char*) malloc(sizeof (char) * BUF_SIZE);
        if(fgets(cores[i].last_line, sizeof(char) * BUF_SIZE, cores[i].data) == NULL)
            perror("One file is empty or difunctionning");
    }

    return cores;
}


void fetch_line(core_info *core){
    core->last_line = fgets(core->last_line, sizeof(char) * BUF_SIZE, core->data);
}

float get_timer(core_info *core){

    if(core->last_line == NULL) {
        exit(1);
    } else  {
//        printf("lets read data %s ...\n",core->last_line );fflush(stdout);
        char* token;
        char str[100];
        strcpy(str, core->last_line);
        token = strtok(str, " ");
        token = strtok(NULL, " ");
        return (float) atof(token);
    }
}

int reach_time(core_info *core, float time_to_reach){
    if(core->last_line == NULL) return 0;

    while(core->last_line != NULL && get_timer(core)<time_to_reach){
        fetch_line(core);
    }

    if(core->last_line == NULL) {
        return 0;
    } else {
        return 1;
    }

}

int read_data(core_info *core, float* data, float max_timer){

    float AM_sum = 0,AT_sum = 0,MT_sum = 0;
    float AM_max = 0,AT_max = 0,MT_max = 0;
    uint32_t nb_value = 0;
    char buffer[100];
    char* token; float value;

    while(core->last_line != NULL && get_timer(core) <= max_timer) {
        strcpy(buffer, core->last_line);

        token = strtok(buffer, " ");    // "t="
        token = strtok(NULL, " ");      // timer
        token = strtok(NULL, " ");      // "A/M ="
        token = strtok(NULL, " ");      // AM

        value = atof(token);
        AM_sum += value;
        if (AM_max < value) AM_max = value;

        token = strtok(NULL, " ");      // "A/T ="
        token = strtok(NULL, " ");      // AT

        value = atof(token);
        AT_sum += value;
        if (AT_max < value) AT_max = value;

        token = strtok(NULL, " ");      // "M/T ="
        token = strtok(NULL, " ");      // MT

        value = atof(token);
        MT_sum += value;
        if (MT_max < value) MT_max = value;

        fetch_line(core);
        nb_value++;
    }

    if(core->last_line == NULL){ // End of file
        return 0;
    }
    data[0] = max_timer;        //data[0] = timer
    data[1] = AM_sum/nb_value;  //data[1] = AM_avg
    data[2] = AM_max;           //data[2] = AM_max
    data[3] = AT_sum/nb_value;  //data[3] = AT_avg
    data[4] = AT_max;           //data[4] = AT_max
    data[5] = MT_sum/nb_value;  //data[5] = MT_avg
    data[6] = MT_max;           //data[6] = MT_max

    for(int i = 0; i<7; i++) {
        //printf("data[%d] = %f,  ", i, data[i]);
    }
    //printf("\n");
    return 1;
}


void write_data(float**** data, uint32_t data_read){

    if(data_read != 0) {
        printf("[");
        for (uint32_t k = 0; k<data_read; k++){
            if(k!=0)printf(",");
            printf("{\"timer\":");
            printf("%f,\"data_freq\":[", data[0][0][k][0]);           // Time
            for (uint32_t i = 0; i <nb_socket; i++){
                if(i!=0)printf(",");
                printf("[");
                for (uint32_t j = 0; j <nb_core/nb_socket; j++){
                    if(j!=0)printf(",");
                    printf("[");
                    for (uint32_t m = 1; m<7; m++) {    // We skip the first data because it is "time"
                        if(m!=1)printf(",");
                        printf("%f", data[i][j][k][m]);
                    }
                    printf("]");
                }
                printf("]");
            }
            printf("]");
            printf("}");
        }
        printf("]");
    }
}

void close_files(core_info *cores, uint32_t nb_core){
    for (uint32_t i = 0; i<nb_core; i++){   // Closing files
        pclose(cores[i].data);
        //printf("closed pipe number %d \n",i);
        free(cores[i].last_line);
    }

    free(cores);
    cores = NULL;
}


int main(int argc, char *argv[]) {


    if (argc < 3) {
        //printf ("Usage \n ./prog timer files*");
        exit(1);
    }
    float start_time = atof(argv[1]);
    float latest_time = 0;

    core_info* cores = open_files(argv+2, argc-2);


    for (int i = 0; i<nb_core; i++) {
//        printf("core %d socket %d ...\n", cores[i].core_number,cores[i].socket_number);fflush(stdout);
        if (get_timer(cores+i) > latest_time){
            latest_time = get_timer(cores+i);
        }
    }
    //printf("latest time found : %f\n", latest_time);
    //printf("requested time    : %f\n", start_time);
    if (start_time < latest_time) start_time = latest_time;
    start_time = (int) (start_time * 2);
    start_time = (start_time)/2;
    //printf("lets reach this number : %f\n", start_time);

    for (int i = 0; i<nb_core; i++){
        reach_time(cores+i, start_time);
        //printf("we have timer %f \n",  get_timer(cores+i));
    }


    nb_socket = cores[nb_core-1].socket_number + 1;    // Asmsuming the last core is on the last socket (alphabetical order)
    //  Preparing the data array

    //printf("We will do a malloc for %d socket, %d core, %d line, %d data\n", nb_socket, nb_core, MAX_LINE_OUTPUT, 7);


    float ****data;          //  Socket/core/time/values

    data = (float****) malloc(sizeof(float***) * nb_socket);
    for (int i = 0; i<nb_socket; i++){
        data[i] = (float***) malloc(sizeof(float**) * nb_core/nb_socket);
        for (int j = 0; j<nb_core/nb_socket; j++){
            data[i][j] = (float**) malloc(sizeof(float*) * MAX_LINE_OUTPUT);
            for(int k = 0; k<MAX_LINE_OUTPUT; k++){
                data[i][j][k] = (float*) malloc(sizeof(float) * 7);
            }
        }
    }


    uint32_t end_of_file = 0, data_read = 0;
    float max_time = start_time + 0.5;


    while (end_of_file == 0 && data_read < MAX_LINE_OUTPUT-1) {
        for (int i = 0; i<nb_core; i++) {
            end_of_file += 1 - read_data(cores+i, data[cores[i].socket_number][i%(nb_core/nb_socket)][data_read], max_time);
        }
        //printf("data reached for timer %f \n",max_time);fflush(stdout);
        max_time += 0.5;
        data_read++;
    }

    if(end_of_file != 0) data_read--;



    write_data(data, data_read);

    for (int i = 0; i<nb_socket; i++){
        for (int j = 0; j<nb_core/nb_socket; j++){
            for(int k = 0; k<21; k++){
                free(data[i][j][k]);
            }
            free(data[i][j]);
        }
        free(data[i]);
    }
    free(data);

    close_files(cores, argc-2);

    return 0;
}
