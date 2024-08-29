#include "CGIResponse.hpp"
# include <vector>
# include <iomanip>

CGIResponse::CGIResponse(Client *client) : _client(client), _cgiPath("/usr/bin/python3")
{
	this->_scriptPath = Utils::getCgiScriptForKey(this->_client->getConfig(), "/");
	setCgiEnv();
}

std::string     intToString(int value)
{
        std::ostringstream      oss;
        oss << value;
        return oss.str();
}

void	CGIResponse::setCgiEnv()
{
	_envMap["GATEWAY_INTERFACE"] = "CGI/1.1";
	_envMap["SERVER_PROTOCOL"] = "HTTP/1.1";
	_envMap["REDIRECT_STATUS"] = "200";
	_envMap["REQUEST_METHOD"] = _client->getRequest().getMethod();
	_envMap["CONTENT_TYPE"] = _client->getRequest().getHeaderField("content-type");
	_envMap["CONTENT_LENGTH"] = _client->getRequest().getHeaderField("content-length");
}

char 	**mapToEnvArray(const std::map<std::string, std::string> &envMap)
{
	char	**envArray = new char*[envMap.size() + 1];
	int 	i = 0;

	for (std::map<std::string, std::string>::const_iterator it = envMap.begin(); it != envMap.end(); ++it)
	{
		std::string envEntry = it->first + "=" + it->second;
		envArray[i] = strdup(envEntry.c_str());
		i++;
	}
	envArray[i] = NULL;
	return envArray;
}

bool contains_null_terminator(const char *buffer, size_t length) {
	for (size_t i = 0; i < length; ++i) {
		if (buffer[i] == '\0') {
			return true;  // Null terminator found
		}
	}
	return false;  // No null terminator found
}

void	printAsHex(std::vector<char>& vec)
{
	for (size_t i = 0; i < vec.size(); ++i)
	{
		unsigned char ch = vec[i];
		std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(ch) << " ";
	}
	std::cout << std::dec << std::endl;
}

std::string 	CGIResponse::execute(void)
{
	char	*const argv[] = {
		(char *)_cgiPath.c_str(), 
		(char *)_scriptPath.c_str(), NULL
	};
	char	**envp = mapToEnvArray(_envMap);
	int	out_pipe[2];
	int	in_pipe[2];
	pid_t	pid;

	std::cout << "Tourabi" <<_client->getRequest().getBody().data() << "tourabi" << std::endl;
	if (pipe(out_pipe) == -1 || pipe(in_pipe) == -1)
	{
		perror("pipe");
		return "Internal Server Error";
	}
	pid = fork();
	if (pid < 0)
	{
		perror("fork failed");
		return "Internal Server Error";
	}
	else if (pid == 0)
	{
		close(out_pipe[0]);
		 dup2(out_pipe[1], STDOUT_FILENO);
		 close(out_pipe[1]);

		 if (_envMap["REQUEST_METHOD"] == "POST")
		 {
			 close(in_pipe[1]);
			 dup2(in_pipe[0], STDIN_FILENO);
			 close(in_pipe[0]);
		 }
		 else
		 {
			 close(in_pipe[0]);
			 close(in_pipe[1]);
		 }

		 // printinf env variables for debuggin
//		 for (std::map<std::string, std::string>::iterator it = _envMap.begin(); it != _envMap.end(); ++it)
//			 std::cerr << it->first << "=" << it->second << std::endl;
		 execve(_cgiPath.c_str(), argv, envp);
		 perror("execve : ");
		 exit(1);
	}
	else
	{
		close(out_pipe[1]);
	
		if (_envMap["REQUEST_METHOD"] == "POST")
		{
			close(in_pipe[0]);
			write(
				in_pipe[1], 
				_client->getRequest().getBody().data(),
				_client->getRequest().getBody().size()
			);
			close(in_pipe[1]);
		}
		else
		{
			close(in_pipe[0]);
			close(in_pipe[1]);
		}

		char	buffer[2048];
		std::string	res;
		ssize_t		bytes_read;

		while ((bytes_read = read(out_pipe[0], buffer, sizeof(buffer))) > 0)
			res.append(buffer, bytes_read);
		close(out_pipe[0]);
		int	status;
		waitpid(pid, &status, 0);

		for (size_t i = 0; envp[i] != NULL; i++)
			free(envp[i]);
		delete[] envp;
		return res;
	}
}
