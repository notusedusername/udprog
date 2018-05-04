#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024

#define PORT 3300

//#define CL_NUM 10



int main( int argc, char* argv[]){

	printf("##### CHAT SERVER 1.0 #####\n");

	//SOCKET LÉTREHOZÁS

	int sock = 0;

	sock = socket(AF_INET, SOCK_STREAM, 0);

	if( sock < 0 )
		exit(1);

	int ext = 0; 														//függvény által visszaadott érték hibakezelésre

	struct sockaddr_in server;

	server.sin_family = AF_INET;

	server.sin_port = htons(PORT);

	server.sin_addr.s_addr = INADDR_ANY;

	int server_addrlen = sizeof server;

	int option = 1;

	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&option, sizeof option);
	setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, (char *)&option, sizeof option);

	ext = bind(sock, (struct sockaddr *) &server, server_addrlen);	//(socket fájlleírója, címleíró struktúra (family, cím+port),  cím mérete)
																	// cím sockethez rendelése
	if ( ext < 0 ){
		printf("Bind error!\n" );
		exit(2);
	}

	int backlog = 20;												// sor mérete, max 20.

	ext = listen( sock, backlog );									// fájlleíró, függőben lévő kérések

	if ( ext < 0 )
	{
		printf("Listen error!\n");
		exit(3);
	}

	struct sockaddr_in client1;

	int client_addrlen = sizeof server;	

	int sockc1 = 0; 														// kliens socket

	sockc1 = accept( sock, (struct sockaddr *) &client1, &client_addrlen); //leíró, kliens címe

	if ( sockc1 < 0){
		printf("Accept error!\n");
		exit(4);
	}
	else printf("Client1 connected!\n");

	struct sockaddr_in client2;

	int sockc2 = 0; 														// kliens socket

	sockc2 = accept( sock, (struct sockaddr *) &client2, &client_addrlen); //leíró, kliens címe

	if ( sockc2 < 0){
		printf("Accept error!\n");
		exit(4);
	}
	else printf("Client2 connected!\n");

	char buffer[BUFFER_SIZE];

	int len = 0;															//üzenet mérete

	int flags = 0;

	int sends = 0;

	int recvs = 0;


	for (;;){


		len = BUFFER_SIZE;

		recvs = recv( sockc1, buffer, len, flags);

		//LEÁLLÍTÁS HA NINCS TÖBB KLIENS


		if ( recvs == 0 ){
			printf("Client1 closed connection.\n");
			
			close(sock);

			close (sockc1);

			close (sockc2);

			return 0;
		}

		//NORMÁL MŰKÖDÉS
		else
			{
			printf("Incoming message from client1: %s\n", buffer);

			len = strlen(buffer)+1;

			sends = send( sockc2, buffer, len, flags);

			printf("Sending to c2...\n");

			if ( sends < 0 )
			{
				printf("Send error!\n");

				sprintf(buffer, "Server: Send error!");

				len = strlen(buffer)+1;	

				printf("Err msg send back..\n");

				sends = send( sockc1, buffer, len, flags);
			}

			
		}
		
		len = BUFFER_SIZE;

		recvs = recv( sockc2, buffer, len, flags);

		if ( recvs == 0 ){
			printf("Client2 closed connection.\n");

			close(sock);

			close (sockc1);

			close (sockc2);
			
			return 0;
		}
		
		else
			{
			printf("Incoming message from client2: %s\n", buffer);

			len = strlen(buffer);

			sends = send( sockc1, buffer, len, flags);

			printf("Send to c1...\n");

			if ( sends < 0 )
			{
				printf("Send error!\n");

				sprintf(buffer, "Server: Send error!");

				len = strlen(buffer)+1;	

				printf("Err msg send back..\n");

				sends = send( sockc2, buffer, len, flags);
			}


		}
		

	}

}
