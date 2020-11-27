/*-------------------------------------------------------------------------*
 *---									---*
 *---		counter.c						---*
 *---									---*
 *---	    To get a number of seconds and a signaling probability from	---*
 *---	the command line, to launch a child process to signal this one	---*
 *---	with the given probability, and to count how many signals were	---*
 *---	received within the given number of seconds.			---*
 *---									---*
 *---	----	----	----	----	----	----	----	----	---*
 *---									---*
 *---	Version 1a					Joseph Phillips	---*
 *---									---*
 *-------------------------------------------------------------------------*/

#include	<stdlib.h>
#include	<stdio.h>
#include	<string.h>	// memset()
#include	<signal.h>	// sigaction()
#include	<unistd.h>	// fork(), execl()
#include	<arpa/inet.h>	// htonl()


#define		SIGNALER_PROGNAME		"./signaler"
#define        BUFFER_LEN        4096

//  YOUR CODE HERE
int shouldRun = 1;
int count = 0;


void		sigAlarmHandler	(int		sigInt
                             )
{
    //  YOUR CODE HERE
    shouldRun = 0;
}


void		sigUser1Handler	(int		sigInt
                             )
{
    //  YOUR CODE HERE
    count++;
}


int		main		(int		argc,
                     char*		argv[]
                     )
{
    int		numSecs;
    float		sigProb;
    char*		cPtr;
    int		errorResponse	= htonl(-1);
    
    //  YOUR CODE HERE
    if (argc > 1) {
        char    buffer1[BUFFER_LEN];
        char    buffer2[BUFFER_LEN];
        snprintf(buffer1,BUFFER_LEN,"%d",numSecs);
        snprintf(buffer2,BUFFER_LEN,"%g",sigProb);
        
        do {
            numSecs = strtol(argv[1],NULL,10);
        }
        while ((numSecs < 1) || (numSecs > 60));
        
        do {
            sigProb = strtod(argv[2],NULL);
        }
        while ((sigProb < 0.0) || (sigProb > 1.0));
    }
    
    int pid = fork();
    
    if (pid == 0) {
        char    buffer[BUFFER_LEN];
        snprintf(buffer,BUFFER_LEN,"%g",sigProb);
        execl("./signaler","./signaler",buffer,NULL);
    }
    
    alarm(numSecs);
    
    while (shouldRun) {
        sleep(1);
    }
    if (pid > 0) {
        kill(pid,SIGINT);
        wait(NULL);
    }
    
    //Convert count to network endian
    count = ntohl(count);
    
    if ((pid < 0) || (argc <= 1)) {
        write(STDOUT_FILENO,&errorResponse,sizeof(errorResponse));
        exit(EXIT_FAILURE);
    }
    
    return(EXIT_SUCCESS);
}

