#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <inttypes.h>
#include <pthread.h>
#include "input-handler.h"
#include "server.h"
#include "mine.h"

int thread_count = 0;
pthread_t tid[25];

// Making a global list for all the submitted work
List* work_queue;

void handle_input(int sockfd, char* buffer, List* list)
{
    // Pointing to the same list global list
    work_queue = list;

    if (strncmp(buffer, PING, 4) == 0)
    {
        handle_ping(sockfd);
    }
    else if (strncmp(buffer, ERRO, 4) == 0)
    {
        handle_erro(sockfd);
    }
    else if (strncmp(buffer, OKAY, 4) == 0)
    {
        handle_okay(sockfd);
    }
    else if (strncmp(buffer, PONG, 4) == 0)
    {
        handle_pong(sockfd);
    }
    else if (strncmp(buffer, SOLN, 4) == 0)
    {
        handle_soln(sockfd, buffer);
    }
    else if (strncmp(buffer, WORK, 4) == 0)
    {
        handle_work(sockfd, buffer);
    }
    else if (strncmp(buffer, ABRT, 4) == 0)
    {
        handle_abrt(sockfd);
    }
    else
    {
        //fprintf(stdout, "Handling other %s\n", buffer);
        handle_other(sockfd);
    }
}

void handle_ping(int sockfd)
{
    send_msg((BYTE*)PONG, sockfd);
}

void handle_pong(int sockfd)
{
    BYTE error[40] = {"Can't send PONG to server"};
    send_erro(error, sockfd);
}

void handle_soln(int sockfd, char* buffer)
{
    //TODO: need to check for formatting as well
    //fprintf(stdout, "Starting soln with buffer %s\n", buffer);
    //fflush(stdout);
    struct soln_args in_args;
    char temp[64+1];
    memset(temp, 0, 65);

    strtok(buffer, " ");

    // Extracting difficulty
    strcpy(temp, strtok(NULL, " "));
    in_args.difficulty = strtol(temp, NULL, 16);
    if (strlen(temp) != 8)
    {
        char *som = malloc(2000);
        sprintf(som, "Temp is %s\n", temp); 
        send_erro((BYTE *)som, sockfd);
        fprintf(stdout, "Difficulty\n");
        return;
    }

    // Extracting Seed
    memset(temp, 0, 65);
    strcpy(temp, strtok(NULL, " "));
    temp[strlen(temp)] = '\0';
    if (strlen(temp) != 64)
    {
        send_erro((BYTE*)"Invalid seed", sockfd);
        fprintf(stdout, "Seed\n");
        return;
    }
    for(int i = 62; i >= 0; i-=2)
    {
        // Putting two hexes into one byte
        char str[3];
        memset(str, 0, 3);
        strncpy(str, temp+i, 2);
        str[2] = '\0';
        in_args.seed[i/2] = (strtol(str, NULL, 16)) & 0xFF;
        temp[i] = '\0';
    }

    // Extracting solution
    memset(temp, '\0', 65);
    
    strcpy(temp, strtok(NULL, "\r\n"));
    in_args.solution = strtol(temp, NULL, 16);
    if (strlen(temp) != 16)
    {
        send_erro((BYTE*)"Invalid solution", sockfd);
        fprintf(stdout, "Temp was %s\n", temp);
        fflush(stdout);
        return;
    }

    //TODO: change this to allow for more threads
    //pthread_t tid[2];
    //pthread_create(&tid[0], NULL, (void *)check_sol, &in_args);

    if (check_sol(in_args))
    {
        send_msg((BYTE*)OKAY, sockfd);
    }
    else
    {
        BYTE error[40];
        sprintf((char*)error, "%" PRIx64 " is invalid", in_args.solution);
        send_erro(error, sockfd);
    }

}

void handle_erro(int sockfd)
{
    BYTE error[40];
    sprintf((char*)error, "Can't send ERRO to server");
	send_erro(error, sockfd);
}

void handle_work(int sockfd, char* buffer)
{
    //TODO: need to check for formatting as well
    fprintf(stdout, "Starting work with buffer %s\n", buffer);
    fflush(stdout);
    struct work_args in_args;
    char temp[64+1];
    memset(temp, 0, 64);

    in_args.sockfd = sockfd;

    strtok(buffer, " ");

    strcpy(temp, strtok(NULL, " "));
    in_args.difficulty = strtol(temp, NULL, 16);
    if (strlen(temp) != 8)
    {
        send_erro((BYTE*)"Invalid difficulty", sockfd);
        fprintf(stdout, "Invalid difficulty of %s\n", temp);
        fflush(stdout);
        return;
    }
    
    strcpy(temp, strtok(NULL, " "));
    temp[64] = '\0';
    for(int i = 62; i >= 0; i-=2)
    {
        char str[2];
        strcpy(str, &temp[i]);
        in_args.seed[i/2] = strtol(str, NULL, 16) & 0xFF;
        temp[i] = '\0';
    }

    in_args.start = strtol((strtok(NULL, " ")), NULL, 16);

    in_args.worker_count = strtol((strtok(NULL, " ")), NULL, 16);

    list_add_end(work_queue, buffer);

    /*fprintf(stderr, "Difficulty is %d\n", in_args.difficulty);
    fprintf(stderr, "Seed is ");
    print_uint256(in_args.seed);
    fprintf(stderr, "Start is %" PRId64 "\n", in_args.start);
    fprintf(stderr, "Worker count is %d\n", in_args.worker_count);
	fprintf(stderr, "Socket is %d\n", in_args.sockfd);*/
    // Make a new thread for the work function
    // TODO: Allow for than 2 threads
    pthread_create(&tid[thread_count], NULL, (void *)work, &in_args);
    thread_count++;
    //work(in_args);

    // The work has been done, so it can be removed from the list
    list_remove_start(work_queue);
}

void handle_abrt(int sockfd){}

void handle_okay(int sockfd)
{
    BYTE error[40];
    memset(error, '\0', 40);
    strcpy((char *)error, "Can't send OKAY to server");
    send_erro(error, sockfd);
}

void handle_other(int sockfd)
{
    BYTE error[40];
    memset(error, 0, 40);
    strcpy((char *)error, "Invalid command");
    send_erro(error, sockfd);
}

void send_erro (BYTE error[40], int sockfd)
{
    int i;
	// TODO: remove this magic number
    BYTE concatenated[40+MSG_HEADER];
    memset(concatenated, '\0', 40+MSG_HEADER);
	strcat((char*)concatenated, ERRO);
	strcat((char*)concatenated, ": ");
	strcat((char*)concatenated, (char*)error);
    for (i = strlen((char*)concatenated); i < 40+MSG_HEADER-2; i++)
    {
        concatenated[i] = ' ';
    }
	//strcat((char*)concatenated, "\r\n");
    //concatenated[strlen((char*)concatenated)] = '\0';
    concatenated[42] = '\r';
    concatenated[43] = '\n';
    int n = send(sockfd, concatenated, strlen((char*)concatenated), 0);
    log_to_file((char*)concatenated, SERV_ADR, sockfd);
	if (n != (int)strlen((char*)concatenated))
	{
        fprintf(stdout, "In send erro and found an error %d and %ld\n", n,
                                                strlen((char*)concatenated));
        fflush(stdout);
		perror("ERROR writing to socket");
        log_to_file("ERROR writing to socket", SERV_ADR, sockfd);
        fflush(stdout);
		// TODO: remove this
		// exit(1);
	}
}

void send_msg (BYTE msg[40], int sockfd)
{
    BYTE new_msg[40+MSG_HEADER];
    memset(new_msg, 0, 40+MSG_HEADER);
    strcat((char*)new_msg, (char*)msg);
    strcat((char*)new_msg, "\r\n");
    log_to_file((char*)new_msg, SERV_ADR, sockfd);
	if (send(sockfd, new_msg, strlen((char*)new_msg), 0) !=
												(int)strlen((char*)new_msg))
	{
        fprintf(stdout, "In send msg with %s\n", new_msg);
        fflush(stdout);
		perror("ERROR writing to socket");
        fflush(stdout);
        log_to_file("ERROR writing to socket", SERV_ADR, sockfd);
        // TODO: Not sure about this
		// exit(1);
	}
}
