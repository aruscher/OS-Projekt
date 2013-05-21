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


/* the port users will be connecting to */
#define MYPORT 3490

/* how many pending connections queue will hold */
#define BACKLOG 10

// max number of bytes we can get at once
#define MAXDATASIZE 300


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
    mkdir(title);
}

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
	{
		printf("Studiengang %s nicht vorhanden\n", input[4]);
      		//return EXIT_FAILURE;
   	}
   	else //vorhanden -> neuen File erstellen
	{
		printf("Erfolgreich nach %s gewechselt!\n", input[4]);
		FILE *newFile = NULL;
		newFile = fopen(input[3], "w");
    		if(newFile)
    		{
				printf("Student angelegt\n");
				fprintf(newFile, "%s", writeString); 
		}
		fclose(newFile);
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
		if( (pFile = fopen(input[2], "r")) == NULL)
		{
      			printf("Student kann nicht gefunden werden");
      			//return EXIT_FAILURE;
		}
		/*fgets(student, 20, pFile);
		printf("%s",student);*/
		
		int nRet;
   		size_t *t = malloc(0);
   		char **gptr = malloc(sizeof(char*));
   		*gptr = NULL;

		while( (nRet=getline(gptr, t, pFile)) > 0)
      		fputs(*gptr,stdout);
		//TODO: get it in a variable and send it to client
	}
	return 2;
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
		//printf("2");            
		findStudent(fd);
        }
        if(strcmp(auswahl,"3")==0){
            createGroup(fd);
            printf("3");
        }
        if(strcmp(auswahl,"4")==0){
            printf("4");
        }
        if(strcmp(auswahl,"5")==0){
            printf("5");
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
