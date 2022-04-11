.PHONY: clean

OBJS = wzip.o
CFLAG = -Wall -g
CC = gcc
TARGET = wzip

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAG) -o $(TARGET) $(OBJS)

run: wzip.o
	./wzip

clean:
	-rm -f $(OBJS) $(TARGET) *~ *.gch

test: wzip
	./test-wzip.sh
