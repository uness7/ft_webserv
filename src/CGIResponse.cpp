#include "../inc/CGIResponse.hpp"
#include <cstring>
#include <iterator>

CGIResponse::CGIResponse(Client *client, std::string &cgi_path)
    : _client(client), _cgiPath(cgi_path) {
  this->_scriptPath = "." + this->_client->getRequest().getPath();
  setCgiEnv();
}
bool CGIResponse::canExecFile() {
  if (_scriptPath == "")
    return false;
  if (access(_scriptPath.c_str(), F_OK) == -1) {
    return false;
  }
  return true;
}

void CGIResponse::setCgiEnv() {
  _envMap["GATEWAY_INTERFACE"] = "CGI/1.1";
  _envMap["SERVER_PROTOCOL"] = "HTTP/1.1";
  _envMap["REDIRECT_STATUS"] = "200"; // this must be the value sent by python scripts
  _envMap["REQUEST_METHOD"] = _client->getRequest().getMethod();
  _envMap["CONTENT_TYPE"] =
      _client->getRequest().getHeaderField("content-type");
  _envMap["CONTENT_LENGTH"] =
      _client->getRequest().getHeaderField("content-length");
  _envMap["HTTP_COOKIE"] = _client->getRequest().getHeaderField("cookie");
}

char **mapToEnvArray(const std::map<std::string, std::string> &envMap) {
  char **envArray = new char *[envMap.size() + 1];
  int i = 0;

  for (std::map<std::string, std::string>::const_iterator it = envMap.begin();
       it != envMap.end(); ++it) {
    std::string envEntry = it->first + "=" + it->second;
    envArray[i] = strdup(envEntry.c_str());
    i++;
  }
  envArray[i] = NULL;
  return envArray;
}

std::string CGIResponse::execute(void) {
  if (!canExecFile())
    return "";
  char *const argv[] = {(char *)_cgiPath.c_str(), (char *)_scriptPath.c_str(),
                        NULL};
  char **envp = mapToEnvArray(_envMap);
  int out_pipe[2];
  int in_pipe[2];
  pid_t pid;

  if (pipe(out_pipe) == -1 || pipe(in_pipe) == -1) {
    perror("pipe");
    return "Internal Server Error";
  }
  pid = fork();
  if (pid < 0) {
    perror("fork failed");
    return "Internal Server Error";
  } else if (pid == 0) {
    dup2(out_pipe[1], STDOUT_FILENO);
    close(out_pipe[0]);
    close(out_pipe[1]);
    if (_envMap["REQUEST_METHOD"] == "POST") {
      close(in_pipe[1]);
      dup2(in_pipe[0], STDIN_FILENO);
      close(in_pipe[0]);
    } else {
      close(in_pipe[0]);
      close(in_pipe[1]);
    }
    execve(_cgiPath.c_str(), argv, envp);
    perror("execve : ");
    exit(1);
  } else {
    close(out_pipe[1]);
    if (_envMap["REQUEST_METHOD"] == "POST") {
      close(in_pipe[0]);
      write(in_pipe[1], _client->getRequest().getBody().data(),
            _client->getRequest().getBody().size());
      close(in_pipe[1]);
    } else {
      close(in_pipe[0]);
      close(in_pipe[1]);
    }
    char buffer[2048];
    std::string res;
    ssize_t bytes_read;
    while ((bytes_read = read(out_pipe[0], buffer, sizeof(buffer))) > 0)
      res.append(buffer, bytes_read);
    close(out_pipe[0]);
    int status;
    waitpid(pid, &status, 0);
    for (size_t i = 0; envp[i] != NULL; i++)
      free(envp[i]);
    delete[] envp;
    return res;
  }
}
