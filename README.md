# matt_daemon

**Work only on linux**

The aim of this project of this project, is to create a daemon and a client that will be able to connect to it from everywhere and who can ask him to do some action (for example spawn a remote shell).

The daemon is `Matt_daemon` and have to be launched with `sudo`

The client is `Ben_afk`:

```
Usage: ./Ben_AFK [hostname/ip] [options]
	-h     print this help screen
	-k     generate your key
	-r     spawn a remote shell
	-e     [your key] send/received crypted messages
	-g     get log from daemon
	-6     connect to daemon with ipv6 address
```

More details in **Matt_daemon.fr.pdf** (in french)
