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

    int n = 3;
    int n_cpu = 1;
    double seed = 32;
    double lambda = 0.001;
    double upper_bound = 1024;

    srand48( seed );


    for (int i = 0; i < n; i++){
        int arrival_time = floor(next_exp(lambda, upper_bound));

        int num_cpu_bursts = ceil(drand48() * 32);

        printf("bound process: arrival time %dms; %d CPU bursts:\n", arrival_time, num_cpu_bursts);

        for (int j = 0; j < num_cpu_bursts; j++){
            int cpu_burst_time = ceil(next_exp(lambda, upper_bound));

            int io_burst_time = 0;

            if (j != num_cpu_bursts-1){
                io_burst_time = ceil(next_exp(lambda, upper_bound)) * 8;
            }

            if (i < n_cpu){
                cpu_burst_time *= 4;
                io_burst_time /= 8;
            }

            if (j == num_cpu_bursts-1){
                printf("==> CPU burst %dms\n", cpu_burst_time);
                break;
            }

            if (j < 5)
                printf("==> CPU burst %dms ==> I/O burst %dms\n", cpu_burst_time, io_burst_time);
        }
    }

}

