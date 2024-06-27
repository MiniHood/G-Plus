#pragma once

#include <iostream>

using namespace std;

class Server {
private:
	string _servername;
	string _ip;
	int _port;

public:
	string servername() const { return _servername;  }
	string ip() const { return _ip; }
	int port() const { return _port; }

	Server(string ip, int port, OPTIONAL string servername) {
		_servername = servername;
		_ip = ip;
		_port = port;
	}
};