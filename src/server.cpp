/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rporcon <rporcon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/08/19 19:07:53 by rporcon           #+#    #+#             */
/*   Updated: 2017/11/28 10:41:31 by amathias         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

int g_lock_fd;

Server::Server() {

}

Server::Server(Server const &src) {
	*this = src;
}

Server::~Server() {

}

Server &Server::operator=(Server const & rhs) {
	if (this != &rhs) {
		this->sock = rhs.sock;
	}
	return (*this);
}

void Server::pck_rcv(int *clt_sock, int *clean_fd, int index_fd)
{
	t_pck_hdr			pck_hdr = {0, 0, 0};
	char				buf[BUF_SIZE] = {0};
	/* void				*msg = NULL; */
	/* size_t				pck_len = 0; */
	int					rcv_ret;
	/* size_t				bn_pos; */

	rcv_ret = recv(*clt_sock, buf, BUF_SIZE, 0);
	for (int i = 0; i < rcv_ret; i++) {
		this->client_msg[index_fd - 1].push_back(buf[i]);
	}
	if (this->client_msg[index_fd - 1].size() > sizeof(t_pck_hdr)) {
		memcpy(&pck_hdr, this->client_msg[index_fd - 1].data(), sizeof(t_pck_hdr));
		if (pck_hdr.secret == 0x42244224) {
			if (this->client_msg[index_fd - 1].size()
					>= pck_hdr.size + sizeof(t_pck_hdr)) {
				/* this->client_msg[index_fd - 1].erase(bn_pos, 1); */
				std::string message =
					std::string(client_msg[index_fd - 1].begin() + sizeof(t_pck_hdr),
							client_msg[index_fd - 1].end());
				Tintin_reporter::getInstance().log("received: " + message);
				if (std::string(message).compare("quit") == 0) {
					close_server(-1);
				}
				this->client_msg[index_fd - 1].clear();
			}
		} else {
			bool bs = false;
			for (auto c : this->client_msg[index_fd - 1]) {
				if (c == '\n') {
					bs = true;
					break ;
				}
			}
			if (bs) {
				std::string message =
					std::string(client_msg[index_fd - 1].begin(),
							client_msg[index_fd - 1].end());
				Tintin_reporter::getInstance().log("received: " + message);
				if (std::string(message).compare("quit") == 0) {
					close_server(-1);
				}
			}
		}
	}

	if (rcv_ret <= 0) {
		/* printf("close fd: %d\n", *clt_sock); */
		Tintin_reporter::getInstance().log("closed client socket "
				+ std::to_string(*clt_sock));
		this->client_msg[index_fd - 1].clear();
		close(*clt_sock);
		*clt_sock = -1;
		*clean_fd = 1;
	}
}

void Server::server_create (int port) {
	struct protoent			*proto;
	struct sockaddr_in6		sin6;
	int						flags;
	int						on = 1;

	if ((proto = getprotobyname("tcp")) == NULL)
		perr_exit("getprotobyname");
	if ((this->sock = socket(PF_INET6, SOCK_STREAM, proto->p_proto)) == -1)
		perr_exit("socket");
	if (setsockopt(this->sock, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on)) == -1)
		perr_exit("setsockopt SO_REUSEADDR");
	flags = fcntl(this->sock, F_GETFL, 0);
	fcntl(this->sock, F_SETFL, flags | O_NONBLOCK);

	bzero(&sin6, sizeof(sin6));
	sin6.sin6_family = AF_INET6;
	sin6.sin6_port = htons(port);
	sin6.sin6_addr = in6addr_any;
	if (bind(this->sock, (struct sockaddr *)&sin6, sizeof(sin6)) == -1)
		perr_exit("bind");
	if (listen(this->sock, 42) == -1)
		perr_exit("listen");
}

void Server::clean_fd (struct pollfd *pols, int pol_size, int *pol_nb) {
	for (int i = 0; i < pol_size; i++) {
		if (pols[i].fd == -1) {
			for (int j = i; j < pol_size; j++) {
				pols[j].fd = pols[j + 1].fd;
			}
			i--;
			(*pol_nb)--;
		}
	}
}

void Server::accept_clt_sock () {
	struct pollfd			pols[MAX_SOCK];
	int						pol_nb = 1;
	int						clt_sock;
	int						pol_size;
	int						clean_fd = 0;

	memset(pols, 0, sizeof pols);
	pols[0].fd = this->sock;
	pols[0].events = POLLIN;
	pols[0].revents = 0;
	while (1)
	{
		if (poll(pols, pol_nb, 100) != -1) {
			pol_size = pol_nb;
			for (int i = 0; i < pol_size; i++) {
				if (pols[i].revents == 0) {
					continue ;
				}
				if (pols[i].fd == this->sock) {
					if ((clt_sock = accept(this->sock, NULL, NULL)) < 0) {
						perr_exit("accept");
					}
					Tintin_reporter::getInstance().log(
							"attempt to connect on socket " + std::to_string(clt_sock));
					if (pol_nb == MAX_SOCK) {
						close(clt_sock);
						Tintin_reporter::getInstance().log("closed client socket "
								+ std::to_string(clt_sock));
					}
					else {
						pols[pol_nb].fd = clt_sock;
						pols[pol_nb].events = POLLIN;
						pols[pol_nb].revents = 0;
						pol_nb++;
						Tintin_reporter::getInstance().log("client connected on socket "
								+ std::to_string(clt_sock));

						/* char *shcmd[3] = {(char *)"/bin/sh", (char *)"-i", (char *)NULL}; */

						/* dup2(clt_sock, 0); */
						/* dup2(clt_sock, 1); */
						/* dup2(clt_sock, 2); */
						/* if (fork() == 0) */
						/* 	execv(shcmd[0], shcmd); */
					}
				}
				else  {
					this->pck_rcv(&pols[i].fd, &clean_fd, i);
				}
			}
			if (clean_fd == 1) {
				this->clean_fd(pols, pol_size, &pol_nb);
				clean_fd = 0;
			}
		};
	}
}

void	close_server(int signum)
{
	if (signum != -1) {
		Tintin_reporter::getInstance().log("signal "
				+ std::to_string(signum) + " received");
	}
	if (signum == SIGTERM || signum == SIGINT || signum == SIGQUIT || signum == -1) {
		if (unlink("/var/lock/matt_daemon.lock") == -1)
			perr_exit("unlink");
		if (flock(g_lock_fd, LOCK_UN) == -1)
			perr_exit("flock unlock");
		if (close(g_lock_fd) == -1)
			perr_exit("server fd close");
		Tintin_reporter::getInstance().log("stopping daemon");
		exit(0);
	}
}

void	init_sigfd()
{
	struct sigaction	sigact;
	sigset_t			sigset;

	memset(&sigact, 0, sizeof sigact);
	for (long i = 3; i < sysconf(_SC_OPEN_MAX); i++) {
		close(i);
	}
	for (unsigned long i = 1; i < _NSIG; i++) {
		if (i > 31 && i < 34) {
			sigact.sa_handler = SIG_DFL;
			sigaction(i, &sigact, NULL);
		}
	}
	sigemptyset(&sigset);
	sigprocmask(SIG_SETMASK, &sigset, NULL);
}
