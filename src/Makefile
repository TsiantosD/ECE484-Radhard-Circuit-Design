GCC := gcc
CFLAGS := -Wall -fopenmp -g
LDLIBS := -lm
SRCS := main.c parser.c levelization.c netlist.c simulation.c

all:
	$(GCC) $(CFLAGS) $(SRCS) -o main $(LDLIBS)

sanitize:
	$(GCC) -fsanitize=address $(CFLAGS) $(SRCS) -o main $(LDLIBS)

fast:
	$(GCC) -Ofast $(CFLAGS) $(SRCS) -o main $(LDLIBS)

clean:
	rm main
