#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <time.h>

double next_exp(double lambda, double upper_bound){
    double x = -1;

    while (x < 0 || ceil(x) > upper_bound){
        double r = drand48();
        x = -log( r ) / lambda;
    }

    return x;
}


int main(int argc, char** argv){

    if (argc != 6){
        fprintf(stderr, "%s", "Non-valid arguments");
    }

    int n = atoi(*(argv+1));
    int n_cpu = atoi(*(argv+2));
    double seed = atof(*(argv+3));
    double lambda = atof(*(argv+4));
    double upper_bound = atof(*(argv+5));

    //TODO: error check input args
    //TODO: error check if n is beyond possible? 

    printf("<<< PROJECT PART 1\n");
    printf("<<< -- process set (n=%d) with ", n);
    if (n_cpu == 1){
        printf("%d CPU-bound process\n", n_cpu);
    }
    else{
        printf("%d CPU-bound processes\n", n_cpu);
    }
    printf("<<< -- seed=%lf; lambda=%lf; bound=%lf\n", seed, lambda, upper_bound);

    srand48( seed );

    char* alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int alphabet_position = 0;
    int process_count = 0;

    double sum_cpu_burst_time = 0;
    double sum_io_burst_time = 0;
    //just sum it all up 

    double cpu_bound_sum_cpu_burst_time = 0;
    double cpu_bound_sum_io_burst_time = 0;
    //in if statement, sum it all up

    double io_bound_sum_cpu_burst_time = 0;
    double io_bound_sum_io_burst_time = 0;

    for (int i = 0; i < n; i++){
        int arrival_time = floor(next_exp(lambda, upper_bound));

        int num_cpu_bursts = ceil(drand48() * 32);

        if (i < n_cpu){
            printf("CPU-bound ");
        }
        else{
            printf("I/O-bound ");
        }

        printf("process %c%d: arrival time %dms; %d CPU bursts:\n", *(alphabet+alphabet_position), process_count, arrival_time, num_cpu_bursts);

        process_count++;
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

            //Calc if cpu bound
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

            //if last cpu burst, only print cpu burst.
            if (j == num_cpu_bursts-1){
                printf("==> CPU burst %dms\n", cpu_burst_time);
                break;
            }

 
            printf("==> CPU burst %dms ==> I/O burst %dms\n", cpu_burst_time, io_burst_time);

            sum_cpu_burst_time += cpu_burst_time;
            sum_io_burst_time += io_burst_time;

        }
    }



    /*
    TODO:
    - n
    - n_cpu
    - IO: n - n_cpu

    */

}

