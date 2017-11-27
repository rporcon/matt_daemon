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
#include "server.hpp"
#include "logger.hpp"

extern int	g_lock_fd;

void		err_exit(const char *err_msg);
void		perr_exit(const char *str);

typedef struct	s_pck_hdr {
	uint64_t	size;
	uint8_t		encrypted;
}				t_pck_hdr;
