// prof Moloney's code
#include<fcntl.h>
#include<sys/stat.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<errno.h>
#include<string.h>
#include<unistd.h>
#include<signal.h>


void sig_handler(int signal){
    printf("\nSIGPIPE recieved: Someone shot the child ...\n");
    printf("nowhere to send data ... parent exiting .. goodbye\n\n");
    exit(1);
}

int main(int argc, char* argv[]){
    int pfdout[2], pfdin[2], fd, nread;
    int gcount = 0;
    char buf[81];
    struct sigaction new_sig_state;
    sigset_t mask;
    FILE *in_pipe_read_end;
    int grep_num1, grep_num2, grep_num3;

    sigemptyset(&mask);
    new_sig_state.sa_mask = mask;
    new_sig_state.sa_handler = sig_handler;
    new_sig_state.sa_flags = 0;

    if(sigaction(SIGPIPE, &new_sig_state, NULL) == -1){
        perror("failed in sigaction: ");
        exit(2);
    }

    if(pipe(pfdout) == -1 || pipe(pfdin) == -1){
        perror("pipe");
        exit(1);
    }
    // end of prof Moloney's code

    switch(fork()){
        case -1:
            perror("fork");
            exit(1);

        default:
            break; // break parent out

        case 0:
            if (close(0) == -1){
                perror("close");
                exit(1);
            }
            if (dup(pfdout[0]) != 0){
                perror("dup");
                exit(1);
            }
            if (close(1) == -1) {
                perror("close");
                exit(1);
            }
            if (dup(pfdin[1]) != 1){
                perror("dup");
                exit(1);
            }

            // close channels
            if (close(pfdout[0]) == -1){
                perror("close");
                exit(1);
            }
            if (close(pfdout[1]) == -1){
                perror("close");
                exit(1);
            }
            if (close(pfdin[0]) == -1){
                perror("close");
                exit(1);
            }
            if (close(pfdin[1]) == -1){
                perror("close");
                exit(1);
            }

            execlp("grep", "grep", "123", NULL); //grep call

            perror("execlp");
            exit(1);
        
    }//switch end

    if(close(pfdout[0]) == -1 || close(pfdin[1]) == -1){
        perror("close");
        exit(1);
    }

    //fd = open("cs308a2_grep_data_1", O_RDONLY, 0);
    fd = open(argv[1], O_RDONLY, 0);

// prof Maloney's code
while ((nread = read(fd, buf, 80))){
    if(write(pfdout[1], buf, nread) == -1){
        perror("write");
        exit(1);
    }
}

write(1, "\nALL DATA SENT\n\n", 16);
if(close(pfdout[1]) == -1){
    perror("close");
    exit(1);
}
    in_pipe_read_end = fdopen(pfdin[0], "r");

// output results
while(fscanf (in_pipe_read_end, "%d %d %d\n", &grep_num1, &grep_num2, &grep_num3 )!= EOF)
{
            printf("%d %d %d\n", grep_num1, grep_num2, grep_num3);
            gcount++;
}
printf("\nTotal matching line count: %4d\n", gcount);
return 0;
}
