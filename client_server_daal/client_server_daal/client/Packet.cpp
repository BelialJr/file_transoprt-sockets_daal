#include "Packet.h"
#include <iostream>

Packet::Packet(unsigned int size, const  byte* datap)
{
    this->dataSize = size;
    this->packetSize = dataSize + headerSize;
    this->data = new byte[packetSize];
    memset(data, 0, packetSize);
    std::copy(datap, datap + dataSize, data + headerSize); 

    byte* dataSizeBytes = (unsigned char*)(static_cast<void*>(&this->dataSize));
    std::copy(dataSizeBytes, dataSizeBytes + headerSize, data);
}
Packet::Packet()
{
  
}

Packet::~Packet()
{
    delete[] this->data;
}

int Packet::getPacketSize()
{
    return packetSize;
}
char* Packet::getAllBytes()
{
    return (char*)this->data;
}

void Packet::print()
{
    std::cout << "PACKET[size:" << packetSize << "]:\n";
    for (int i = 0; i < packetSize; i++)
    {
        if (i > headerSize)
        {
            if (i % 45 == 0)
                std::cout << "\n";
            std::cout << data[i];
        }
        else if (i == headerSize)
            std::cout << "|" << data[i];
        else
        {
            std::cout << (int)data[i];
            if (i != headerSize - 1)
                std::cout << ",";
        }
    }
    std::cout << "\n---------------------- " << std::endl;;
}