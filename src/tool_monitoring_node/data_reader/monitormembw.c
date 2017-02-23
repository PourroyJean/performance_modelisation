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
int nb_channel;                     //channel per socket

socket_info* open_files(char *file_name[], int number_of_file){
    nb_socket = number_of_file;
    socket_info *sockets = (socket_info*) malloc(sizeof (socket_info) * nb_socket);

    for (int i = 0; i<nb_socket; i++){   // Opening files
        // Assuming the file name looks like "membwstats.x"

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

int read_data(socket_info *socket, float* data, float max_timer){


//   r0= 0.6 r1= 0.6 r2= 0.8 r3= 0.7 r4= 0.0 r5= 0.0 w0= 0.4 w1= 0.5 w2= 0.5 w3= 0.6 w4= 0.0 w5= 0.0

    float read_sum = 0.0, write_sum = 0.0;
    float read_max = 0.0, write_max = 0.0;
    float sum_max = 0.0;

    float *sum_per_channel = (float*) malloc(sizeof(float) * nb_channel*2);
    for(int i = 0; i<nb_channel*2; i++) {
        sum_per_channel[i] = 0;
    }

    uint32_t nb_value = 0;
    char buffer[BUF_SIZE];
    char* token; float value;

    while(socket->last_line != NULL && get_timer(socket) <= max_timer) {
        float max_temp = 0;               // to sum before comparing max
        float sum_max_temp = 0;           // to sum before comparing sum_max
        strcpy(buffer, socket->last_line);

        token = strtok(buffer, " ");    // "t="
        token = strtok(NULL, " ");      // timer

        // READ
        for(int i = 0; i < nb_channel; i++) {
            token = strtok(NULL, " ");      // "ri ="
            token = strtok(NULL, " ");      // read_i

            value = atof(token);

            sum_per_channel[i] += value;
            read_sum += value;
            max_temp += value;

        }
        sum_max_temp += max_temp;
        if (read_max < max_temp) read_max = max_temp;

        // WRITE
        max_temp = 0.0;
        for(int i = 0; i < nb_channel; i++) {
            token = strtok(NULL, " ");      // "wi ="
            token = strtok(NULL, " ");      // write_i

            value = atof(token);

            sum_per_channel[i + nb_channel] += value;
            write_sum += value;
            max_temp += value;

        }
        sum_max_temp += max_temp;
        if (write_max < max_temp) write_max = max_temp;

        if (sum_max < sum_max_temp) sum_max = sum_max_temp;


        fetch_line(socket);
        nb_value++;
    }

    if(socket->last_line == NULL){ // End of file
        return 0;
    }
    data[0] = max_timer;           //data[0] = timer
    data[1] = read_sum/nb_value;   //data[1] = read_avg
    data[2] = read_max;            //data[2] = read_max
    data[3] = write_sum/nb_value;  //data[3] = write_avg
    data[4] = write_max;           //data[4] = write_max
    data[5] = sum_max;             //data[5] = sum_max

    free(sum_per_channel);

    return 1;
}


void write_data(float*** data, uint32_t data_read){

    if(data_read != 0) {
        printf("[");
        for (uint32_t k = 0; k<data_read; k++){
            if(k!=0)printf(",");
            printf("{\"timer\":%f", data[0][k][0]);// Time
            printf(",\"data_membw\":[");
            for (uint32_t i = 0; i <nb_socket; i++){
                if(i!=0)printf(",");
                printf("[");
                for (uint32_t m = 1; m<6; m++) {    // We skip the first data because it is "time"
                    if(m!=1)printf(",");
                    printf("%f", data[i][k][m]);
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
    for(nb_channel = 0; token!=NULL; nb_channel++){
            token = strtok(NULL, " ");      // "ri=" or "wi="
            token = strtok(NULL, " ");      // read_i or write_i
    }
    nb_channel = nb_channel/2;               // because there is one channel for one "ri" and one "wi"

    //  Preparing the data array

    float ***data;          //

    data = (float***) malloc(sizeof(float**) * nb_socket);
    for (int i = 0; i<nb_socket; i++){
        data[i] = (float**) malloc(sizeof(float*) * MAX_LINE_OUTPUT);
        for(int k = 0; k<MAX_LINE_OUTPUT; k++){
            data[i][k] = (float*) malloc(sizeof(float) * 6);
        }
    }


    uint32_t end_of_file = 0, data_read = 0;
    float max_time = start_time + 0.5;


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
