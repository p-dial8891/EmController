main.o : main.cpp 
	g++ -g -std=c++17 -I../config -I /usr/include/libevdev-1.0 -c main.cpp

emc.out : main.o  
	g++ -g -o emc.out main.o -L /usr/lib/arm-linux-gnueabihf/ -levdev -lpthread -lstdc++


