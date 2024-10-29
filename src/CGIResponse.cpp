#include "CGIResponse.hpp"

CGIResponse::CGIResponse(Client *client, std::string &cgi_path)
    : _client(client),
      _cgiPath(cgi_path),
      _envp(NULL),
      _pid(-1),
      _statusCode(200) {
	this->_scriptPath = "." + this->_client->getRequest().getPath();
	initCgiEnv();
	setArgv();
}

void CGIResponse::setArgv() {
	_argv[0] = (char *)_cgiPath.c_str();
	_argv[1] = (char *)_scriptPath.c_str();
	_argv[2] = NULL;
}

void CGIResponse::initCgiEnv() {
	Request &req = _client->getRequest();
	_envMap["GATEWAY_INTERFACE"] = "CGI/1.1";
	_envMap["SCRIPT_FILENAME"] = _cgiPath;
	_envMap["SERVER_PROTOCOL"] = req.getHttpv();
	_envMap["REDIRECT_STATUS"] = "200";
	_envMap["REQUEST_METHOD"] = req.getMethod();
	_envMap["CONTENT_TYPE"] = req.getHeaderField("content-type");
	_envMap["CONTENT_LENGTH"] = req.getHeaderField("content-length");
	_envMap["HTTP_COOKIE"] = req.getHeaderField("cookie");
	_envMap["QUERY_STRING"] = req.getQuery();
}

char **mapToEnvArray(const std::map<std::string, std::string> &envMap) {
	char **envArray = new char *[envMap.size() + 1];
	int i = 0;

	for (std::map<std::string, std::string>::const_iterator it =
		 envMap.begin();
	     it != envMap.end(); ++it) {
		std::string envEntry = it->first + "=" + it->second;
		envArray[i] = strdup(envEntry.c_str());
		i++;
	}
	envArray[i] = NULL;
	return envArray;
}

void CGIResponse::clear() {
	close(_pipe_out[0]);
	close(_pipe_out[1]);
	close(_pipe_in[0]);
	close(_pipe_in[1]);
	if (_envp) {
		for (size_t i = 0; _envp[i] != NULL; i++) free(_envp[i]);
		delete[] _envp;
		_envp = NULL;
	}
}

void CGIResponse::runProcess() {
	signal(SIGPIPE, SIG_IGN);

	if (pipe(_pipe_in) < 0) {
		_statusCode = 500;
		return;
	}

	if (pipe(_pipe_out) < 0) {
		clear();
		_statusCode = 500;
		return;
	}

	_envp = mapToEnvArray(_envMap);
	_pid = fork();

	Bytes &body = _client->getRequest().getBody();

	if (_pid < 0) {
		clear();
		_statusCode = 500;
		return;
	} else if (_pid == 0) {
		dup2(_pipe_out[1], STDOUT_FILENO);
		close(_pipe_out[0]);
		close(_pipe_out[1]);
		close(_pipe_in[1]);
		if (!body.empty()) dup2(_pipe_in[0], STDIN_FILENO);
		close(_pipe_in[0]);
		int exit_status = execve(_cgiPath.c_str(), _argv, _envp);
		exit(exit_status);
	} else {
		close(_pipe_out[1]);
		close(_pipe_in[0]);

		if (!body.empty() &&
		    write(_pipe_in[1], body.data(), body.size()) == -1) {
			std::cerr << "[ERROR]: FD -> " << _client->getFd()
				  << " - Impossible to use write function -> "
				  << strerror(errno) << std::endl;
			clear();
			_statusCode = 500;
			return;
		}

		close(_pipe_in[1]);
	}
}

std::string CGIResponse::getScriptResult() {
	std::string cgiResponse;
	fd_set readfds;
	struct timeval timeout = {
	    .tv_sec = 3,
	    .tv_usec = 0,
	};

	FD_ZERO(&readfds);
	FD_SET(_pipe_out[0], &readfds);

	int retval = select(_pipe_out[0] + 1, &readfds, NULL, NULL, &timeout);
	if (retval == -1) {
		clear();
		_statusCode = 500;
		return ("");
	} else if (retval == 0) {
		if (kill(_pid, SIGKILL) == -1) {
			std::cerr << "[ERROR]: FD -> " << _client->getFd()
				  << " - Impossible to kill child process\n";
			clear();
			_statusCode = 500;
			return "";
		}

		int status;
		waitpid(_pid, &status, 0);
		std::cerr << "[ERROR]: FD -> " << _client->getFd()
			  << " - child process is killed after timeout\n";
		clear();
		_statusCode = 504;
		return "";
	} else {
		char buffer[BUFFER_SIZE];
		ssize_t bytes_read;
		while ((bytes_read = read(_pipe_out[0], buffer, BUFFER_SIZE)) >
		       0) {
			cgiResponse.append(buffer, bytes_read);
		}
	}

	close(_pipe_out[0]);

	int status;
	if (waitpid(_pid, &status, 0) == -1) {
		std::cerr << "[ERROR]: FD -> " << _client->getFd()
			  << " - error occured on waitpid syscall -> "
			  << strerror(errno) << std::endl;
		clear();
		_statusCode = 500;
		return "";
	}

	if (WIFEXITED(status)) {
		if (WEXITSTATUS(status) != 0) {
			_statusCode = 500;
		} else {
			_statusCode = 200;
			clear();
			return cgiResponse;
		}
	} else if (WIFSIGNALED(status)) {
		std::cerr << "[ERROR]: FD -> " << _client->getFd()
			  << " - child process was killed with signal "
			  << WTERMSIG(status) << std::endl;
		_statusCode = 500;
	}
	clear();
	return cgiResponse;
}

std::string CGIResponse::exec(unsigned short &code) {
	if (_cgiPath.empty() || access(_cgiPath.c_str(), F_OK) == -1) {
		return (code = 500, "");
	}
	if (_scriptPath.empty() || access(_cgiPath.c_str(), F_OK) == -1) {
		return (code = 404, "");
	}

	runProcess();
	if (_statusCode != 200) return (code = _statusCode, "");

	std::string response = getScriptResult();
	code = _statusCode;

	return response;
}
