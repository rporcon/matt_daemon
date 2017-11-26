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
#include <libgen.h>
#include <stdlib.h>
#include <signal.h>
#include <poll.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include "logger.hpp"

using namespace std;

# define BUF_SIZE 1024
# define MAX_CLIENT 3

void		err_exit(const char *err_msg);
void		perr_exit(const char *str);

# endif
