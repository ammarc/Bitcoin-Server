#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <inttypes.h>
#include "mine.h"

bool check_sol(struct soln_args args)
{
	//fprintf(stdout, "----------------Start Soln----------------\n");
	int i;
    uint32_t difficulty = args.difficulty;
    BYTE seed[64];
	for (i = 0; i < 32; i++)
		seed[i] = args.seed[i];
    uint64_t solution = args.solution;
	// First we need to concatenate the seed and the solution
	BYTE x[40];
	uint256_init(x);
	BYTE temp[64];
	uint256_init(temp);
	BYTE target[64];
	uint256_init(target);
	BYTE hash[SHA256_BLOCK_SIZE];
	uint256_init(hash);

	SHA256_CTX ctx;

	uint32_t alpha = 0;
	uint32_t beta = 0;
	// Finding the x to be checked
	/*fprintf(stdout, "Seed is: ");
	print_uint256(seed);
	fprintf(stdout, "\n");*/
	for (i = 0; i < 32; i++)
	{
		//int temp = solution & 0xFF;
		//x[i] = seed[i] | temp;
		x[i] = seed[i];
	}
	for(i = 39; i >= 32; i--)
	{
		int temp = solution & 0xFF;
		x[i] = temp;
		solution = solution >> 8;
	}
	
	/*fprintf(stdout, "x is: ");
	printf ("0x");
    for (size_t i = 0; i < 40; i++) {
        printf ("%02x", x[i]); 
    }
    printf ("\n");
	fprintf(stdout, "\n");*/
	
	// Applying the hash twice
	sha256_init(&ctx);
	sha256_update(&ctx, x, 40);
	sha256_final(&ctx, hash);
	//fprintf(stderr, "First hash:\n");
	//print_uint256(hash);
	sha256_init(&ctx);
	sha256_update(&ctx, hash, SHA256_BLOCK_SIZE);
	uint256_init(hash);
	sha256_final(&ctx, hash);
	//fprintf(stderr, "Second hash:\n");
	//print_uint256(hash);

	// Extracting alpha
	// We need bits 0..7 for alpha from difficulty (MSB)
	//fprintf(stderr, "difficulty is %d\n", difficulty);
    // Extracting beta
	beta = ((1 << 24) - 1) & difficulty;
	//fprintf(stderr, "Beta is %d\n", beta);
	alpha = ((1 << 8) - 1) & (difficulty >> 24);
	//fprintf(stderr, "Alpha is %d\n", alpha);

	uint32_t exponent;
	
	// Setting temp to 0
	uint256_init(temp);
	BYTE base[32];
	uint256_init(base);
	uint32_t beta_copy = beta;
	base[31] = 2;
	exponent = 8 * (alpha - 3);
	uint256_exp(temp, base, exponent);
	BYTE beta_rep[3];

	for(int i = 31; i >= 0; i--)
	{
		beta_rep[i] = beta_copy & 0xFF;
		beta_copy = beta_copy >> 8;
	}
    uint256_mul(target, beta_rep, temp);
	//fprintf(stderr, "Target is: ");
	//print_uint256(target);
	//fprintf(stderr, "\n");


	//fprintf(stdout, "----------------End Soln----------------\n");
	if (sha256_compare(target, hash) > 0)
		return true;
	return false;
}

void work(struct work_args args)
{
	//fprintf(stdout, "----------------Start Work----------------\n");
	int i;
	BYTE curr_value[32];
	uint256_init(curr_value);
	
	char soln_msg[40];
	char temp[40];

	uint32_t difficulty = args.difficulty;
    BYTE seed[64];
	for (i = 0; i < 64; i++)
		seed[i] = args.seed[i];
    uint64_t start = args.start;
    uint8_t worker_count = args.worker_count;
    int sockfd = args.sockfd;
	uint64_t nonce = start;


	struct soln_args in_args;
	in_args.difficulty = difficulty;
	
	for (i = 0; i < 64; i++)
		in_args.seed[i] = args.seed[i];
	
	in_args.solution = nonce;

	while (!check_sol(in_args))
	{
		nonce++;
		in_args.solution = nonce;
	}

    
	// need to send the solution to the client
	sprintf(soln_msg, "%s %x ", "SOLN", difficulty);
	
	for (i = 0; i < 32; i++)
	{
		sprintf(temp, "%02x", seed[i]);
		strcat(soln_msg, temp);
	}
    
	sprintf(temp, " %" PRIx64 "\r\n", nonce);
	strcat(soln_msg, temp);

	if (send(args.sockfd, soln_msg, strlen(soln_msg), 0) != 
													(int)strlen(soln_msg))
	{
		perror("ERROR writing to socket");
		fprintf(stdout, "Error in work\n");
		exit(1);
	}
	//fprintf(stdout, "----------------End Work----------------\n");
}
