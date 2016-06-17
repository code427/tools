CC = gcc

TARGET = cbuff

CFLAGS = -Wall -O -g
SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))

all:$(OBJS)
	$(CC) -o $(TARGET) $(OBJS)

%.o:%.c
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY:clean
clean:
	rm $(TARGET) $(OBJS)
