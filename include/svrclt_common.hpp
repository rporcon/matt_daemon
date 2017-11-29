#pragma once
#include <stdint.h>
#include "logger.hpp"

struct	t_pck_hdr {
	uint32_t	secret;
	uint64_t	size;
	uint8_t	encrypted;
};

void		perr_exit(const char *str);
void		err_exit(const char *err_msg);
void		log_exit(const char *str, int status_code);
void		print_help();

