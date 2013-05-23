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
#include <sys/stat.h>

/* the port users will be connecting to */
#define MYPORT 3490

/* how many pending connections queue will hold */
#define BACKLOG 10

// max number of bytes we can get at once
#define MAXDATASIZE 300


//0 if not exist, else 1
int checkStudent(char* mNr){
    DIR *folder = opendir("./");
	DIR *sfolder;
	struct stat attribut;
	struct dirent *mainfile;
	struct dirent *subfile;
	while((mainfile=readdir(folder))!=NULL){
		stat(mainfile->d_name,&attribut);
		if( attribut.st_mode & S_IFDIR){
			char* name = mainfile->d_name;
			if(strcmp(name,".")!=0 && strcmp(name,"..") && strcmp(name,".git")){
				printf("DIR: %s\n",mainfile->d_name);
				sfolder = opendir(mainfile->d_name);
				while((subfile=readdir(sfolder))!=NULL){
					char* subname = subfile->d_name;
					if(strcmp(subname,mNr)==0){
						return 1;
					}
					printf("%s/%s\n",mainfile->d_name,subfile->d_name);
				}
			}
		}
	}
	if(folder==NULL){
		perror("opendir");
	}
	printf("After traversal\n");
    return 0;
}

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
	if(strcmp(title,"0")==0){
		return;
	}
	if(mkdir(title,S_IRWXU|S_IRGRP|S_IXGRP)!=0){
		perror("mkdir\n");
		sendMsg(fd,"Gruppe konnte nicht erstellt werden");
	}
	printf("Group created\n");
	sendMsg(fd,"Gruppe erfolreich erstellt");
	return;
}

double average(char* student)
{
	char seps[]   = ";";
	char* token;
	int countSemikolon = 0;
	char* input[MAXDATASIZE];

	double notenSumme = 0.0;
	double notenwert=0.0;
	double notenDurchschnitt = 0.0;

   	token = strtok(student, seps);	
	while (token != NULL)
	{
		countSemikolon++;

		//save current token
		input[countSemikolon] = token;	
		printf("token: %s, attribute: %s\n", token, input[countSemikolon]);
		if(countSemikolon >6)
		{
			notenwert = atof(token); //String into Double
			notenSumme += notenwert;
		}
      		// Get next token:
      		token = strtok( NULL, seps );
   	}

	if(countSemikolon > 6)
	{
		//printf("\nNotenAnzahl %i\n", (countSemikolon-5);
		printf("\nNotenSumme %f\n", notenSumme);
		notenDurchschnitt = notenSumme/(countSemikolon-6);
		notenDurchschnitt = (int)(notenDurchschnitt*10)/10.0;
		printf("\nNotenDurchschnitt %f\n", notenDurchschnitt);
		
		return notenDurchschnitt;
	}
	return -1;
}
/*
int newStudent(char* student) 
{
	char mnrCounter[9]= "\0";
	int mnrCounterInt = 0;

	char writeString[MAXDATASIZE];
	strcpy(writeString,student);
	
	char seps[]   = ";";
	char* token;
	int countSemikolon = 1;
	char* input[MAXDATASIZE];

   	token = strtok(student, seps);	
	while (token != NULL)
	{
		//save current token
		input[countSemikolon] = token;	
		printf("token: %s, attribute: %s\n", token, input[countSemikolon]);
      		// Get next token:
      		token = strtok( NULL, seps );

		countSemikolon++;
   	}
	
	FILE *mnrFile = NULL;
	if((mnrFile = fopen("MNR", "r")) == NULL)
		printf("Fehler1 beim MatrikelCounter");
	else
	{
		fgets(mnrCounter,15,mnrFile);
		mnrCounterInt = atoi(mnrCounter);
		mnrCounterInt++;
		fclose(mnrFile); 
	}

	printf("Counter++: %i", mnrCounterInt); 

	if((mnrFile = fopen("MNR", "w")) == NULL)
		printf("Fehler2 beim MatrikelCounter");
	else
	{
		fprintf(mnrFile, "%i", mnrCounterInt); 
		fclose(mnrFile);
	}

	if(chdir(input[4]) == -1) //in den Studiengangsordner wechseln, falls vorhanden
	{ printf("Studiengang %s nicht vorhanden\n", input[4]); }
   	else //vorhanden -> neuen File erstellen
	{
		printf("Erfolgreich nach %s gewechselt!\n", input[4]);
		FILE *newFile = NULL;
		newFile = fopen(mnrCounter, "w");
		if(newFile == NULL){
			perror("fopen");
		}
		printf("MNR ist: %s/n", mnrCounter);
    		if(newFile)
    		{
			printf("Student angelegt\n");
			fprintf(newFile, "%s;%s", mnrCounter,writeString);

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
		return 1;
	}
	return -1;
}*/

int findStudent(int fd)
{
	printf("find Student\n");
	char* directory;
	directory = recMsg(fd);

	if(strcmp(directory,"0")==0){
		return;//TODO: Fehlerstring an Client?
	}

	char seps[]   = ";";
	char* token;
	int countSemikolon = 1;
	char* input[MAXDATASIZE];

   	token = strtok(directory, seps);	
	while (token != NULL)
	{
		//save current token
		input[countSemikolon] = token;	
		printf("token: %s, attribute: %s\n", token, input[countSemikolon]);
      		// Get next token:
      		token = strtok( NULL, seps );

		countSemikolon++;
   	}
	
	// In Gruppen-Verzeichnis wechseln
   	if(chdir(input[1]) == -1) 
	{
		printf("Gruppe nicht vorhanden\n");
		sendMsg(fd, "\nGruppe nicht vorhanden.\n");
   	}
	else
	{
		printf("Erfolgreich nach %s gewechselt!\n", input[1]);

		FILE *pFile = NULL;     
		if( (pFile = fopen(input[2], "r")) == NULL)
		{
      			printf("Student kann nicht gefunden werden\n");
      			sendMsg(fd, "\nDer Student konnte nicht gefunden werden.\n");
		}
		else
		{
			char *datenStudent;
			datenStudent=malloc(500);

			while((fscanf(pFile,"%500s",datenStudent)) != EOF)
			printf("%s\n",datenStudent);
			fclose(pFile);

			//char* token2;
			countSemikolon = 0;
			char* student[MAXDATASIZE];

   			token = strtok(datenStudent, seps);	
			while (token != NULL)
			{
				countSemikolon++;

				//save current token
				student[countSemikolon] = token;	
      				// Get next token:
      				token = strtok( NULL, seps );
   			}

			char message[MAXDATASIZE];
			message[0] = '\0';
			strcat(message,"\nMNR: ");strcat(message,student[1]);
			strcat(message,"\nPasswort: ");strcat(message,student[2]);
			strcat(message,"\nVorname: ");strcat(message,student[3]);
			strcat(message,"\nName: ");strcat(message,student[4]);
			strcat(message,"\nGruppe: ");strcat(message,student[5]);
			strcat(message,"\nGeb: ");strcat(message,student[6]);

			if(countSemikolon < 7)
				sendMsg(fd, message);
			else
			{
				int i;
				for(i = 7; i <= countSemikolon; i++)
				{
					strcat(message,"\nNote: ");strcat(message,student[i]);
				}
				strcat(message,"\n\n");
				sendMsg(fd, message);
			} 
			
			/*double avg = average(datenStudent); //TODO char** übergeben? Durchschnitt ausgeben?
			if(avg == -1)
			{
				printf("Keine Noten vorhanden\n");
				sendMsg(fd, "\nStudent wurde gefunden. Zukünftig hier Daten des Studenten.\n");
			}
			else
			{
				sendMsg(fd, "\nStudent wurde gefunden. Zukünftig hier Daten des Studenten.\n");
			}*/			

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
	char* student;

	char seps[]   = ";";
	char* token;
	int countSemikolon = 1;
	char* input[MAXDATASIZE];

   	token = strtok(directory, seps);	
	while (token != NULL)
	{
		//save current token
		input[countSemikolon] = token;	
		printf("token: %s, attribute: %s\n", token, input[countSemikolon]);
      		// Get next token:
      		token = strtok( NULL, seps );

		countSemikolon++;
   	}

	if(checkStudent(input[2]) == 1)
	{
		// In Gruppen-Verzeichnis wechseln
   		if(chdir(input[1]) == -1) 
		{
			printf("Gruppe nicht vorhanden\n");
			sendMsg(fd, "\nGruppe nicht gefunden.\n");
   		}
		else
		{
			printf("Erfolgreich nach %s gewechselt!\n", input[1]);
	
			FILE *pFile = NULL;     
			if( (pFile = fopen(input[2], "a")) == NULL)
			{
	      			printf("Student kann nicht gefunden werden");
	      			sendMsg(fd, "\nStudent wurde nicht gefunden.\n");
			}
			else
			{
				fprintf(pFile, ";%s", input[3]); 
				fclose(pFile);
				printf("Note hinzugefügt");
				sendMsg(fd, "\nDie Note wurde hinzugefügt.\n");
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
	else
	{	sendMsg(fd, "\nStudent existiert nicht.\n");}
}

int findGroup(int fd) {

	printf("find Group\n");
    char* directory;
    directory = recMsg(fd);
	if(strcmp(directory,"0")){
		return;
	}
	DIR *dir;
	struct dirent *dirzeiger;

  	 /* das Verzeichnis öffnen */
   	if((dir=opendir(directory)) == NULL) 
	{ printf("Fehler bei opendir\n"); sendMsg(fd, "\nGruppe nicht gefunden.\n");}
	else
	{
		off_t pos;

		printf("Im Studiengang %s sind folgende Studenten:\n", directory);
   		/* das komplette Verzeichnis auslesen */
		
   		while((dirzeiger=readdir(dir)) != NULL)
      		{	printf("%s\n",(*dirzeiger).d_name); pos=telldir(dir); printf("Zeiger:%ld\n",pos); }

		//TODO: auch 2 falsche einträge ("." und "..")
		// vllt daher: if entry name is a symbolic link, a value is unspecified

   		/* Lesezeiger wieder schließen */
   		if(closedir(dir) == -1)
      			printf("Fehler beim Schließen von %s\n", directory);
		
		sendMsg(fd, "\nGruppe gefunden. In Zukunft hier Ausgabe der enthaltenen Studenten.\n");
	}
}

int createStudent(int fd)
{
	printf("create Student\n");
	char* student;
	student = recMsg(fd);
	printf("StudentRec: %s \n",student);
	if (strcmp(student,"0")==0)
	{	printf("create Student Abortet\n"); } 
	else 
	{	
		char mnrCounter[9]= "\0";
		int mnrCounterInt = 0;

		char writeString[MAXDATASIZE];
		strcpy(writeString,student);
	
		char seps[]   = ";";
		char* token;
		int countSemikolon = 1;
		char* input[MAXDATASIZE];

   		token = strtok(student, seps);	
		while (token != NULL)
		{
			//save current token
			input[countSemikolon] = token;	
			printf("token: %s, attribute: %s\n", token, input[countSemikolon]);
      			// Get next token:
      			token = strtok( NULL, seps );
	
			countSemikolon++;
	   	}
		
		FILE *mnrFile = NULL;
		if((mnrFile = fopen("MNR", "r")) == NULL)
			printf("Fehler1 beim MatrikelCounter");
		else
		{
			fgets(mnrCounter,15,mnrFile);
			mnrCounterInt = atoi(mnrCounter);
			mnrCounterInt++;
			fclose(mnrFile); 
		}
	
		printf("Counter++: %i\n", mnrCounterInt); 
	
		if((mnrFile = fopen("MNR", "w")) == NULL)
			printf("Fehler2 beim MatrikelCounter");
		else
		{
			fprintf(mnrFile, "%i", mnrCounterInt); 
			fclose(mnrFile);
		}
	
		if(chdir(input[4]) == -1) //in den Studiengangsordner wechseln, falls vorhanden
		{ printf("Studiengang %s nicht vorhanden\n", input[4]); 
		sendMsg(fd, "\nDie Gruppe ist nicht vorhanden.\n");}
	   	else //vorhanden -> neuen File erstellen
		{
			printf("Erfolgreich nach %s gewechselt!\n", input[4]);
			FILE *newFile = NULL;
			if((newFile = fopen(mnrCounter, "w")) == NULL){
				perror("fopen");
				sendMsg(fd, "\nFehler bei der Erstellung, bitte erneut versuchen.\n");
			}
			else
	    		{
				printf("Student angelegt\n");
				fprintf(newFile, "%s;%s", mnrCounter,writeString);

				fclose(newFile);

				printf("%s\n", mnrCounter);
				char message[MAXDATASIZE];
				sprintf(message, "\nStudent wurde erfolgreich erstellt\nMNR: %s\n", 						mnrCounter);
			
				sendMsg(fd, message);
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
