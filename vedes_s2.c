#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>

#define PORT_NO 3300
#define BUFF_SIZE 1024
#define MAX_CLIENT 4
#define ROOM1 ((const unsigned char*)"ROOM1")
#define ROOM2 ((const unsigned char*)"ROOM2")


struct args_struct
{

	int sock[MAX_CLIENT];

	int mysock;

	int counter;
}args;

struct chat_rooms{
	
	int r1sock[2];

	int r2sock[2];

	int choose[BUFF_SIZE];

	int choose_count;
}rooms;


int ack( int sock){

	char buffer[BUFF_SIZE];

	recv(sock, buffer, BUFF_SIZE, 0);

	if(!strcmp(buffer, "CLT_*_ACK"))
		return 0;
	else 
		return -1;
}

void sendctrl(int sock, char message[]){

	int ctrl = ack(sock);

	if (ctrl == -1){

		printf("Send error, retry\n");

		send(sock, message, strlen(message)+1, 0);

		sendctrl(sock, message);
	}

}
	
void terminate(int thread_no, int sock, int mysock){
	

	args.counter--;
	//free(sock_desc);
	
	if(args.counter == 0 ){
		
		printf("All clients disconnected, shutdown...\n");

		close(sock);

		close(mysock);

		exit(0);

	}
	else{
		close(sock);

		pthread_exit(0);
	}

}

int chat_rooms(int sock, char message[], char in_message[BUFF_SIZE], int thread_no, void *arguments){


	sprintf(message, "We have 2 chat rooms. These are:\n\t 1.) %s\n\t 2.) %s\n You can choose by typing its number.\n", ROOM1, ROOM2 );
	//printf("%s\n", message);
	int sends = 0, try = 0;
	
	sends = send (sock, message, strlen(message), 0);

	sendctrl(sock, message);


	LOOP1: if(sends > 0){

		printf("Roomlist sent to client%d\n", thread_no+1);
	}

	else{
		try++;

		if(try == 1)
			printf("Can't send to %d roomlist! Retry..\n", thread_no);

		goto LOOP1;
	}	
		
	int recvs;

	LOOP2: recvs = recv( sock, in_message, BUFF_SIZE, 0 );

	if ( recvs == 0 ){
		printf("Client%d disconnected.\n", thread_no+1);
		return 0;
	}

	else if ( recvs < 0)
		printf("Recv error!\n");

	else {
		int choose = 0;
		
		choose = atoi(in_message);

		rooms.choose[thread_no] = choose;

		int flag = 0;

		if (choose == 1 || choose == 2){

			switch (choose){

				case 1:{
					
					printf("Client%d: Selected room is %s\n",thread_no+1,ROOM1 );

					sprintf(message,"<<Selected room is %s>>\n",ROOM1 );
					
					send (sock, message, strlen(message)+1, 0);

					sendctrl(sock, message);

					for ( int i = 0; i < 2; i++){
						
						if (rooms.r1sock[i] != 0 ){
							flag++;
						}

					}

					if ( flag == 0 ){
						sprintf(message, "<<Waiting for partner...>>\n");
						
						send(sock, message, strlen(message)+1, 0);
						
						sendctrl(sock, message);

						printf("Waiting for partner...\n");
					}
					else if (flag == 1) {
						

						printf("Connected, you can start chatting.\n");
					}

					else {
							sprintf(message,"<<Room is full. Try an another one.>>\n");
						
							send (sock, message, strlen(message)+1, 0);

							sendctrl(sock, message);

							goto LOOP2;
					}
					for ( int i = 0; i < 2; i++){
						if(rooms.r1sock[i] == 0){

							rooms.r1sock[i] = sock;

							break;
						}

					}
					break;
				}

				case 2:{
					printf("Client%d: Selected room is %s\n",thread_no+1,ROOM2 );

					sprintf(message, "<<Selected room is %s>>\n",ROOM2 );
					
					send (sock, message, strlen(message)+1, 0);

					sendctrl(sock, message);

					for ( int i = 0; i < 2; i++){
						
						if (rooms.r2sock[i] != 0 ){
							flag++;
						}

					}

						if ( flag == 0 ){
							sprintf(message, "<<Waiting for partner...>>\n");
							
							send(sock, message, strlen(message)+1, 0);
							
							sendctrl(sock, message);

							printf("Waiting for partner...\n");
						}
						else if (flag == 1) {
							

							printf("Connected, you can start chatting.\n");
						}

						else {
								sprintf(message,"<<Room is full. Try an another one.>>\n");
							
								send (sock, message, strlen(message)+1, 0);

								sendctrl(sock, message);

								goto LOOP2;
						}
						for ( int i = 0; i < 2; i++){
							if(rooms.r2sock[i] == 0){

								rooms.r2sock[i] = sock;

								break;
							}

						}
					break;
				}
			}
		}
		
		else{

			sprintf(message, "<<Funny... try again.>>\n");	
		
			send (sock, message, strlen(message)+1, 0);

			sendctrl(sock, message);

			goto LOOP2;

		} 
	}

	return recvs;

}

void *handler( void *arguments){

	struct args_struct *args = arguments;

	int thread_no = (args -> counter)-1;

	printf("counter: %d  thread_no: %d\n", args->counter, thread_no);

	char message[BUFF_SIZE], in_message[BUFF_SIZE], name[BUFF_SIZE];	

	sprintf(message, "<<Server message: Type your name!>>\n\nYour name: ");

	if(send (args -> sock[thread_no], message, sizeof(message)+1, 0)){

		printf("Allow sent\n");
	}

	sendctrl(args -> sock[thread_no], message);

	int recvs;

	recvs = recv( args -> sock[thread_no], in_message, BUFF_SIZE, 0 );

	if ( recvs == 0 ){
		
		terminate(thread_no, args -> sock[thread_no], args -> mysock);

	}

	else if ( recvs < 0)
		printf("Recv error!\n");

	printf("Client%i name: %s\n",thread_no+1, in_message);

	strcpy(name, in_message);

	for(int i = 0; i< sizeof name; i++ ){

		if (name[i] == '\n')
			name[i] = name[i+1];
	}

	sprintf(message, "<<Hello there %s!>>\n", name);

	if(send (args -> sock[thread_no], message, sizeof(message)+1, 0)){

		printf("Greeting sent\n");
	}

	sendctrl(args -> sock[thread_no], message);

	if (chat_rooms(args -> sock[thread_no], message, in_message, thread_no, arguments) == 0){

		terminate(thread_no, args -> sock[thread_no], args -> mysock);
	}

	int psock = 0; 																	//partner's socket
	for(;;){
		if(rooms.choose[thread_no] == 1){
			
			if (rooms.r1sock[0] != 0 && rooms.r1sock[1] != 0){
				printf("Partners connected.\n" );

				for(int i = 0; i < 2 ; i++){

					if(rooms.r1sock[i] != args -> sock[thread_no])
						psock = rooms.r1sock[i];
						
				}

				sprintf(message, "<<Connected to partner, you can start chatting.>>\n-------------------------------------------------");

				send (args -> sock[thread_no], message, sizeof(message)+1, 0);

				sendctrl(args -> sock[thread_no], message);

				break;
			}
		}

			if(rooms.choose[thread_no] == 2){
				if (rooms.r2sock[0] != 0 && rooms.r2sock[1] != 0){
					printf("Partners connected.\n" );

					for(int i = 0; i < 2 ; i++){

						if(rooms.r2sock[i] != args -> sock[thread_no])
							psock = rooms.r2sock[i];
					}
					sprintf(message, "<<Connected to partner, you can start chatting.>>\n-------------------------------------------------");

					send (args -> sock[thread_no], message, sizeof(message)+1, 0);
					
					sendctrl(args -> sock[thread_no], message);

					break;

				}
			}
	}

	sprintf(message, "<<STOP_*_ACK>>");

	send(args -> sock[thread_no], message, strlen(message)+1, 0);

	while ( recvs != 0 ){

		//BEJÖVŐ

		recvs = recv( args -> sock[thread_no], in_message, BUFF_SIZE, 0 );

		if ( recvs == 0 ){
			printf("Client%i disconnected.\n", thread_no+1);

			sprintf(message, "<<%s disconnected.>>\n\n << You can start a new conversation, or exit.>>\n<< If you don't know what to do type --help.>>", name);	
			
			if(send (psock, message, strlen(message)+1, 0)){

				printf("Server message sent to %d\n", args -> sock[thread_no]);
			}


			for(int i = 0; i < 2 ; i++){

				if(rooms.r1sock[i] == args -> sock[thread_no])
					rooms.r1sock[i] = 0;	
			}
			terminate(thread_no, args -> sock[thread_no], args -> mysock);

		}

		else if ( recvs < 0)
			printf("Recv error!\n");

		else {
			
			printf("Client%i message: %s\n",thread_no+1, in_message);
		}

		if(!strcmp(in_message, "--exit\n")){
			printf("Client%i disconnected.\n", thread_no+1);

			sprintf(message, "<<%s disconnected.>>\n\n << You can start a new conversation, or exit.>>\n<< If you don't know what to do type --help.>>", name);	
			
			if(send (psock, message, strlen(message)+1, 0)){

				printf("Server message sent to %d\n", args -> sock[thread_no]);
			}


			terminate(thread_no, args -> sock[thread_no], args -> mysock);

		}
	
		else if (!strcmp(in_message, "--help\n")){
			sprintf(message, "<<'--exit' - leave the room and quit.>>\n<<'--change' - Changes room... surpriseee>>\n");
			
			send ( args -> sock[thread_no], message, strlen(message)+1, 0);

			continue;
		}

		else if(!strcmp(in_message, "--change\n")){
			
			printf(message, "Changing  room ...\n");



			if (chat_rooms(args -> sock[thread_no], message, in_message, thread_no, arguments) == 0){

				terminate(thread_no, args -> sock[thread_no], args -> mysock);
			}

		}


		//KIMENő


		sprintf(message, "%s: %s", name, in_message);

		send (psock, message, strlen(message)+1, 0);

		printf("Message sent to %d\n", psock);
		
	
		
	}


	terminate(thread_no, args -> sock[thread_no], args -> mysock);

}


int main( int argc, char* argv[]){

	int sock, new_socket, c, *new_sock;

	struct sockaddr_in server, client;

	char* message;

	sock = socket(AF_INET, SOCK_STREAM, 0);

	if ( sock == -1 ){
		
		printf("Socket create error!\n");

		return 1;
	}

	printf("Socket created.\n");

	server.sin_family = AF_INET;

	server.sin_addr.s_addr = INADDR_ANY;

	server.sin_port = htons(PORT_NO);

	args.counter = 0;

	if ( bind(sock, (struct sockaddr *) &server, sizeof(server)) == -1 ){

		printf("Bind error!\n");
	
		return 2;
	}

	printf("Bind done.\n");

	listen( sock, 4 );

	printf("Waiting incoming connection...\n");

	c = sizeof(struct sockaddr_in );
	for (int i = 0; i < sizeof rooms.r1sock ; ++i)
		{
			rooms.r1sock[i] = 0;

			rooms.r2sock[i] = 0;
		}

	while ( new_socket = accept( sock, ( struct sockaddr *)&client, (socklen_t* ) &c )){

		printf("Connection accepted\n");
	
		message = "<<Server message: Connection accepted>>\n";

		if ( send (new_socket, message, strlen(message)+1, 0) == -1 )

			printf("Cannot send :/\n");

		sendctrl(new_socket, message);

		pthread_t thread;

		new_sock = malloc(1);

		*new_sock = new_socket;

		int recvs = 0;

		args.mysock = sock;

		args.sock[args.counter] = new_socket;

		args.counter++;

		if(args.counter > MAX_CLIENT){

			message = "<<Server message: We are full, sorry. Try again later...>>";

			send(new_socket, message, strlen(message)+1, 0);

			close(new_socket);

			args.counter --;

			if (send (new_socket, message, strlen(message)+1, 0) > 0)

				printf("Deny send OK\n");

		}

		else if ( pthread_create ( &thread, NULL, handler, (void*) &args) < 0){

			message = "<<Server message: Something went wrong, sorry. Try again later...>>";

			close(new_socket);

			args.counter --;

			send (new_socket, message, strlen(message)+1, 0);

			sendctrl(*new_sock, message);

				

		}
	
		else if ( new_socket < 0 ){

			printf("Accept error!\n");

			return 3;
		}


	}

	close(sock);

	return 0;
}
