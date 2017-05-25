#ifndef SERVER_H
#define SERVER_H

#define PONG "PONG"
#define PING "PING"
#define OKAY "OKAY"
#define ERRO "ERRO"
#define SOLN "SOLN"
#define WORK "WORK"
#define ABRT "ABRT"
#define SERV_ADR "0.0.0.0"

#define MAX_CLIENTS 100

#include <stdbool.h>
#include "uint256.h"
#include "sha256.h"

void log_to_file(char* msg, char* ip, int sockfd);

#endif