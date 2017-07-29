/*  Project 2 for COMP30023: Computer Systems
 *  at the University of Melbourne
 *  Semester 1, 2017
 *  by: Ammar Ahmed
 */

#ifndef HANDLER_H
#define HANDLER_H

#include "sha256.h"
#include "list.h"
#define MSG_HEADER 4
#define MAX_MSG_LEN 128
#define SOLN_LEN 95
#define WORK_LEN 98

void handle_input(int sockfd, char* buffer, List* work_queue);

void handle_ping(int sockfd);

void handle_pong(int sockfd);

void handle_soln(int sockfd, char* buffer);

void handle_erro(int sockfd);

void handle_work(int sockfd, char* buffer);

void handle_abrt(int sockfd, List* work_queue, bool flag);

void handle_okay(int sockfd);

void handle_other(int sockfd);

void send_erro (BYTE error[40], int newsockfd);

void send_msg (BYTE msg[MAX_MSG_LEN], int sockfd);

#endif
