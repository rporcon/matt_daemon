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
		void pck_rcv(int *, int *, int);
		void pck_wrt(int, int);
	private:
		int							sock;
		std::vector<char>			client_msg[MAX_SOCK - 1];
		std::vector<std::string>	logs[MAX_SOCK - 1];
};

void		init_sigfd();
void		close_server(int signum);
