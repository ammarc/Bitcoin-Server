/*
 *  Project 2 for COMP30023: Computer Systems
 *  at the University of Melbourne
 *  Semester 1, 2017
 *  by: Ammar Ahmed
 *  Username: ammara
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
#include <stdbool.h>
#include "server.h"
#include "mine.h"
#include "input-handler.h"


int main(int argc, char **argv)
{
	int sockfd, newsockfd, portno, clilen;
	char buffer[256];
	char temp[256];
	struct sockaddr_in serv_addr;
	int n, i, j;
	int client_sockets[MAX_CLIENTS], max_sd, activity;
	fd_set readfds;

	for (i = 0; i < MAX_CLIENTS; i++)
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

	//try to specify maximum of 100 pending connections for the master socket 
    if (listen(sockfd, MAX_CLIENTS) < 0)
    {  
        perror("listen");  
        exit(EXIT_FAILURE);  
    }  
	
	//clilen = sizeof(cli_addr);
	clilen = sizeof(serv_addr);

	//TODO: change this to accomodate for windows carriage return
	//TODO: not sure about this loop to keep the server running
	//fprintf(stdout, "Starting infinite loop\n");
	while (true)
	{
		// Setting all the file descriptors to 0
		FD_ZERO(&readfds);

		// Adding our original socket to the set
		FD_SET(sockfd, &readfds);
		max_sd = sockfd;

		for (i = 0; i < MAX_CLIENTS; i++)
		{
			if (client_sockets[i] > 0)
				FD_SET(client_sockets[i], &readfds);

			if (client_sockets[i] > max_sd)
				max_sd = client_sockets[i];
		}

		// This waits for any activity in any of the sockets
		//fprintf(stderr, "Waiting for an activity\n");
		activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
		//fprintf(stderr, "Finished waiting for an activity\n");

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
            for (i = 0; i < MAX_CLIENTS; i++)  
            {  
                //if position is empty 
                if( client_sockets[i] == 0 )  
                {  
                    client_sockets[i] = newsockfd ;  
                    //printf("Adding to list of sockets as %d\n" , i);  
                    break;  
                }  
            }
        }

		//else its some IO operation on some other socket
		//TODO: change this to handle clients
        for (i = 0; i < MAX_CLIENTS; i++)  
        {
            newsockfd = client_sockets[i];
                
            if (FD_ISSET(newsockfd , &readfds))  
            {  
				//fprintf(stderr, "newsockfd is %d\n", newsockfd);
				bzero(buffer,256);
				memset(temp, '\0', 256);
                //Check if it was for closing , and also read the 
                //incoming message 
				//TODO: remove magic number for the size of val read
                if ((n = read(newsockfd, buffer, 256)) == 0)  
                {  
                    //Close the socket and mark as 0 in list for reuse 
					//fprintf(stdout, "Closing socket %d\n", newsockfd);
                    close(newsockfd);
                    client_sockets[i] = 0;  
                }
				else
				{
					/*for (i = 0, j = 0; i < 256; i++, j++)
					{
						if (i != 255 && buffer[i] == '\r' && buffer[i+1] == '\n')
						{
							//fprintf(stdout, "Temp is %s\n", temp);
							//fprintf(stdout, "Buffer is %s\n", buffer);
							handle_input(newsockfd, temp);
							j = 0;
							i++;
							memset(temp, '\0', 256);
							continue;
						}
						temp[j] = buffer[i];
					}*/
					//fprintf(stdout, "Buffer is %s\n", buffer);
					handle_input(newsockfd, buffer);
				}
            }
        }
	}
	/* close socket */
	close(sockfd);
	return 0; 
}
