#include "ben_afk.hpp"

int		connect_to_daemon()
{
	struct sockaddr_in	serv_addr;
	int					fd;
	int					ret;

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	serv_addr.sin_port = htons(4242);
	fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (fd == -1)
		perr_exit("socket");
	ret = connect(fd, (const struct sockaddr*)&serv_addr,
			sizeof(struct sockaddr_in));
	if (ret == -1)
		perr_exit("connect");
	return (fd);
}

void	send_message(int fd, std::string message)
{
	int ret;

	ret = send(fd, message.c_str(), message.length(), 0);
	if (ret == -1)
		perr_exit("send");
}

void	get_args(t_opt *opt, int ac, char **av)
{
	char c;

	while ((c = getopt (ac, av, "hke:")) != -1) {
		switch (c)
		{
			case 'h':
				print_help();
			case 'k':
				keygen();
			case 'e':
				if (strlen(optarg) != KEYLEN)
					err_exit("incorrect key length");
				memcpy(opt->public_key, optarg, KEYLEN);
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
				printf("default");
		}
	}
}

int		main(int ac, char **av)
{
	t_opt		opt;
	std::string buffer;
	int			fd;

	memset(&opt, 0, sizeof opt);
	get_args(&opt, ac, av);
	exit(0);
	fd = connect_to_daemon();
	while (std::cin.good()){
		std::cout << "$ ";
		std::cin >> buffer;
		send_message(fd, buffer);
		if (buffer == "quit")
			break ;
	}
	close(fd);
}
