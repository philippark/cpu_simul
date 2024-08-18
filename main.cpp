#include <bits/stdc++.h>
#include <fcntl.h>

using namespace std;


// global variables that need to be turned into arguments 
int context_switch = 4;
int time_slice = 256;   
double alpha = .75;
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
		short process_state; // 0 is arrival; 1 is just finished cpu burst; 2 just finished io burst;
		double tau;
        int original_cpu_burst_time = -1;
        bool preemption = true;
	
};

class Compare{
public:
    bool operator() (Process p1, Process p2)
    {
        return p1.time > p2.time;
    }
};

class CompareSJF{
public:
    bool operator() (Process p1, Process p2)
    {
        return p1.tau > p2.tau;
    }
};

void fcfs(priority_queue<Process, vector<Process>, Compare> tasks);

void sjf(priority_queue<Process, vector<Process>, Compare> tasks);

void round_robin(priority_queue<Process, vector<Process>, Compare>& tasks);


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
	process.tau = 1.0 / lambda;
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
                process.burst_times.push_back({cpu_burst_time, -1});
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
    //sjf(tasks);
    //round_robin(tasks);
    return EXIT_SUCCESS;
}

string queueState(queue<Process> &ready){
    string queue_state = "[Q";

    int size = ready.size();
    if (size == 0){
        queue_state += " empty";
    }

    for (int i = 0; i < size; i++){
        Process temp = ready.front();
        queue_state += " " + temp.name;
        ready.pop();
        ready.push(temp);
    }

    queue_state += "]";

    return queue_state;
}

string queueState(priority_queue<Process, vector<Process>, CompareSJF> ready){
    string queue_state = "[Q";

    int size = ready.size();
    if (size == 0){
        queue_state += " empty";
    }

    for (int i = 0; i < size; i++){
        Process temp = ready.top();
        queue_state += " " + temp.name;
        ready.pop();
    }

    queue_state += "]";

    return queue_state;
}

void fcfs(priority_queue<Process, vector<Process>, Compare> tasks){
    queue<Process> ready;
    bool running = false;
    int system_time = 0; //keeps track of the current timestamp

    cout << "time " << system_time << "ms: Simulator started for FCFS [Q empty]" << endl;

    while (!tasks.empty() || !ready.empty()){
        //if nothing running, or no tasks to be done, and there are tasks that are ready
        //grab next in ready, calculate its cpu burst finish time, and add to tasks
        if ((!running || tasks.empty()) && !ready.empty()){
            Process curr = ready.front();
            ready.pop();

            system_time += (context_switch / 2);

            cout << "time " << system_time << "ms: Process " << curr.name << " started using the CPU for " << curr.burst_times[curr.index].first << "ms burst " << queueState(ready) << endl;

            curr.time = system_time + curr.burst_times[curr.index].first;
            curr.process_state = 1;

            tasks.push(curr);

            running = true;
        }

        Process curr = tasks.top();
        tasks.pop();
        system_time = curr.time;

        //if arrived, add to ready queue
        if (curr.process_state == 0){

            cout << "time " << system_time << "ms: Process " << curr.name << " arrived; added to ready queue ";

            ready.push(curr);

            cout << queueState(ready) << endl;
        }

        //if finished cpu burst, move to io burst time
        else if (curr.process_state == 1){
            running = false;

            if (curr.burst_times[curr.index].second == -1){
                cout << "time " << system_time << "ms: Process " << curr.name << " terminated " << queueState(ready) << endl;
                //reset variables
                curr.index = 0;
                //curr.time = original arrival time
                continue;

                continue;
            }

            cout << "time " << system_time << "ms: Process " << curr.name << " completed a CPU burst; ";
            cout << curr.burst_times.size() - curr.index - 1;
            if (curr.burst_times.size() - curr.index - 1 == 1){
                cout << " burst to go ";
            }
            else {
                cout << " bursts to go ";
            }
            cout << queueState(ready) << endl;
            curr.time += curr.burst_times[curr.index].second + (context_switch / 2);
            curr.process_state = 2;
            tasks.push(curr);

            cout << "time " << system_time << "ms: Process " << curr.name << " switching out of CPU; blocking on I/O until time " << curr.time << "ms " << queueState(ready) << endl;

            system_time += (context_switch / 2);
        }

        //if finished io burst, add to ready queue
        else if (curr.process_state == 2){
            cout << "time " << system_time << "ms: Process " << curr.name << " completed I/O; added to ready queue ";

            curr.index++;
            
            ready.push(curr);

            cout << queueState(ready) << endl;
        }

    }

    system_time += (context_switch / 2);
    cout << "time " << system_time << "ms: Simulator ended for FCFS [Q empty]" << endl;

}


void sjf(priority_queue<Process, vector<Process>, Compare> tasks){
    priority_queue<Process, vector<Process>, CompareSJF> ready;
    bool running = false;
    int system_time = 0; //keeps track of the current timestamp

    cout << "time " << system_time << "ms: Simulator started for SJF [Q empty]" << endl;
    // tau_i+1 = ceil(alpha * t_actual + (1-alpha) * tau_i
    while (!tasks.empty() || !ready.empty()){
        //if nothing running, or no tasks to be done, and there are tasks that are ready
        //grab next in ready, calculate its cpu burst finish time, and add to tasks
        if ((!running || tasks.empty()) && !ready.empty()){
            Process curr = ready.top();
            ready.pop();

            system_time += (context_switch / 2);

            cout << "time " << system_time << "ms: Process " << curr.name << " (tau " << curr.tau << "ms) started using the CPU for " << curr.burst_times[curr.index].first << "ms burst " << queueState(ready) << endl;

            curr.time = system_time + curr.burst_times[curr.index].first;
            curr.process_state = 1;

            tasks.push(curr);

            running = true;
        }

        Process curr = tasks.top();
        tasks.pop();
        system_time = curr.time;

        //if arrived, add to ready queue
        if (curr.process_state == 0){

            cout << "time " << system_time << "ms: Process " << curr.name << " (tau " << curr.tau << "ms) arrived; added to ready queue ";

            ready.push(curr);

            cout << queueState(ready) << endl;
        }

        //if finished cpu burst, move to io burst time
        else if (curr.process_state == 1){
            running = false;

            if (curr.burst_times[curr.index].second == -1){
                cout << "time " << system_time << "ms: Process " << curr.name << " terminated " << queueState(ready) << endl;
                continue;
            }

            cout << "time " << system_time << "ms: Process " << curr.name << " (tau " << curr.tau << "ms) completed a CPU burst; ";
            cout << curr.burst_times.size() - curr.index - 1;
            if (curr.burst_times.size() - curr.index - 1 == 1){
                cout << " burst to go ";
            }
            else {
                cout << " bursts to go ";
            }
            cout << queueState(ready) << endl;
	    double new_tau = ceil(alpha * curr.burst_times[curr.index].first + (1.0 - alpha) * curr.tau);
	    cout << "time " << system_time << "ms: Recalculated tau for process " << curr.name <<": old tau " << curr.tau <<"ms ==> new tau "<< new_tau <<"ms "; 
	    curr.tau = new_tau;
            cout << queueState(ready) << endl;
            curr.time += curr.burst_times[curr.index].second + (context_switch / 2);
            curr.process_state = 2;
            tasks.push(curr);

            cout << "time " << system_time << "ms: Process " << curr.name << " switching out of CPU; blocking on I/O until time " << curr.time << "ms " << queueState(ready) << endl;

            system_time += (context_switch / 2);
        }

        //if finished io burst, add to ready queue
        else if (curr.process_state == 2){
            cout << "time " << system_time << "ms: Process " << curr.name << " (tau " << curr.tau << "ms) completed I/O; added to ready queue ";

            curr.index++;
            
            ready.push(curr);

            cout << queueState(ready) << endl;
        }

    }

    system_time += (context_switch / 2);
    cout << "time " << system_time << "ms: Simulator ended for SJF [Q empty]" << endl;

}


void round_robin(priority_queue<Process, vector<Process>, Compare>& tasks){
    queue<Process> ready;
    bool running = false;
    int system_time = 0; //keeps track of the current timestamp

    cout << "time " << system_time << "ms: Simulator started for RR [Q empty]" << endl;

    while (!tasks.empty() || !ready.empty()){
        //if nothing running, or no tasks to be done, and there are tasks that are ready
        //grab next in ready, calculate its cpu burst finish time, and add to tasks
        if ((!running || tasks.empty()) && !ready.empty()){
            Process curr = ready.front();
            ready.pop();

            system_time += (context_switch / 2);
        
            if (curr.original_cpu_burst_time == -1){
                curr.original_cpu_burst_time = curr.burst_times[curr.index].first;
            }

            if (curr.burst_times[curr.index].first == curr.original_cpu_burst_time){
                cout << "time " << system_time << "ms: Process " << curr.name << " started using the CPU for " << curr.burst_times[curr.index].first << "ms burst " << queueState(ready) << endl;
            }
            else if (curr.preemption){
                cout << "time " << system_time << "ms: Process " << curr.name << " started using the CPU for remaining " << curr.burst_times[curr.index].first << "ms of " << curr.original_cpu_burst_time << "ms burst " << queueState(ready) << endl;
            }
            else {
                system_time -= (context_switch / 2);
            }

            if (curr.burst_times[curr.index].first - time_slice < 0){
                curr.time = system_time + curr.burst_times[curr.index].first;
                curr.burst_times[curr.index].first = 0;
            }
            else {
                curr.burst_times[curr.index].first -= time_slice;
                curr.time = system_time + time_slice;
            }



            curr.process_state = 1;
            tasks.push(curr);
            running = true;
        }

        Process curr = tasks.top();
        tasks.pop();
        system_time = curr.time;

        //if arrived, add to ready queue
        if (curr.process_state == 0){

            cout << "time " << system_time << "ms: Process " << curr.name << " arrived; added to ready queue ";

            ready.push(curr);

            cout << queueState(ready) << endl;
        }

        //if finished cpu burst, move to io burst time
        else if (curr.process_state == 1){
            running = false;

            //no more remaining time
            if (curr.burst_times[curr.index].first == 0) {
                if (curr.burst_times[curr.index].second == -1){
                    cout << "time " << system_time << "ms: Process " << curr.name << " terminated " << queueState(ready) << endl;
                    system_time += (context_switch / 2);
                    continue;
                }


                cout << "time " << system_time << "ms: Process " << curr.name << " completed a CPU burst; ";
                cout << curr.burst_times.size() - curr.index - 1;
                if (curr.burst_times.size() - curr.index - 1 == 1){
                    cout << " burst to go ";
                }
                else {
                    cout << " bursts to go ";
                }
                cout << queueState(ready) << endl;
                curr.time += curr.burst_times[curr.index].second + (context_switch / 2);
                curr.process_state = 2;
                tasks.push(curr);

                cout << "time " << system_time << "ms: Process " << curr.name << " switching out of CPU; blocking on I/O until time " << curr.time << "ms " << queueState(ready) << endl;

                system_time += (context_switch) / 2;
            }

            else {
                if (ready.empty()){
                    cout << "time " << system_time << "ms: Time slice expired; no preemption because ready queue is empty " << queueState(ready) << endl;
                    curr.preemption = false;
                }
                else {
                    cout << "time " << system_time << "ms: Time slice expired; preempting process " << curr.name << " with " << curr.burst_times[curr.index].first << "ms remaining " << queueState(ready) << endl;
                    curr.preemption = true;
                    system_time += (context_switch / 2);
                }

                ready.push(curr);
            }

        }

        //if finished io burst, add to ready queue
        else if (curr.process_state == 2){
            cout << "time " << system_time << "ms: Process " << curr.name << " completed I/O; added to ready queue ";

            curr.index++;

            curr.original_cpu_burst_time = curr.burst_times[curr.index].first;
            
            ready.push(curr);

            cout << queueState(ready) << endl;
        }

    }

    cout << "time " << system_time << "ms: Simulator ended for RR [Q empty]" << endl;



}
