#pragma once

#include <sys/resource.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <string>
#include <iostream>
#include <random>

# define KEYLEN 32

typedef struct	g_opt {
	char		public_key[KEYLEN];
}				t_opt;

void		err_exit(const char *err_msg);
void		perr_exit(const char *str);
void		print_help(void);
void		rc4(unsigned char *key, int keylen,
				char *data, size_t data_len);
void		keygen(void);
