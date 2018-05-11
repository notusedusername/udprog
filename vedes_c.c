#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <ncurses.h>



#define BUFFER_SIZE 1024

#define PORT 3300


struct args_struct
{
	char buffer[BUFFER_SIZE];

	int len;

	int recvs;

	int sock;

	int flag;
}args;


//void *incoming(char buffer[BUFFER_SIZE], int len, int recvs, int sock){		thread nélkül


void *incoming(void *arguments){
	//BEJÖVŐ ÜZENET

	struct args_struct *args = arguments;
	for(;;){
		args -> recvs = 0;

		args -> len = BUFFER_SIZE+1;
		
		args -> recvs = recv(args -> sock, args -> buffer, args -> len, 0);

		if ( args -> recvs == 0 ){
			printf("Server closed connection. Bye.\n");
			
			close(args -> sock);

			exit(4);	
		}
		else if(args -> recvs > 0 && strcmp(args -> buffer,"<<STOP_*_ACK>>" )) {
		
			printf("%s\n\n",args -> buffer );
			
		}
		else printf("Recv error!\n");

		if( !strcmp(args -> buffer, "<<STOP_*_ACK>>")){

			args -> flag = 1;
		}

		if ( args -> flag == 0 ){

			sprintf(args -> buffer, "CLT_*_ACK");

			send(args -> sock, args -> buffer, strlen(args -> buffer)+1, 0 );
	
		}
	}	
	//incoming(buffer, len, recvs, sock);
}

void outgoing (char buffer[BUFFER_SIZE], int len, int sends, int sock){

	sends = 0;

	//printf("\nYou:\t");

	//scanf("%s", buffer);
	//noecho();

	fgets(buffer, BUFFER_SIZE, stdin);										// a space-ket ignorálja, kicsit pofásabb
		
	len = strlen(buffer)+1;

	//ÜZENET KÜLDÉSE

	sends = send(sock, buffer, len, 0); 								//célsocket, küldendő infó, hossza, 0

	if ( sends < 0 ){
		printf("Can't send the message! Try it again!\n");
	}
	printf("\n");
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


	ext = connect ( sock, (struct sockaddr *) &server, server_addrlen );

	if ( ext < 0 )
	{
		printf("Can't connect to the server!\n");
		exit(2);
	}

	char in_buffer[BUFFER_SIZE];

	char out_buffer[BUFFER_SIZE];


	int len;

	int sends;

	pthread_t thread;

	//args.buffer = in_buffer;

	args.len = 0;

	args.recvs = 0;

	args.sock = sock;

	args.flag = 0;

	if (pthread_create( &thread, NULL, incoming, (void *) &args ) != 0 ){

		printf("Can't create incoming thread. \n");
	}

	for(;;){
 		
 		//initscr();
		//incoming(in_buffer, len, recvs, sock);			tread nélkül
		
		outgoing(out_buffer, len, sends, sock);
		
	}



}
