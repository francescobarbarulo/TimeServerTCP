#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define BUFFER_SIZE 1024
#define TIME_LEN 9
#define CMD_LEN 5

int main(int argc, char* argv[]){
	if (argc < 2){
		printf("Inserire la porta di ascolto\n");
		exit(-1);
	}

	// elementi per implemenare l'I/O multipplexing
	fd_set master;
	fd_set read_fds;
	int fdmax;

	// strutture per client e server
	struct sockaddr_in sv_addr;
	struct sockaddr_in cl_addr;

	// struttura per info time
	time_t rawtime;
	struct tm *timeinfo = malloc(sizeof(struct tm));

	int listener; // socket di ascolto
	int ret;
	int new_sd; // nuovo socket per la accept()
	int addrlen; // lunghezza indirizzo client su cui fare la accept()
	int ready_sd;
	int i;

	// azzeramento sets
	FD_ZERO(&master);
	FD_ZERO(&read_fds);

	char buffer[BUFFER_SIZE];
	int port = atoi(argv[1]);

	listener = socket(AF_INET, SOCK_STREAM, 0);

	// creazione indirizzo server
	sv_addr.sin_family = AF_INET;
	sv_addr.sin_addr.s_addr = INADDR_ANY;
	sv_addr.sin_port = htons(port);

	ret = bind(listener, (struct sockaddr *)&sv_addr, sizeof(sv_addr));
	if (ret == -1){
		perror("[E] bind()");
		exit(-1);
	}

	ret = listen(listener, 10);
	if (ret == -1){
		perror("[E] listen()");
		exit(-1);
	}

	// aggiungo il listener al master e aggiorno fdmax
	FD_SET(listener, &master);
	fdmax = listener;

	while(1){
		read_fds = master;
		printf("[I] In attesa di un client pronto\n");
		select(fdmax + 1, &read_fds, NULL, NULL, NULL);
		if (ret == -1){
			perror("[E] select()");
			exit(-1);
		}

		time(&rawtime);
		timeinfo = localtime(&rawtime);

		for(i = 0; i <= fdmax; i++){
			if (FD_ISSET(i, &read_fds)){
				if (i == listener) {
					addrlen = sizeof(cl_addr);
					new_sd = accept(listener, (struct sockaddr *)&cl_addr, (socklen_t *)&addrlen);
					printf("[I] listener at %s", asctime(timeinfo));
          printf("[I] Connessione aperta sul socket-%d\n", new_sd);
					// aggiungo il nuovo socket al master set e aggiorno fdmax
					FD_SET(new_sd, &master);
					if (new_sd > fdmax)
						fdmax = new_sd;
				} else {
					// pulizia buffer
					memset(buffer, 0, BUFFER_SIZE);
					ready_sd = i;
					ret = recv(ready_sd, (void *)buffer, CMD_LEN, 0);
					if (ret == -1)
						perror("[E] recv()");
          if (strcmp(buffer, "bye") == 0) {
            close(ready_sd);
            FD_CLR(ready_sd, &master);
            printf("[S] socket-%d chiuso\n", ready_sd);  
          } else {
					  sprintf(buffer, "%d:%d:%d", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
					  ret = send(ready_sd, (void *)buffer, TIME_LEN, 0);
					  if (ret == -1)
						  perror("[E] send()");
					  else
						  printf("[S] Ora inviata correttamente sul socket-%d\n", ready_sd);
          }
				}
			}
		}
	}
}
