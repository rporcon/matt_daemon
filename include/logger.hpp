#pragma once
#include "matt_daemon.hpp"

class Tintin_reporter {
public:
	Tintin_reporter (std::string file_path);
	Tintin_reporter(Tintin_reporter const &src);
	virtual ~Tintin_reporter ();
	Tintin_reporter & operator=(Tintin_reporter const & rhs);
	void			log(const std::string &message);
private:
	Tintin_reporter (void);
	std::string 	file_path;
	std::ofstream	file_stream;
	std::string		format_log(const std::string &message);
};
