#include <unistd.h>
#include <iostream>
#include <algorithm>
#include "RS405CB.h"

RS405CB::RS405CB(const char *serial_port_device_name) : port(serial_port_device_name)
{
}

RS405CB::~RS405CB()
{
}

int RS405CB::sendAndReceiveShortPacket(const int id, std::vector<unsigned char> &recv_data, unsigned char flag, unsigned char address, unsigned char length, unsigned char count)
{
	std::vector<unsigned char> data;
	return sendAndReceiveShortPacket(id, recv_data, flag, address, length, count, data);
}

int RS405CB::sendAndReceiveShortPacket(const int id, std::vector<unsigned char> &recv_data, unsigned char flag, unsigned char address, unsigned char length, unsigned char count, std::vector<unsigned char> data)
{
	const int ret = sendShortPacket(id, flag, address, length, count, data);
	if(ret > 0) {
		if(receivePacket(recv_data) == 0)
			return 0;
	}

	return 1;
}

int RS405CB::sendShortPacket(const int id, unsigned char flag, unsigned char address, unsigned char length, unsigned char count)
{
	std::vector<unsigned char> data;
	return sendShortPacket(id, flag, address, length, count, data);
}

int RS405CB::sendShortPacket(const int id, unsigned char flag, unsigned char address, unsigned char length, unsigned char count, std::vector<unsigned char> data)
{
	if(length != data.size()) {
		std::cerr << "invalid data" << std::endl;
		return -1;
	}
	std::vector<unsigned char> buf;

	buf.push_back(0xFA); // header
	buf.push_back(0xAF); // header
	buf.push_back(static_cast<unsigned char>(id));
	buf.push_back(flag);
	buf.push_back(address);
	buf.push_back(length);
	buf.push_back(count);

	for(int i = 0; i < data.size(); i++) {
		buf.push_back(data[i]);
	}

	unsigned char check_sum = buf[2];
	for(int i = 3; i < buf.size(); i++) {
		check_sum = check_sum ^ buf[i];
	}
	buf.push_back(check_sum);

	const int write_len = port.writeData(buf);
	return write_len;
}

int RS405CB::sendLongPacket(unsigned char address, unsigned char length, unsigned char count, std::vector<unsigned char> data)
{
	std::vector<unsigned char> buf;

	buf.push_back(0xFA); // header
	buf.push_back(0xAF); // header
	buf.push_back(0x00); // id
	buf.push_back(0x00); // flags
	buf.push_back(address);
	buf.push_back(length);
	buf.push_back(count);

	for(auto d : data) {
		buf.push_back(d);
	}

	unsigned char check_sum = buf[2];
	for(int i = 3; i < buf.size(); i++) {
		check_sum = check_sum ^ buf[i];
	}
	buf.push_back(check_sum);

	const int write_len = port.writeData(buf);
	return write_len;
}

int RS405CB::receivePacket(std::vector<unsigned char> &data)
{
	data.clear();
	std::vector<unsigned char> recv_buf;
	const int len = port.readData(recv_buf);
	if(len >= 6) {
		if(recv_buf[0] != 0xFD || recv_buf[1] != 0xDF) {
			return 1;
		}
		const unsigned char flags = recv_buf[3];
		if(flags != 0) {
			if(flags & 0x80) {
				std::cerr << "error. tempeature limit" << std::endl;
			}
			if(flags & 0x20) {
				std::cerr << "error. tempeature alarm" << std::endl;
			}
			if(flags & 0x08) {
				std::cerr << "error. flash writing error" << std::endl;
			}
			if(flags & 0x02) {
				std::cerr << "error. invalid packet received" << std::endl;
			}
			return 2;
		}
		int data_length = recv_buf[7];
		for(int i = 0; i < data_length; i++) {
			data.push_back(recv_buf[7 + i]);
		}
		return 0;
	} else {
		return 3;
	}
}

double RS405CB::getVoltage(const int id)
{
	std::vector<unsigned char> recv_data;
	// get number from 42 to 59 of memory map
	const int result = sendAndReceiveShortPacket(id, recv_data, 0x09, 0x00, 0x00, 0x01);

	const double voltage = ((recv_data[11] << 8) | recv_data[10]) / 100.0;
	return voltage;
}

int RS405CB::getTemperature(const int id)
{
	std::vector<unsigned char> recv_data;
	// get number from 42 to 59 of memory map
	sendAndReceiveShortPacket(id, recv_data, 0x09, 0x00, 0x00, 0x01);

	const int temperature = (recv_data[9] << 8) | recv_data[8];
	return temperature;
}

int RS405CB::getLoad(const int id)
{
	std::vector<unsigned char> recv_data;
	// get number from 42 to 59 of memory map
	sendAndReceiveShortPacket(id, recv_data, 0x09, 0x00, 0x00, 0x01);

	const int load = (recv_data[7] << 8) | recv_data[6];
	return load;
}

double RS405CB::getAngle(const int id)
{
	std::vector<unsigned char> recv_data;
	// get number from 42 to 59 of memory map
	sendAndReceiveShortPacket(id, recv_data, 0x09, 0x00, 0x00, 0x01);

	const double angle = ((recv_data[1] << 8) | recv_data[0]) / 10.0;
	return angle;
}

int RS405CB::setTorque(const int id, bool torque_on)
{
	std::vector<unsigned char> data;
	if(torque_on) {
		data.push_back(0x01);
	} else {
		data.push_back(0x00);
	}
	return sendShortPacket(id, 0x00, 0x24, 0x01, 0x01, data);
}

int RS405CB::setAngle(const int id, double angle)
{
	std::vector<unsigned char> data;
	angle = std::max<double>(angle, -150.0);
	angle = std::min<double>(angle, 150.0);
	angle *= 10.0;
	short angle_int = static_cast<signed short>(angle);
	data.push_back(angle_int & 0xff);
	data.push_back(angle_int >> 8);
	return sendShortPacket(id, 0x00, 0x1e, 0x02, 0x01, data);
}

int RS405CB::setAngles(std::vector<std::pair<int, double>> angles)
{
	std::vector<unsigned char> data;

	for(auto id_and_angle : angles) {
		const unsigned char id = id_and_angle.first;
		double angle = id_and_angle.second;
		angle = std::max<double>(angle, -150.0);
		angle = std::min<double>(angle, 150.0);
		angle *= 10.0;
		short angle_int = static_cast<signed short>(angle);
		data.push_back(id);
		data.push_back(angle_int & 0xff);
		data.push_back(angle_int >> 8);
	}
	const unsigned char length = 3;
	return sendLongPacket(0x1e, length, angles.size(), data);
}

int RS405CB::setMovingTime(const int id, double time)
{
	std::vector<unsigned char> data;
	time *= 100.0;
	unsigned short time_int = static_cast<unsigned short>(time);
	data.push_back(time_int & 0xff);
	data.push_back(time_int >> 8);
	return sendShortPacket(id, 0x00, 0x20, 0x02, 0x01, data);
}

int RS405CB::setAngleAndMovingTime(const int id, double angle, double time)
{
	std::vector<unsigned char> data;

	angle = std::max<double>(angle, -150.0);
	angle = std::min<double>(angle, 150.0);
	angle *= 10.0;
	short angle_int = static_cast<signed short>(angle);
	data.push_back(angle_int & 0xff);
	data.push_back(angle_int >> 8);

	time *= 100.0;
	unsigned short time_int = static_cast<unsigned short>(time);
	data.push_back(time_int & 0xff);
	data.push_back(time_int >> 8);
	sendShortPacket(id, 0x00, 0x1e, 0x04, 0x01, data);

	return 0;
}

int RS405CB::storeDataToROM(const int id)
{
	const int return_value = sendShortPacket(id, 0x40, 0xff, 0x00, 0x00);
	sleep(1);
	return return_value;
}

int RS405CB::reboot(const int id)
{
	return sendShortPacket(id, 0x20, 0xff, 0x00, 0x00);
}

int RS405CB::resetMemoryMap(const int id)
{
	// reset memory map to factory setting
	return sendShortPacket(id, 0x10, 0xff, 0xff, 0x00);
}

int RS405CB::setServoID(const int current_id, const int new_id)
{
	std::vector<unsigned char> data;

	data.push_back(static_cast<unsigned char>(new_id));

	return sendShortPacket(current_id, 0x00, 0x04, 0x01, 0x01, data);
}

int RS405CB::setReverseMode(const int id, const bool reverse)
{
	std::vector<unsigned char> data;

	if(reverse) {
		data.push_back(0x01);
	} else {
		data.push_back(0x00);
	}

	return sendShortPacket(id, 0x00, 0x04, 0x01, 0x01, data);
}

int RS405CB::setBaudrate(const int id, const RS405CB_BAUDRATE baudrate)
{
	std::vector<unsigned char> data(baudrate);

	return sendShortPacket(id, 0x00, 0x06, 0x01, 0x01, data);
}

