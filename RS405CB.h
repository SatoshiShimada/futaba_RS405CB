
#ifndef RS405CB_H
#define RS405CB_H

#include "serial_port.h"

class RS405CB
{
public:
	RS405CB(const char *);
	~RS405CB();
	double getVoltage(const int);
	int getTemperature(const int);
	int setTorque(const int, bool);
	double getAngle(const int);
	int setAngle(const int, double);
	int storeDataToROM(const int);
	int reboot(const int);
	int resetMemoryMap(const int);
	int getLoad(const int);
private:
	SerialPort port;
	int sendShortPacket(const int, unsigned char, unsigned char, unsigned char, unsigned char);
	int sendShortPacket(const int, unsigned char, unsigned char, unsigned char, unsigned char, std::vector<unsigned char>);
	int sendAndReceiveShortPacket(const int, std::vector<unsigned char> &, unsigned char, unsigned char, unsigned char, unsigned char, std::vector<unsigned char>);
	int sendAndReceiveShortPacket(const int, std::vector<unsigned char> &, unsigned char, unsigned char, unsigned char, unsigned char);
	int receivePacket(std::vector<unsigned char> &);
};

#endif

