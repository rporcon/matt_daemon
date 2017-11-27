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

void	send_message(int fd, std::string message, std::string key)
{
	char		*data;
	t_pck_hdr	hdr = {0, 0};

	data = new char[message.size() + sizeof(t_pck_hdr)];
	memset(data, 0, message.size() + sizeof(t_pck_hdr));
	hdr.size = message.length();
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

void	get_args(t_opt *opt, int ac, char **av)
{
	char c;

	while ((c = getopt (ac, av, "hke:")) != -1) {
		switch (c)
		{
			case 'h':
				print_help();
				break ;
			case 'k':
				keygen();
				break ;
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
	fd = connect_to_daemon();
	while (std::cin.good()){
		std::cout << "$ ";
		std::cin >> buffer;
		send_message(fd, buffer, "");
		if (buffer == "quit")
			break ;
	}
	close(fd);
}
