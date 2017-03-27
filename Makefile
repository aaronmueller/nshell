# Aaron Mueller, Connor VanMeter
# CS485G-004: Systems Programming
# 25 March 2017
# Project 4: A new shell

CC=gcc
CFLAGS=-Wall
OBJ = nsh.o

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

nsh: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)
