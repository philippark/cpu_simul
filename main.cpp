#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdio.h>

# include <cstdlib>
# include <iostream>
# include <iomanip>
# include <ctime>

double next_expn(double seed, double lambda){
    return 1;
}

int main(int argc, char** argv){
    if (argc != 6){
        std::cout << "Not enough arguments" << std::endl;
        return -1;
    }

    int n = std::stoi(*(argv+1));
    int n_cpu = std::stoi(*(argv+2));
    int seed = std::stoi(*(argv+3));
    int lambda = std::stoi(*(argv+4));
    int random_upper_bound = std::stoi(*(argv+5));

    

    for (int i = 0; i < n; i++){
        int arrival_time = floor(next_expn(seed, lambda));

        int num_cpu_bursts = ceiling(drand() * 32);

        for (int j = 0; j < num_cpu_bursts; j++){
            int cpu_burst_time = next_exp(seed, lambda);

            //if last cpu burst, dont generate anymore
            if (j == num_cpu_bursts-1) continue;


            int io_burst_time = next_exp(seed, lambda);

            while (io_burst_time <= 2*cpu_burst_time){
                io_burst_time *= 8;
            }

            //if cpu bound
            if (i < n_cpu){
                cpu_burst_time *- 4;
                io_burst_time /= 8;
            }

        }
    }

}
