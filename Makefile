CC=gcc

TARGET=detect

LDFALGS=-lpcap
CFLAGS=-o $(TARGET)

SOURCE=main.c detect.c

all:detect listen

detect:$(SOURCE)
	$(CC) $(SOURCE) $(CFLAGS) $(LDFALGS)

listen:listen.c
	$(CC) listen.c -o listen -pthread

clean:
	-rm $(TARGET) listen
