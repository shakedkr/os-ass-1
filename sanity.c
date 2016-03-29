#include "types.h"
#include "stat.h"
#include "user.h"

void
match_number (int num, char* ans){
    
    switch (num){
        case 0:
            strcpy(ans, "CPU");
            break;
        case 1:
            strcpy(ans, "SCPU");
            break;
        case 2:
            strcpy(ans, "IO");
            break;
            
    }
   
        
}



int
main(int argc, char *argv[]) {
    
    //char* h ="12345678 \n";
    //printf(1,"sizeof: %d strlen: %d \n" , sizeof(h) ,strlen(h));
    
    
    char tmp[10];
    int n, i, j, s;
    //printf(1, "%d", argc);
    if (argc != 2) {
        printf(1, "error with number of args \n");
        exit();
    }
    int pid = 1;
    n = atoi(argv[1]);
    //printf(1, "n:%d\n", n);

    if (n == -1) {
        printf(1, "error with number \n");
        exit();
    }
    for (i = 0; i < 3 * n && pid; i++) {
        set_prio(3);
        pid = fork();
        
    }
    if (pid == 0) {
        pid = getpid();
        if (pid % 3 == 0) {
            for (j = 0; j < 100; j++)
                for (s = 0; s < 1000000; s++);
        } else if (pid % 3 == 1) {
            //set_prio(1);
            for (j = 0; j < 100; j++) {
                for (s = 0; s < 1000000; s++);
                yield();
            }
        } else {
            for (j = 0; j < 100; j++)
                sleep(1);
        }
    } else {
        int retime, setime, rutime, total_retime_type1, total_setime_type1, total_rutime_type1;
        int total_retime_type2, total_setime_type2, total_rutime_type2;
        int total_retime_type3, total_setime_type3, total_rutime_type3;
        for (i = 0; i < 3*n; i++) {
            pid = wait2(&retime, &rutime, &setime);
            match_number(pid%3, tmp);                    
            printf(1, "pid:%d  type:%s wait_time:%d sleep_time:%d run_time:%d\n", pid, tmp, retime, setime, rutime);

            if (pid % 3 == 0) {
                total_retime_type1 += retime;
                total_setime_type1 += setime;
                total_rutime_type1 += rutime;
            } else if (pid % 3 == 1) {
                total_retime_type2 += retime;
                total_setime_type2 += setime;
                total_rutime_type2 += rutime;

            } else {
                total_retime_type3 += retime;
                total_setime_type3 += setime;
                total_rutime_type3 += rutime;
            }
        }
        printf(1, "CPU bound :  ready time:%d , sleep time:%d, Turnaround Time:%d\n", total_retime_type1 / n, total_setime_type1 / n, (total_setime_type1 + total_retime_type1 + total_rutime_type1)/n);
        printf(1, "SCPU bound:  ready time:%d , sleep time:%d, Turnaround Time:%d\n", total_retime_type2 / n, total_setime_type2 / n, (total_setime_type2 + total_retime_type2 + total_rutime_type2)/n);
        printf(1, "IO bound  :  ready time:%d , sleep time:%d, Turnaround Time:%d\n", total_retime_type3 / n, total_setime_type3 / n, (total_setime_type3 + total_retime_type3 + total_rutime_type3)/n);
    }
    exit();
}
