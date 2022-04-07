.PHONY: clean

OBJS = wish.o
CFLAG = -Wall -g
CC = gcc
TARGET = wish

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAG) -o $(TARGET) $(OBJS)

run: main.o
	./wish

clean:
	-rm -f $(OBJS) $(TARGET) *~ *.gch

test: wish
	./test-wish.sh
