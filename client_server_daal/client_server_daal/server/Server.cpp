#include "Server.h"
#include <iostream>
#include <fstream>
//#include <daal.h>
using namespace daal;
using namespace data_management;

my_Server::my_Server(unsigned short port) {
	this->port = port;
}

my_Server::~my_Server() {
	closeServer();
}

void my_Server::startServer()
{
	if (WSAStartup(MAKEWORD(2, 2), &wData) == 0)
	{
		SOCKADDR_IN addr;
		addr.sin_addr.S_un.S_addr = INADDR_ANY;
		addr.sin_port = htons(this->port);
		addr.sin_family = AF_INET;
		this->server_socket = socket(AF_INET, SOCK_STREAM, NULL);

		if (this->server_socket != SOCKET_ERROR) {
			std::cout << "Socket has been created\n";
		}
		if (bind(this->server_socket, (struct sockaddr*) & addr, sizeof(addr)) != SOCKET_ERROR) {
			std::cout << "Socket binded successfully\n";
		}
		if (listen(this->server_socket, 1) != SOCKET_ERROR) {
			printf("Start listening at port %u\n", ntohs(addr.sin_port));
		}
		handleConnection();
	}
	else
	std::cout << ("Failed to start the server \n");
}
void my_Server::handleConnection()
{
	SOCKET newConnection;
	SOCKADDR_IN addr_connect;
	int addrlen = sizeof(addr_connect);
	if ((newConnection = accept(this->server_socket, (struct sockaddr*) & addr_connect, &addrlen)) != 0)
	{
		printf("Server_Log : User %u.%u.%u.%u:%u has been connected\n",
			(unsigned char)addr_connect.sin_addr.S_un.S_un_b.s_b1,
			(unsigned char)addr_connect.sin_addr.S_un.S_un_b.s_b2,
			(unsigned char)addr_connect.sin_addr.S_un.S_un_b.s_b3,
			(unsigned char)addr_connect.sin_addr.S_un.S_un_b.s_b4,

			ntohs(addr_connect.sin_port));

			char buff[1024];
			
			std::ofstream outfile("result.txt", std::ofstream::binary);
			

			while(true){
				ZeroMemory(buff, sizeof(buff));
				int status = recv(newConnection, buff, sizeof(buff), NULL);

				if (status == 0)
					break;

				unsigned int dataSize = parseDataSize(buff);

				std::cout << "PACKET RECEIVED : " << dataSize << " bytes" << std::endl;;
				
				char* data = new char[dataSize];
				std::copy(buff + 4, buff + 4 + dataSize, data);

				DataBlock fileCompressedData;
				DataBlock fileUncropressedData;

				deCompressData(fileCompressedData, fileUncropressedData,((byte*)data),dataSize);

				outfile.write((char*)fileUncropressedData.getPtr(), fileUncropressedData.getSize());

				delete[] fileUncropressedData.getPtr();
				delete[] fileCompressedData.getPtr();
			}
			std::cout << "END OF THE FILE \n";
			outfile.close();
			my_Server::closeServer();
	}
	else
	{
		std::cout << "User failed to connect" << std::endl;
	}
}
void my_Server::deCompressData(DataBlock& fileCompressedData, DataBlock& fileUncropressedData,byte* buff, unsigned int buffSize)
{
	fileCompressedData.setPtr(buff);
	fileCompressedData.setSize(buffSize);

	Decompressor<zlib> decompressor;
	decompressor.parameter.gzHeader = true;
	DecompressionStream deComprStream(&decompressor);
	deComprStream << fileCompressedData;

	fileUncropressedData.setSize(deComprStream.getDecompressedDataSize());
	fileUncropressedData.setPtr(new byte[fileUncropressedData.getSize()]);
	deComprStream.copyDecompressedArray(fileUncropressedData);
}
unsigned int my_Server::parseDataSize(const char* buff)
{
	byte bytes[4] = {0};
	bytes[0] = (byte)buff[0];
	bytes[1] = (byte)buff[1];
	bytes[2] = (byte)buff[2];
	bytes[3] = (byte)buff[3];
	unsigned int dataSize = 0;
	memcpy(&dataSize, bytes, sizeof(int));
	return dataSize;
}
void my_Server::closeServer()
{
	closesocket(this->server_socket);
	WSACleanup();
	std::cout << "Server has been stoped" << std::endl;
}