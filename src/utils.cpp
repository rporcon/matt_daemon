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
