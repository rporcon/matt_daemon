#pragma once

#include <sys/resource.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <iostream>
#include <unistd.h>
#include <string.h>

struct	t_pck_hdr {
	uint64_t	size;
	uint32_t	encrypted;
};

void		err_exit(const char *err_msg);
void		perr_exit(const char *str);
void		rc4(const unsigned char *key, int keylen,
				char *data, size_t data_len);
