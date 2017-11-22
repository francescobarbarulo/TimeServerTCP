main:
	gcc -Wall -c server.c
	gcc -Wall -c client.c
	gcc -o server server.o
	gcc -o client client.o
