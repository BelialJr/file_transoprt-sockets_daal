#include <iostream>
#include "Server.h"



int main()
{
	my_Server server(5400);
	server.startServer();
	system("pause");
}
