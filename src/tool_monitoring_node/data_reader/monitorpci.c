#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define BUF_SIZE 200
#define NB_LINE_TAIL 6050
#define MAX_LINE_OUTPUT 61

typedef struct socket_file_and_info{
    uint32_t socket_number;
    FILE *data;
    char *last_line;                // 100 char are allocated in open_files()
} socket_info;



uint32_t nb_socket;
int nb_bus;                     // pci bus per socket

socket_info* open_files(char *file_name[], int number_of_file){
    nb_socket = number_of_file;
    socket_info *sockets = (socket_info*) malloc(sizeof (socket_info) * nb_socket);

    for (int i = 0; i<nb_socket; i++){   // Opening files
        // Assuming the file name looks like "pciestats.x"
        // In fact we just assume that the name file is "xxxxxx.[0-9]"


        uint32_t length = strlen(file_name[i]);
        sockets[i].socket_number = file_name[i][length - 1]-'0';

        char cmd[200];
        sprintf(cmd, "tail -n %d %s", NB_LINE_TAIL, file_name[i]);
        //printf("opening pipe with for the socket #%d with the cmd \"%s\" \n", sockets[i].core_number, sockets[i].socket_number, cmd);
        sockets[i].data = (FILE*) popen(cmd, "r");
        if (sockets[i].data == NULL)
            perror("Failed while opening a pipe");
        sockets[i].last_line = (char*) malloc(sizeof (char) * BUF_SIZE);
        if(fgets(sockets[i].last_line, sizeof(char) * BUF_SIZE, sockets[i].data) == NULL)
            perror("One file is empty or difunctionning");
    }

    return sockets;
}


void fetch_line(socket_info *socket){
    socket->last_line = fgets(socket->last_line, sizeof(char) * BUF_SIZE, socket->data);
}

float get_timer(socket_info *socket){

    if(socket->last_line == NULL) {
        exit(1);
    } else  {
        char* token;
        char str[BUF_SIZE];
        strcpy(str, socket->last_line);
        token = strtok(str, " ");
        token = strtok(NULL, " ");
        return (float) atof(token);
    }
}

int reach_time(socket_info *socket, float time_to_reach){
    if(socket->last_line == NULL) return 0;

    while(socket->last_line != NULL && get_timer(socket)<time_to_reach){
        fetch_line(socket);
    }

    if(socket->last_line == NULL) {
        return 0;
    } else {
        return 1;
    }

}

int read_data(socket_info *socket, float** data, float max_timer){


//   inDMA= 0.0 inPCIe0= 0.0 inPCIe1= 0.0 inPCIe2= 0.0 outDMA= 0.0 outPCIe0= 0.0 outPCIe1= 0.0 outPCIe2= 0.0

    float *read_sum = (float*) calloc(sizeof(float), nb_bus);
    float *read_max = (float*) calloc(sizeof(float), nb_bus);
    float *write_sum = (float*) calloc(sizeof(float), nb_bus);
    float *write_max = (float*) calloc(sizeof(float), nb_bus);

    uint32_t nb_value = 0;
    char buffer[BUF_SIZE];
    char* token; float value;

    while(socket->last_line != NULL && get_timer(socket) <= max_timer) {
        strcpy(buffer, socket->last_line);

        token = strtok(buffer, " ");    // "t="
        token = strtok(NULL, " ");      // timer

        // READ
        for(int bus = 0; bus < nb_bus; bus++) {
            token = strtok(NULL, " ");      // "inDMA =" OR "inPCIe[0-2] ="
            token = strtok(NULL, " ");      // read_i

            value = atof(token);

            read_sum[bus] += value;
            if (read_max[bus] < value) read_max[bus] = value;
        }

        // WRITE
        for(int bus = 0; bus < nb_bus; bus++) {
            token = strtok(NULL, " ");      // "outDMA =" OR "outPCIe[0-2] ="
            token = strtok(NULL, " ");      // write_i

            value = atof(token);

            write_sum[bus] += value;
            if (write_max[bus] < value) write_max[bus] = value;

        }

        fetch_line(socket);
        nb_value++;
    }

    if(socket->last_line == NULL){ // End of file
        return 0;
    }
    for(int bus = 0; bus < nb_bus; bus++) {
      data[bus][0] = max_timer;                //data[0] = timer
      data[bus][1] = read_sum[bus]/nb_value;   //data[1] = read_avg
      data[bus][2] = read_max[bus];            //data[2] = read_max
      data[bus][3] = write_sum[bus]/nb_value;  //data[3] = write_avg
      data[bus][4] = write_max[bus];           //data[4] = write_max
    }
    free(read_sum);
    free(read_max);
    free(write_sum);
    free(write_max);

    return 1;
}


void write_data(float**** data, uint32_t data_read){

    if(data_read != 0) {
        printf("[");
        for (uint32_t k = 0; k<data_read; k++){
            if(k!=0)printf(",");
            printf("{\"timer\":%f", data[0][k][0][0]);// Time
            printf(",\"data_pci\":[");
            for (uint32_t i = 0; i <nb_socket; i++){
                if(i!=0)printf(",");
                printf("[");
                for (uint32_t j = 0; j <nb_bus; j++){
                    if(j!=0)printf(",");
                    printf("[");
                    for (uint32_t m = 1; m<5; m++) {    // We skip the first data because it is "time"
                        if(m!=1)printf(",");
                        printf("%f", data[i][k][j][m]);
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

void close_files(socket_info *sockets, uint32_t nb_socket){
    for (uint32_t i = 0; i<nb_socket; i++){   // Closing files
        pclose(sockets[i].data);
        //printf("closed pipe number %d \n",i);
        free(sockets[i].last_line);
    }

    free(sockets);
    sockets = NULL;
}


int main(int argc, char *argv[]) {


    if (argc < 3) {
        exit(1);
    }
    float start_time = atof(argv[1]);
    float latest_time = 0;

    socket_info* sockets = open_files(argv+2, argc-2);


    for (int i = 0; i<nb_socket; i++) {
        if (get_timer(sockets+i) > latest_time){
            latest_time = get_timer(sockets+i);
        }
    }
    if (start_time < latest_time) start_time = latest_time;
    start_time = (int) (start_time * 2);
    start_time = (start_time)/2;

    for (int i = 0; i<nb_socket; i++){
        reach_time(sockets+i, start_time);
    }

    char first_line[500];
    char* token;
    strcpy(first_line, sockets->last_line);

    token = strtok(first_line, " ");
    token = strtok(NULL, " ");
    for(nb_bus = 0; token!=NULL; nb_bus++){
            token = strtok(NULL, " ");      // "inPCIe=" or "outPCIe="
            token = strtok(NULL, " ");      // read_i or write_i
    }
    nb_bus = nb_bus/2;               // because there is one channel for one "in" and one "out"

    //  Preparing the data array

    float ****data;          //

    data = (float****) malloc(sizeof(float***) * nb_socket);
    for (int i = 0; i<nb_socket; i++){
      data[i] = (float***) malloc(sizeof(float**) * MAX_LINE_OUTPUT);
      for (int j = 0; j<MAX_LINE_OUTPUT; j++){
        data[i][j] = (float**) malloc(sizeof(float*) * nb_bus);
        for(int k = 0; k<nb_bus; k++){
            data[i][j][k] = (float*) malloc(sizeof(float) * 5);
        }
      }
    }


    uint32_t end_of_file = 0, data_read = 0;
    float max_time = start_time + 0.5;        // data => [max_time-0.5; max_time]


    while (end_of_file == 0 && data_read < MAX_LINE_OUTPUT-1) {
        for (int i = 0; i<nb_socket; i++) {
            end_of_file += 1 - read_data(sockets+i, data[sockets[i].socket_number][data_read], max_time);
        }
        max_time += 0.5;
        data_read++;
    }

    if(end_of_file != 0) data_read--;



    write_data(data, data_read);

    for (int i = 0; i<nb_socket; i++){
        for(int k = 0; k<MAX_LINE_OUTPUT; k++){
            free(data[i][k]);
        }
        free(data[i]);
    }
    free(data);


    close_files(sockets, argc-2);

    return 0;
}
