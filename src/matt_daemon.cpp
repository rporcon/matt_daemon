#include "matt_daemon.hpp"

void skeleton_daemon()
{
    pid_t pid;

    /* Fork off the parent process */
    pid = fork();

    /* An error occurred */
    if (pid < 0)
        exit(EXIT_FAILURE);

    /* Success: Let the parent terminate */
    if (pid > 0)
        exit(EXIT_SUCCESS);

    /* On success: The child process becomes session leader */
    if (setsid() < 0)
        exit(EXIT_FAILURE);

    /* Catch, ignore and handle signals */
    //TODO: Implement a working signal handler */
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);

    /* Fork off for the second time*/
    pid = fork();

    /* An error occurred */
    if (pid < 0)
        exit(EXIT_FAILURE);

    /* Success: Let the parent terminate */
    if (pid > 0)
        exit(EXIT_SUCCESS);

    /* Set new file permissions */
    umask(0);

    /* Change the working directory to the root directory */
    /* or another appropriated directory */
    chdir("/");

    /* Close all open file descriptors */
    int x;
    for (x = sysconf(_SC_OPEN_MAX); x>=0; x--)
    {
        close (x);
    }

    /* Open the log file */
    /* openlog ("firstdaemon", LOG_PID, LOG_DAEMON); */
}

int main(void) {
	Server				serv;
	struct sigaction	sigact;
	Tintin_reporter logger = Tintin_reporter("/var/log/matt_daemon/matt_daemon.log");

	memset(&sigact, 0, sizeof sigact);
	if (getuid() != 0) {
		std::cerr << "Error: must be root" << std::endl;
		exit(1);
	}
	logger.log("starting daemon");

	/* skeleton_daemon(); */
	/* sleep(20); */
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
