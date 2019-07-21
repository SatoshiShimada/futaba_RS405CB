
CXX=g++
CXXFLAGS=
LIBS=

all: serial

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $^

serial: main.o RS405CB.o serial_port.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

.PHONY: clean
clean:
	rm *.o

