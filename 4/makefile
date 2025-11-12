CC = gcc
CFLAGS = -Wall -Wextra -pthread -O2
TARGET = MultiFile
SRC = MultiFile.c

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET) merged_log.txt test*.txt

test: $(TARGET)
	./test_merger.sh

.PHONY: clean test