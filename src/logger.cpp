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
}

Tintin_reporter::Tintin_reporter(Tintin_reporter const & src) {
	*this = src;
}

Tintin_reporter::~Tintin_reporter() {

}

Tintin_reporter & Tintin_reporter::operator=(Tintin_reporter const & rhs) {
	if (this != &rhs) {
		this->file_path = rhs.file_path;
		this->parent_directory_path = rhs.parent_directory_path;
	}
	return (*this);
}

void Tintin_reporter::log(const std::string &message) {
	struct stat buffer;
	std::ofstream file_stream(this->file_path,std::ios::out | std::ios::app);

	file_stream << format_log(message);
	file_stream.flush();
	stat(this->file_path.c_str(), &buffer);
	if (buffer.st_size > 10000000) // Archive when filesize exceed 10Mb
		archive();
	file_stream.close();
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
	std::string filename;

	ss << std::time(nullptr);
	archive_name = "matt_daemon_" + ss.str() + ".tar.gz";
	filename = this->file_path.substr(this->file_path.find_last_of("/\\") + 1);
	cmd = "tar -czf " + archive_name + " " + filename
		+ " -C " + this->parent_directory_path;
	system(cmd.c_str());
}
