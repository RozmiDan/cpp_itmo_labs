TARGET = FstLabCPP
CC = g++

CFLAGS = -I/usr/local/include -Wall -O2
LDFLAGS = -L/lib/x86_64-linux-gnu

PREF_SRC = ./src/
PREF_OBJ = ./obj/

SRC = $(wildcard $(PREF_SRC)*.cpp)
OBJ = $(patsubst $(PREF_SRC)%.cpp, $(PREF_OBJ)%.o, $(SRC))


$(TARGET) : $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -o $(TARGET)

$(PREF_OBJ)%.o : $(PREF_SRC)%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean: 
	rm $(TARGET) $(PREF_OBJ)*.o