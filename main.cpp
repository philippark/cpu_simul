#include <bits/stdc++.h>
#include <fcntl.h>

using namespace std;

/*
class process:
    - [ [cpu, io], [cpu, io], ... ]
    - index
    - name

    - time
    - string process_state


*/

class Process{
	public:
		vector<pair<int, int>> burst_times; 
        int index = 0;
		string name;
		int time;
		short process_state; // 0 is arrival; 1 is just finished cpu burst; 2 just finished io burst; 3 is just finished waiting
	
};

class Foo
{

};

class Compare
{
public:
    bool operator() (Process p1, Process p2)
    {
        return p1.time > p2.time;
    }
};

void fcfs(priority_queue<Process, vector<Process>, Compare> &tasks);




float next_exp(float lambda, float upper_bound) {
    float x = -1;

    while (x < 0 || std::ceil(x) > upper_bound) {
        float r = drand48();
        x = -std::log(r) / lambda;
    }

    return x;
}

int main(int argc, char** argv) {
    /* Parse argument input */
    if (argc != 6) {
        std::cerr << "ERROR: Non-valid arguments" << std::endl;
        return EXIT_FAILURE;
    }

    char* e;
    errno = 0;

    int n = std::strtol(argv[1], &e, 10);
    if (*e != '\0' || errno != 0) {
        std::cerr << "ERROR: Failed to get n" << std::endl;
        return EXIT_FAILURE;
    }

    if (n <= 0) {
        std::cerr << "ERROR: N must be > 0" << std::endl;
        return EXIT_FAILURE;
    }
    if (n > 260) {
        std::cerr << "ERROR: N must be <= 260" << std::endl;
        return EXIT_FAILURE;
    }

    int n_cpu = std::strtol(argv[2], &e, 10);
    if (*e != '\0' || errno != 0) {
        std::cerr << "ERROR: Failed to get n_cpu" << std::endl;
        return EXIT_FAILURE;
    }

    if (n_cpu < 0) {
        std::cerr << "ERROR: Number of CPU bound can't be negative" << std::endl;
        return EXIT_FAILURE;
    }
    if (n_cpu > n) {
        std::cerr << "ERROR: Number of CPU bound can't be greater than number of processes" << std::endl;
        return EXIT_FAILURE;
    }

    long int seed = std::strtol(argv[3], &e, 10);
    if (*e != '\0' || errno != 0) {
        std::cerr << "ERROR: Failed to get seed" << std::endl;
        return EXIT_FAILURE;
    }

    float lambda = std::strtod(argv[4], &e);
    if (*e != '\0' || errno != 0) {
        std::cerr << "ERROR: Failed to get lambda" << std::endl;
        return EXIT_FAILURE;
    }

    long int upper_bound = std::strtol(argv[5], &e, 10);
    if (*e != '\0' || errno != 0) {
        std::cerr << "ERROR: Failed to get upper_bound" << std::endl;
        return EXIT_FAILURE;
    }
    if (upper_bound < 0) {
        std::cerr << "ERROR: upper bound can't be negative" << std::endl;
        return EXIT_FAILURE;
    }

    /* Print heading */
    std::cout << "<<< PROJECT PART I" << std::endl;
    std::cout << "<<< -- process set (n=" << n << ") with ";
    if (n_cpu == 1) {
        std::cout << n_cpu << " CPU-bound process" << std::endl;
    } else {
        std::cout << n_cpu << " CPU-bound processes" << std::endl;
    }
    std::cout << "<<< -- seed=" << seed << "; lambda=" << lambda << "; bound=" << upper_bound << std::endl;

    /* Initialize variables */
    srand48(seed);

    const char* alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
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
    //vector for processes for use in part 2 algorithms
    //priority_queue<Process, vector<Process>, Compare> tasks; 
    priority_queue<Process, std::vector<Process>, Compare> tasks;

    for (int i = 0; i < n; i++) { // loop through all processes
        Process process;
	    //vector<pair<int,int>> burst_times;
        int arrival_time = std::floor(next_exp(lambda, upper_bound));
        int num_cpu_bursts = std::ceil(drand48() * 32);
        num_cpu_burst_time += num_cpu_bursts;
        num_io_burst_time += num_cpu_bursts - 1;

        if (i < n_cpu) {
            cpu_bound_num_cpu_burst_time += num_cpu_bursts;
            cpu_bound_num_io_burst_time += num_cpu_bursts - 1;
        } else {
            io_bound_num_cpu_burst_time += num_cpu_bursts;
            io_bound_num_io_burst_time += num_cpu_bursts - 1;
        }

        if (i < n_cpu) {
            std::cout << "CPU-bound ";
        } else {
            std::cout << "I/O-bound ";
        }

        std::cout << "process " << alphabet[alphabet_position] << process_count << ": arrival time " << arrival_time << "ms; ";
        process.name = alphabet[alphabet_position] + to_string(process_count);
        process.time = arrival_time;
        process.process_state = 0;

        if (num_cpu_bursts == 1) {
            std::cout << num_cpu_bursts << " CPU burst:" << std::endl;
        } else {
            std::cout << num_cpu_bursts << " CPU bursts:" << std::endl;
        }

        process_count++;
        // Check if to move from A9->B0
        if (process_count == 10) {
            process_count = 0;
            alphabet_position++;
        }

        for (int j = 0; j < num_cpu_bursts; j++) {
            int cpu_burst_time = std::ceil(next_exp(lambda, upper_bound));

            int io_burst_time = 0;

            // If not the last CPU burst, get the I/O burst time.
            if (j != num_cpu_bursts - 1) {
                io_burst_time = std::ceil(next_exp(lambda, upper_bound)) * 8;
            }

            // Separate calculations if CPU bound
            if (i < n_cpu) {
                cpu_burst_time *= 4;
                io_burst_time /= 8;

                cpu_bound_sum_cpu_burst_time += cpu_burst_time;
                cpu_bound_sum_io_burst_time += io_burst_time;
            } else {
                io_bound_sum_cpu_burst_time += cpu_burst_time;
                io_bound_sum_io_burst_time += io_burst_time;
            }

            sum_cpu_burst_time += cpu_burst_time;
            sum_io_burst_time += io_burst_time;
            
            // If last CPU burst, only print CPU burst.
            if (j == num_cpu_bursts - 1) {
                std::cout << "==> CPU burst " << cpu_burst_time << "ms" << std::endl;
                process.burst_times.push_back({cpu_burst_time, 0});
                break;
            }
            
            std::cout << "==> CPU burst " << cpu_burst_time << "ms ==> I/O burst " << io_burst_time << "ms" << std::endl;
            process.burst_times.push_back({cpu_burst_time, io_burst_time});
        }

        //Test output to see if matches
        cout << process.name << " " << process.process_state << " " << endl;
        for (int i = 0; i < process.burst_times.size(); i++){
            cout << process.burst_times[i].first << " " << process.burst_times[i].second << endl;
        }
        tasks.push(process);
    }

    /* Output statistics to simout.txt */
    int fd = open("simout.txt", O_WRONLY | O_CREAT | O_TRUNC, 0660);
    if (fd == -1) {
        std::cerr << "ERROR: open() failed" << std::endl;
        return EXIT_FAILURE;
    }

    float avg_cpu_burst_time = num_cpu_burst_time ? (std::ceil((sum_cpu_burst_time * 1000.0) / num_cpu_burst_time) / 1000.0) : 0.f;
    float avg_io_burst_time = num_io_burst_time ? (std::ceil((sum_io_burst_time * 1000.0) / num_io_burst_time) / 1000.0) : 0.f;
    float cpu_bound_avg_cpu_burst_time = cpu_bound_num_cpu_burst_time ? (std::ceil((cpu_bound_sum_cpu_burst_time * 1000.0) / cpu_bound_num_cpu_burst_time) / 1000.0) : 0.f;
    float cpu_bound_avg_io_burst_time = cpu_bound_num_io_burst_time ? (std::ceil((cpu_bound_sum_io_burst_time * 1000.0) / cpu_bound_num_io_burst_time) / 1000.0) : 0.f;
    float io_bound_avg_cpu_burst_time = io_bound_num_cpu_burst_time ? (std::ceil((io_bound_sum_cpu_burst_time * 1000.0) / io_bound_num_cpu_burst_time) / 1000.0) : 0.f;
    float io_bound_avg_io_burst_time = io_bound_num_io_burst_time ? (std::ceil((io_bound_sum_io_burst_time * 1000.0) / io_bound_num_io_burst_time) / 1000.0) : 0.f;

    dprintf(fd, "-- number of processes: %d\n", n);
    dprintf(fd, "-- number of CPU-bound processes: %d\n", n_cpu);
    dprintf(fd, "-- number of I/O-bound processes: %d\n", n - n_cpu);
    dprintf(fd, "-- CPU-bound average CPU burst time: %.3f ms\n", cpu_bound_avg_cpu_burst_time);
    dprintf(fd, "-- I/O-bound average CPU burst time: %.3f ms\n", io_bound_avg_cpu_burst_time);
    dprintf(fd, "-- overall average CPU burst time: %.3f ms\n", avg_cpu_burst_time);
    dprintf(fd, "-- CPU-bound average I/O burst time: %.3f ms\n", cpu_bound_avg_io_burst_time);
    dprintf(fd, "-- I/O-bound average I/O burst time: %.3f ms\n", io_bound_avg_io_burst_time);
    dprintf(fd, "-- overall average I/O burst time: %.3f ms\n", avg_io_burst_time);
    close(fd);

    fcfs(tasks);

    return EXIT_SUCCESS;
}

void fcfs(priority_queue<Process, vector<Process>, Compare>& tasks){
    cout << "FCFS" << endl;

    queue<Process> ready;
    bool running = false;

    //initialize the first process
    Process first = tasks.top();
    tasks.pop();

    cout << first.name << " arrived; added to ready queue [Q A0]" << endl;

    first.time += first.burst_times[first.index].first;
    first.process_state = 1;

    cout << first.name << " started using the cpu for " << first.burst_times[first.index].first << " burst [Q empty] " << endl;
    tasks.push(first);
    running = true;

    int count = 0;

    while ( (!tasks.empty() || !ready.empty()) && count != 22){
        count++;

        cout << "tasks: " << tasks.size() << " ready: " << ready.size() << endl;

        Process curr;
        if (tasks.empty()){
            curr = ready.front();
            ready.pop();

            //run its cpu? and change state to 1?
        }
        else {
            curr = tasks.top();
            tasks.pop();
        }

        cout << curr.name << " " << curr.process_state << " " << curr.time << endl;

        if (curr.index >= curr.burst_times.size()){
            cout << curr.name << " finished" << endl;
            continue;
        }

        /*
        if task is arrival: 
            add to ready queue
        */
        if (!running){
            if (ready.empty()) continue;

            Process next = ready.front();
            ready.pop();

            next.time = curr_time + next.burst_times[next.index].first;
            next.process_state = 1;
            
            cout << next.name << " started using the cpu " << endl;

            tasks.push(next);

            running = true;
        }

        if (curr.process_state == 0){
            cout << curr.name << " arrived: added to ready queue" << endl;
            ready.push(curr);
        }

        /*
        if task is cpu finished:
            add back to task its io finish time
            grab next in ready, calculate its cpu finish and add to tasks
        */

        
        else if (curr.process_state == 1){
            cout << curr.name << " completed a cpu burst " << endl;
            
            int curr_time = curr.time;

            curr.time += curr.burst_times[curr.index].second;
            curr.process_state = 2;

            cout << curr.name << " switching out of cpu; blocking on I/O until " << curr.time << endl;

            tasks.push(curr);

            if (ready.empty()) continue;


            Process next = ready.front();
            ready.pop();

            next.time = curr_time + next.burst_times[next.index].first;
            next.process_state = 1;
            
            cout << next.name << " started using the cpu " << endl;

            tasks.push(next);

        }
        

       /*
        else if task is io finished:
            move index
            add to ready queue	
        */
       
        else if (curr.process_state == 2){
            cout << curr.name << " completed I/O; added to ready queue " << endl;

            curr.index++;
            ready.push(curr);
        }
        
    }

}

