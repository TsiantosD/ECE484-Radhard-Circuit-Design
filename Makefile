GCC := gcc
CFLAGS := -Wall -fopenmp -g
SRCS := main.c parser.c levelization.c netlist.c

all:
	$(GCC) $(CFLAGS) $(SRCS) -o main

sanitize:
	$(GCC) -fsanitize=address $(CFLAGS) $(SRCS) -o main

fast:
	$(GCC) -Ofast $(CFLAGS) $(SRCS) -o main

clean:
	rm main
