#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <inttypes.h>
#include "input-handler.h"
#include "server.h"
#include "mine.h"

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
    uint32_t difficulty;
    BYTE seed[32];
    uint256_init(seed);
    uint64_t solution;
    char temp[64+1];

    strtok(buffer, " ");

    difficulty = strtol((strtok(NULL, " ")), NULL, 16);
    strcpy(temp, strtok(NULL, " "));
    temp[64] = '\0';
    for(int i = 62; i >= 0; i-=2)
    {
        char str[2];
        strcpy(str, &temp[i]);
        seed[i/2] = strtol(str, NULL, 16) & 0xFF;
        temp[i] = '\0';
    }

    //strcpy(temp, strtok(NULL, " "));
    solution = strtol((strtok(NULL, " ")), NULL, 16);

    if (check_sol(difficulty, seed, solution))
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
    uint32_t difficulty;
    BYTE seed[32];
    uint256_init(seed);
    uint64_t start;
    uint8_t worker_count;
    char temp[64+1];

    strtok(buffer, " ");

    difficulty = strtol((strtok(NULL, " ")), NULL, 16);
    strcpy(temp, strtok(NULL, " "));
    temp[64] = '\0';
    for(int i = 62; i >= 0; i-=2)
    {
        char str[2];
        strcpy(str, &temp[i]);
        seed[i/2] = strtol(str, NULL, 16) & 0xFF;
        temp[i] = '\0';
    }

    start = strtol((strtok(NULL, " ")), NULL, 16);

    worker_count = strtol((strtok(NULL, " ")), NULL, 16);

    fprintf(stderr, "Difficulty is %d\n", difficulty);
    fprintf(stderr, "Seed is ");
    print_uint256(seed);
    fprintf(stderr, "Start is %" PRId64 "\n", start);
    fprintf(stderr, "Worker count is %d\n", worker_count);
    work(difficulty, seed, start, worker_count, sockfd);
}

void handle_abrt(int sockfd);

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

void send_erro (BYTE error[40], int newsockfd)
{
	// TODO: remove this magic number
	char error_message[40 + sizeof(ERRO) + 1] = "";
	printf("%s", (char *)error);
	memset(error_message, 0,  40 + sizeof(ERRO) + 1);
	char *concatenated = malloc(sizeof *error + sizeof(ERRO));
	strcat(concatenated, ERRO);
	strcat(concatenated, (char *)error);

	if (send(newsockfd, concatenated, strlen(concatenated), 0) != 
												(int)strlen(concatenated))
	{
		perror("ERROR writing to socket");
		exit(1);
	}
}

