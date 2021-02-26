CFLAGS=-Wall -Wextra -ggdb3
main: main.o

interpreter: interpreter.o -lavcall

clean: 
	rm main main.o interpreter interpreter.o
	
