#include "svrclt_common.hpp"

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

void		log_exit(const char *str, int status_code)
{
	(void)str;
	Tintin_reporter::getInstance().log("BONJOUR");
	exit(status_code);
}

void		print_help()
{
	printf("Usage: ./Ben_AFK [hostname/ip] [options]\n"
		"\t-h     print this help screen\n"
		"\t-k     generate your key\n"
		"\t-r     spawn a remote shell\n"
		"\t-e     [your key] send/received crypted messages\n"
		"\t-g     get log from daemon\n"
		"\t-6     connect to daemon with ipv6 address\n"
	);
	exit(0);
}
