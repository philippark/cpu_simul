#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>

float next_exp(float lambda, float upper_bound){
    float x = -1;

    while (x < 0 || ceil(x) > upper_bound){
        float r = drand48();
        x = -log( r ) / lambda;
    }

    return x;
}


int main(int argc, char** argv){

    /*Parse argument input*/
    if (argc != 6){
        fprintf(stderr, "%s", "Non-valid arguments");
        return EXIT_FAILURE;
    }

    char* e;
    errno = 0;

    int n = (int)strtol(*(argv+1), &e, 10);
    if (*e != '\0' || errno != 0){
        fprintf(stderr, "Failed to get n\n");
        return EXIT_FAILURE;
    }

    if (n <= 0){
        fprintf(stderr, "N must be > 0\n");
        return EXIT_FAILURE;
    }

    int n_cpu = (int)strtol(*(argv+2), &e, 10);
    if (*e != '\0' || errno != 0){
        fprintf(stderr, "Failed to get n_cpu\n");
        return EXIT_FAILURE;
    }

    if (n_cpu < 0){
        fprintf(stderr, "Number of CPU bound can't be negative\n");
    }

    long int seed = strtol(*(argv+3), &e, 10);
    if (*e != '\0' || errno != 0){
        fprintf(stderr, "Failed to get seed\n");
        return EXIT_FAILURE;
    }

    float lambda = strtod(*(argv+4), &e);
    if (*e != '\0' || errno != 0){
        fprintf(stderr, "Failed to get lambda\n");
        return EXIT_FAILURE;
    }

    long int upper_bound = strtol(*(argv+5), &e, 10);
    if (*e != '\0' || errno != 0){
        fprintf(stderr, "Failed to get upper_bound\n");
        return EXIT_FAILURE;
    }

    /*print heading*/
    printf("<<< PROJECT PART I\n");
    printf("<<< -- process set (n=%d) with ", n);
    if (n_cpu == 1){
        printf("%d CPU-bound process\n", n_cpu);
    }
    else{
        printf("%d CPU-bound processes\n", n_cpu);
    }
    printf("<<< -- seed=%ld; lambda=%f; bound=%ld\n", seed, lambda, upper_bound);

    /*initialize variables*/
    srand48( seed );


    char* alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int alphabet_position = 0;
    int process_count = 0;

    float sum_cpu_burst_time = 0;
    int num_cpu_burst_time = 0;

    float sum_io_burst_time = 0;
    int num_io_burst_time = 0;

    float cpu_bound_sum_cpu_burst_time = 0;
    int cpu_bound_num_cpu_burst_time = 0;

    float cpu_bound_sum_io_burst_time = 0;
    int cpu_bound_num_io_burst_time = 0;

    float io_bound_sum_cpu_burst_time = 0;
    int io_bound_num_cpu_burst_time = 0;

    float io_bound_sum_io_burst_time = 0;
    int io_bound_num_io_burst_time = 0;


    for (int i = 0; i < n; i++){
        int arrival_time = floor(next_exp(lambda, upper_bound));

        int num_cpu_bursts = ceil(drand48() * 32);
        num_cpu_burst_time += num_cpu_bursts;
        num_io_burst_time += num_cpu_bursts - 1;

        if (i < n_cpu){
            cpu_bound_num_cpu_burst_time += num_cpu_bursts;
            cpu_bound_num_io_burst_time += num_cpu_bursts - 1;
        }
        else{
            io_bound_num_cpu_burst_time += num_cpu_bursts;
            io_bound_num_io_burst_time += num_cpu_bursts - 1;
        }

        if (i < n_cpu){
            printf("CPU-bound ");
        }
        else{
            printf("I/O-bound ");
        }

        printf("process %c%d: arrival time %dms; ", *(alphabet+alphabet_position), process_count, arrival_time);

        if (num_cpu_bursts == 1){
            printf("%d CPU burst:\n", num_cpu_bursts);
        }
        else{
            printf("%d CPU bursts:\n", num_cpu_bursts);
        }

        process_count++;
        //check if to move from A9->B0
        if (process_count == 10){
            process_count = 0;
            alphabet_position++;
        }

        if (alphabet_position == 26) {
            break;
        }

        for (int j = 0; j < num_cpu_bursts; j++){
            int cpu_burst_time = ceil(next_exp(lambda, upper_bound));

            int io_burst_time = 0;

            //if not the last cpu burst, get the io burst time.
            if (j != num_cpu_bursts-1){
                io_burst_time = ceil(next_exp(lambda, upper_bound)) * 8;
            }

            //Seperate calculations if cpu bound
            if (i < n_cpu){
                cpu_burst_time *= 4;
                io_burst_time /= 8;

                cpu_bound_sum_cpu_burst_time += cpu_burst_time;
                cpu_bound_sum_io_burst_time += io_burst_time;

                
            }
            else{
                io_bound_sum_cpu_burst_time += cpu_burst_time;
                io_bound_sum_io_burst_time += io_burst_time;
            }

            sum_cpu_burst_time += cpu_burst_time;
            sum_io_burst_time += io_burst_time;

            //if last cpu burst, only print cpu burst.
            if (j == num_cpu_bursts-1){
                printf("==> CPU burst %dms\n", cpu_burst_time);
                break;
            }

 
            printf("==> CPU burst %dms ==> I/O burst %dms\n", cpu_burst_time, io_burst_time);

        }
    }


    
    /*Output statistics to simout.txt*/
    
    close(1);
    int fd = open("simout.txt", O_WRONLY | O_CREAT | O_TRUNC, 0660);
    if (fd == -1){
        perror("open() failed\n");
        return EXIT_FAILURE;
    }

    float avg_cpu_burst_time = num_cpu_burst_time ? (ceil((sum_cpu_burst_time * 1000.0) / num_cpu_burst_time) / 1000.0) : 0.f;

    float avg_io_burst_time = num_io_burst_time ? (ceil((sum_io_burst_time * 1000.0) / (num_io_burst_time)) / 1000.0) : 0.f;

    float cpu_bound_avg_cpu_burst_time = cpu_bound_num_cpu_burst_time ? (ceil((cpu_bound_sum_cpu_burst_time * 1000.0) / cpu_bound_num_cpu_burst_time) / 1000.0) : 0.f;
    float cpu_bound_avg_io_burst_time = cpu_bound_num_io_burst_time ? (ceil((cpu_bound_sum_io_burst_time * 1000.0) / cpu_bound_num_io_burst_time) / 1000.0) : 0.f;

    float io_bound_avg_cpu_burst_time = io_bound_num_cpu_burst_time ? (ceil((io_bound_sum_cpu_burst_time * 1000.0) / io_bound_num_cpu_burst_time) / 1000.0) : 0.f;
    float io_bound_avg_io_burst_time = io_bound_num_io_burst_time ? (ceil((io_bound_sum_io_burst_time * 1000.0) / io_bound_num_io_burst_time) / 1000.0) : 0.f;



    printf("-- number of processes: %d\n", n);
    printf("-- number of CPU-bound processes: %d\n", n_cpu);
    printf("-- number of I/O-bound processes: %d\n", n - n_cpu);

    printf("-- CPU-bound average CPU burst time: %.3f ms\n", cpu_bound_avg_cpu_burst_time);
    printf("-- I/O-bound average CPU burst time: %.3f ms\n", io_bound_avg_cpu_burst_time);
    printf("-- overall average CPU burst time: %.3f ms\n", avg_cpu_burst_time);

    printf("-- CPU-bound average I/O burst time: %.3f ms\n", cpu_bound_avg_io_burst_time);
    printf("-- I/O-bound average I/O burst time: %.3f ms\n", io_bound_avg_io_burst_time);
    printf("-- overall average I/O burst time: %.3f ms\n", avg_io_burst_time);

    close(fd);
    
}

