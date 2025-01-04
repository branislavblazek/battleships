CC 	= g++
CFLAGS  = -std=c++17 -Wall -g
SRC 	= main.cpp
OBJ 	= $(SRC:.cpp=.o)
TARGET 	= BattleshipsApp

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)

