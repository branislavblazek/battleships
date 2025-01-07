CC 		= g++
CFLAGS  	= -std=c++17 -Wall -Wextra -g -pthread
SRCS 		= main.c server.c client.c pipe.c communication.c
OBJS 		= $(SRCS:.c=.o)
TARGET 		= BattleshipsApp

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)

