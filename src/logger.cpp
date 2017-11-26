#include "logger.hpp"

Tintin_reporter::Tintin_reporter(){

}

Tintin_reporter::Tintin_reporter(std::string file_path) : file_path(file_path) {
	struct stat buffer;

	this->parent_directory_path =
		file_path.substr(0, file_path.find_last_of("/\\") + 1);
	if (stat(file_path.c_str(), &buffer) != 0) {
		if (stat(this->parent_directory_path.c_str(), &buffer) != 0) {
			if (mkdir(this->parent_directory_path.c_str(), 0700) != 0)
				std::cerr << "Error: cannot create directory" << std::endl;
		}
	}
	this->file_stream = std::ofstream(this->file_path,
			std::ios::out | std::ios::app);
}

Tintin_reporter::Tintin_reporter(Tintin_reporter const & src) {
	*this = src;
}

Tintin_reporter::~Tintin_reporter() {
	this->file_stream.flush();
	this->file_stream.close();
}

Tintin_reporter & Tintin_reporter::operator=(Tintin_reporter const & rhs) {
	if (this != &rhs) {
		this->file_path = rhs.file_path;
		this->file_stream = std::ofstream(rhs.file_path,
				std::ios::out | std::ios::app);
	}
	return (*this);
}

void Tintin_reporter::log(const std::string &message) {
	struct stat buffer;
	this->file_stream << format_log(message);
	this->file_stream.flush();
	archive();
	stat(this->file_path.c_str(), &buffer);
	if (buffer.st_size > 10000000) // Archive when filesize exceed 10Mb
		archive();
}

std::string Tintin_reporter::format_log(const std::string &message) {
	std::stringstream ss;
	std::time_t current_time;

	current_time = std::time(nullptr);

	ss << std::put_time(std::localtime(&current_time),
			"[%d/%m/%Y - %H:%M:%S] ") << message << std::endl;
	return (ss.str());
}

void		Tintin_reporter::archive() {
	std::stringstream ss;
	std::string archive_name;
	std::string cmd;

	this->file_stream.close();
	ss << std::time(nullptr);
	archive_name = this->parent_directory_path +
		"matt_daemon_" + ss.str() + ".tar.gz";
	cmd = "tar -czf "  + archive_name + " " + this->file_path;
	std::cout << "archive"<< std::endl;
	system(cmd.c_str());
}
