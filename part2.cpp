/*
curr_time = 0;
init with the first process

while all 3 are not empty
    if (next process arrival >= curr_time){
        add to ready
    }
    if (ready){
        int process = ready.top();
        ready.pop();
        curr_time += ready cpu burst time
        waiting add curr_time + ready io burst time

    }
    else{
        add min(waiting top and next process arrival time)
    }

*/