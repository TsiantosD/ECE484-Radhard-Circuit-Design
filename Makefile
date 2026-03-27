all:
	gcc -Wall -fopenmp -g main.c parser.c -o main

clean:
	rm main
