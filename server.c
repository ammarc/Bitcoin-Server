/* A simple server in the internet domain using TCP
The port number is passed as an argument 


 To compile: gcc server.c -o server 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <inttypes.h>
#include "server.h"
#include "mine.h"
#include "input-handler.h"


int main(int argc, char **argv)
{
	int sockfd, newsockfd, portno, clilen;
	char buffer[256];
	struct sockaddr_in serv_addr;//, cli_addr;
	int n, i;
	int client_sockets[10], max_sd, activity;
	fd_set readfds;

	for (i = 0; i < 10; i++)
		client_sockets[i] = 0;

	if (argc < 2) 
	{
		fprintf(stderr,"ERROR, no port provided\n");
		exit(1);
	}

	 /* Create TCP socket */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0) 
	{
		perror("ERROR opening socket");
		exit(1);
	}

	
	bzero((char *) &serv_addr, sizeof(serv_addr));

	portno = atoi(argv[1]);
	
	/* Create address we're going to listen on (given port number)
	 - converted to network byte order & any IP address for 
	 this machine */
	
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);  // store in machine-neutral format

	 /* Bind address to the socket */
	
	if (bind(sockfd, (struct sockaddr *) &serv_addr,
			sizeof(serv_addr)) < 0) 
	{
		perror("ERROR on binding");
		exit(1);
	}

	//try to specify maximum of 3 pending connections for the master socket 
    if (listen(sockfd, 10) < 0)  
    {  
        perror("listen");  
        exit(EXIT_FAILURE);  
    }  
	
	//clilen = sizeof(cli_addr);
	clilen = sizeof(serv_addr);

	//TODO: change this to accomodate for windows carriage return
	//TODO: not sure about this loop to keep the server running
	while (1)
	{
		// Setting all the file descriptors to 0
		FD_ZERO(&readfds);

		// Adding our original socket to the set
		FD_SET(sockfd, &readfds);
		max_sd = sockfd;

		for (i = 0; i < 10; i++)
		{
			if (client_sockets[i] > 0)
				FD_SET(client_sockets[i], &readfds);

			if (client_sockets[i] > max_sd)
				max_sd = client_sockets[i];
		}

		// This waits for any activity in any of the sockets
		fprintf(stderr, "Waiting for an activity\n");
		activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
		fprintf(stderr, "Finished waiting for an activity\n");

		if ((activity < 0) && (errno!=EINTR))  
            printf("select error");

		//If something happened on the master socket , 
        //then its an incoming connection 
        if (FD_ISSET(sockfd, &readfds))  
        {  
			/* Accept a connection - block until a connection is ready to
			be accepted. Get back a new file descriptor to communicate on. */
            if ((newsockfd = accept(sockfd, 
                    (struct sockaddr *)&serv_addr, (socklen_t*)&clilen)) < 0)  
            {  
                perror("accept");  
                exit(EXIT_FAILURE);  
            }  
            
			bzero(buffer,256);
			/* Read characters from the connection,
				then process */
		
            //add new socket to array of sockets 
            for (i = 0; i < 10; i++)  
            {  
                //if position is empty 
                if( client_sockets[i] == 0 )  
                {  
                    client_sockets[i] = newsockfd ;  
                    printf("Adding to list of sockets as %d\n" , i);  
                    break;  
                }  
            }
        }

		//else its some IO operation on some other socket
		//TODO: change this to handle clients
        for (i = 0; i < 10; i++)  
        {
            newsockfd = client_sockets[i];  
                
            if (FD_ISSET(newsockfd , &readfds))  
            {  
				bzero(buffer,256);
                //Check if it was for closing , and also read the 
                //incoming message 
				//TODO: remove magic number for the size of val read
                if ((n = read(newsockfd, buffer, 255)) == 0)  
                {  
                    //Close the socket and mark as 0 in list for reuse 
					fprintf(stderr, "Closing socket\n");
                    close(newsockfd);
                    client_sockets[i] = 0;  
                }

				if (strncmp(buffer, PING, 4) == 0)
					handle_ping(newsockfd);
				
				else if (strncmp(buffer, ERRO, 4) == 0)
					handle_erro(newsockfd);

				else if (strncmp(buffer, OKAY, 4) == 0)
					handle_okay(newsockfd);

				else if (strncmp(buffer, PONG, 4) == 0)
					handle_pong(newsockfd);

				else if (strncmp(buffer, SOLN, 4) == 0)
					handle_soln(newsockfd, buffer);

				else if (strncmp(buffer, WORK, 4) == 0)
					handle_work(newsockfd, buffer);

				else if (strncmp(buffer, ABRT, 4) == 0)
					handle_abrt(newsockfd);

				else
					handle_other(newsockfd);
            }
        }
	}
	/* close socket */
	close(sockfd);
	return 0; 
}