#pragma once

#include <vector>
#include <string>
#include <ctime>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <libtar.h> // need libtar-dev installed
#include <unistd.h>
#include <fcntl.h>

class Tintin_reporter {
public:
	static Tintin_reporter& getInstance() {
		static Tintin_reporter instance("/var/log/matt_daemon/matt_daemon.log");
		return (instance);
	}
	void						log(const std::string &message);
	std::vector<std::string>	get_logs();

private:
	Tintin_reporter(void);
	Tintin_reporter(std::string file_path);
	Tintin_reporter(Tintin_reporter const &src);
	Tintin_reporter & operator=(Tintin_reporter const & rhs);
	virtual ~Tintin_reporter ();

	std::string 	file_path;
	std::string		parent_directory_path;
	std::string		format_log(const std::string &message);
	void			archive();
};
