
#include <iostream>
#include <fstream>
#include <daal.h>
#include <WS2tcpip.h>
#include "Packet.h"

#pragma comment(lib,"ws2_32.lib")
//#pragma warning(disable:4996)

#define BLOCKSIZE 1024

typedef unsigned char byte;

using namespace daal;
using namespace data_management;

void  prepareMemory(const char* filePath, DataBlock& rawData);
void  freeMemory(std::vector<DataBlock> dataBlocksQueue);
byte* readAllBytes(const char* filename, int& read);
void  check_file_exist(const char* fileName);
int   getFileSize(const char* filename);
void  doConnect(const char* ip, int port,SOCKET & connSocket);
DataBlock getCompressedDataBlock(const char* buff);
std::vector<DataBlock> getAllCompressedBlocks(const char* filePath);
std::vector<Packet*> getAllPackets(std::vector<DataBlock> &vector);

//napisanie klient dlap rotokolu sftp lista funckojalnosci
//client vpn 


int main(int argc, char** argv)
{ 
    char filePath[] = "C:/Users/mrrom/Desktop/test.txt";
    char serverIp[] = "127.0.0.1";
    int  serverPort =  5400;

    SOCKET connSocket;

    check_file_exist(filePath);
    
    std::vector<DataBlock> compressedDataBlocksQueue = getAllCompressedBlocks(filePath);
    
    std::vector<Packet*> packetsQueue = getAllPackets(compressedDataBlocksQueue);


    doConnect(serverIp, serverPort, connSocket);

    for (const auto& ptr : packetsQueue)
    {
           std::cout << "PACKET SENDED : " << ptr->getPacketSize() << " bytes | "<< (int)ptr->getAllBytes()[0] << "," << (int)ptr->getAllBytes()[1] << "," << (int)ptr->getAllBytes()[2] << "," << (int)ptr->getAllBytes()[3] << " [data size]" << std::endl ;
           send(connSocket,ptr->getAllBytes(),ptr->getPacketSize(),NULL);
           Sleep(50);
    }

    for (const auto& ptr : packetsQueue)
    {
        delete[] ptr->getAllBytes();
    }


    closesocket(connSocket);
    freeMemory(compressedDataBlocksQueue);
    system("pause");
}

std::vector<Packet*> getAllPackets(std::vector<DataBlock> &vector)
{
    std::vector<Packet*> result;
    for (auto &ptr : vector)
    {
        Packet *packet = new  Packet(ptr.getSize(), ptr.getPtr());
        result.push_back(packet);
    }
    return result;
}



std::vector<DataBlock> getAllCompressedBlocks(const char * filePath)
{
    int   allBytesSize = 0;
    byte* allBytes = readAllBytes(filePath, allBytesSize);
    std::vector<DataBlock> dataBlocksQueue;

    int iterationEnd = (allBytesSize / BLOCKSIZE);
    int iteration = 0;
    int bytesIndex = 0;

    do
    {
        char data[BLOCKSIZE];
        ZeroMemory(data, sizeof(data));
        for (int i = 0; i < BLOCKSIZE; i++)
        {
            if (bytesIndex < allBytesSize - 1)
                data[i] = allBytes[bytesIndex++];
            else
                data[i] = ' ';
        }
      //std::copy(allBytes + 0 , allBytes + BLOCKSIZE , data);
        DataBlock datablock = getCompressedDataBlock(data);
        dataBlocksQueue.push_back(datablock);
        iteration++;

    }while (iteration <= iterationEnd);

    std::cout << "Datablocks was generated : " << dataBlocksQueue.size() << " x (1024 bytes)" << std::endl;

    delete[] allBytes;

    return dataBlocksQueue;
}


DataBlock getCompressedDataBlock(const char* buff)
{
    Compressor<zlib> compressor;
    compressor.parameter.gzHeader = true;
    compressor.parameter.level = level9;

    DataBlock fileUncropressedData;
    DataBlock fileCompressedData;

    fileUncropressedData.setSize(BLOCKSIZE);
    fileUncropressedData.setPtr((byte*)buff);

    CompressionStream compressionStream(&compressor);
    compressionStream << fileUncropressedData;

    fileCompressedData.setSize(compressionStream.getCompressedDataSize());
    fileCompressedData.setPtr(new byte[fileCompressedData.getSize()]);
    compressionStream.copyCompressedArray(fileCompressedData);

    return fileCompressedData;
}

void  freeMemory(std::vector<DataBlock> dataBlocksQueue)
{
    for (const auto& ptr : dataBlocksQueue)
    {
        delete[] ptr.getPtr();
    }
}

byte* readAllBytes(const char* filename, int& read)
{
    std::ifstream ifs(filename, std::ios::binary | std::ios::ate);
    std::ifstream::pos_type pos = ifs.tellg();
    int length = pos;
    unsigned char* pChars = new unsigned char[length];
    ifs.seekg(0, std::ios::beg);
    ifs.read((char*)pChars, length);
    ifs.close();
    std::cout << "File size : " <<length<<" bytes" << std::endl;
    read = length;
    return pChars;
}

void check_file_exist(const char* fileName)
{
    std::ifstream infile(fileName);
    std::cout << "Locating file \"" << fileName << "\" : ";
    if (infile.good()) {
        std::cout << "Succeed" << std::endl;
    }
    else {
        std::cout << "Failed" << std::endl;
        exit(-1);
    }
}

void doConnect(const char* ip, int port,  SOCKET& connSocket) {
    WSADATA data;
    WORD word = MAKEWORD(2, 2);
    if (WSAStartup(word, &data) != 0)
    {
        std::cout << "FAILED";
        exit(-1);
    }
    SOCKADDR_IN adr;
    int sizeofadr = sizeof(adr);
    adr.sin_port = htons(port);
    adr.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &adr.sin_addr);

    connSocket = socket(AF_INET, SOCK_STREAM, NULL);
    if ((connect(connSocket, (SOCKADDR*)&adr, sizeofadr)) != 0)
    {
        std::cout << "Failed to connect to server ";
        exit(-1);
    }
    std::cout << "Connected to the server " << std::endl;

}
