#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <ctype.h>

// the port client will be connecting to
#define MYPORT 3490

// max number of bytes we can get at once
#define MAXDATASIZE 300

void sendMsg(int socket,char message[MAXDATASIZE]){
    send(socket,message,strlen(message),0);
}

char* recMsg(int socket){
    static char rec[MAXDATASIZE];
    int msg;
    msg = recv(socket,rec,MAXDATASIZE-1,0);
    rec[msg]='\0';
    return rec;
}

//mNr if Student, 1 Admin, -1 for invalid
char* manageLogin(int socket){
    char login[11];
    char password[11];
    char message[MAXDATASIZE];
    char* rec;
    int check;
    printf("REC BEFORE: %s\n",rec);
    printf("Anmeldung auf Notenserver\n");
    printf("#########################\n");
    printf("Benutzername(Mnr): >");
    scanf("%s",&login);
    printf("\nPasswort: >");
    scanf("%s",&password);
    sprintf(message,"%s;%s",login,password);
    sendMsg(socket,message); 
    rec=recMsg(socket);
    printf("Rec after: %s\n",rec);
    if(strcmp(rec,"0")==0){
        strcpy(rec,login);
    }
    return rec;
}



//1 is valid, 0 unvalid
int validStudentInput(char* input){
    if(strchr(input,';')){
            return 0;
    }
    return 1;
}

//1 is valid,0 unvalid
int validMarkInput(char* input){
    if(isdigit(input[0])==0){
        printf("point 0 fail");
        return 0;
    }
    if(isdigit(input[2])==0){
        printf("point 1 fail");
        return 0;
    }
    if (input[1]!='.'){
        printf("pointp fail");
        return 0;
    }
    return 1;
}

void showMainMenu(){
	printf("Menu\n----\n");
	printf("1)Student anlegen\n");
	printf("2)Student finden\n");
	printf("3)Studiengang anlegen\n");
	printf("4)Studiengang Mitglieder anzeigen\n");
    printf("5)Note hinzufügen\n");
    printf("6)Studiengangsbesten anzeigen\n");
    printf("7)Gesamtbesten anzeigen\n");
	printf("8)Beenden\n");
	printf("---------------\n");
	printf("Bitte Nummer eingeben:\n >");
}

void findStudent(int socket){
    printf("Student finden\n");
    printf("---------------\n");
    printf("0 für Beenden\n");
    char mNr[11];
    char studiengang[20];
    char auswahl[2];

    printf("Studiengang (max.20): >");
    scanf("%s",studiengang);
    if(strcmp(studiengang,"0")==0){
        sendMsg(socket,"0");
        return;
    }

    printf("Matrikelnummer (max.9): >");
    scanf("%s",mNr);
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
    scanf("%s",auswahl);
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
    char* rec;
    rec = recMsg(socket);
    printf("%s\n", rec);
    return;
}

void addMark(int socket){
    printf("Note hinzufügen\n");
    printf("---------------\n");
    printf("0 für Beenden\n");
    char mNr[11];
    char studiengang[20];
    char note[10]; //bei Länge 3 ist der string zuerst okay, aber beim aneinanderhängen müll
    char auswahl[2];

    printf("Studiengang (max.20): >");
    scanf("%s",studiengang);
    if(strcmp(studiengang,"0")==0){
        sendMsg(socket,"0");
        return;
    }

    printf("Matrikelnummer (max.9): >");
    scanf("%s",mNr);
    if(strcmp(mNr,"0")==0){
        sendMsg(socket,"0");
        return;
    }

    printf("Note (x.y): >");
    scanf("%s/0",note);
    while(!validMarkInput(note)){
        printf("Ungülite Eingabe\n");
        printf("Note (x.y): >");
        scanf("%s/0",note);
    }
    if(strcmp(mNr,"0")==0){
        sendMsg(socket,"0");
        return;
    }

    char backupG[20];
    char backupN[10];
    strcpy(backupG,studiengang);
    strcpy(backupN,note);
    printf("#############################\n");
    printf("Für Student im Studiengang: %s mit Mnr: %s Note %s hinzufügen\n",studiengang,mNr,note);
    printf("Bestätigen(1) Abbruch(0)\n");
    printf(">");
    scanf("%s",auswahl);
    if (strcmp(auswahl,"0")==0){
        return;
    }
    printf("Note %s",note);
    char message[MAXDATASIZE];
    message[0] = '\0';
    strcat(message,backupG);
    strcat(message,";");
    strcat(message,mNr);
    strcat(message,";");
    strcat(message,backupN);

    printf("Message: %s\n",message);
    sendMsg(socket,message);
    char* rec;
    rec = recMsg(socket);
    printf("%s\n", rec);
}

void createStudent(int socket){
    printf("Student anlegen\n");
    printf("---------------\n");
    printf("0 für Beenden\n");
    char passwort[11];
    char vorname[21];
    char nachname[21];
    char bday[11];
    char studiengang[21];
    char auswahl[2];

    printf("Vorname (max.20): >");
    scanf("%s",vorname);
    while(!validStudentInput(vorname)){
        printf("Ungültige Eingabe mit ;\n");
        printf("Vorname (max.20): >");
        scanf("%s",vorname); 
    }
    if(strcmp(vorname,"0")==0){
        sendMsg(socket,"0");
        return;
    }

    printf("Nachname (max.20): >");
    scanf("%s",nachname);
    while(!validStudentInput(nachname)){
        printf("Ungültige Eingabe mit ;\n");
        printf("Nachname (max.20): >");
        scanf("%s",nachname); 
    }
    if(strcmp(nachname,"0")==0){
        sendMsg(socket,"0");
        return;
    }

    printf("Geburtstag (dd.mm.yyyy): >");
    scanf("%s",bday); 
    while(!validStudentInput(bday)){
        printf("Ungültige Eingabe mit ;\n");
        printf("Geburtstag (dd.mm.yyyy): >");
        scanf("%s",bday); 
    }
    if(strcmp(bday,"0")==0){
        sendMsg(socket,"0");
        return;
    }


    printf("Studiengang (muss vorhanden sein, max.20): >");
    scanf("%s",studiengang);
    while(!validStudentInput(studiengang)){
        printf("Ungültige Eingabe mit ;\n");
        printf("Studiengang (max.20): >");
        scanf("%s",studiengang); 
    }
    if(strcmp(studiengang,"0")==0){
        sendMsg(socket,"0");
        return;
    }

    printf("Passwort (max.10): >");
    scanf("%s",passwort);
    while(!validStudentInput(passwort)){
        printf("Ungültige Eingabe mit ;\n");
        printf("Passwort (max.10): >");
        scanf("%s",passwort); 
    }
    if(strcmp(passwort,"0")==0){
        sendMsg(socket,"0");
        return;
    }

    char backup1[11];
    char backup2[20];
    strcpy(backup1,bday);
    strcpy(backup2,studiengang);
    printf("#############################\n");
    printf("Zusammenfassung:\n");
    printf("%s %s, Studiengang: %s, Geb.: %s \n, Passwort: %s",vorname,nachname,studiengang,bday,passwort);
    printf("Bestätigen(1) Abbruch(0)\n");
    printf(">");
    scanf("%s",auswahl);
    if (strcmp(auswahl,"0")==0){
        return;
    }

    char message[MAXDATASIZE];
    sprintf(message,"%s;%s;%s;%s;%s",passwort,vorname,nachname,backup2,backup1);
    sendMsg(socket,message);	
    char* rec;
    rec = recMsg(socket);
    printf("%s\n", rec);

}

void findGroup(int socket)
{
    printf("Studiengangsmitglieder finden\n");
    printf("---------------\n");
    printf("0 für Beenden\n");
    char title[MAXDATASIZE];
    printf("Bitte geben Sie den gesuchten Studiengang an: >");
    scanf("%s",title);
    if(strcmp(title,"0")==0)
    {
        sendMsg(socket,"0");
        return;
    }
    sendMsg(socket,title);
    printf("Folgende Studenten enthalten:\n");
    char* rec;
    while(strcmp(rec = recMsg(socket),"0") != 0)
	{printf("%s\n", rec);}
}

void createGroup(int socket){
    printf("Studiengang anlegen\n");
    printf("---------------\n");
    printf("0 für Beenden\n");
    char title[MAXDATASIZE];
    printf("Bitte Titel eingeben: >");
    scanf("%s",title);
    if(strcmp(title,"0")==0){
        sendMsg(socket,"0");
        return;
    }
    sendMsg(socket,title);
    char* rec;
    rec = recMsg(socket);
    printf("%s\n", rec);
}

void groupsBest(int socket)
{
    printf("Studiengangsbesten anzeigen\n");
    printf("---------------\n");
    printf("0 für Beenden\n");
    char title[MAXDATASIZE];
    printf("Bitte geben Sie den gesuchten Studiengang an: >");
    scanf("%s",title);
    /*if(strcmp(title,"0")==0)
    {
        sendMsg(socket,"0");
        return;
    }*/ //Unnötig, da dann ja eh title=="0" und beim sendMsg eh so verschickt?!
    sendMsg(socket,title);
    char* rec;
    rec = recMsg(socket);
    printf("%s\n", rec);
}

void bestOfAll(int socket)
{
    printf("Gesamtbesten anzeigen\n");
    printf("---------------\n");

    char* rec;
    rec = recMsg(socket);
    printf("%s\n", rec);
}

void exitProgramm(){
	exit(0);
}
char MainMenu(int socket){
	int option;
	showMainMenu();
	while(1){
		scanf("%i",&option);
		switch(option){
			case 0: system("clear");showMainMenu();break;
			case 1: system("clear");sendMsg(socket,"1");createStudent(socket);return '1';
			case 2: system("clear");sendMsg(socket,"2");findStudent(socket);return '2';
			case 3: system("clear");sendMsg(socket,"3");createGroup(socket);return '3';
			case 4: system("clear");sendMsg(socket,"4");findGroup(socket);return '4';
			case 5: system("clear");sendMsg(socket,"5");addMark(socket);return '5';
			case 6: system("clear");sendMsg(socket,"6");groupsBest(socket);return '6';
			case 7: system("clear");sendMsg(socket,"7");bestOfAll(socket);return '7';
			case 8: sendMsg(socket,"8");exitProgramm(); break;
			default: printf("Ungültige Nummer. 0 für Hauptmenu\n >"); break;
		}
	
	}
	return '9';
}

void showSMenu(){
	printf("Menu\n----\n");
	printf("1)Daten anzeigen\n");
	printf("2)Beenden\n");
	printf("---------------\n");
	printf("Bitte Nummer eingeben:\n >");
}

void showSData(int socket,char* mNr){
	char go[10];
	char mnr[10];
	strcpy(mnr,mNr);
    printf("Meine Daten\n");
    printf("----------------\n");
    sendMsg(socket,mNr);
    printf("Vorname: %s\n",recMsg(socket));
    printf("Nachname: %s\n",recMsg(socket));
    printf("Geburstag: %s\n",recMsg(socket));
    printf("Matrikelnummer: %s\n",recMsg(socket));
    printf("Password: %s\n",recMsg(socket));
    printf("Studiengang: %s\n",recMsg(socket));
    printf("---------------\n");
    printf("1 für Weiter\n");
	printf(">");
	scanf("%s",&go);
	while(strcmp(go,"1")!=0){
		printf("Ungültige Eingabe\n");
    	printf("1 für Weiter\n");
		printf(">");
		scanf("%s",go);
	}
	system("clear");
}

char SMenu(int socket, char* mNr){

	int option;
	char mnr[10];
	strcpy(mnr,mNr);
	showSMenu();
	while(1){
		scanf("%i",&option);
		switch(option){
            case 0: system("clear");showSMenu();break;
            case 1: system("clear");sendMsg(socket,"9");showSData(socket,mNr);return '9';
			case 2: sendMsg(socket,"8");exitProgramm();
			default: printf("Ungültige Nummer. 0 für Hauptmenu\n >"); break;
		}
	
	}
	return 'X';
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
    char* typ;
    typ = manageLogin(sock);
	char mnr[11];
	strcpy(mnr,typ);
    printf("TYP: %s\n",mnr);
	while(run){
        if(strcmp(typ,"-1")==0){
            printf("Login fehlerhaft\n");
            while(strcmp(typ,"-1")==0){
                typ = manageLogin(sock);
                printf("TYP: %s\n",typ);
            }
            break;
        } else if(strcmp(typ,"1")==0){
            while(1){
                MainMenu(sock);
            }
        } else {
            while(1){
				SMenu(sock,mnr);				
            }
        }
	}
	return 0;
}


