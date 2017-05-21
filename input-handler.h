#ifndef HANDLER_H
#define HANDLER_H

#include "sha256.h"

void handle_ping(int sockfd);

void handle_pong(int sockfd);

void handle_soln(int sockfd, char* buffer);

void handle_erro(int sockfd);

void handle_work(int sockfd, char* buffer);

void handle_abrt(int sockfd);

void handle_okay(int sockfd);

void handle_other(int sockfd);

void send_erro (BYTE error[40], int newsockfd);

#endif