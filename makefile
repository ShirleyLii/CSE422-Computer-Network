all:server client

server: sensor.cpp message.h
	g++ -std=c++11 sensor.cpp -g -o sensor

client: base.cpp message.h
	g++  base.cpp -g -o base

clean: 
	rm -rf sensor base
