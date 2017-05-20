#ifndef SERVER_H
#define SERVER_H

#define PONG "PONG\r\n"
#define PING "PING\r\n"
#define OKAY "OKAY\r\n"
#define ERRO "ERRO\r\n"
#define SOLN "SOLN\r\n"
#define WORK "WORK\r\n"

#include <stdbool.h>
#include "uint256.h"

void send_erro(BYTE error[40], int newsockfd);

bool check_sol(uint32_t difficulty, BYTE seed[64], uint64_t solution);

#endif