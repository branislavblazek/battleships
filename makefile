CC 			= gcc
CFLAGS  = -Wall -Wextra -g -pthread
SRCS 		= main.c server.c client.c pipe.c communication.c client.c grid.c player.c ship.c utils.c interface.c shm.c
OBJS 		= $(SRCS:.c=.o)
TARGET 	= BattleshipsApp
SERVER   = server
CLIENT   = client 

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(CLIENT): client.c pipe.o communication.c utils.c grid.c player.c ship.c shm.c interface.c
	$(CC) $(CFLAGS) -DCLIENT_MAIN -o $(CLIENT) $^

$(SERVER): server.c pipe.c communication.c utils.c grid.c player.c ship.c shm.c interface.c
	$(CC) $(CFLAGS) -DSERVER_MAIN -o $(SERVER) $^

run: $(TARGET)
	./$(TARGET)

valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)

