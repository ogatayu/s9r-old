CFLAGS=-g -Wall -Werror
OBJS=main.o
LIBS=-lpthread -lm -lsoundio
TARGET=s9r
DEPS=

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -rf $(TARGET) *.o
