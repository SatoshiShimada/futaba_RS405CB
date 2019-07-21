
#include <iostream>
#include "serial_port.h"

SerialPort::SerialPort(const char *device_name) : opened(false)
{
	fd = open(device_name, O_RDWR);
	if(fd < 0) {
		std::cerr << "serial port open error. device name: " << device_name << std::endl;
	} else {
		opened = true;

		struct termios tio;
		tio.c_cflag += CREAD; // enable receive
		tio.c_cflag += CLOCAL; // local line (no modem)
		tio.c_cflag += CS8; // data bit: 8bits
		tio.c_cflag += 0; // stop bit: 1bit
		tio.c_cflag += 0; // parity: none

		const int baud_rate = B460800;
		cfsetispeed(&tio, baud_rate);
		cfsetospeed(&tio, baud_rate);

		cfmakeraw(&tio);

		tcsetattr(fd, TCSANOW, &tio);

		ioctl(fd, TCSETS, &tio);
	}
}

SerialPort::~SerialPort()
{
	if(opened) {
		close(fd);
	}
}

int SerialPort::writeData(std::vector<unsigned char> data)
{
	if(opened)
		return write(fd, data.data(), data.size());
	else
		return -1;
}

int SerialPort::readData(std::vector<unsigned char> &data)
{
	if(opened) {
		unsigned char buf[1024];
		const int ret = read(fd, buf, sizeof(buf));
		data = std::vector<unsigned char>(buf, buf + ret);
		return ret;
	} else {
		return -1;
	}
}

