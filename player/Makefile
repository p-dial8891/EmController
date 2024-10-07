main.o player.o : main.cpp player.cpp
	g++ -g -std=c++17 -I../config -c main.cpp player.cpp 

player.out : main.o player.o 
	g++ -g -o player.out main.o player.o ../mouse/mouse.o -L /usr/lib/arm-linux-gnueabihf/ -levdev -lpthread -lstdc++


