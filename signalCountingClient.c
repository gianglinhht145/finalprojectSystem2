/*-------------------------------------------------------------------------*
 *---									---*
 *---		signalCountingClient.c					---*
 *---									---*
 *---	    This file defines a C program that gets commands from the	---*
 *---	user, and sends them to a server via a socket, waits for a	---*
 *---	reply, and outputs the response to the user.			---*
 *---									---*
 *---	----	----	----	----	----	----	----	----	---*
 *---									---*
 *---	Version 1a					Joseph Phillips	---*
 *---									---*
 *-------------------------------------------------------------------------*/

//	Compile with:
//	$ gcc columnClient.c -o columnClient

//---		Header file inclusion					---//

#include	"clientServer.h"


//---		Definition of constants:				---//

#define	DEFAULT_HOSTNAME	"localhost"



//---		Definition of functions:				---//

//  PURPOSE:  To ask the user for the name and the port of the server.  The
//	server name is returned in 'url' up to length 'urlLen'.  The port
//	number is returned in '*portPtr'.  No return value.
void	obtainUrlAndPort	(int		urlLen,
				 char*		url,
				 int*		portPtr
				)
{
  //  I.  Application validity check:
  if  ( (url == NULL)  ||  (portPtr == NULL) )
  {
    fprintf(stderr,"BUG: NULL ptr to obtainUrlAndPort()\n");
    exit(EXIT_FAILURE);
  }

  if   (urlLen <= 1)
  {
    fprintf(stderr,"BUG: Bad string length to obtainUrlAndPort()\n");
    exit(EXIT_FAILURE);
  }

  //  II.  Get server name and port number:
  //  II.A.  Get server name:
  printf("Machine name [%s]? ",DEFAULT_HOSTNAME);
  fgets(url,urlLen,stdin);

  char*	cPtr	= strchr(url,'\n');

  if  (cPtr != NULL)
    *cPtr = '\0';

  if  (url[0] == '\0')
    strncpy(url,DEFAULT_HOSTNAME,urlLen);

  //  II.B.  Get port numbe:
  char	buffer[BUFFER_LEN];

  printf("Port number? ");
  fgets(buffer,BUFFER_LEN,stdin);

  *portPtr = strtol(buffer,NULL,10);

  //  III.  Finished:
}


//  PURPOSE:  To attempt to connect to the server named 'url' at port 'port'.
//	Returns file-descriptor of socket if successful, or '-1' otherwise.
int	attemptToConnectToServer	(const char*	url,
					 int		port
					)
{
  //  I.  Application validity check:
  if  (url == NULL)
  {
    fprintf(stderr,"BUG: NULL ptr to attemptToConnectToServer()\n");
    exit(EXIT_FAILURE);
  }


  //  II.  Attempt to connect to server:
  //  II.A.  Create a socket:
  int socketDescriptor = socket(AF_INET, // AF_INET domain
				SOCK_STREAM, // Reliable TCP
				0);

  //  II.B.  Ask DNS about 'url':
  struct addrinfo* hostPtr;
  int status = getaddrinfo(url,NULL,NULL,&hostPtr);

  if (status != 0)
  {
    fprintf(stderr,gai_strerror(status));
    return(-1);
  }

  //  II.C.  Attempt to connect to server:
  struct sockaddr_in server;
  // Clear server datastruct
  memset(&server, 0, sizeof(server));

  // Use TCP/IP
  server.sin_family = AF_INET;

  // Tell port # in proper network byte order
  server.sin_port = htons(port);

  // Copy connectivity info from info on server ("hostPtr")
  server.sin_addr.s_addr =
	((struct sockaddr_in*)hostPtr->ai_addr)->sin_addr.s_addr;

  status = connect(socketDescriptor,(struct sockaddr*)&server,sizeof(server));

  if  (status < 0)
  {
    fprintf(stderr,"Could not connect %s:%d\n",url,port);
    return(-1);
  }

  freeaddrinfo(hostPtr);

  //  III.  Finished:
  return(socketDescriptor);
}


//  PURPOSE:  To do the work of the application.  Gets letter from user, sends
//	it to server over file-descriptor 'socketFd', and prints returned text.
//	No return value.
void		communicateWithServer
				(int		socketFd
				)
{
  //  I.  Application validity check:

  //  II.  Do work of application:
  //  II.A.  Get letter from user:
  char	buffer[BUFFER_LEN];
  int	numBytes;
  int	numSecs;
  int	count;
  float	sigProb;
  char*	cPtr;

  do
  {
    printf("Please enter the number of seconds (1-%d): ",MAX_NUM_SECONDS);
    fgets(buffer,BUFFER_LEN,stdin);
    numSecs	= strtol(buffer,&cPtr,0);
  }
  while  ( ((*cPtr != '\0') && (*cPtr != '\n'))	||
	   (numSecs < 1)			||
	   (numSecs > MAX_NUM_SECONDS)
	 );

  do
  {
    printf("Please enter the prob of sending a signal after each second (0-1): "
	  );
    fgets(buffer,BUFFER_LEN,stdin);
    sigProb	= strtod(buffer,&cPtr);
  }
  while  ( ((*cPtr != '\0') && (*cPtr != '\n'))	||
	   (sigProb < 0.0)			||
	   (sigProb > 1.0)
	 );

  int*	intBufferPtr	= (int*)buffer;

  intBufferPtr[0]	= htonl(numSecs);
  intBufferPtr[1]	= *(int*)&sigProb;
  intBufferPtr[1]	= htonl(intBufferPtr[1]);

  write(socketFd,buffer,2*sizeof(int));
  numBytes = read(socketFd,&count,sizeof(count));

  if  ( (numBytes < 0)  ||  ((count = ntohl(count)) < 0) )
  {
    printf("There was an error.\n");
  }
  else
  {
    printf("There were %d signals sent in %d second(s).\n",count,numSecs);
  }

  //  III.  Finished:
}


//  PURPOSE:  To do the work of the client.  Ignores command line parameters.
//	Returns 'EXIT_SUCCESS' to OS on success or 'EXIT_FAILURE' otherwise.
int	main	()
{
  char		url[BUFFER_LEN];
  int		port;
  int		socketFd;

  obtainUrlAndPort(BUFFER_LEN,url,&port);
  socketFd	= attemptToConnectToServer(url,port);

  if  (socketFd < 0)
    exit(EXIT_FAILURE);

  communicateWithServer(socketFd);
  close(socketFd);
  return(EXIT_SUCCESS);
}
