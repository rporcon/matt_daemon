#pragma once

#include "logger.hpp"
#define BUF_SIZE 1024
#define MAX_SOCK 4

class Server {
public:
	Server(void);
	Server(Server const &src);
	Server &operator=(Server const & rhs);
	virtual ~Server ();
	void server_create (int);
	void accept_clt_sock (void);
	void clean_fd(struct pollfd *, int, int *);
	void pck_rcv(int *, int *);
private:
	int		sock;
};

void		init_sigfd();
void		close_server(int signum);
