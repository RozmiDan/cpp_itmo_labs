TARGET = MyProgram

CC = g++

CFLAGS = -Wall -O2

SRC = main.cpp res_funk.cpp
OBJ = main.o res_funk.o

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o $(TARGET)