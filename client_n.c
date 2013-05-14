#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>



// the port client will be connecting to
#define MYPORT 3490

// max number of bytes we can get at once
#define MAXDATASIZE 300

int main(int argc, char *argv[ ]){
	char buf[MAXDATASIZE];

	// connectors address information
   	struct sockaddr_in their_addr;

	//Container for serveraddr
	struct hostent *he;

	//Recv holder
	int recieve;
	/* create a new socket */
	/* AF_INET -> IPV4; SOCK_STREAM -> TCP */
	int sock;
	sock = socket(AF_INET, SOCK_STREAM, 0);

	/* checks for mistakes on socket creation */
	if(sock==-1){
		perror("CANT CREATE SOCKET");
		exit(1);
	} else {
		printf("SOCKET Creation successfull\n");
	}
	
	if(argv[1]==NULL){
		perror("Wrong Syntax: client $IPAddr");
		exit(1);
	}

	/* connect to server */
	he = gethostbyname(argv[1]);
	//get host by name of the given ip
	if(he == NULL){
		perror("CANT FIND SERVER");
		exit(1);
	} else {
		printf("Server found at %s\n",argv[1]);
	}


    	// host byte order
    	their_addr.sin_family = AF_INET;

	/* short, network byte order */
	their_addr.sin_port = htons(MYPORT);
	their_addr.sin_addr = *((struct in_addr *)he->h_addr);
	memset(&(their_addr.sin_zero), '\0', 8);
	printf("CONNECT to %s:%d\n",argv[1],MYPORT);
	if((connect(sock, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)))==-1){
		perror("CANT CONNECT");
		exit(1);
	} else {
		printf("Connected\n");
	}
	recieve = recv(sock,buf,MAXDATASIZE-1,0);
	buf[recieve]= '\0';
	printf("%s \n",buf);

	int run = 1;
	char message[MAXDATASIZE];
	while(run){
		int option;
		option = printMainMenu();
        message[0] = option;
		//printf(">");
		//scanf("%s",&message);
		send(sock,message,strlen(message),0);
		recieve = recv(sock,buf,MAXDATASIZE-1,0);
		buf[recieve]= '\0';
		printf("RECIVE: %s \n",buf);
	}
	return 0;
}

int printMainMenu(){
	int option;
	int i = 1;
	printf("Menu\n----\n");
	printf("1)Student anlegen\n");
	printf("2)Student finden\n");
	printf("3)Gruppe anlegen\n");
	printf("4)Gruppe finden\n");
	printf("5)Beenden\n");
	printf("---------------\n");

	while(i==1){
		printf("Bitte Nummer eingeben:");
		scanf("%i",&option);
		if(option==1 ||option==2 ||option==3 ||option==4 ||option==5 ){
			return option;
		} else {
			printf("Ungültige Eingabe\n");
		} 
	}
}

