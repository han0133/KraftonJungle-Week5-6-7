CC = gcc
CFLAGS = -Wall -g
TARGET = tiny
CGI_TARGET = cgi-bin/adder
SRCS = tiny.c ../csapp.c
CGI_SRCS = cgi-bin/adder.c

all: $(TARGET) $(CGI_TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

$(CGI_TARGET): $(CGI_SRCS)
	$(CC) $(CFLAGS) -o $(CGI_TARGET) $(CGI_SRCS)

clean:
	rm -f $(TARGET) $(CGI_TARGET) *.o