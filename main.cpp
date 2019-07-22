#include <iostream>
#include <stdlib.h>

#include <unistd.h>

#include "RS405CB.h"

int main(int argc, char *argv[])
{
	int id = 0;
	if(argc == 2)
		id = atoi(argv[1]);

	RS405CB servo("/dev/ttyUSB0");

	double voltage = servo.getVoltage(id);
	std::cout << "voltage: " << voltage << std::endl;
	int temperature = servo.getTemperature(id);
	std::cout << "temperature: " << temperature << std::endl;
	std::cout << "angle: " << servo.getAngle(id) << std::endl;
	servo.setTorque(id, true); // torque on

	//servo.setAngle(id, 0.0);
	for(int i = -14; i < 15; i++) {
		servo.setAngle(id, i * 0.1);
		sleep(1);
	}
	std::cout << "angle: " << servo.getAngle(id) << std::endl;

	return 0;
}

