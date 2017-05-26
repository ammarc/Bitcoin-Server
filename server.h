/*  Project 2 for COMP30023: Computer Systems
 *  at the University of Melbourne
 *  Semester 1, 2017
 *  by: Ammar Ahmed
 *  Username: ammara
 */

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
#include "list.h"

void log_to_file(char* msg, char* ip, int sockfd);

void search_for_work();

void add_to_queue(void* in_args);

#endif