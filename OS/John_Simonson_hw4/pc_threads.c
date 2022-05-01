#include"pc_threads.h"

struct donut_ring 	the_store;
pthread_mutex_t		prod [NUMFLAVORS];
pthread_mutex_t		cons [NUMFLAVORS];
pthread_cond_t		prod_cond [NUMFLAVORS];
pthread_cond_t		cons_cond [NUMFLAVORS];
pthread_t           thread_id[(NUMCONSUMERS + NUMPRODUCERS)], sig_wait_id;

//int thread_count = 0; //for generating files

void *sig_waiter ( void *arg ) {
        sigset_t        sigterm_signal;
        int             signo;
        sigemptyset     ( &sigterm_signal );
        sigaddset       ( &sigterm_signal, SIGTERM);
        sigaddset       ( &sigterm_signal, SIGINT);

        if (sigwait ( &sigterm_signal, & signo) != 0 ) {
                printf ( "\n sigwait() failed, exiting \n");
                exit(2);
        }

        printf("Process exits on SIGNAL %d\n\n", signo );
        exit(1);
        return NULL;
}

void sig_handler ( int sig ) {
        pthread_t       signaled_thread_id;
        int             i, thread_index;

        signaled_thread_id = pthread_self ( );

        for (i = 0; i < ( NUMCONSUMERS ); i++){
                if ( signaled_thread_id == thread_id[i]){
                        thread_index = i + 1;
                        break;
                }
        }
        printf ( "\nThread %d took signal # %d, PROCESS HALT\n",
                                thread_index, sig );
        exit(1);
}

int main(int argc, char* argv[]){
        int             i, j, k, l, nsigs;
        struct          timeval	randtime, first_time, last_time;
	int		arg_array[NUMCONSUMERS];
        struct          sched_param sched_struct;
        sigset_t        all_signals;
        pthread_attr_t  thread_attr;
        struct          sigaction new_act;       
        int sigs[] =    {SIGPIPE, SIGBUS, SIGSEGV, SIGFPE};

        // starter code
        /**********************************************************************/
        /* INITIAL TIMESTAMP VALUE FOR PERFORMANCE MEASURE                    */
        /**********************************************************************/        

         gettimeofday (&first_time, (struct timezone *) 0 );
        for ( i = 0; i < NUMCONSUMERS; i++ ) {
		arg_array [i] = i+1;	/** SET ARRAY OF ARGUMENT VALUES **/
        }

        /**********************************************************************/
        /* GENERAL PTHREAD MUTEX AND CONDITION INIT AND GLOBAL INIT           */
        /**********************************************************************/

	for ( i = 0; i < NUMFLAVORS; i++ ) {
		pthread_mutex_init ( &prod [i], NULL );
		pthread_mutex_init ( &cons [i], NULL );
		pthread_cond_init ( &prod_cond [i],  NULL );
		pthread_cond_init ( &cons_cond [i],  NULL );
		the_store.out_ptr [i]		= 0;
		the_store.in_ptr [i]		= 0;
		the_store.serial [i]		= 0;
		the_store.space_count [i]		= NUMSLOTS;
		the_store.donut_count [i]		= 0;
	}

       // from code prof. moloney showed in class 

        sigfillset(&all_signals);
        nsigs = sizeof(sigs)/sizeof(int);

        for(i=0; i < nsigs; i++)
            sigdelset(&all_signals, sigs[i]);

        sigprocmask(SIG_BLOCK, &all_signals, NULL);

        for(i=0; i< nsigs; i++){
            new_act.sa_handler         = sig_handler;
            new_act.sa_mask            = all_signals;
            new_act.sa_flags           = 0;
            if(sigaction(sigs[i], &new_act, NULL) == -1){
                    perror("can't set signals: ");
                    exit(1);
            }        
         } 

	printf ( "just before threads created\n" );

        if((errno = pthread_create(&sig_wait_id, NULL, sig_waiter, NULL)) != 0){
                perror("pthread_create failed ");
                exit(3);
        }

        pthread_attr_init(&thread_attr);
        pthread_attr_setinheritsched(&thread_attr, PTHREAD_INHERIT_SCHED);

        #ifdef GLOBAL
                sched_struct.sched_priority = sched_get_priority_max(SCHED_OTHER);
                pthread_attr_setinheritsched(&thread_attr, PTHREAD_EXPLICIT_SCHED);
                pthread_attr_setschedpolicy(&thread_attr, SCHED_OTHER);
                pthread_attr_setschedparam(&thread_attr, &sched_struct);
                pthread_attr_setscope(&thread_attr, PTHREAD_SCOPE_SYSTEM);
        #endif  
        // end of class code

        // more starter code
	for ( i = 0; i < NUMCONSUMERS ; i++, j++ ) {
	  if ( pthread_create ( &thread_id [i], NULL,
		    consumer, ( void * )&arg_array [i]) != 0 ){
		printf ( "pthread_create failed" );
		exit ( 3 );
	  }
	}
 
	for ( ; i < NUMPRODUCERS + NUMCONSUMERS; i++ ) { 
    	  if ( pthread_create (&thread_id[i], NULL,
					  producer, NULL ) != 0 ) {
		printf ( "pthread_create failed " );
		exit ( 3 );
	  }
	}

	printf ( "just after threads created\n" );

       /*********************************************************************/
        /* WAIT FOR ALL CONSUMERS TO FINISH, SIGNAL WAITER WILL              */
        /* NOT FINISH UNLESS A SIGTERM ARRIVES AND WILL THEN EXIT            */
        /* THE ENTIRE PROCESS....OTHERWISE MAIN THREAD WILL EXIT             */
        /* THE PROCESS WHEN ALL CONSUMERS ARE FINISHED                       */
        /*********************************************************************/

	for ( i = 0; i < NUMCONSUMERS; i++ ) 
             		pthread_join ( thread_id [i], NULL );  


        /*****************************************************************/
        /* GET FINAL TIMESTAMP, CALCULATE ELAPSED SEC AND USEC           */
        /*****************************************************************/


        gettimeofday (&last_time, ( struct timezone * ) 0 );
        if ( ( i = last_time.tv_sec - first_time.tv_sec) == 0 )
			j = last_time.tv_usec - first_time.tv_usec;
        else{
		if ( last_time.tv_usec - first_time.tv_usec < 0 ) {
				i--;
				j = 1000000 + 
				   ( last_time.tv_usec - first_time.tv_usec );
            	} else {
				j = last_time.tv_usec - first_time.tv_usec; }
        }
	printf ( "Elapsed consumer time is %d sec and %d usec\n", i, j );

	printf ( "\n\n ALL CONSUMERS FINISHED, KILLING  PROCESS\n\n" );
	exit ( 0 );
}


void *producer(void *arg)
{
        int     i, j, len;
        float   etime;
        char    msg[100];
        ushort  xsub1[3];
        struct  timeval randtime;
        int     cn;
        char    file_name[10] = "prod";

        gettimeofday((&randtime), (struct timezone *)0);
        xsub1[0] = (ushort)randtime.tv_usec;
        xsub1[1] = (ushort)(randtime.tv_usec >> 16);
        xsub1[2] = (ushort)(getpid());

        while(1){
                j=nrand48(xsub1) & 3;
                pthread_mutex_lock(&prod[j]);
                while(the_store.space_count[j] == 0){
                        pthread_cond_wait(&prod_cond[j], &prod[j]);
                }
                the_store.space_count[j]--;
                the_store.donut_ring_buffers[j][the_store.in_ptr[j]] = the_store.serial[j];

                the_store.in_ptr[j] = (the_store.in_ptr[j]+1) % NUMSLOTS;
                the_store.serial[j]++;
                pthread_mutex_unlock(&prod[j]);

                pthread_mutex_lock(&cons[j]);
                the_store.donut_count[j]++;
                pthread_mutex_unlock(&cons[j]);

                pthread_cond_signal(&cons_cond[j]);
        }
        return NULL;
}

void    *consumer ( void *arg )
{
        int             extracted_donut;
        int             i, j, id, len;
        //char            msg[300], lbuf[300];
        ushort          xsub1[3];
        struct          timeval randtime;
        char            number[2] = {'\060', '\n'};
        char            file_name[10] = "cons";
        char            thread_number[5];
        FILE*           cn;
        int             doz[12][4];
        int             d[4], m, n;
        time_t          t;
        struct          tm * tp;
        float           ms;
        int             ims;

        id = *(int *)arg;

        gettimeofday(&randtime, (struct timezone *)0);
        xsub1[0] = (ushort)randtime.tv_usec;
        xsub1[1] = (ushort)(randtime.tv_usec >> 16);
        xsub1[2] = (ushort)(syscall(SYS_gettid));

#ifdef DEBUG
        itoa(pthread_self(), thread_number);

        strcat(file_name, thread_number);

        if((cn = open(file_name, O_WRONLY | O_CREAT, 0666)) == -1){
                perror("failed to open producer log ");
        }
#endif

        if((id%10) == 0){
            sprintf(file_name, "cons%d", id);
            if((cn = fopen(file_name, "w+")) == NULL){
                    perror("failed to open consumer output file ");
            }
        }

        for(i=0; i < NUM_DOZEN; i++){
                for(m = 0; m < 12; ++m)
                 for(n = 0; n < 4; ++n) doz[m][n] = 0;
                for(n = 0; n < 4; ++n) d[n] = 0;

                t = time(NULL);
                tp = localtime(&t);
                gettimeofday(&randtime, NULL);

                ms = (float)randtime.tv_usec/1000000;

                ims = (int)(ms*1000);

                if((i < 10) && ((id%10) == 0)){
                        fprintf(cn, "consumer PID %ld     time: %d:%d:%d:%d     dozen number: %d  \n\n",
                                syscall(SYS_gettid), tp->tm_hour, tp->tm_min, tp->tm_sec, ims, i+1);
                }

                for(m=0; m<12; m++){
                        
                        j=nrand48(xsub1) & 3;

                #ifdef DEBUG
                        number[0] = j + 060;
                        write(cn, number, 2);
                #endif

                pthread_mutex_lock(&cons[j]);
                while(the_store.donut_count[j] == 0){
                        pthread_cond_wait(&cons_cond[j], &cons[j]);
                }
                the_store.donut_count[j]--;

                extracted_donut = the_store.donut_ring_buffers[j][the_store.out_ptr[j]];

                the_store.out_ptr[j] = (the_store.out_ptr[j]+1) % NUMSLOTS;
                pthread_mutex_unlock(&cons[j]);

                pthread_mutex_lock(&prod[j]);
                the_store.space_count[j]++;
                pthread_mutex_unlock(&prod[j]);

                pthread_cond_signal(&prod_cond[j]);

                doz[d[j]][j] = extracted_donut;
                d[j] = d[j]+1;
                }// end getting one dozen

                if((i<10) && ((id%10) == 0)){
                        fprintf(cn, "Jelly\t\tCoconut\t\tPlain\t\tGlazed\n");

                        for(m=0; m < 12; ++m){
                                (doz[m][0] == 0)?fprintf(cn, "\t\t"):fprintf(cn, "%d\t\t", doz[m][0]);
                                (doz[m][1] == 0)?fprintf(cn, "\t\t"):fprintf(cn, "%d\t\t", doz[m][1]);
                                (doz[m][2] == 0)?fprintf(cn, "\t\t"):fprintf(cn, "%d\t\t", doz[m][2]);
                                (doz[m][3] == 0)?fprintf(cn, "\t\t\n"):fprintf(cn, "%d\n", doz[m][3]);
                                if((doz[m][0] == 0) && (doz[m][1] == 0) &&
                                    (doz[m][2] == 0) && (doz[m][3] == 0))break;
                        }
                }
        usleep(100);
        }
        return NULL;
}//end consumer