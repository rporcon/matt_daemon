#ifndef MATT_DAEMON_HPP
# define MATT_DAEMONH_HPP

#pragma once

#include <iostream>
#include <string>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/file.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <libgen.h>
#include <stdlib.h>
#include <signal.h>
#include <poll.h>
#include <netdb.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include "logger.hpp"

using namespace std;

# define BUF_SIZE 1024
# define MAX_SOCK 4

extern int	g_lock_fd;

class Server {
	int		sock;

	public:
		void server_create (int);
		void accept_clt_sock (void);
		void clean_fd(struct pollfd *, int, int *);
		void pck_rcv(int *, int *);
};

void		err_exit(const char *err_msg);
void		perr_exit(const char *str);
void		init_sigfd();
void		close_server(int signum);

# endif
