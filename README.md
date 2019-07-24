# A library for Futaba RS405CB servo motor.

## Usage

```shell
git clone https://github.com/satoshishimada/futaba_RS405CB
cd futaba_RS405CB
make
sudo chmod a+rw /dev/ttyUSB0
./serial [Servo ID]
```

The description of all supported method are in `RS405CB.h`.

## Examples

### Print current angle

```c++
#include <iostream>
#include "RS405CB.h"

int main(int argc, char *argv[])
{
    RS405CB servo("/dev/ttyUSB0");
    constexpr int id = 1;

    std::cout << "Angle: " << servo.getAngle(id);

    return 0;
}
```

### Move motor

```c++
#include <iostream>
#include "RS405CB.h"

int main(int argc, char *argv[])
{
    RS405CB servo("/dev/ttyUSB0");
    constexpr int id = 1;

    servo.setAngle(id);

    return 0;
}
```

### Setting ID and baudrate

```c++
#include <iostream>
#include <unistd.h> // for sleep() function
#include "RS405CB.h"

int main(int argc, char *argv[])
{
    RS405CB servo("/dev/ttyUSB0");
    constexpr int id = 1;
    constexpr int new_id = 2;

    servo.setID(id, new_id);
    servo.setBaudrate(new_id, BAURATE_9600);
    servo.storeDataToROM(new_id);
    sleep(2);

    return 0;
}
```

## Reference

- [Futaba RS405CB](http://www.futaba.co.jp/robot/command_type_servos/rs405cb)

## License

MIT License

