#pragma once
typedef unsigned char byte;
class Packet
{
private:
    unsigned int dataSize;
    unsigned int packetSize;
    const short headerSize = 4;
    byte* data;

public:
    Packet(unsigned int size, const  byte* datap);
    Packet();
    ~Packet();
    void  print();
    int   getPacketSize();
    char* getAllBytes();

};
