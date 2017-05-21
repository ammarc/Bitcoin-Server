#include "mine.h"
#include <inttypes.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

bool check_sol(uint32_t difficulty, BYTE seed[32], uint64_t solution)
{
	// First we need to concatenate the seed and the solution
	int i;
	BYTE x[32];
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
	fprintf(stderr, "Seed is: ");
	print_uint256(seed);
	fprintf(stderr, "\n");

	for (i = 31; i >= 0; i--)
	{
		int temp = solution & 0xFF;
		x[i] = seed[i] | temp;
		solution = solution >> 8;
	}
	
	fprintf(stderr, "x is: ");
	print_uint256(x);
	fprintf(stderr, "\n");

	
	// Applying the hash twice
	sha256_init(&ctx);
	sha256_update(&ctx, x, SHA256_BLOCK_SIZE);
	fprintf(stderr, "First data:\n");
	print_uint256(ctx.data);
	sha256_final(&ctx, hash);
	fprintf(stderr, "First hash:\n");
	print_uint256(hash);
	sha256_init(&ctx);
	sha256_update(&ctx, hash, SHA256_BLOCK_SIZE);
	fprintf(stderr, "Second data:\n");
	print_uint256(ctx.data);
	uint256_init(hash);
	sha256_final(&ctx, hash);
	fprintf(stderr, "Second hash:\n");
	print_uint256(hash);

	// Extracting alpha
	// We need bits 0..7 for alpha from difficulty (MSB)
	fprintf(stderr, "difficulty is %d\n", difficulty);
    // Extracting beta
	beta = ((1 << 24) - 1) & difficulty;
	fprintf(stderr, "Beta is %d\n", beta);
	alpha = ((1 << 8) - 1) & (difficulty >> 24);
	fprintf(stderr, "Alpha is %d\n", alpha);

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
	fprintf(stderr, "Target is: ");
	print_uint256(target);
	fprintf(stderr, "\n");


	fprintf(stderr, "Final hash is: ");
	print_uint256(hash);
	fprintf(stderr, "\n");

	if (sha256_compare(target, hash) > 0)
		return true;
	return false;
}

void work(uint32_t difficulty, BYTE seed[32], uint64_t start,
                                       uint8_t worker_count, int sockfd)
{
	int i;
	BYTE curr_value[32];
	uint256_init(curr_value);
	uint64_t nonce = start;
	char soln_msg[40];
	char temp[40];

	while (!check_sol(difficulty, seed, nonce))
		nonce++;
    
	// need to send the solution to the client
	sprintf(soln_msg, "%s %x ", "SOLN", difficulty);
	
	for (i = 0; i < 32; i++)
	{
		sprintf(temp, "%02x", seed[i]);
		strcat(soln_msg, temp);
	}
    
	sprintf(temp, " %" PRIx64 "\r\n", nonce);
	strcat(soln_msg, temp);

	printf("%s", soln_msg);
	if (send(sockfd, soln_msg, strlen(soln_msg), 0) != 
												(int)strlen(soln_msg))
	{
		perror("ERROR writing to socket");
		exit(1);
	}
}