/*  Project 2 for COMP30023: Computer Systems
 *  at the University of Melbourne
 *  Semester 1, 2017
 *  by: Ammar Ahmed
 */

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
#include "server.h"
#include "input-handler.h"

int abrt_flag = 0;

bool check_sol(struct soln_args args, BYTE* target)
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
	//BYTE target[64];
	BYTE hash[SHA256_BLOCK_SIZE];
	uint256_init(hash);

	SHA256_CTX ctx;

	// Finding the x to be checked
	/*fprintf(stdout, "Seed is: ");
	print_uint256(seed);
	fprintf(stdout, "\n");*/
	for (i = 0; i < 32; i++)
	{
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
	sha256_init(&ctx);
	sha256_update(&ctx, hash, SHA256_BLOCK_SIZE);
	uint256_init(hash);
	sha256_final(&ctx, hash);
	/*fprintf(stdout, "The final hash is\n");
	print_uint256(hash);
	fflush(stdout);*/

	if (target == NULL)
		target = find_target(difficulty);
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
	//fflush(stdout);
	/*if(args.worker_count>1)
		return;*/

    if (abrt_flag)
	{
		set_abrt_false();
		return;
	}

	int i;
	BYTE curr_value[32];
	uint256_init(curr_value);
	
	BYTE soln_msg[MAX_MSG_LEN];
	memset(soln_msg, '\0', MAX_MSG_LEN);
	char temp[40];

	uint32_t difficulty = args.difficulty;
    BYTE seed[64];
	for (i = 0; i < 64; i++)
		seed[i] = args.seed[i];
    uint64_t start = args.start;
    uint8_t worker_count = args.worker_count;
    int sockfd = args.sockfd;
	uint64_t nonce = args.start;
	//fprintf(stdout, "Nonce is %" PRIx64 "\n", nonce);
	//fflush(stdout);

	struct soln_args in_args;
	in_args.difficulty = difficulty;

	args.target = find_target(args.difficulty);

	//fprintf(stdout, "Found target in work:\n");
	//print_uint256(args.target);
	
	for (i = 0; i < 64; i++)
		in_args.seed[i] = args.seed[i];
	
	in_args.solution = nonce;

	while (!check_sol(in_args, args.target))
	{
		if (abrt_flag)
		{
			set_abrt_false();
			return;
		}
		nonce++;
		in_args.solution = nonce;
	}

	//fprintf(stdout, "Nonce is %" PRIx64 "\n", nonce);

    // Free the target since we are done with it
	free(args.target);
	// need to send the solution to the client
	sprintf((char*)soln_msg, "%s %08x ", SOLN, difficulty);
	
	for (i = 0; i < 32; i++)
	{
		sprintf(temp, "%02x", seed[i]);
		strcat((char*)soln_msg, temp);
	}
    
	sprintf(temp, " %016" PRIx64, nonce);
	strcat((char*)soln_msg, temp);

	//fprintf(stdout, "I've to %s\n", soln_msg);
	//fflush(stdout);

	send_msg((BYTE*)soln_msg, sockfd);
	//fprintf(stdout, "---------END-------\n"); fflush(stdout);
}

BYTE* find_target(uint32_t difficulty)
{
	BYTE* target = malloc(sizeof(BYTE)*64);
	uint256_init(target);
	uint32_t alpha = 0;
	uint32_t beta = 0;
	BYTE temp[64];
	uint256_init(temp);
	uint32_t exponent;

	// Extracting beta
	beta = ((1 << 24) - 1) & difficulty;
	// Extracting alpha
	alpha = ((1 << 8) - 1) & (difficulty >> 24);

	
	// Setting temp to 0
	uint256_init(temp);
	BYTE base[32];
	uint256_init(base);
	uint32_t beta_copy = beta;
	base[31] = 2;
	exponent = 8 * (alpha - 3);
	uint256_exp(temp, base, exponent);
	BYTE beta_rep[32];

	for(int i = 31; i >= 0; i--)
	{
		beta_rep[i] = beta_copy & 0xFF;
		beta_copy = beta_copy >> 8;
	}
	uint256_mul(target, beta_rep, temp);

	return target;
}

void set_abrt_false()
{
	abrt_flag = 0;
}

void set_abrt_true()
{
	abrt_flag = 1;
}
