#pragma once

#include <iostream>



class Server {
public:
	std::string servername = "";
	const char* wservername;
	std::string ip;
	std::string port;

		Server(std::string server_name,  std::string _ip, std::string _port)
	{
		servername = server_name;
		ip = _ip;
		port = _port;
	}
};