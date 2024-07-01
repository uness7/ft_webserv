#ifndef WEBSERV_HPP
#define WEBSERV_HPP
#include <iostream>

class WebServ
{
public:
	/* Default Constructor */
	WebServ(void);

	/* Destructor */
	~WebServ(void);

	/* Constructor */
	WebServ(std::string name);

	/* Copy Constructor */
	WebServ(const WebServ &copy);

	/* Assignment Copy operator overload */
	WebServ &operator=(const WebServ &copy);

private:
};

#endif // WEBSERV_HPP
