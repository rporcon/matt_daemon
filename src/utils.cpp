#include "matt_daemon.hpp"

void		perr_exit(const char *str)
{
	perror(str);
	exit(1);
}

void		err_exit(const char *err_msg)
{
	if (err_msg != NULL)
		fprintf(stderr, "Error: %s\n", err_msg);
	exit(1);
}

void		print_help()
{
	printf("Usage: ft_nmap [options]\n"
		"\t-h     print this help screen\n"
		"\t-k     generate public and private key\n"
		"\t-e     [your public key] send crypted messages\n"
	);
	exit(0);
}
