CFLAGS=-Wall -Wextra -ggdb3
main: main.o

interpreter: interpreter.o

clean: 
	rm main main.o interpreter interpreter.o
	
