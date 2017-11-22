main:
	gcc -Wall -c source/server.c
	gcc -Wall -c source/client.c
	gcc -o dist/server server.o
	gcc -o dist/client client.o
