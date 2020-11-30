all: build

build:
	g++ -O2 -std=c++11 src/main.cpp src/v3.cpp -o raytrace -Wno-write-strings
