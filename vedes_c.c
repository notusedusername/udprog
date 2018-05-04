#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>


#define BUFFER_SIZE 1024

#define PORT 3300

void incoming(char buffer[BUFFER_SIZE], int len, int recvs, int sock){

	//BEJÖVŐ ÜZENET

	recvs = 0;

	len = BUFFER_SIZE;

	recvs = recv(sock, buffer, len, 0);

	if ( recvs == 0 ){
		printf("Server closed connection.\n");
		
		close(sock);

		exit(4);	
	}
	else printf("Not you:%s",buffer );

}

void outgoing (char buffer[BUFFER_SIZE], int len, int sends, int sock){

	sends = 0;

	printf("You:\t");

	//scanf("%s", buffer);

	fgets(buffer, BUFFER_SIZE, stdin);										// a space-ket ignorálja, kicsit pofásabb
		
	len = strlen(buffer)+1;

	//ÜZENET KÜLDÉSE

	sends = send(sock, buffer, len, 0); 								//célsocket, küldendő infó, hossza, 0

	if ( sends < 0 ){
		printf("Can't send the message! Try it again!\n");
	}

}

int main( int argc, char* argv[]){

	//SOCKET LÉTREHOZÁS

	int sock = 0;

	sock = socket(AF_INET, SOCK_STREAM, 0);

	if( sock < 0 )
		exit(1);

	int ext = 0; 														//függvény által visszaadott érték hibakezelésre

	struct sockaddr_in server;

	server.sin_family = AF_INET;

	server.sin_port = htons(PORT);

	char server_addr[16];

	sprintf(server_addr, "%s", argv[1]);

	server.sin_addr.s_addr = inet_addr(server_addr);

	int server_addrlen = sizeof server;

	char option = 1;

	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&option, sizeof option);
	setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, (char *)&option, sizeof option);

	ext = connect ( sock, (struct sockadrr *) &server, server_addrlen );

	if ( ext < 0 )
	{
		printf("Can't connect to the server!\n");
		exit(2);
	}

	char in_buffer[BUFFER_SIZE];

	char out_buffer[BUFFER_SIZE];


	int len;

	int sends;

	int recvs;

	printf("HELLO THERE! TYPE YOUR MESSAGE HERE!\n");


	for(;;){

		
 		outgoing(out_buffer, len, sends, sock);
 		
		incoming(in_buffer, len, recvs, sock);
		
		
		
	}

}