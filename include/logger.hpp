#pragma once
#include "matt_daemon.hpp"

class Tintin_reporter {
public:
	static Tintin_reporter& getInstance() {
		static Tintin_reporter instance("/var/log/matt_daemon/matt_daemon.log");
		return (instance);
	}
	void			log(const std::string &message);
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
