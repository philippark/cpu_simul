#include <iostream>
#include <string>

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

}
