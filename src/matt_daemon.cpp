#include "matt_daemon.hpp"

void matt_daemon()
{
    pid_t 				pid;
	struct sigaction	sigact;

	memset(&sigact, 0, sizeof sigact);
    pid = fork();
    if (pid < 0)
        exit(1);
    if (pid > 0)
        exit(0);

    if (setsid() < 0)
        exit(1);
	sigact.sa_handler = SIG_IGN;
	sigaction(SIGCHLD, &sigact, NULL);
	sigaction(SIGHUP, &sigact, NULL);

    pid = fork();
    if (pid < 0)
        exit(1);
    if (pid > 0)
        exit(0);
    umask(0);
    chdir("/");
}

int main(void) {
	Server				serv;
	struct sigaction	sigact;

	memset(&sigact, 0, sizeof sigact);
	if (getuid() != 0) {
		std::cerr << "Error: must be root" << std::endl;
		exit(1);
	}

	Tintin_reporter::getInstance().log("starting daemon");
	matt_daemon();
	init_sigfd();
	g_lock_fd = open("/var/lock/matt_daemon.lock", O_CREAT);
	if (g_lock_fd == -1) {
		if (errno == EACCES)
			err_exit("lock already there");
		else
			perr_exit("open");
	}
	if (flock(g_lock_fd, LOCK_EX) == -1)
		perr_exit("flock lock");
	sigact.sa_handler = &close_server;
	sigaction(SIGINT, &sigact, NULL);

	serv.server_create(4242);
	serv.accept_clt_sock();
	return (0);
}
