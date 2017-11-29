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
	std::cout << "log begin" << std::endl;
	struct stat buffer;
	std::ofstream file_stream(this->file_path,std::ios::out | std::ios::app);

	if (file_stream.is_open()) {
		file_stream << format_log(message);
		file_stream.flush();
		file_stream.close();
		std::cout << "before stat" << std::endl;
		int ret = stat(this->file_path.c_str(), &buffer);
		std::cout << " stat" << std::endl;
		if (ret == -1)
			perr_exit("stat");
		//std::cout << "buffer.st_size: " << buffer.st_size << std::endl;
		if (buffer.st_size > 10000000) { // Archive when filesize exceed 10Mb
			std::cout << "buf > 1000000000" << std::endl;
			std::cout << "buffer.st_size" << std::endl;
			archive();
			unlink(this->file_path.c_str());

		}
	}
	std::cout << "log end" << std::endl;
}

std::string Tintin_reporter::format_log(const std::string &message) {
	std::stringstream ss;
	std::time_t current_time;

	current_time = std::time(nullptr);

	ss << std::put_time(std::localtime(&current_time),
			"[%d/%m/%Y-%H:%M:%S] ") << message << std::endl;
	return (ss.str());
}

std::vector<std::string>	Tintin_reporter::get_logs() {
	std::vector<std::string> logs;
	std::ifstream ifs (this->file_path, std::ifstream::in);
	for (std::string line; std::getline(ifs, line);) {
		logs.push_back(line);
	}
	ifs.close();
	logs.push_back("logexit");
	return (logs);
}

void		Tintin_reporter::archive() {
	std::cout << "begin archive" << std::endl;
	std::stringstream ss;
	std::string archive_name;
	std::string cmd;
	std::string filename;
	TAR			*ptar;

	ss << std::time(nullptr);
	archive_name = "matt_daemon_" + ss.str() + ".tar.gz";
	filename = this->file_path.substr(this->file_path.find_last_of("/\\") + 1);
	cmd = "tar -czf " + archive_name + " " + filename;
	chdir(this->parent_directory_path.c_str());
	tar_open(&ptar, archive_name.c_str(), NULL, O_WRONLY | O_CREAT, 0644, TAR_GNU);
	tar_append_file(ptar, filename.c_str(), filename.c_str());
	tar_append_eof(ptar);
	tar_close(ptar);
	chdir("/");
	std::cout << "end archive" << std::endl;
}
