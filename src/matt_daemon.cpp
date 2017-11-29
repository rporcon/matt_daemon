#include "matt_daemon.hpp"

void matt_daemon()
{
    pid_t 				pid;
	struct sigaction	sigact;

	memset(&sigact, 0, sizeof sigact);
    pid = fork();
    if (pid < 0) {
		close_server(-1);
	}
    if (pid > 0) {
        exit(0);
	}

    if (setsid() < 0) {
		close_server(-1);
	}
	sigact.sa_handler = SIG_IGN;
	sigaction(SIGCHLD, &sigact, NULL);
	sigaction(SIGHUP, &sigact, NULL);

    pid = fork();
    if (pid < 0) {
		close_server(-1);
	}
    if (pid > 0) {
        exit(0);
	}
    umask(0);
    chdir("/");
}

int main(void) {
	Server				serv;
	struct sigaction	sigact;

	if (getuid() != 0) {
		std::cerr << "Error: must be root" << std::endl;
		exit(1);
	}
	init_sigfd();
	g_lock_fd = open("/var/lock/matt_daemon.lock", O_CREAT);
	if (flock(g_lock_fd, LOCK_EX | LOCK_NB) == -1) {
		Tintin_reporter::getInstance().log("cannot launch matt_daemon (because of lock)");
		err_exit("cannot launch matt_daemon (because of lock)");
	}

	Tintin_reporter::getInstance().log("starting daemon");
	matt_daemon();

	memset(&sigact, 0, sizeof sigact);
	sigact.sa_handler = &close_server;
	for (unsigned long i = 1; i < _NSIG; i++) {
		if (i != SIGCHLD || i != SIGHUP || (i > 31 && i < 34)) {
			sigaction(i, &sigact, NULL);
		}
	}

	serv.create(4242);
	/* serv.create_localhost(4242); */
	serv.accept_clt_sock();
	return (0);
}
