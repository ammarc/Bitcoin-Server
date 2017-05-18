#ifndef SERVER_H
#define SERVER_H

#define PONG "PONG\r\n"
#define PING "PING\r\n"
#define OKAY "OKAY\r\n"
#define ERRO "ERRO\r\n"
#define SOLN "SOLN\r\n"
#define WORK "WORK\r\n"

typedef unsigned char BYTE;

void send_erro(BYTE error[40], int newsockfd);

#endif