#include <iostream>

#ifndef PAGES_AAA
#define PAGES_AAA

class pages {
public:
	class client {
	public:
		void mainmenu();
	};
	class server {
	public:
		void mainmenu();
		void list_saved_servers();
		void search_server_by_name();
		void add_new_server();
		void delete_server_records();
	};
	class home {
	public:
		void mainmenu();
	};
};

#endif
