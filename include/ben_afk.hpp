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
#include <unistd.h>
#include <random>
#include <vector>
#include <string.h>
#include <fcntl.h>
#include <poll.h>
#include <limits.h>
#define BUF_SIZE 1024
# define KEYLEN 32

struct	t_pck_hdr {
	uint32_t	secret;
	uint64_t	size;
	uint8_t		encrypted;
	uint8_t		rs;
};


typedef struct			g_opt {
	char				host[64];
	uint16_t			port;
	char				public_key[KEYLEN];
	uint8_t				rs; // remote shell
	uint8_t				ipv6; // remote shell
	uint8_t				flag_getlog:1;
	std::vector<char>	log_content;
}						t_opt;

void		err_exit(const char *err_msg);
void		perr_exit(const char *str);
void		print_help(void);
void		rc4(const unsigned char *key, int keylen,
				char *data, size_t data_len);
void		keygen(void);
void		gen_random(char *s);
