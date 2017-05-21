#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <inttypes.h>
#include <pthread.h>
#include "input-handler.h"
#include "server.h"
#include "mine.h"

void handle_input(int sockfd, char* buffer)
{
    if (strncmp(buffer, PING, 4) == 0)
        handle_ping(sockfd);
    
    else if (strncmp(buffer, ERRO, 4) == 0)
        handle_erro(sockfd);

    else if (strncmp(buffer, OKAY, 4) == 0)
        handle_okay(sockfd);

    else if (strncmp(buffer, PONG, 4) == 0)
        handle_pong(sockfd);

    else if (strncmp(buffer, SOLN, 4) == 0)
        handle_soln(sockfd, buffer);

    else if (strncmp(buffer, WORK, 4) == 0)
        handle_work(sockfd, buffer);

    else if (strncmp(buffer, ABRT, 4) == 0)
        handle_abrt(sockfd);

    else
        handle_other(sockfd);
}

void handle_ping(int sockfd)
{
    if(send(sockfd, PONG, strlen(PONG), 0) != strlen(PONG) )  
    {
        perror("ERROR writing to socket");
        exit(1);
    }
}

void handle_pong(int sockfd)
{
    BYTE error[40] = {" Can't send PONG to server\r\n"};
    send_erro(error, sockfd);
}

void handle_soln(int sockfd, char* buffer)
{
    //TODO: need to check for formatting as well
    struct soln_args in_args;
    /*uint32_t difficulty;
    BYTE seed[32];
    uint256_init(seed);
    uint64_t solution;*/
    char temp[64+1];
    memset(temp, 0, 65);

    strtok(buffer, " ");

    in_args.difficulty = strtol((strtok(NULL, " ")), NULL, 16);
    strcpy(temp, strtok(NULL, " "));
    temp[64] = '\0';
    for(int i = 62; i >= 0; i-=2)
    {
        char str[3];
        memset(str, 0, 3);
        strncpy(str, temp+i, 2);
        str[2] = '\0';
        in_args.seed[i/2] = (strtol(str, NULL, 16)) & 0xFF;
        temp[i] = '\0';
    }

    in_args.solution = strtol((strtok(NULL, " ")), NULL, 16);

    //TODO: change this to allow for more threads
    //pthread_t tid[2];
    //pthread_create(&tid[0], NULL, (void *)check_sol, &in_args);

    if (check_sol(in_args))
    {
        if(send(sockfd, OKAY, strlen(OKAY), 0) != strlen(OKAY))
        {
            perror("ERROR writing to socket");
            exit(1);
        }
    }
    else
    {
        if(send(sockfd, "WRONG", strlen("WRONG"), 0) != strlen("WRONG"))
        {
            perror("ERROR writing to socket");
            exit(1);
        }
    }

}

void handle_erro(int sockfd)
{
    BYTE error[40] = {" Can't send ERRO to server\r\n"};
	send_erro(error, sockfd);
}

void handle_work(int sockfd, char* buffer)
{
    //TODO: need to check for formatting as well
    struct work_args in_args;
    /*uint32_t difficulty;
    BYTE seed[32];
    uint256_init(seed);
    uint64_t start;
    uint8_t worker_count;*/
    char temp[64+1];

    fprintf(stderr, "sockfd is %d\n", sockfd);
    in_args.sockfd = sockfd;
    fprintf(stderr, "sockfd is %d\n", sockfd);

    strtok(buffer, " ");

    in_args.difficulty = strtol((strtok(NULL, " ")), NULL, 16);
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

    fprintf(stderr, "Difficulty is %d\n", in_args.difficulty);
    fprintf(stderr, "Seed is ");
    print_uint256(in_args.seed);
    fprintf(stderr, "Start is %" PRId64 "\n", in_args.start);
    fprintf(stderr, "Worker count is %d\n", in_args.worker_count);
	fprintf(stderr, "Socket is %d\n", in_args.sockfd);
    work(in_args);
}

void handle_abrt(int sockfd){}

void handle_okay(int sockfd)
{
    BYTE error[40];
    memset(error, 0, 40);
    strcpy((char *)error, " Can't send OKAY to server\r\n");
    send_erro(error, sockfd);
}

void handle_other(int sockfd)
{
    BYTE error[40];
    memset(error, 0, 40);
    strcpy((char *)error, " Invalid command.\r\n");
    send_erro(error, sockfd);
}

void send_erro (BYTE error[40], int sockfd)
{
	// TODO: remove this magic number
	char error_message[40 + sizeof(ERRO) + 1] = "";
	printf("%s", (char *)error);
	memset(error_message, 0,  40 + sizeof(ERRO) + 1);
	char *concatenated = malloc(sizeof *error + sizeof(ERRO));
	strcat(concatenated, ERRO);
	strcat(concatenated, (char *)error);

	if (send(sockfd, concatenated, strlen(concatenated), 0) != 
												(int)strlen(concatenated))
	{
		perror("ERROR writing to socket");
		exit(1);
	}
}

