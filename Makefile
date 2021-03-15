CC = g++
DEBUG = -g -Wall
CFLAGS = $(DEBUG) -std=c++17
LDLIBS =
SRC = main
TARGET = main

$(TARGET): $(SRC).o
	$(CC) $(SRC).o $(LDLIBS) -o $(TARGET)
$(SRC).o: src/$(SRC).cpp
	$(CC) $(CFLAGS) -c src/$(SRC).cpp
clean:
	rm -f *.o $(TARGET)
