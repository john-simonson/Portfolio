/*****************************************************************/ /* INCLUDE FILES, 
DEFINES AND DECLARATIONS AS SEEN IN CLASS */ /* THESE PROBABLY BELONG IN A .h FILE */ 
/*****************************************************************/


#define _GNU_SOURCE
#include <sched.h>
#include <utmpx.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <linux/unistd.h>
#include <strings.h>
#include <signal.h>
#include <sys/time.h>
#include <pthread.h>
#include <sys/fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>


#define         NUMFLAVORS       	4
#define         NUMSLOTS        	1850
#define         NUMCONSUMERS     	50
#define		NUMPRODUCERS		30
#define         NUM_DOZEN		2000


struct  donut_ring {
	int     donut_ring_buffers [NUMFLAVORS] [NUMSLOTS];
	int     out_ptr [NUMFLAVORS];
	int		in_ptr [NUMFLAVORS];
	int		serial [NUMFLAVORS];
	int		space_count [NUMFLAVORS];
	int		donut_count [NUMFLAVORS];
};

/**********************************************************************/
/* PRODUCER AND CONSUMER THREAD FUNCTIONS DECLARATIONS                */
/**********************************************************************/

	void    *sig_waiter ( void *arg );
	void	*producer   ( void *arg );
	void	*consumer   ( void *arg );
	void    sig_handler ( int );

/**********************    END HEADER FILE         *******************/
