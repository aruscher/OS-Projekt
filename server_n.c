#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <dirent.h>


/* the port users will be connecting to */
#define MYPORT 3490

/* how many pending connections queue will hold */
#define BACKLOG 10

// max number of bytes we can get at once
#define MAXDATASIZE 300

/*
//0 if not exist, else 1
int checkStudent(char* mNr){
    DIR *folder = opendir("./");    
    struct dirent *dics;
    struct dirent *file;

    while((dics=readdir(folder))!=NULL){
        while((file=readdir(folder->d_name))!=NULL){
            if(strcmp(mNr,file->d_name)==0){
		 printf("current File: %s",file->d_name);
                return 1;
            }
        } 
    }
    return 0;
}
*/
void sendMsg(int fd,char message[MAXDATASIZE]){
    send(fd,message,strlen(message),0);
}

char* recMsg(int fd){
    printf("Waint for msg\n");
    static char rec[MAXDATASIZE];
    int msg;
    msg = recv(fd,rec,MAXDATASIZE-1,0);
    rec[msg]='\0';
    printf("recMSG: %s\n",rec);
    return rec;
}

void createGroup(int fd){
    char* title = recMsg(fd);
    mkdir(title);
}
//BEGIN: CD


int countSimi(char* student)
{
	char *datenStudent;
	datenStudent=malloc(500);
	datenStudent=student;

	char *geradeGelsenesChar;
	geradeGelsenesChar=malloc(2);
	int datenAnzahl=1;

	int laenge;
	laenge = strlen(datenStudent);

    	int i;
	for(i=0; i<laenge; i++)
	{
		if(datenStudent[i]==59)
		{
			datenAnzahl++;
		}
	}
	return datenAnzahl;
}

double calcAverage(char* student)
{
	char *datenStudent;
	datenStudent=malloc(500);
	datenStudent=student;

	char *notenStudent;
	notenStudent=malloc(500);

	int datenAnzahl=countSimi(datenStudent);

	char *geradeGelsenesChar;
	geradeGelsenesChar=malloc(2);

	int naechstesStudentenAttribut=0;

	int laenge = strlen(datenStudent);

    	int i;
	for(i=0; i<laenge; i++)
	{
		if(datenStudent[i]==59)
		{
			naechstesStudentenAttribut++;
		}
		else
		{
			if(naechstesStudentenAttribut > 4)
			{
				*geradeGelsenesChar=datenStudent[i];
				strcat(notenStudent, geradeGelsenesChar);
			}
		}
	}
	//Ermitteln der Noten
	int laengeNoten = strlen(notenStudent);
	char *letzteNote;
	letzteNote=malloc(500);

	letzteNote[0]='\0';
	double letzterNotenWert=0.0;
	double notenSumme = 0.0;
	double notenDurchschnitt = 0.0;
	int countTo3=0;
	
	naechstesStudentenAttribut = 0;

	for(i=0; i<laengeNoten; i++)
	{
		*geradeGelsenesChar=notenStudent[i];
		//Vergleich auf ASCII-Basis Simikolon => 59 ist ;
		if(countTo3==3)
		{
			printf("%s & ", letzteNote);
			letzterNotenWert = atof(letzteNote);
			notenSumme+=letzterNotenWert;
			letzteNote[0]='\0';
			countTo3=0;
		}
		strcat(letzteNote, geradeGelsenesChar);	
		countTo3+=1;	
	}
	printf("%s", letzteNote);
	letzterNotenWert = atof(letzteNote);
	notenSumme+=letzterNotenWert;
	printf("\nNotenSumme %f\n", notenSumme);
	notenDurchschnitt= notenSumme/(datenAnzahl-5);
	notenDurchschnitt = (int)(notenDurchschnitt*10)/10.0;
	if(laengeNoten==0)
	{
		notenDurchschnitt=0.0;
	}
	printf("Durchschnitte %f\n",notenDurchschnitt);

	return notenDurchschnitt;
}

void getNoten(char* student)
{
	char *datenStudent;
	datenStudent=malloc(500);
	datenStudent=student;

	char *notenStudent;
	notenStudent=malloc(500);

	int datenAnzahl=countSimi(datenStudent);

	char *geradeGelsenesChar;
	geradeGelsenesChar=malloc(2);

	int naechstesStudentenAttribut=0;

	int laenge = strlen(datenStudent);

    	int i;
	//Extrahieren der Noten aus gesamtString
	for(i=0; i<laenge; i++)
	{
		if(datenStudent[i]==59)
		{
			naechstesStudentenAttribut++;
		}
		else
		{
			if(naechstesStudentenAttribut > 4)
			{
				*geradeGelsenesChar=datenStudent[i];
				strcat(notenStudent, geradeGelsenesChar);
			}
		}
	}

	//Ermitteln der Noten
	int laengeNoten = strlen(notenStudent);
	char *letzteNote;
	letzteNote=malloc(500);

	letzteNote[0]='\0';
	double letzterNotenWert=0.0;
	int countTo3=0;
	
	naechstesStudentenAttribut = 0;
	printf("Der Student besitzt %i  Noten : ", datenAnzahl-5);
	for(i=0; i<laengeNoten; i++)
	{
		*geradeGelsenesChar=notenStudent[i];
		//Vergleich auf ASCII-Basis Simikolon => 59 ist ;
		if(countTo3==3)
		{
			printf("%s & ", letzteNote);
			letzterNotenWert = atof(letzteNote);
			letzteNote[0]='\0';
			countTo3=0;
		}
		strcat(letzteNote, geradeGelsenesChar);	
		countTo3+=1;	
	}
	printf("%s\n", letzteNote);

	//Varialbe zum Client senden
}

void getVorname(char* student)
{
	char *datenStudent;
	datenStudent=malloc(500);
	datenStudent=student;

	char vorname[21];
	vorname[0]='\0';

	int laenge;
	int i;
	char *geradeGelsenesChar;
	geradeGelsenesChar=malloc(2);
	int naechstesStudentenAttribut=0;
	laenge = strlen(datenStudent);

	for(i=0; i<laenge; i++)
	{
		if(datenStudent[i]==59)
		{
			naechstesStudentenAttribut++;
		}
		else
		{
			*geradeGelsenesChar=datenStudent[i];
			switch(naechstesStudentenAttribut) 
			{
				case 0:
					strcat(vorname, geradeGelsenesChar);
				break;
			}
		}
	}
	strcat(vorname, "\0");
	printf("vorname: %s. \n", vorname);

	//Varialbe zum Client senden
}

void getNachname(char* student)
{
	char *datenStudent;
	datenStudent=malloc(500);
	datenStudent=student;

	char nachname[21];
	nachname[0]='\0';

	int laenge;
	int i;
	char *geradeGelsenesChar;
	geradeGelsenesChar=malloc(2);
	int naechstesStudentenAttribut=0;
	laenge = strlen(datenStudent);

	for(i=0; i<laenge; i++)
	{
		if(datenStudent[i]==59)
		{
			naechstesStudentenAttribut++;
		}
		else
		{
			*geradeGelsenesChar=datenStudent[i];
			switch(naechstesStudentenAttribut) 
			{
				case 1:
					strcat(nachname, geradeGelsenesChar);
				break;
			}
		}
	}
	strcat(nachname, "\0");
	printf("nachname: %s. \n", nachname);

	//Varialbe zum Client senden
}

void getMnr(char* student)
{
	char *datenStudent;
	datenStudent=malloc(500);
	datenStudent=student;

	char mnr[10];
	mnr[0]='\0';

	int laenge;
	int i;
	char *geradeGelsenesChar;
	geradeGelsenesChar=malloc(2);
	int naechstesStudentenAttribut=0;
	laenge = strlen(datenStudent);

	for(i=0; i<laenge; i++)
	{
		if(datenStudent[i]==59)
		{
			naechstesStudentenAttribut++;
		}
		else
		{
			*geradeGelsenesChar=datenStudent[i];
			switch(naechstesStudentenAttribut) 
			{
				case 2:
					strcat(mnr, geradeGelsenesChar);
				break;
			}
		}
	}
	strcat(mnr, "\0");
	printf("MNR: %s. \n", mnr);

	//Varialbe zum Client senden
}

void getStudiengang(char* student)
{
	char *datenStudent;
	datenStudent=malloc(500);
	datenStudent=student;

	char studiengang[21];
	studiengang[0]='\0';

	int laenge;
	int i;
	char *geradeGelsenesChar;
	geradeGelsenesChar=malloc(2);
	int naechstesStudentenAttribut=0;
	laenge = strlen(datenStudent);

	for(i=0; i<laenge; i++)
	{
		if(datenStudent[i]==59)
		{
			naechstesStudentenAttribut++;
		}
		else
		{
			*geradeGelsenesChar=datenStudent[i];
			switch(naechstesStudentenAttribut) 
			{
				case 3:
					strcat(studiengang, geradeGelsenesChar);
				break;
			}
		}
	}
	strcat(studiengang, "\0");
	printf("studiengang: %s. \n", studiengang);

	//Varialbe zum Client senden
}

void getBday(char* student)
{
	char *datenStudent;
	datenStudent=malloc(500);
	datenStudent=student;

	char bday[21];
	bday[0]='\0';

	int laenge;
	int i;
	char *geradeGelsenesChar;
	geradeGelsenesChar=malloc(2);
	int naechstesStudentenAttribut=0;
	laenge = strlen(datenStudent);

	for(i=0; i<laenge; i++)
	{
		if(datenStudent[i]==59)
		{
			naechstesStudentenAttribut++;
		}
		else
		{
			*geradeGelsenesChar=datenStudent[i];
			switch(naechstesStudentenAttribut) 
			{
				case 4:
					strcat(bday, geradeGelsenesChar);
				break;
			}
		}
	}
	strcat(bday, "\0");
	printf("bday: %s. \n", bday);

	//Varialbe zum Client senden
}


//ENDE: CD


//FAB

//PROBLEM: Wie wird verhindert, dass mehrere Studenten gleiche MNR haben? UserID muss eindeutig sein!
void newStudent(char* student) 
{
	/*int input[50];
	input = seperateCSV(student);*/
	char writeString[MAXDATASIZE];
	strcpy(writeString,student);
	char seps[]   = ";";
	char *token;
	int countSemikolon = 1;
	int input[50];

   	token = strtok(student, seps);	
	while (token != NULL)
	{
		//save current token
		input[countSemikolon] = token;		
      		// Get next token:
      		token = strtok( NULL, seps );

		countSemikolon++;
   	}

	if(chdir(input[4]) == -1) //in den Studiengangsordner wechseln, falls vorhanden
	{ printf("Studiengang %s nicht vorhanden\n", input[4]); }
   	else //vorhanden -> neuen File erstellen
	{
		printf("Erfolgreich nach %s gewechselt!\n", input[4]);
		FILE *newFile = NULL;
		newFile = fopen(input[3], "w");
    		if(newFile)
    		{
			printf("Student angelegt\n");
			fprintf(newFile, "%s", writeString); 
			fclose(newFile);
		}
		
		char parentD[200];
		if(getcwd(parentD, sizeof(parentD)) == NULL)
		{
			printf("Fehler bei getcwd\n");
		}
		else
		{
			char *h;
			h = strrchr(parentD, '/');
			*h = '\0';
			chdir(parentD);
		}
	}
}

int findStudent(int fd)
{
	printf("find Student\n");
    	char* directory;
    	directory = recMsg(fd);
	/*int input[50];
	input = seperateCSV(werte);*/
	char* student;

	char seps[]   = ";";
	char *token;
	int countSemikolon = 1;
	int input[50];

   	token = strtok(directory, seps);	
	while (token != NULL)
	{
		//save current token
		input[countSemikolon] = token;		
      		// Get next token:
      		token = strtok( NULL, seps );

		countSemikolon++;
   	}
	
	// In Gruppen-Verzeichnis wechseln
   	if(chdir(input[1]) == -1) 
	{
		printf("Gruppe nicht vorhanden\n");
		//return EXIT_FAILURE;

   	}
	else
	{
		printf("Erfolgreich nach %s gewechselt!\n", input[1]);

		FILE *pFile = NULL;     
		if( (pFile = fopen(input[2], "r")) == NULL) //TODO: wenn nicht vorhanden, erstellt? NEIN!<-ar
		{
      			printf("Student kann nicht gefunden werden\n");
      			//return EXIT_FAILURE;
		}
		else
		{
			/*fgets(student, 20, pFile);
			printf("%s",student);
			
			int nRet;
   			size_t *t = malloc(0);
   			char **gptr = malloc(sizeof(char*));
   			*gptr = NULL;
	
			while( (nRet=getline(gptr, t, pFile)) > 0)
      			fputs(*gptr,stdout);
			*/
			//TODO: send it to client
			//BEGIN CD

			double notenDurchschnitt = 0.0;

			char *datenStudent;
			datenStudent=malloc(500);

			//char datei[10]="123456789";

			FILE *pFile = NULL;
			//pFile = fopen(datei, "r");
			pFile = fopen(input[2], "r");

			while((fscanf(pFile,"%500s",datenStudent)) != EOF)
			printf("%s\n",datenStudent);
			fclose(pFile);
	
			notenDurchschnitt=calcAverage(datenStudent);
			getVorname(datenStudent);
			getNachname(datenStudent);
			getMnr(datenStudent);
			getStudiengang(datenStudent);
			getBday(datenStudent);
			getNoten(datenStudent);

			//ENDE CD			

			fclose(pFile);

		}
		char parentD[200];
		if(getcwd(parentD, sizeof(parentD)) == NULL)
		{
			printf("Fehler bei getcwd\n");
		}
		else
		{
			char *h;
			h = strrchr(parentD, '/');
			*h = '\0';
			chdir(parentD);
		}
	}
	return 2;
}

int addMark(int fd)
{
	printf("add Mark\n");
    	char* directory;
    	directory = recMsg(fd);
	/*int input[50];
	input = seperateCSV(werte);*/
	char* student;

	char seps[]   = ";";
	char *token;
	int countSemikolon = 1;
	int input[50];

   	token = strtok(directory, seps);	
	while (token != NULL)
	{
		//save current token
		input[countSemikolon] = token;		
      		// Get next token:
      		token = strtok( NULL, seps );

		countSemikolon++;
   	}
	
	// In Gruppen-Verzeichnis wechseln
   	if(chdir(input[1]) == -1) 
	{
		printf("Gruppe nicht vorhanden\n");
		//return EXIT_FAILURE;
   	}
	else
	{
		printf("Erfolgreich nach %s gewechselt!\n", input[1]);

		//TODO: Wenn Datei nicht vorhanden wird sie im moment erstellt -> darf nicht!
		FILE *pFile = NULL;     
		if( (pFile = fopen(input[2], "a")) == NULL)
		{
      			printf("Student kann nicht gefunden werden");
      			//return EXIT_FAILURE;
		}
		else
		{
			fprintf(pFile, ";%s", input[3]); 
			fclose(pFile);
			printf("Note hinzugefügt");
		}

		char parentD[200];
		if(getcwd(parentD, sizeof(parentD)) == NULL)
		{
			printf("Fehler bei getcwd\n");
		}
		else
		{
			char *h;
			h = strrchr(parentD, '/');
			*h = '\0';
			chdir(parentD);
		}
	}
}

int findGroup(int fd) {

	printf("find Group\n");
    	char* directory;
    	directory = recMsg(fd);

	DIR *dir;
	struct dirent *dirzeiger;

  	 /* das Verzeichnis öffnen */
   	if((dir=opendir(directory)) == NULL) 
	{ printf("Fehler bei opendir\n"); }
	else
	{
		off_t pos;

		printf("Im Studiengang %s sind folgende Studenten:\n", directory);
   		/* das komplette Verzeichnis auslesen */
		
   		while((dirzeiger=readdir(dir)) != NULL)
      		{	printf("%s\n",(*dirzeiger).d_name); pos=telldir(dir); printf("Zeiger:%llu\n",pos); }

		//TODO: auch 2 falsche einträge ("." und "..")
		// vllt daher: if entry name is a symbolic link, a value is unspecified

   		/* Lesezeiger wieder schließen */
   		if(closedir(dir) == -1)
      			printf("Fehler beim Schließen von %s\n", directory);
	}
}

/*
int seperateCSV(char* student)
{
	char seps[]   = ";";
	char *token;
	int countSemikolon = 1;
	int input[50];

   	token = strtok(student, seps);	
	while (token != NULL)
	{
		//save current token
		input[countSemikolon] = token;		
      		// Get next token:
      		token = strtok( NULL, seps );

		countSemikolon++;
   	}
	return input;
}*/
//Ende FAB

int createStudent(int fd){
    printf("create Student\n");
    char* student;
    student = recMsg(fd);
    printf("StudentRec: %s \n",student);
    if (strcmp(student,"0")==0){
        printf("creaate Student Abortet\n");
    } else {
        printf("NEED TO WRITE %s TO FILE \n",student);
	newStudent(student);
    }
    return 1;
}

void handleMenu(int fd){
    char* auswahl;
    printf("Handle Menu\n");
    while(1){
        auswahl = recMsg(fd);
        if(strcmp(auswahl,"1")==0){
            createStudent(fd);
        }
        if(strcmp(auswahl,"2")==0){           
		findStudent(fd);
        }
        if(strcmp(auswahl,"3")==0){
            createGroup(fd);
        }
        if(strcmp(auswahl,"4")==0){
            findGroup(fd);
        }
        if(strcmp(auswahl,"5")==0){
            addMark(fd);
        }
    }
}


int main(int argc, char *argv[ ]){
	/* later used for sock-conf */
	int yes = 1;
	
	/* later used */
	int new_fd;

	/* my address information */
	struct sockaddr_in my_addr;

	/* connectors address information */
	struct sockaddr_in their_addr;
	struct sigaction sa;

	/* recive */
	char buf[MAXDATASIZE];
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
	
	/* SOL_SOCKET -> request to socket layer, 
     * SO_REUSEADDR -> socketadress can reused */
	/* checks for mistakes on socket creation */
	if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) ==-1){
		perror("CANT CONF SOCKET");
		exit(1);
	} else {
		printf("SOCKET Configuration successfull\n");
	}

	/* host byte order */
	my_addr.sin_family = AF_INET;

	/* short, network byte order */
	my_addr.sin_port = htons(MYPORT);

	/* automatically fill with my IP */
	my_addr.sin_addr.s_addr = INADDR_ANY;

	printf("Server-Using %s and port %d...\n", inet_ntoa(my_addr.sin_addr), MYPORT);
	memset(&(my_addr.sin_zero), '\0', 8);

	/* tells the OS which port and address used */
	int binder;
	binder = bind(sock,(struct sockaddr *)&my_addr,sizeof(struct sockaddr));
	if(binder==-1){
		perror("CANT BIND SOCKET");
		exit(1);
	} else {
		printf("SOCKET Binded to OS\n");
	}

	/* config listener-queue */
	int listener;
	listener = listen(sock,BACKLOG);
	if(listener==-1){
		perror("CANT LISTEN SOCKET");
		exit(1);
	} else {
		printf("SOCKET Listen\n");
	}

	/* clean all the dead processes */
	//sa.sa_handler = sigchld_handler;
	//sigemptyset(&sa.sa_mask);
	//sa.sa_flags = SA_RESTART;

	/* start accept loop */
    int socklen = sizeof(struct sockaddr_in);
    if((new_fd=accept(sock, (struct sockaddr *)&their_addr, &socklen))==-1){
        perror("CANT ACCEPT CONNECTION");
        exit(1);
    } else {
        printf("SOCKET Accept\n");
    }

    handleMenu(new_fd);
    

	exit(0);
}
