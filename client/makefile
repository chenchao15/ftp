CC := gcc
CFLAGS := -Wall -g -Os

TARGET=client

SRCS=client.c file.c handle.c mode.c

OBJS = $(SRCS:.c=.o)

$(TARGET):$(OBJS)
	$(CC) -o $@ $^

clean:
	rm -rf $(TARGET) $(OBJS)

%.o:%.c
	$(CC) $(CFLAGS) -o $@ -c $<
