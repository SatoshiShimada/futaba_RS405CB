#ifndef RS405CB_H
#define RS405CB_H

#include <vector>
#include <utility>
#include "serial_port.h"

class RS405CB
{
public:
	RS405CB(const char *);
	~RS405CB();

	/*
	 * get current voltage
	 * return: current voltage. unit is volt [V]
	 * argument: servo id
	 */
	double getVoltage(const int);

	/*
	 * get current temperature
	 * return: current temperature. unit is degree [C]
	 * argument: servo id
	 */
	int getTemperature(const int);

	/*
	 * get current load
	 * return: motor current. unit is milli ampere [mA]
	 * argument: servo id
	 */
	int getLoad(const int);

	/*
	 * set motor torque
	 * return: written length. if error, return -1
	 * arguments: servo id and torque setting
	 *   true: torque on
	 *   false: torque off
	 */
	int setTorque(const int, bool);

	/*
	 * get current angle
	 * return: current angle. unit is degree
	 * argument: servo id
	 */
	double getAngle(const int);

	/*
	 * set target angle
	 * return: written length. if error, return -1
	 * arguments: servo id and target angle
	 *   valid angle range is -150 <= angle <= 150.
	 *   unit is degree.
	 */
	int setAngle(const int, double);

	/*
	 */
	int setAngles(std::vector<std::pair<int, double>>);

	/*
	 * set moving time
	 * return: written length. if error, return -1
	 * arguments: servo id and moving time
	 *   unit is second [s]
	 *   moving time is zero means use max velocity.
	 */
	int setMovingTime(const int, double);

	/*
	 * set angle and moving time
	 * return: written length. if error, return -1
	 * arguments: servo id, target angle and moving time
	 */
	int setAngleAndMovingTime(const int, double, double);

	/*
	 * store memory map to flash ROM. need to wait 1 second for writing time.
	 * return: written length. if error, return -1
	 * argument: servo id
	 */
	int storeDataToROM(const int);

	/*
	 * reboot servo motor.
	 * return: written length. if error, return -1
	 * argument: servo id
	 */
	int reboot(const int);

	/*
	 * return: written length. if error, return -1
	 * argument: servo id
	 */
	int resetMemoryMap(const int);
private:
	SerialPort port;
	int sendShortPacket(const int, unsigned char, unsigned char, unsigned char, unsigned char);
	int sendShortPacket(const int, unsigned char, unsigned char, unsigned char, unsigned char, std::vector<unsigned char>);
	int sendAndReceiveShortPacket(const int, std::vector<unsigned char> &, unsigned char, unsigned char, unsigned char, unsigned char, std::vector<unsigned char>);
	int sendAndReceiveShortPacket(const int, std::vector<unsigned char> &, unsigned char, unsigned char, unsigned char, unsigned char);
	int sendLongPacket(unsigned char, unsigned char, unsigned char, std::vector<unsigned char>);
	int receivePacket(std::vector<unsigned char> &);
};

#endif

