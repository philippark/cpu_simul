#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <time.h>

double next_exp(double lambda, double upper_bound){
    double x = -1;

    while (x < 0 || x > upper_bound){
        double r = drand48();
        x = -log( r ) / lambda;
    }

    return x;
}


int main(){
    
    double seed = 32;
    double lambda = 0.001;
    double upper_bound = 1024;

    srand48( seed );

    printf("%lf\n", next_exp(lambda, upper_bound));

    printf("%lf\n", drand48() * 32);
}

