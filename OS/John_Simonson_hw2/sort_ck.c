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
    int count=0, tcount=0;
    char buf[81], print_buf[100];
    struct sigaction new_sig_state;
    sigset_t mask;
    FILE *in_pipe_read_end;
    char last_name[50], first_name[50];
    int area_code, old_code, prefix, telnum;
    int acode_count = 0, total_count = 0;

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

            execlp("sort", "sort", "-k 3.3n", "-k 1.1", "-k 2.2", NULL);

            perror("execlp");
            exit(1);
        
    }//switch end

    if(close(pfdout[0]) == -1 || close(pfdin[1]) == -1){
        perror("close");
        exit(1);
    }

    fd = open("cs308a2_sort_data", O_RDONLY, 0);

// prof Maloney's code
while ((nread = read(fd, buf, 80))){
    if(write(pfdout[1], buf, nread) == -1){
        perror("write");
        exit(1);
    }

    if((count += nread) < 4096){
        write(1, "*", 1);
    }
    else{
        tcount += count;
        count = 0;
        sprintf(print_buf, "   %d bytes sent\n", tcount);
        write(1, print_buf, strlen(print_buf)+1);
    }
}

write(1, "\nALL DATA SENT\n\n", 16);
if(close(pfdout[1]) == -1){
    perror("close");
    exit(1);
}
    in_pipe_read_end = fdopen(pfdin[0], "r");

fscanf (in_pipe_read_end, "%s %s %d %d %d\n", last_name,
            first_name, &area_code, &prefix, &telnum);
printf("FIRST LINE: %s  %s  %03d  %03d  %04d\n\n", last_name,
            first_name, area_code, prefix, telnum);
acode_count++;
old_code = area_code;

while(fscanf (in_pipe_read_end, "%s %s %d %d %d\n", last_name,
            first_name, &area_code, &prefix, &telnum) != EOF)
{
        if(area_code != old_code){
            printf("area code %d occurs %4d times\n", old_code, acode_count);
            total_count += acode_count;
            acode_count = 1;
            old_code = area_code;
        }
        else{
            acode_count++;
        }
}
printf("area code %d occurs %4d times\n\n", area_code, acode_count);
printf("%d RECORDS PROCESSED ... goodbye\n", total_count);
return 0;
}
