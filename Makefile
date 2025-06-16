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
	./integral -R 4:5:1.0:3.0:0.00001:2.0
	./integral -R 4:6:0.0:1.0:0.00001:0.501468
	./integral -R 5:6:1.0:4.0:0.00001:0.240557
	./integral -I 4:1.0:5.0:0.00001:16.0
	./integral -I 5:1.0:5.0:0.00001:37.333
	./integral -I 6:1.0:5.0:0.00001:1089654.7623
