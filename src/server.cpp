/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rporcon <rporcon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/08/19 19:07:53 by rporcon           #+#    #+#             */
/*   Updated: 2017/09/04 18:33:49 by rporcon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "matt_daemon.hpp"

class Server {
	int		sock;

	public:
		void server_create (int);
		void accept_clt_sock (void);
		void clean_fd(struct pollfd *, int, int *);
		void pck_rcv(int *);
};

void Server::pck_rcv(int *clt_sock)
{
	char				buf[BUF_SIZE];
	int					rcv_ret;

	bzero(buf, BUF_SIZE);
	rcv_ret = recv(*clt_sock, buf, BUF_SIZE, 0);
	printf("received: %sfrom fd %d (ret: %d)\n", buf, *clt_sock, rcv_ret);
	/* while ((rcv_ret = recv(clt_sock, buf, BUF_SIZE, 0)) > 0) { */
	/* 	printf("received: %s\n", buf); */
	/* } */
	if (rcv_ret <= 0) {
		printf("close fd: %d\n", *clt_sock);
		close(*clt_sock);
		*clt_sock = -1;
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
	fcntl(this->sock, F_GETFL, 0);
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
	struct pollfd			pols[MAX_CLIENT + 1] = {0};
	int						pol_nb = 1;
	int						clt_sock;
	int						pol_size;

	pols[0].fd = this->sock;
	pols[0].events = POLLIN;
	pols[0].revents = 0;
	while (1)
	{
		if (poll(pols, pol_nb, 100) < 0)
			perr_exit("poll");

		pol_size = pol_nb;
		for (int i = 0; i < pol_size; i++) {
			if (pols[i].revents == 0) {
				continue ;
			}
			if (pols[i].fd == this->sock) {
				if ((clt_sock = accept(this->sock, NULL, NULL)) < 0) {
					perr_exit("accept");
				}
				if (pol_nb == MAX_CLIENT + 1) {
					fprintf(stderr, "Cannot handle more than 3 clients\n");
					close(clt_sock);
				}
				else {
					pols[pol_nb].fd = clt_sock;
					pols[pol_nb].events = POLLIN;
					pols[pol_nb].revents = 0;
					pol_nb++;
				}
			}
			else  {
				this->pck_rcv(&pols[i].fd);
			}
		}
		this->clean_fd(pols, pol_size, &pol_nb);
	}
}

int		main(int ac, char **av)
{
	Server serv;

	serv.server_create(4242);
	serv.accept_clt_sock();
	return (EXIT_SUCCESS);
}
