#pragma once
# include <fstream>
# include <iostream>
# include <map>
# include <ostream>
# include <sstream>
# include <string>
# include <vector>
# include <algorithm>
# include <cctype>
# include <fstream>
# include <iterator>
# include "Config.hpp"
# include "Utils.hpp"

enum REQ_STATE {
	EMPTY,
	HEADER,
	BODY
};

class Request {
	private:
		std::string _method;
		std::string _path;
		std::string _query;
		std::string _mimetype;
		std::vector<char> _body;
		std::map<std::string, std::string> _headers;
		long long _contentLength;
		REQ_STATE _state;

		long handleFirstLineHeader(unsigned int);
		void saveHeaderLine(std::string &);
		bool checkHeaderLocation(ServerConfig &config);
		void parseHeader(std::string &header);

	public:
		Request();
		Request(const Request &);
		Request &operator=(const Request &);
		~Request();

		long readFromSocket(unsigned int, ServerConfig &config);

		std::string getMethod() const;
		std::string getPath() const;
		std::string getMimeType() const;
		std::string getQuery() const;
		std::vector<char> getBody() const;
		std::string getHeaderField(std::string) const;
		long long getContentLength() const;

		bool isCGI() const;

		void setHeaders(std::string &);
		void appendToBody();
		void setMethod(std::string);
		void setPath(std::string);
		void setMimeType();
};
