#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024

#define PORT 3300



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

	ext = connect ( sock, (const struct sockadrr *) &server, server_addrlen );

	if ( ext < 0 )
	{
		printf("Can't connect to the server!\n");
		exit(2);
	}

	char buffer[BUFFER_SIZE];

	int len = 0;																//üzenet mérete

	int flags = 0;

	int sends = 0;

	int recvs = 0;

	printf("HELLO THERE! TYPE YOUR MESSAGE HERE!\n");


	while(1){

		printf("You:\t");

		//scanf("%s", buffer);

		fgets(buffer, BUFFER_SIZE, stdin);										// a space-ket ignorálja, kicsit pofásabb
		
				
		len = strlen(buffer)+1;

		//ÜZENET KÜLDÉSE

		sends = send(sock, buffer, len, flags); 								//célsocket, küldendő infó, hossza, 0

		if ( sends < 0 ){
			printf("Can't send the message! Try it again!\n");
		}

		len = BUFFER_SIZE;

		//VISSZAIHAZOLÁS FOGADÁSA

		recvs = recv(sock, buffer, len, flags);

		if ( strcmp ( buffer, "Noice") == 0){
			printf("\t\t\t\t\t\tSent\n");
		}

		//BEJÖVŐ ÜZENET

		len = BUFFER_SIZE;

		recvs = recv(sock, buffer, len, flags);

		if ( recvs == 0 ){
			printf("Server closed connection.\n");
			return 0;
		}
		else printf("Not you:%s",buffer );
	}

}