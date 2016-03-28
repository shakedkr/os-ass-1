#include "types.h"
#include "stat.h"
#include "user.h"


//#include "defs.h"


int
main(int argc, char *argv[]) {
    //char* tmp= "CPU";
    int n = 20, i, j, s;
    int pid = 1;
    int creationTimePerProcess [n];
    //int creationTime;
    
   // creationTime = uptime();
    for (i = 0; i < n && pid; i++) {
        pid = fork();
        creationTimePerProcess[pid]=uptime();
        if (getpid()%3==0){
            set_prio(3);
        }
        else if (getpid()%3==1){
            set_prio(2);
        }
       
        else{
            set_prio(1);
        }
    }
    
    if (pid == 0) {
        //pid = getpid();
       
        for (j = 0; j < 100; j++)
            for (s = 0; s < 1000000; s++);

    }
    
    else {
        int retime, setime, rutime;// total_retime_type1, total_setime_type1, total_rutime_type1;
        for (i = 0; i <  n; i++) {
            
            pid = wait2(&retime, &rutime, &setime);
            
            printf(1, "pid:%d  termination_time:%d\n", pid, (creationTimePerProcess[i]+retime+rutime+setime) );

                //total_retime_type1 += retime;
                //total_setime_type1 += setime;
                //total_rutime_type1 += rutime;
                
        }
       // printf(1, "CPU bound :  ready time:%d , sleep time:%d, Turnaround Time:%d\n", total_retime_type1 / n, total_setime_type1 / n, (total_setime_type1 + total_retime_type1 + total_rutime_type1) / n);
    }
    exit();
}
