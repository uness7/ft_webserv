#include "../inc/Response.hpp"
#include "../inc/CGIResponse.hpp"
#include <cstdio>
#include <string>

Response::Response() : _client(NULL) {}

Response::Response(Client *client) : _client(client) { this->build(); }

Response::Response(const Response &cp) { *this = cp; }

Response &Response::operator=(const Response &rhs) {
  if (this != &rhs) {
    this->_value = rhs._value;
    this->_statusCode = rhs.getStatusCode();
    this->_contentType = rhs._contentType;
    this->_buffer = rhs._buffer;
    this->_client = rhs._client;
    this->_cookies = rhs._cookies;
    this->_target = rhs._target;
  }
  return *this;
}

Response::~Response() {}

void Response::updateResponse(unsigned short statusCode,
                              std::string contentType, std::string buffer) {
  setStatusCode(statusCode);
  _contentType = contentType;
  _buffer = buffer;
}

void Response::build(void) {
  Request &request = _client->getRequest();
  _target = request.getPathLocation();

  if (request.getStatusCode() != 0) {
    setStatusCode(request.getStatusCode());
    updateResponse(_statusCode.code, "text/plain", _statusCode.status);
    return finalizeHTMLResponse();
  }

  if (!_target.redirect.empty()) {
    setStatusCode(302);
    return finalizeHTMLResponse();
  }

  if (_target.content["autoindex"] == "on" &&
      request.getPath() == _target.root) {
    generateAutoIndex();
    updateResponse(200, "text/html", _buffer);
    return finalizeHTMLResponse();
  }

  if (_target.content["cgi_path"] != "") {
    std::string ext = _target.content["cgi_ext"];
    size_t dotPos = request.getPath().find_last_of('.');
    if (dotPos != std::string::npos) {
      std::string path_ext = request.getPath().substr(dotPos);
      if (ext.find(path_ext) != std::string::npos)
        return handleCGI();
    }
  }
  handleStaticFiles();
  finalizeHTMLResponse();
}

std::vector<std::string> splitString(const std::string &str) {
  std::vector<std::string> result;
  std::istringstream iss(str);
  std::string part;

  if (std::getline(iss, part, ' ')) {
    result.push_back(part);
    if (std::getline(iss, part))
      result.push_back(part);
  }
  return result;
}

void Response::buildError() {
  Request &request = _client->getRequest();

  std::string error = _client->getConfig().error_page;
  if (_target.content.count("error_page"))
    error = _target.content["error_page"];

  if (error.empty())
    return;
  std::vector<std::string> error_page = splitString(error);
  if (error_page.size() != 2)
    return;
  std::string newPath = error_page[1];
  _client->getRequest().setPath(newPath);
  std::stringstream buffer;
  std::ifstream inFile(std::string("." + request.getPath()).c_str());
  buffer << inFile.rdbuf();
  inFile.close();
  updateResponse(atoi(error_page[0].c_str()), request.getMimeType(),
                 buffer.str());
  return;
}

void Response::handleCGI() {
  CGIResponse cgi = CGIResponse(this->_client, _target.content["cgi_path"]);
  std::string resp = cgi.execute();

  if (!resp.empty()) {
    std::string headers;
    std::string body;
    std::size_t pos = resp.find("\r\n");
    if (pos != std::string::npos) {
      headers = resp.substr(0, pos);
      body = resp.substr(pos + 3);
    } else
      body = resp;
    std::istringstream headerStream(body);
    std::string line;

    while (std::getline(headerStream, line)) {
      if (line.find("Set-Cookie:") == 0)
        _cookies.push_back(line);
    }
    this->_value = resp;
  } else {
    buildError();
    finalizeHTMLResponse();
  }
}

void Response::handleStaticFiles(void) {
  Request &request = _client->getRequest();
  std::string newPath = request.getPath();
  std::string mimetype = request.getMimeType();
  std::ifstream inFile(std::string("." + newPath).c_str());
  std::stringstream buffer;

  if (inFile.is_open()) {
    buffer << inFile.rdbuf();
    inFile.close();
    updateResponse(200, request.getMimeType(), buffer.str());
  } else {
    buildError();
    if (_statusCode.code == 0) {
      setStatusCode(404);
      updateResponse(_statusCode.code, _statusCode.status, _statusCode.status);
    }
  }
}

std::string formatSize(off_t size) {
  const char *units[] = {"o", "Ko", "Mo", "Go"};
  int unitIndex = 0;
  double formattedSize = size;

  while (formattedSize >= 1024 && unitIndex < 3) {
    formattedSize /= 1024;
    unitIndex++;
  }

  std::ostringstream ss;
  ss.precision(2);
  ss << std::fixed << formattedSize << " " << units[unitIndex];
  return ss.str();
}

std::string formatTime(time_t time) {
  char buffer[80];
  struct tm *tm_info = localtime(&time);
  strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm_info);
  return std::string(buffer);
}

void Response::finalizeHTMLResponse(void) {
  std::ostringstream ss;
  const Request &req = _client->getRequest();

  ss << req.getHttpv() << " " << this->getStatusToString() << "\r\n";
  if (_statusCode.code == 302) {
    ss << "Location: " << _target.redirect << "\r\n";
  }
  ss << "Content-Type: " << _contentType << "\r\n"
     << "Content-Length: " << _buffer.size() << "\r\n";

  for (std::vector<std::string>::iterator it = _cookies.begin();
       it != _cookies.end(); ++it)
    ss << *it << "\r\n";

  ss << "\r\n";
  if (req.getMethod() != "HEAD")
    ss << _buffer;
  this->_value = ss.str();
}

void Response::generateAutoIndex() {
  Request &request = _client->getRequest();
  std::string path = request.getPath();
  std::ostringstream html;
  html << "<!DOCTYPE html>\n<html>\n<head>\n<title>Index of " << path
       << "</title>\n</head>\n<body>\n";
  html << "<h1>Index of " << path << "</h1>\n";
  html << "<table border=\"1\">\n<tr><th>Name</th><th>Size</th><th>Last "
          "Modified</th></tr>\n";

  DIR *dir;
  struct dirent *entry;
  std::string directoryPath = "." + path;

  if ((dir = opendir(directoryPath.c_str())) != NULL) {
    while ((entry = readdir(dir)) != NULL) {
      std::string name = entry->d_name;

      if (name == "." || name == "..")
        continue;

      struct stat fileStat;
      std::string fullPath = directoryPath + "/" + name;

      if (stat(fullPath.c_str(), &fileStat) < 0)
        continue;
      if (S_ISDIR(fileStat.st_mode))
        html << "<tr><td><a href=\"/" << name << "/";
      else
        html << "<tr><td><a href=\"" << name;
      html << "\">" << name << "</a></td>" << "<td>"
           << formatSize(fileStat.st_size) << "</td>" << "<td>"
           << formatTime(fileStat.st_mtime) << "</td></tr>\n";
    }
    closedir(dir);
  } else {
    html << "<tr><td colspan=\"3\">Empty directory</td></tr>\n";
  }

  html << "</table>\n</body>\n</html>\n";
  _buffer = html.str();
}

const std::map<unsigned short, std::string> &Response::getStatusCodes() {
  static std::map<unsigned short, std::string> statusCodes;

  if (statusCodes.empty()) {
    statusCodes[200] = "OK";
    statusCodes[201] = "Created";
    statusCodes[202] = "Accepted";
    statusCodes[204] = "No Content";
    statusCodes[301] = "Moved Permanently";
    statusCodes[302] = "Found";
    statusCodes[304] = "Not Modified";
    statusCodes[400] = "Bad Request";
    statusCodes[401] = "Unauthorized";
    statusCodes[403] = "Forbidden";
    statusCodes[404] = "Not Found";
    statusCodes[405] = "Method Not Allowed";
    statusCodes[408] = "Request Timeout";
    statusCodes[413] = "Payload Too Large";
    statusCodes[500] = "Internal Server Error";
    statusCodes[501] = "Not Implemented";
    statusCodes[502] = "Bad Gateway";
    statusCodes[503] = "Service Unavailable";
    statusCodes[504] = "Gateway Timeout";
  }
  return statusCodes;
}

void Response::setStatusCode(unsigned short code) {
  const std::map<unsigned short, std::string> &statusCodes = getStatusCodes();
  std::map<unsigned short, std::string>::const_iterator it =
      statusCodes.find(code);
  if (it != statusCodes.end()) {
    _statusCode.code = code;
    _statusCode.status = it->second;
  } else {
    _statusCode.code = 500;
    _statusCode.status = "Internal Server Error";
  }
}

const std::string Response::getResponse() const { return this->_value; }

STATUS_CODE Response::getStatusCode() const { return this->_statusCode; }

std::string Response::getStatusToString() const {
  std::stringstream ss;
  ss << _statusCode.code << " " << _statusCode.status;
  return ss.str();
}
