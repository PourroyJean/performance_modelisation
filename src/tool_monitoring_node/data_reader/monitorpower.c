#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define BUF_SIZE 200
#define NB_LINE_TAIL 205
#define MAX_LINE_OUTPUT 42

typedef struct socket_file_and_info{
    uint32_t socket_number;
    FILE *data;
    char *last_line;                // 100 char are allocated in open_files() 
} socket_info;



uint32_t nb_socket;

socket_info* open_files(char *file_name[], int number_of_file){
    nb_socket = number_of_file;
    socket_info *sockets = (socket_info*) malloc(sizeof (socket_info) * nb_socket);
    
    for (int i = 0; i<nb_socket; i++){   // Opening files
        // Assuming the file name looks like "cpupowerstats.x"
        
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
//        printf("lets read data %s ...\n",socket->last_line );fflush(stdout);
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

    float CPU_P_sum = 0,CPU_T_sum = 0; 
    float CPU_P_max = 0,CPU_T_max = 0; 
    uint32_t nb_value = 0;
    char buffer[BUF_SIZE];
    char* token; float value;
    
    while(socket->last_line != NULL && get_timer(socket) <= max_timer) {
        strcpy(buffer, socket->last_line);
        
        token = strtok(buffer, " ");    // "t="
        token = strtok(NULL, " ");      // timer
        token = strtok(NULL, " ");      // "CPU_P ="
        token = strtok(NULL, " ");      // CPU_P
    
        value = atof(token);
        CPU_P_sum += value;
        if (CPU_P_max < value) CPU_P_max = value;
        
        token = strtok(NULL, " ");      // "CPU_T ="
        token = strtok(NULL, " ");      // CPU_T
        
        value = atof(token);
        CPU_T_sum += value;
        if (CPU_T_max < value) CPU_T_max = value;

        fetch_line(socket);
        nb_value++;
    }
    
    if(socket->last_line == NULL){ // End of file
        return 0;
    }
    data[0] = max_timer;        //data[0] = timer
    data[1] = CPU_P_sum/nb_value;  //data[1] = CPU_P_avg
    data[2] = CPU_P_max;           //data[2] = CPU_P_max
    data[3] = CPU_T_sum/nb_value;  //data[3] = CPU_T_avg
    data[4] = CPU_T_max;           //data[4] = CPU_T_max
    
    for(int i = 0; i<5; i++) {
        //printf("data[%d] = %f,  ", i, data[i]);
    }
    //printf("\n");
    return 1;
}


void write_data(float*** data, uint32_t data_read){
    
    if(data_read != 0) {
        printf("[");
        for (uint32_t k = 0; k<data_read; k++){
            if(k!=0)printf(",");
            printf("{\"timer\":%f", data[0][k][0]);
            printf(",\"data_power_temp\":[");           // Time
            for (uint32_t i = 0; i <nb_socket; i++){
                if(i!=0)printf(",");
                printf("[");
                for (uint32_t m = 1; m<5; m++) {    // We skip the first data because it is "time"
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
        //printf ("Usage \n ./prog timer files*");
        exit(1);
    }
    float start_time = atof(argv[1]);
    float latest_time = 0;
    
    socket_info* sockets = open_files(argv+2, argc-2);

    
    for (int i = 0; i<nb_socket; i++) {
//        printf("socket %d socket %d ...\n", sockets[i].core_number,sockets[i].socket_number);fflush(stdout);
        if (get_timer(sockets+i) > latest_time){
            latest_time = get_timer(sockets+i);
        }
    }
    //printf("latest time found : %f\n", latest_time);
    //printf("requested time    : %f\n", start_time);
    if (start_time < latest_time) start_time = latest_time;
    start_time = (int) (start_time * 2);
    start_time = (start_time)/2;
    //printf("lets reach this number : %f\n", start_time);

    for (int i = 0; i<nb_socket; i++){
        reach_time(sockets+i, start_time);
        //printf("we have timer %f \n",  get_timer(sockets+i));
    }


    //  Preparing the data array
    
    //printf("We will do a malloc for %d socket, %d socket, %d line, %d data\n", nb_socket,  MAX_LINE_OUTPUT, 5);
    float ***data;          //  
    
    data = (float***) malloc(sizeof(float***) * nb_socket);
    for (int i = 0; i<nb_socket; i++){
        data[i] = (float**) malloc(sizeof(float*) * MAX_LINE_OUTPUT);
        for(int k = 0; k<MAX_LINE_OUTPUT; k++){
            data[i][k] = (float*) malloc(sizeof(float) * 5);
        }
    }
    
    
    uint32_t end_of_file = 0, data_read = 0;
    float max_time = start_time + 0.5;

    
    while (end_of_file == 0 && data_read < MAX_LINE_OUTPUT-1) {
        for (int i = 0; i<nb_socket; i++) { 
            end_of_file += 1 - read_data(sockets+i, data[sockets[i].socket_number][data_read], max_time);
        }
        //printf("data reached for timer %f \n",max_time);fflush(stdout);
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
