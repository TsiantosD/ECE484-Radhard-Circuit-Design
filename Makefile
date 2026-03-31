all:
	gcc -Wall -fopenmp -g main.c parser.c -o main

sanitize:
	gcc -Wall -fopenmp -fsanitize=address -g main.c parser.c -o main

fast:
	gcc -Ofast -Wall -fopenmp -g main.c parser.c -o main

clean:
	rm main
