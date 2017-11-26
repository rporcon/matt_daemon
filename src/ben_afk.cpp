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

int		main(void)
{
	std::string buffer;
	int			fd;

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
