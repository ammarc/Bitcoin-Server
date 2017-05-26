/*  Project 2 for COMP30023: Computer Systems
 *  at the University of Melbourne
 *  Semester 1, 2017
 *  by: Ammar Ahmed
 *  Username: ammara
 */

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


void handle_input(int sockfd, char* buffer, List* work_queue)
{
    //fprintf(stdout, "Handling input %s\n", buffer);
    //fflush(stdout);

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
        handle_abrt(sockfd, work_queue, true);
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
    // Checking the length of SOLN
    if (strlen(buffer) < SOLN_LEN)
    {
        send_erro((BYTE*)"SOLN message too short", sockfd);
        return;
    }

    struct soln_args in_args;
    char temp[64+1];
    memset(temp, 0, 65);
    uint256_init(in_args.seed);

    strtok(buffer, " ");

    // Extracting difficulty
    strcpy(temp, strtok(NULL, " "));
    in_args.difficulty = strtol(temp, NULL, 16);
    if (strlen(temp) != 8)
    {
        send_erro((BYTE *)"Invalid difficulty", sockfd);
        return;
    }

    // Extracting Seed
    memset(temp, 0, 65);
    strcpy(temp, strtok(NULL, " "));
    temp[strlen(temp)] = '\0';
    if (strlen(temp) != 64)
    {
        send_erro((BYTE*)"Invalid seed", sockfd);
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
    in_args.solution = strtoull(temp, NULL, 16);
    if (strlen(temp) != 16)
    {
        send_erro((BYTE*)"Invalid solution", sockfd);
        return;
    }

    //TODO: change this to allow for more threads
    //pthread_t tid[2];
    //pthread_create(&tid[0], NULL, (void *)check_sol, &in_args);

    if (check_sol(in_args, NULL))
    {
        send_msg((BYTE*)OKAY, sockfd);
    }
    else
    {
        BYTE error[40];
        sprintf((char*)error, "%" PRIx64 " is invalid solution"
                                                        , in_args.solution);
        send_erro(error, sockfd);
    }
    //fprintf(stdout, "------------------End Soln---------------\n");
    //fflush(stdout);
}

void handle_erro(int sockfd)
{
    BYTE error[40];
    sprintf((char*)error, "Can't send ERRO to server");
	send_erro(error, sockfd);
}

void handle_work(int sockfd, char* buffer)
{
    // Checking the length of WORK
    //fprintf(stdout, "Starting work with buffer %s\n", buffer);
    fflush(stdout);
    if (strlen(buffer) < WORK_LEN)
    {
        send_erro((BYTE*)"WORK message too short", sockfd);
        return;
    }


    //TODO: need to check for formatting as well
    //fprintf(stdout, "Starting work with buffer %s\n", buffer);
    //fflush(stdout);
    //list_add_end(work_queue, buffer);

    //buffer = list_remove_start(work_queue);

    // TODO: possible error in this line:
    struct work_args *in_args = malloc(sizeof(struct work_args));

    char temp[64+1];
    memset(temp, 0, 64);

    in_args->sockfd = sockfd;

    strtok(buffer, " ");

    strcpy(temp, strtok(NULL, " "));
    in_args->difficulty = strtol(temp, NULL, 16);
    if (strlen(temp) != 8)
    {
        send_erro((BYTE*)"Invalid difficulty", sockfd);
        //fprintf(stdout, "Invalid difficulty of %s\n", temp);
        //fflush(stdout);
        return;
    }

    
    //strcpy(temp, strtok(NULL, " "));
    //temp[64] = '\0';
    uint256_init(in_args->seed);
    // Extracting Seed
    memset(temp, 0, 65);
    strcpy(temp, strtok(NULL, " "));
    temp[strlen(temp)] = '\0';
    if (strlen(temp) != 64)
    {
        send_erro((BYTE*)"Invalid seed", sockfd);
        //fprintf(stdout, "Seed\n");
        return;
    }
    for(int i = 62; i >= 0; i-=2)
    {
        // Putting two hexes into one byte
        char str[3];
        memset(str, 0, 3);
        strncpy(str, temp+i, 2);
        str[2] = '\0';
        in_args->seed[i/2] = (strtol(str, NULL, 16)) & 0xFF;
        temp[i] = '\0';
    }

    // Extracting solution
    memset(temp, '\0', 65);
    
    strcpy(temp, strtok(NULL, " "));
    if (strlen(temp) != 16)
    {
        send_erro((BYTE*)"Invalid start", sockfd);
        //fprintf(stdout, "Temp was %s\n", temp);
        //fflush(stdout);
        return;
    }
    in_args->start = strtoull(temp, NULL, 16);
    //fprintf(stdout, "Start is read as %" PRIx64 "\n", in_args->start);
    //fflush(stdout);
    // Extracting worker count
    memset(temp, '\0', 65);
    
    strcpy(temp, strtok(NULL, "\r\n"));
    in_args->worker_count = strtol(temp, NULL, 16);
    if (strlen(temp) != 2)
    {
        send_erro((BYTE*)"Invalid worker count", sockfd);
        //fprintf(stdout, "Temp was %s\n", temp);
        //fflush(stdout);
        return;
    }

    //fprintf(stdout, "Worker count: %d\n", in_args.worker_count); 


    /*fprintf(stderr, "Difficulty is %d\n", in_args.difficulty);
    fprintf(stderr, "Seed is ");
    print_uint256(in_args.seed);
    fprintf(stderr, "Start is %" PRId64 "\n", in_args.start);
    fprintf(stderr, "Worker count is %d\n", in_args.worker_count);
	fprintf(stderr, "Socket is %d\n", in_args.sockfd);*/

    add_to_queue(in_args);
    //work(*in_args);
}

void handle_abrt(int sockfd, List* work_queue, bool flag)
{
    if (work_queue->size == 0)
        return;
    // Loop through the entire list and delete all nodes with sockfd
    Node* node = work_queue->head;
    int node_sockfd;
    struct work_args* node_args;
    while(node)
    {
        //fprintf(stdout, "FINE\n"); fflush(stdout);
        if(node == work_queue->head)
        {
            node = node->next;
            if (!node)
                break;
        }
        node_args = (struct work_args*)(node->data);
        node_sockfd = node_args->sockfd;
        if(node_sockfd == sockfd)
        {
            list_remove_middle(work_queue, node);
        }
        node = node->next;
    }
    if(((struct work_args *)work_queue->head->data)->sockfd == sockfd) {
        set_abrt_true();
    }
    //fprintf(stdout, "After aborting queue size is %d\n", work_queue->size);
    //fflush(stdout);
    if (flag)
        send_msg((BYTE*)OKAY, sockfd);
}

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
		perror("ERROR writing to socket");
        log_to_file("ERROR writing to socket", SERV_ADR, sockfd);
        fflush(stdout);
		// TODO: remove this
		// exit(1);
	}
}

void send_msg (BYTE msg[MAX_MSG_LEN], int sockfd)
{
    //int i;
    BYTE new_msg[MAX_MSG_LEN+2];
    memset(new_msg, '\0', MAX_MSG_LEN+2);
    strcat((char*)new_msg, (char*)msg);

	//strcat((char*)concatenated, "\r\n");
    //concatenated[strlen((char*)concatenated)] = '\0';
    strcat((char*)new_msg, "\r\n");
    //fprintf(stdout, "I have to send message %s\n\n\n\n", new_msg);
    //fflush(stdout);

    log_to_file((char*)new_msg, SERV_ADR, sockfd);
	if (send(sockfd, new_msg, strlen((char*)new_msg), 0) !=
												(int)strlen((char*)new_msg))
	{
		perror("ERROR writing to socket");
        fflush(stdout);
        log_to_file("ERROR writing to socket", SERV_ADR, sockfd);
        // TODO: Not sure about this
		// exit(1);
	}
    //fprintf(stdout, "After\n");
    //fflush(stdout);
}
