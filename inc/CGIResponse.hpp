#ifndef CGIRESPONSE_HPP
#define CGIRESPONSE_HPP

#include <string>
#include <map>

class CGIResponse
{
	public:
		CGIResponse(const std::string& scriptPath);
		void	setCgiEnv(std::map<std::string, std::string> &map);

		std::string	execute(
				const std::map<std::string, std::string> &envMap, 
				const std::string &cgiPath, 
				const std::string &scriptPath, 
				const std::string &method, 
				const std::string &postData);

		std::map<std::string, std::string>	envMap;

	private:
		std::string scriptPath_;
};

#endif 
