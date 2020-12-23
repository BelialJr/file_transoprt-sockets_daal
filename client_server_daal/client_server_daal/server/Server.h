#pragma once
#include <WS2tcpip.h>
#include <daal.h>
#pragma comment(lib,"ws2_32.lib")

using namespace daal;
using namespace data_management;

class my_Server
{
public:
	my_Server(unsigned short port);
	~my_Server();
	void startServer();
	void closeServer();
	void handleConnection();
	void deCompressData(DataBlock &fileCompressedData,DataBlock& fileUncropressedData,byte* buff,unsigned int buffSize);
	unsigned int  parseDataSize(const char* buff);
	unsigned short port;
private:
	SOCKET server_socket;
	WSAData wData;

};
