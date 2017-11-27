#pragma once

#include <sys/resource.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <iostream>
#include <unistd.h>

void		err_exit(const char *err_msg);
void		perr_exit(const char *str);
void		rc4(unsigned char *key, int keylen,
				char *data, size_t data_len);
