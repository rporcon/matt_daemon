#include "ben_afk.hpp"

int		connect_to_daemon(t_opt *opt)
{
	struct sockaddr_in	serv_addr;
	struct hostent		*hostinfo;
	int					fd;
	int					ret;

	hostinfo = gethostbyname(opt->host);
	if (hostinfo == NULL)
		err_exit("invalid hostname");

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = *(unsigned int *)hostinfo->h_addr_list[0];
	serv_addr.sin_port = htons(4242);
	fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (fd == -1)
		perr_exit("socket");
	ret = connect(fd, (const struct sockaddr*)&serv_addr,
			sizeof(struct sockaddr_in));
	if (ret == -1)
		perr_exit("connect");
	return (fd);
}

int		connect_to_daemon6(t_opt *opt)
{
	struct sockaddr_in6	serv_addr;
	struct hostent		*hostinfo;
	int					fd;
	int					ret;

	hostinfo = gethostbyname2(opt->host, AF_INET6);
	if (hostinfo == NULL)
		err_exit("invalid hostname");

	serv_addr.sin6_family = AF_INET6;
	memcpy(serv_addr.sin6_addr.s6_addr, hostinfo->h_addr_list[0],
			sizeof serv_addr.sin6_addr.s6_addr);
	serv_addr.sin6_port = htons(4242);
	fd = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
	if (fd == -1)
		perr_exit("socket");
	ret = connect(fd, (const struct sockaddr*)&serv_addr,
			sizeof(struct sockaddr_in6));
	if (ret == -1)
		perr_exit("connect");
	return (fd);
}

void	send_message(int fd, std::string message, std::string key)
{
	char		*data;
	t_pck_hdr	hdr = {0x42244224, 0, 0};

	data = new char[message.size() + sizeof(t_pck_hdr)];
	memset(data, 0, message.size() + sizeof(t_pck_hdr));
	hdr.size = message.length();
	if (message.compare("getlog") == 0) // send getlog unencrypted
		key.clear();
	if (key.empty()) {
		hdr.encrypted = 0;
		memcpy(data, &hdr, sizeof(t_pck_hdr));
		memcpy(data + sizeof(t_pck_hdr), message.c_str(), message.length());
	} else {
		hdr.encrypted = 1;
		memcpy(data, &hdr, sizeof(t_pck_hdr));
		char *message_enc = new char[message.size() + 1];
		std::copy(message.begin(), message.end(), message_enc);
		message_enc[message.size()] = '\0';
		rc4(reinterpret_cast<const unsigned char *>(key.c_str()),
			key.length(), message_enc, message.length());
		memcpy(data + sizeof(t_pck_hdr), message_enc, message.length());
		delete[] message_enc;
	}
	if (send(fd, data, message.length() + sizeof(t_pck_hdr), 0) == -1)
		perr_exit("send");
	delete[] data;
}

void	process_logs(t_opt *opt, std::string key) {
	std::string			output;
	t_pck_hdr			pck_hdr = {0, 0, 0};
	std::string			tmp;

	std::cout << key << std::endl;
	for (unsigned int i = 0; i < opt->log_content.size(); i++) {
		memset(&pck_hdr, 0, sizeof(t_pck_hdr));
		memcpy(&pck_hdr, &opt->log_content[i],
			std::min(sizeof(t_pck_hdr), opt->log_content.size() - i));
		if (key.empty() == false &&
				pck_hdr.secret == 0x42244224 && pck_hdr.encrypted == 1) {
			char *message_enc = new char[(pck_hdr.size + 1)];
			std::copy(opt->log_content.begin() + i + sizeof(t_pck_hdr),
				opt->log_content.begin() + i + sizeof(t_pck_hdr) + pck_hdr.size,
				message_enc);
			message_enc[pck_hdr.size] = '\0';
			rc4(reinterpret_cast<const unsigned char *>(key.c_str()),
				key.length(), message_enc, pck_hdr.size);
			tmp = std::string(message_enc);
			delete[] message_enc;
			size_t bs_pos;
			if ((bs_pos = tmp.find_last_of('\n')) != std::string::npos) {
				tmp.erase(bs_pos, 1);
			}
			output += tmp;
			i += (sizeof(t_pck_hdr) + pck_hdr.size) - 1;
		} else {
			output += opt->log_content[i];
		}
	}
	std::cout << output << std::endl;
}

int 	receive_logs(t_opt *opt, int fd, std::string key) {
	std::string	message;
	char		buf[BUF_SIZE];
	int			rcv_ret;
	size_t		pos;

	rcv_ret = recv(fd, buf, BUF_SIZE, 0);
	for (int i = 0; i < rcv_ret; i++) {
		opt->log_content.push_back(buf[i]);
	}
	if (rcv_ret > 0) {
		message = std::string(buf, rcv_ret);
		if ((pos = message.find("logexit\n")) != std::string::npos) {
			process_logs(opt, key);
			exit(0);
		}
	} else if (rcv_ret <= 0){
		// Server down ?
		exit(0);
	}
	return (rcv_ret);
}

void	get_args(t_opt *opt, int ac, char **av)
{
	char	c;
	int		reqarg_nb = 0;

	while ((c = getopt (ac, av, "6hrke:g")) != -1) {
		switch (c)
		{
			case 'h':
				print_help();
				break ;
			case 'k':
				keygen();
				break ;
			case 'r':
				opt->rs = 1;
				break ;
			case 'e':
				if (strlen(optarg) != KEYLEN)
					err_exit("incorrect key length");
				memcpy(opt->public_key, optarg, KEYLEN);
				break;
			case 'g':
				opt->flag_getlog = 1;
				break;
			case '6':
				opt->ipv6 = 1;
				break;
			case '?':
				if (optopt == 'e') {
					fprintf(stderr, "Option -%c requires an argument.\n", optopt);
				}
				else if (isprint (optopt)) {
					fprintf(stderr, "Unknown option '-%c'.\n", optopt);
				}
				else {
					fprintf(stderr, "Unknown option character '%c'.\n",
						optopt);
				}
				exit(1);
			default:
				break ;
		}
	}
	for (int i = optind; i < ac; i++) {
		if (reqarg_nb == 0) {
			if (strlen(av[i]) > 64)
				err_exit("invalid hostname (must be < 64 char)");
			strcpy(opt->host, av[i]);
		}
		else if (reqarg_nb > 1)
			err_exit("invalid require arguments");
		reqarg_nb++;
	}
	if (reqarg_nb != 1)
		print_help();
}

void	rs_connect(int fd)
{
	struct pollfd	pols[2];
	int				pol_nb = 2;
	char			buf[BUF_SIZE];
	int				rcv_ret;

	fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
	pols[0].fd = fd;
	pols[0].events = POLLIN;
	pols[0].revents = -1;
	pols[1].fd = 0;
	pols[1].events = POLLIN;
	pols[1].revents = -1;
	send_message(fd, "-<_rs_>-", std::string(""));
	while (1) {
		if (poll(pols, pol_nb, -1) != -1) {
			if (pols[0].revents == 0 && pols[1].revents == 0) {
				continue ;
			}
			if (pols[0].revents & POLLIN) {
				rcv_ret = read(fd, buf, BUF_SIZE);
				write(1, buf, rcv_ret);
				/* printf("rcv ret client socket: %d\n", rcv_ret); */
			}
			if (pols[1].revents & POLLIN) {
				rcv_ret = read(0, buf, BUF_SIZE);
				/* printf("rcv ret stdin socket: %d\n", rcv_ret); */
				write(fd, buf, rcv_ret);
			}
		}
	}
}

void	default_connect(int fd, t_opt *opt)
{
	struct pollfd	pols[2];
	int				pol_nb = 2;
	char			buf[BUF_SIZE];
	int				rcv_ret;

	fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
	pols[0].fd = fd;
	pols[0].events = POLLIN;
	pols[0].revents = -1;
	pols[1].fd = 0;
	pols[1].events = POLLIN;
	pols[1].revents = -1;
	printf("$ "); fflush(stdout);
	while (1) {
		if (poll(pols, pol_nb, -1) != -1) {
			if (pols[0].revents == 0 && pols[1].revents == 0) {
				continue ;
			}
			if (pols[0].revents & POLLIN) {
				rcv_ret = read(fd, buf, BUF_SIZE);
				if (rcv_ret <= 0) {
					printf("server connection lost\n");
					exit(0);
				}
			}
			if (pols[1].revents & POLLIN) {
				rcv_ret = read(0, buf, BUF_SIZE);
				printf("$ "); fflush(stdout);
				send_message(fd, std::string(buf, rcv_ret),
						std::string(opt->public_key));
				//write(fd, buf, rcv_ret);

			}
		}
	}
}

int		main(int ac, char **av)
{
	t_opt					opt;
	std::string				buffer;
	int						fd;

	memset(&opt, 0, sizeof opt);
	get_args(&opt, ac, av);
	if (opt.ipv6 == 0)
		fd = connect_to_daemon(&opt);
	else
		fd = connect_to_daemon6(&opt);
	if (opt.flag_getlog) {
		send_message(fd, "getlog", std::string(opt.public_key));
		while (1) {
			receive_logs(&opt, fd, std::string(opt.public_key));
		}
	} else if (opt.rs) {
		rs_connect(fd);
	} else {
		default_connect(fd, &opt);
	}
	close(fd);
}
