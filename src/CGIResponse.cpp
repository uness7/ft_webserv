#include "../inc/CGIResponse.hpp"
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <iterator>
#include <sys/_select.h>

CGIResponse::CGIResponse(Client *client, std::string &cgi_path)
    : _client(client), _cgiPath(cgi_path), _envp(NULL), _pid(-1),
      _statusCode(0) {
  this->_scriptPath = "." + this->_client->getRequest().getPath();
  setCgiEnv();
  setArgv();
}

void CGIResponse::setArgv() {
  _argv[0] = (char *)_cgiPath.c_str();
  _argv[1] = (char *)_scriptPath.c_str();
  _argv[2] = NULL;
}

bool CGIResponse::canExecScript() {
  if (_scriptPath == "")
    return false;
  if (access(_scriptPath.c_str(), F_OK) == -1) {
    return false;
  }
  return true;
}

void CGIResponse::setCgiEnv() {
  Request &req = _client->getRequest();
  _envMap["GATEWAY_INTERFACE"] = "CGI/1.1";
  _envMap["SCRIPT_FILENAME"] = _cgiPath;
  _envMap["SERVER_PROTOCOL"] = req.getHttpv();
  _envMap["REDIRECT_STATUS"] = "200";
  _envMap["REQUEST_METHOD"] = req.getMethod();
  _envMap["CONTENT_TYPE"] = req.getHeaderField("content-type");
  _envMap["CONTENT_LENGTH"] = req.getHeaderField("content-length");
  _envMap["HTTP_COOKIE"] = req.getHeaderField("cookie");
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

void CGIResponse::clear() {
  close(_pipe_out[0]);
  close(_pipe_out[1]);
  close(_pipe_in[0]);
  close(_pipe_in[1]);
  if (_envp) {
    for (size_t i = 0; _envp[i] != NULL; i++)
      free(_envp[i]);
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
    if (!body.empty())
      dup2(_pipe_in[0], STDIN_FILENO);
    close(_pipe_in[0]);
    int exit_status = execve(_cgiPath.c_str(), _argv, _envp);
    exit(exit_status);
  } else {
    close(_pipe_out[1]);
    close(_pipe_in[0]);

    if (!body.empty()) {
      ssize_t written = write(_pipe_in[1], body.data(), body.size());
      if (written == -1) {
        std::cerr << "Erreur lors de l'écriture dans le pipe\n";
        std::cerr << "error: " << strerror(errno) << std::endl;
        clear();
        _statusCode = 500;
        return;
      }
    }

    close(_pipe_in[1]);
  }
}

std::string CGIResponse::exec(unsigned short &code) {
  if (!canExecScript())
    return (code = 404, "");

  runProcess();
  if (_statusCode)
    return (code = _statusCode, "");

  std::string cgiResponse;
  fd_set readfds;
  struct timeval timeout = {
      .tv_sec = 1,
      .tv_usec = 0,
  };

  FD_ZERO(&readfds);
  FD_SET(_pipe_out[0], &readfds);

  int retval = select(_pipe_out[0] + 1, &readfds, NULL, NULL, &timeout);
  if (retval == -1) {
    clear();
    code = 500;
    return ("");
  } else if (retval == 0) {
    if (kill(_pid, SIGKILL) == -1) {
      std::cerr << "Erreur lors de la tentative de tuer le processus enfant.\n";
      clear();
      code = 500;
      return "";
    }

    // Attendre que le processus enfant soit effectivement tué
    int status;
    waitpid(_pid, &status, 0);
    std::cerr << "Le processus enfant a été tué en raison du timeout.\n";

    // Code HTTP 504 : Gateway Timeout
    clear();
    code = 504;
    return "";
  } else {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    while ((bytes_read = read(_pipe_out[0], buffer, BUFFER_SIZE)) > 0) {
      cgiResponse.append(buffer, bytes_read);
    }
  }

  // Fermer manuellement le pipe après la lecture pour éviter un blocage
  close(_pipe_out[0]);

  // Attendre que le processus enfant se termine (bloquant cette fois)
  int status;
  pid_t result =
      waitpid(_pid, &status, 0); // Bloquant jusqu'à la fin de l'enfant
  if (result == -1) {
    std::cerr << "Erreur lors de l'attente du processus enfant\n";
    clear();
    code = 500;
    return "";
  }

  // Vérification du statut de l'enfant
  if (WIFEXITED(status)) {
    int exit_status = WEXITSTATUS(status);
    std::cout << "Le processus enfant s'est terminé avec le code: "
              << exit_status << std::endl;
    if (exit_status != 0) {
      code = 500; // L'enfant s'est terminé avec une erreur
    } else {
      code = 0; // Succès
      std::cout << "END" << std::endl;

      // Fermer les autres ressources ici
      clear();

      // Ajoute un log après la libération des ressources
      std::cout << "Ressources libérées avec succès" << std::endl;

      return cgiResponse;
    }
  } else if (WIFSIGNALED(status)) {
    std::cerr << "Le processus enfant a été tué par un signal: "
              << WTERMSIG(status) << std::endl;
    code = 500; // L'enfant a été tué par un signal
  }
  clear();
  return cgiResponse;
}
