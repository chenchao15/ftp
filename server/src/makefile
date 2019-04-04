CC := gcc
CFLAGS := -Wall -g -Os

TARGET=server

SRCS=server.c file.c directory.c handle.c mode.c

OBJS = $(SRCS:.c=.o)

$(TARGET):$(OBJS)
	$(CC) -o $@ $^

clean:
	rm -rf $(TARGET) $(OBJS)

%.o:%.c
	$(CC) $(CFLAGS) -o $@ -c $<
