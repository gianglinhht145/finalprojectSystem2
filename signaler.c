/*-------------------------------------------------------------------------*
 *---									---*
 *---		signaler.c						---*
 *---									---*
 *---	    To get a signaling probability from	the command line, and	---*
 *---	signal the parent process with that probability after each	---*
 *---	second.  Stops when receives SIGINT.				---*
 *---									---*
 *---	----	----	----	----	----	----	----	----	---*
 *---									---*
 *---	Version 1a					Joseph Phillips	---*
 *---									---*
 *-------------------------------------------------------------------------*/

#include	<stdlib.h>
#include	<stdio.h>
#include	<string.h>k		// memset()
#include	<sys/types.h>
#include	<signal.h>


//  YOUR CODE HERE
int shouldRun = 1;
pid_t        pidToSignal    = -1;

void		sigIntHandler	(int		sigInt
				)
{
  //  YOUR CODE HERE
    shouldRun = 0;
}


int		main		(int		argc,
				 char*		argv[]
				)
{
  float		sigProb;
  char*		cPtr;

  //  YOUR CODE HERE
    if (argc > 1) {
        
        char    buffer[BUFFER_LEN];
        snprintf(buffer,BUFFER_LEN,"%g",sigProb);
    }
    
    srand(getpid());
    
    pidToSignal    = getppid();
    while  (shouldRun)
    {
      sleep(1);

      if (drand48()*sigProb)
          //(((float)rand()/(float)(RAND_MAX))*sigProb)
      {
        //  Send SIGUSR1 to parent process
          kill(pidToSignal,SIGUSR1);
      }
    }


  return(EXIT_SUCCESS);
}
