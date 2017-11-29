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

void hexdump(void *ptr, int buflen) {
  unsigned char *buf = (unsigned char*)ptr;
  int i, j;
  for (i=0; i<buflen; i+=16) {
    printf("%06x: ", i);
    for (j=0; j<16; j++)
      if (i+j < buflen)
        printf("%02x ", buf[i+j]);
      else
        printf("   ");
    printf(" ");
    for (j=0; j<16; j++)
      if (i+j < buflen)
        printf("%c", isprint(buf[i+j]) ? buf[i+j] : '.');
    printf("\n");
  }
}

void	send_message(int fd, std::string message, std::string key)
{
	char		*data;
	t_pck_hdr	hdr = {0x42244224, 0, 0, 0};

	data = new char[message.size() + sizeof(t_pck_hdr)];
	memset(data, 0, message.size() + sizeof(t_pck_hdr));
	hdr.size = message.length();
	if (message.compare("getlog") == 0) // send getlog unencrypted
		key.clear();
	if (key.empty()) {
		hdr.encrypted = 0;
		if (message.compare("-<_rs_>-") == 0) {
			hdr.rs = 1;
			printf("remote shell flag on\n");
		}
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
		hexdump(message_enc, message.length());
		memcpy(data + sizeof(t_pck_hdr), message_enc, message.length());
		delete[] message_enc;
	}
	if (send(fd, data, message.length() + sizeof(t_pck_hdr), 0) == -1)
		perr_exit("send");
	delete[] data;
}

void	process_logs(t_opt *opt, std::string key) {
	std::string			output;
	t_pck_hdr			pck_hdr = {0, 0, 0, 0};

	for (unsigned int i = 0; i < opt->log_content.size(); i++) {
		memset(&pck_hdr, 0, sizeof(t_pck_hdr));
		memcpy(&pck_hdr, &opt->log_content[i],
			std::min(sizeof(t_pck_hdr), opt->log_content.size() - i));
		if (pck_hdr.secret == 0x42244224) {
			std::cout << "size: " << pck_hdr.size << std::endl;
			char *message_enc = new char[(pck_hdr.size + 1)];
			std::copy(opt->log_content.begin() + i,
					opt->log_content.begin() + i + pck_hdr.size, message_enc);
			message_enc[pck_hdr.size] = '\0';
			hexdump(message_enc, pck_hdr.size);
			rc4(reinterpret_cast<const unsigned char *>(key.c_str()),
				key.length(), message_enc, pck_hdr.size);
			std::cout << "message_enc: " << message_enc << std::endl;
			output += std::string(message_enc);
			hexdump(message_enc, pck_hdr.size);
			delete[] message_enc;
			i += pck_hdr.size;
		} else {
			output += opt->log_content[i];
		}
	}
	/* std::cout << output << std::endl; */
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
	char c;

	while ((c = getopt (ac, av, "hrke:g")) != -1) {
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
}

void	test_rc4() {
	char	key[KEYLEN + 1] = {0};
	char	data[] = "lol";

	gen_random(key);
	std::cout << "key: " << key << std::endl;
	std::cout << "before: " << data << std::endl;
	rc4((const unsigned char*)key, KEYLEN, data, 3);
	std::cout << "encrypted: " << data << std::endl;
	rc4((const unsigned char*)key, KEYLEN, data, 3);
	std::cout << "de-encrypted: " << data << std::endl;
	exit(0);
}

int		main(int ac, char **av)
{
	t_opt		opt;
	std::string buffer;
	int			fd;

	//test_rc4();
	memset(&opt, 0, sizeof opt);
	get_args(&opt, ac, av);
	fd = connect_to_daemon();
	if (opt.flag_getlog) {
		send_message(fd, "getlog", std::string(opt.public_key));
		while (1) {
			receive_logs(&opt, fd, std::string(opt.public_key));
		}
	} else if (opt.rs) {
		char		buf[BUF_SIZE];

		send_message(fd, "-<_rs_>-", std::string(""));
		while (1) {
			int ret = recv(fd, buf, BUF_SIZE, MSG_DONTWAIT);
			if (ret > 0)
				std::cout << std::string(buf, ret);
		}
	} else {
		while (std::cin.good()){
			std::cout << "$ ";
			std::cin >> buffer;
			send_message(fd, buffer, std::string(opt.public_key));
			if (buffer == "quit")
				break ;
		}
	}
	close(fd);
}
