CC = gcc
CFLAGS = -O2 -g -std=gnu99 -m32 -no-pie -fno-pie
CFLAGS += -Wall -Wextra -Wformat-security
CFLAGS += -Wstrict-prototypes -Wmissing-prototypes

LDLIBS = -lm

.PHONY: all clean test

all: integral

integral: main.c functions.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDLIBS)

functions.o: functions.asm
	nasm -f elf32 $< -o $@

clean:
	rm -f integral *.o

test:
	./integral -R 4:5:0.5:2.0:0.00001:0.87673
	./integral -R 4:6:1.0:2.0:0.00001:1.61803
	./integral -R 5:6:-2.0:0.0:0.00001:-1.93456
	./integral -I 4:-3.0:11.0:0.001:452.67
	./integral -I 5:1.0:3.0:0.00001:1.5303
	./integral -I 6:-2.0:9.0:0.001:49.5
