all:
	g++ src/glad.cpp src/shader.cpp src/mazeGenerator.cpp src/main.cpp -o run -Wall -pedantic -std=c++17 -O2 -I./include -I/usr/include/SDL2 -I/usr/local/include/SDL2 -s -lSDL2 -lSDL2_image -lGL -ldl

