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

void sendMsg(int socket,char message[MAXDATASIZE]){
    send(socket,message,strlen(message),0);
}

void showMainMenu(){
	printf("Menu\n----\n");
	printf("1)Student anlegen\n");
	printf("2)Student finden\n");
	printf("3)Gruppe anlegen\n");
	printf("4)Gruppe finden\n");
	printf("5)Beenden\n");
	printf("---------------\n");
	printf("Bitte Nummer eingeben:\n >");
}

//FAB
void findStudent(int socket){
    printf("Student finden\n");
    printf("---------------\n");
    printf("0 für Beenden\n");
    char mNr[11];
    char studiengang[20];
    char auswahl[1];

    printf("Studiengang (max.20): >");
    scanf("%s",&studiengang);
    if(strcmp(studiengang,"0")==0){
        sendMsg(socket,"0");
        return;
    }

    printf("Matrikelnummer (max.9): >");
    scanf("%s",&mNr);
    if(strcmp(mNr,"0")==0){
        sendMsg(socket,"0");
        return;
    }

    char backupG[20];
    strcpy(backupG,studiengang);
    printf("#############################\n");
    printf("Sie suchen den folgenden Student:\n");
    printf("Studiengang: %s, Mnr: %s\n",studiengang,mNr);
    printf("Bestätigen(1) Abbruch(0)\n");
    printf(">");
    scanf("%s",&auswahl);
    if (strcmp(auswahl,"0")==0){
        return;
    }

    char message[MAXDATASIZE];
    message[0] = '\0';
    strcat(message,backupG);
    strcat(message,";");
    strcat(message,mNr);

    printf("Message: %s\n",message);
    sendMsg(socket,message);
}
//Ende FAB

void createStudent(int socket){
    printf("Student anlegen\n");
    printf("---------------\n");
    printf("0 für Beenden\n");
    char vorname[23];
    char nachname[23];
    char mNr[11];
    char bday[10];
    char studiengang[20];
    char auswahl[1];

    printf("Vorname (max.20): >");
    scanf("%s",&vorname);
    if(strcmp(vorname,"0")==0){
        sendMsg(socket,"0");
        return;
    }

    printf("Nachname (max.20): >");
    scanf("%s",&nachname);
    if(strcmp(nachname,"0")==0){
        sendMsg(socket,"0");
        return;
    }

    printf("Matrikelnummer (max.9): >");
    scanf("%s",&mNr);
    if(strcmp(mNr,"0")==0){
        sendMsg(socket,"0");
        return;
    }

    printf("Geburtstag (dd.mm.yyyy): >");
    scanf("%s",&bday); 
    if(strcmp(bday,"0")==0){
        sendMsg(socket,"0");
        return;
    }


    printf("Studiengang (max.20): >");
    scanf("%s",&studiengang);
    if(strcmp(studiengang,"0")==0){
        sendMsg(socket,"0");
        return;
    }

    char backup1[11];
    char backup2[20];
    strcpy(backup1,bday);
    strcpy(backup2,studiengang);
    printf("#############################\n");
    printf("Zusammenfassung:\n");
    printf("%s %s, Mnr: %s, Studiengang: %s, Geb.: %s \n",vorname,nachname,mNr,studiengang,bday);
    printf("Bestätigen(1) Abbruch(0)\n");
    printf(">");
    scanf("%s",&auswahl);
    if (strcmp(auswahl,"0")==0){
        return;
    }

    char message[MAXDATASIZE];
    message[0] = '\0';
    strcat(message,vorname);
    strcat(message,";");
    strcat(message,nachname);
    strcat(message,";");
    strcat(message,mNr);
    strcat(message,";");
    strcat(message,backup2);
    strcat(message,";");
    strcat(message,backup1);

    printf("Message: %s\n",message);
    sendMsg(socket,message);
}

void createGroup(int socket){
    char title[MAXDATASIZE];
    printf("Bitte Titel eingeben: >");
    scanf("%s",&title);
    sendMsg(socket,title);

}

void exitProgramm(){
	exit(0);
}
char MainMenu(int socket){
	int option;
	int i = 1;
	showMainMenu();

	while(i==1){
		scanf("%i",&option);
		switch(option){
			case 0: system("clear");showMainMenu();break;
			case 1: system("clear");sendMsg(socket,"1");createStudent(socket);return '1';
			case 2: system("clear");sendMsg(socket,"2");findStudent(socket);return '2';
			case 3: system("clear");sendMsg(socket,"3");createGroup(socket);return '3';
			case 4: system("clear");sendMsg(socket,"4");return '4';
			case 5: exitProgramm(); break;
			default: printf("Ungültige Nummer. 0 für Hauptmenu\n >"); break;
		}
	
	}
	return '9';
}

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

	int run = 1;
	char message[MAXDATASIZE];
	while(run){
       // scanf("%s",&message);
        //sendMsg(sock,message);
        //system("clear");
		MainMenu(sock);
    	//message[0] = option;
		//printf(">");
		//scanf("%s",&message);
		//send(sock,message,strlen(message),0);
		//recieve = recv(sock,buf,MAXDATASIZE-1,0);
		//buf[recieve]= '\0';
		//printf("RECIVE: %s \n",buf);
	}
	return 0;
}


