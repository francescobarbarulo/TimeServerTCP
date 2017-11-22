#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>

#define BUFFER_SIZE 1024
#define TIME_LEN 9
#define CMD_LEN 5

char buffer[BUFFER_SIZE];
int sd;
int ret;


void chiudiSocket(){
  strcpy(buffer, "bye");
  ret = send(sd, (void *)buffer, CMD_LEN, 0);
  if (ret == -1)
    perror("[E] send()");
  close(sd);
  exit(0);
}

void richiediOra(){
	strcpy(buffer, "time");
	ret = send(sd, (void *)buffer, CMD_LEN, 0);
	if (ret == -1)
		perror("[E] send()");
	else
		printf("[S] Richiesta inviata\n");
	signal(SIGALRM, richiediOra);
	alarm(5);
}

int main(int argc, char *argv[]){
	if (argc < 3){
		printf("Inserire indirizzo IP e porta del server\n");
		exit(-1);
	}

	// puntatore timeinfo
	struct tm *t = malloc(sizeof(struct tm));

	// struttura indirizzo server
	struct sockaddr_in sv_addr;

	char *addr = malloc(sizeof(argv[1]));
	addr = argv[1];

	int port = atoi(argv[2]);

	sd = socket(AF_INET, SOCK_STREAM, 0);

	// creazione indirizzo
	sv_addr.sin_family = AF_INET;
	sv_addr.sin_port = htons(port);
	inet_pton(AF_INET, addr, &sv_addr.sin_addr);

	ret = connect(sd, (struct sockaddr *)&sv_addr, sizeof(sv_addr));
	if (ret == -1){
		perror("[E] connect()");
		exit(-1);
	}

	signal(SIGALRM, richiediOra);
	alarm(5);
  
  signal(SIGINT, chiudiSocket);

	while(1){
		ret = recv(sd, (void *)buffer, TIME_LEN, 0);
		if (ret == -1)
			perror("[E] recv()");
		else {
			sscanf(buffer, "%d:%d:%d", &t->tm_hour, &t->tm_min, &t->tm_sec);
			printf("%d:%d:%d\n", t->tm_hour, t->tm_min, t->tm_sec);
			// printf("Ho ricevuto: %s\n", buffer);
		}
	}
}
