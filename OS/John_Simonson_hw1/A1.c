// A1 driver program

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <sys/resource.h>

typedef union{
    int exit_status;
    struct{
            unsigned sig_num:7;
            unsigned core_dmp:1;
            unsigned exit_num:8;
    }parts;
}LE_Wait_Status;

int counter = 0, counter_2G = 0;

void sig_handler(int signal){

    printf("\nCHILD PROG: Awake in handler - You Got Me With Signal\n\
    Number %d after spinning for more \n\
    than %d %s loop iterations\n", signal,
    ((counter_2G)?counter_2G*2:counter),((counter_2G)?"Billion":"\0"));

    printf("CHILD PROG: Now beginning to exec target program, goodbye\n");
    fflush(stdout);
    execl("./a1", "a1", NULL);
    perror("if you see this we failed exec, bad news, goodbye");
    exit(3);
}

int main(void){
    pid_t           pid, ppid;
    int             ruid, rgid, euid, egid;
    int             priority;
    char            read_buf[10];
    sigset_t        mask, proc_mask;
    struct          sigaction new_act;
    int             synch_pipe[2];
    LE_Wait_Status  exit_union;

    if(pipe(synch_pipe) == -1){
        perror("failed in Parent pipe creation:");
        exit(7);
    }

    printf("\nThis is the Parent process report:\n");
    pid     = getpid();
    ppid    = getppid();
    ruid    = getuid();
    euid    = geteuid();
    rgid    = getgid();
    egid    = getegid();
    if((priority = getpriority(PRIO_PROCESS, 0)) == -1){
        perror("getpriority failed ");
        exit(2);
    }
    
    printf("\nPARENT PROG:  Process ID is:\t\t%d\n\
PARENT PROG:  Process parent ID is:\t%d\n\
PARENT PROG:  Real UID is:\t\t%d\n\
PARENT PROG:  Real GID is:\t\t%d\n\
PARENT PROG:  Effective UID is:\t\t%d\n\
PARENT PROG:  Effective GID is:\t\t%d\n\
PARENT PROG:  Process priority is:\t%d\n",
	pid, ppid, ruid, rgid, euid, egid, priority);

    printf("\nPARENT PROG: will now create child, read pipe,\n \
    signal child, and wait for obituary from child\n");

    switch(pid = fork()){
        case -1:    perror("Parent cannot create the child, goodbye:");
                    exit(4);

        default:    printf("\nPARENT PROG: created Child with %d PID\n", pid);
                    break;

        case 0:     // Child's code
                    sigemptyset(&mask);
                    new_act.sa_mask = mask;
                    new_act.sa_handler = sig_handler;
                    new_act.sa_flags = 0;

                    if(sigaction(SIGTERM, &new_act, NULL) == -1){
                        perror("failed in sigaction:");
                        exit(2);
                    }

                    printf("\nThis is the Child process report:\n");
                    pid     = getpid();
                    ppid    = getppid();
                    ruid    = getuid();
                    euid    = geteuid();
                    rgid    = getgid();
                    egid    = getegid();
                    if((priority = getpriority(PRIO_PROCESS, 0)) == -1){
                        perror("getpriority failed ");
                        exit(2);
                    }

printf("\nCHILD PROG:  Process ID is:\t\t%d\n\
CHILD PROG:  Process parent ID is:\t%d\n\
CHILD PROG:  Real UID is:\t\t%d\n\
CHILD PROG:  Real GID is:\t\t%d\n\
CHILD PROG:  Effective UID is:\t\t%d\n\
CHILD PROG:  Effective GID is:\t\t%d\n\
CHILD PROG:  Process priority is:\t%d\n",
pid, ppid, ruid, rgid, euid, egid, priority);

                    printf("\nCHILD PROG: about to write pipe and go to endless loop\n");
                    fflush(stdout);
                    if(write(synch_pipe[1], "x", 1) == -1){
                        perror("write pipe failed ");
                        exit(2);
                    }

                    while (1){
                        counter ++;
                        if (counter < 0){
                                counter = 0;
                                counter_2G++;
                        }
                    }
        } //switch and child end

        if(read(synch_pipe[0], read_buf, 1) == -1){
            perror("read pipe failed ");
            exit(2);
        }
        if(kill(pid, SIGTERM) == -1){
            perror("kill failed ");
            exit(2);
        }
        printf("\nPARENT PROG: read pipe and sent SIGTERM, now wait exit\n");
        //sleep(120);
        if((pid = wait(&exit_union.exit_status)) == -1){
            perror("Wait failed in Parent, goodbye:");
            exit(2);
        }

        if (WIFEXITED(exit_union.exit_status)){
             printf("\nPARENT PROG: Child %d exited with exit code %d, goodbye \n", pid, WEXITSTATUS(exit_union.exit_status));
        }

        exit(0);
}

