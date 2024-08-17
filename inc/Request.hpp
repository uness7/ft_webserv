#pragma once
#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>
#include <map>

class Request
{
	private:
		std::string 	_method;
		std::string 	_path;
		std::string 	_query;
		std::string 	_mimetype;
		std::string 	_body;
		std::map<std::string, std::string>	_headers;

		void	parseData();

	public:
		std::string _data;
		Request(std::string);
		Request(const Request &);
		Request &operator=(const Request &);
		~Request();

		std::string 	getMethod() const;
		std::string 	getPath() const;
		std::string 	getMimeType() const;
		std::string 	getQuery() const;
		std::string 	getBody() const;
		std::string 	getHeaderField(std::string) const;
		std::string	    getPostData() const;
        bool            isCGI() const;

		void	setMethod(std::string);
		void 	setPath(std::string);
		void 	setMimeType();
};

std::ostream &operator<<(std::ostream &out, const Request &);
