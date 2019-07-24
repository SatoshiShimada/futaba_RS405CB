
CXX=g++
CXXFLAGS=-Wall
LIBS=

all: librs405cb.so serial

librs405cb.so: RS405CB.o serial_port.o
	$(CXX) $(CXXFLAGS) -shared -Wl,-soname,$@ $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -fPIC -c -o $@ $^

serial: main.o librs405cb.so
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS) -L. -lrs405cb

.PHONY: clean
clean:
	rm *.o

