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

//0 for fail, 1 for succsess
int handleLogin(int fd){
    printf("HANDLE LOGIN\n------------\n");
    int indicator = validateLogin(fd);
    printf("Indi: %d\n",indicator);
    if(indicator==-1){
        printf("Error in Anmeldung\n");
        sendMsg(fd,"-1");
        return 0;
    } else if (indicator==0){
        printf("Anmeldung Student erfolgreich\n");
        sendMsg(fd,"0");
        return 1;
    } else {
        printf("Anmeldung Admin erfolgreich\n");
        sendMsg(fd,"1");
        return 1;
    }
    return 0;
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

char* getPath(char* mNr){
    printf("GET PATH\n");
    DIR *folder = opendir("./");
	DIR *sfolder;
	struct stat attribut;
	struct dirent *mainfile;
	struct dirent *subfile;
    static char ret[300] = "PATH NOT FOUND";
	while((mainfile=readdir(folder))!=NULL){
		stat(mainfile->d_name,&attribut);
		if( attribut.st_mode & S_IFDIR){
			char* name = mainfile->d_name;
			if(strcmp(name,".")!=0 && strcmp(name,"..")!=0 && strcmp(name,".git")!=0){
				printf("DIR: %s\n",mainfile->d_name);
                sfolder = opendir(mainfile->d_name);
				while((subfile=readdir(sfolder))!=NULL){
					char* subname = subfile->d_name;
					if(strcmp(subname,mNr)==0){
                        printf("%s/%s\n",mainfile->d_name,subfile->d_name);
                        sprintf(ret,"%s",mainfile->d_name);
						printf("GET PATH DONE\n");
    					return ret;
					}
				}
			}
		}
	}
	if(folder==NULL){
		perror("opendir"); //TODO: müsste diese Abfrage nicht vor readdir(folder) sein?
	}
    printf("No such student\n");
    return "-1";
}

//Sends all Student data to the client by taking the mNr
void getSData(int fd)
{
	char* mNr = recMsg(fd);
   	printf("Find mNr: %s\n",mNr);
   	char* path = getPath(mNr);
   	FILE *pFile = NULL;
   	chdir(path);
   	pFile = fopen(mNr,"r");
   	printf("PATH: %s\n",path);
   	if(pFile==NULL)
	{
		sendMsg(fd, "\nStudent nicht vorhanden.\n");
		sendMsg(fd, "0");
   		perror("fopen");
   		return;
   	}

   	char *datenStudent;
   	char seps[] = ";";
   	int countSemikolon = 0;
   	char* token;
   	datenStudent=malloc(500);
	
   	while((fscanf(pFile,"%500s",datenStudent)) != EOF)
   	    printf("%s\n",datenStudent);
   	fclose(pFile);

   	char copyStudent[MAXDATASIZE];
   	strcpy(copyStudent, datenStudent);

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
	sprintf(message,"\nMNR: %s\nPasswort: %s\nVorname: %s\nName: %s\nStudiengang: %s	\nGeburtstag: %s\n",student[1],student[2],student[3],student[4],student[5],student[6]);
	sendMsg(fd, message);

	if(countSemikolon < 7)
	{
		sleep(1);
		sendMsg(fd, "0");
	}
	else
	{
		int i;
		for(i = 7; i <= countSemikolon; i++)
		{
			sprintf(message,"Note: %s\n",student[i]);
			sendMsg(fd, message);
		}
		double avg = average(copyStudent);
		printf("Average: %g\n", avg);
		if(avg != -1)
		{
			char formatAvg[20];
			sprintf(formatAvg, "%g",avg);
			sprintf(message,"Notendurchschnitt: %s\n",formatAvg); 
			sendMsg(fd,message);
		}
		sleep(1);
		sendMsg(fd, "0");
	}			
   	chdir("..");
}

//creates a new group directory
void createGroup(int fd)
{
    char* title = recMsg(fd);
	if(strcmp(title,"0")==0)
	{	 sendMsg(fd, "\nFehler bei der Datenübertragung.\n"); return; }
	if(mkdir(title,S_IRWXU|S_IRGRP|S_IXGRP)!=0){
		perror("mkdir\n");
		sendMsg(fd,"Studiengang konnte nicht erstellt werden"); return;
	}
	printf("Group created\n");
	sendMsg(fd,"Studiengang erfolreich erstellt");
	return;
}

//Checks the Login name and password
int validateLogin(int fd)
{
	printf("Login\n");
    char* login = recMsg(fd); 
    printf("Login Request: %s\n",login);
	if(strcmp(login,"0")==0)
	{	 sendMsg(fd, "-1"); return; }

	char seps[]   = ";";
	char* token;
	int countSemikolon = 0;
	char* input[MAXDATASIZE];

   	token = strtok(login, seps);	
	while (token != NULL)
	{
		countSemikolon++;

		//save current token
		input[countSemikolon] = token;	
		printf("token: %s\n", token);
        // Get next token:
        token = strtok( NULL, seps );
   	}

	char* path = getPath(input[1]);
	if(strcmp(path,"-1") != 0)
	{
		FILE *pFile = NULL;
		if(chdir(path) == -1 || ((pFile = fopen(input[1], "r")) == NULL))
		{
			printf("Problem beim Öffnen des Ordners/Datei\n");
        	perror("chdir");
			perror("fopen");
        	return -1;
   		}
		else
        {
			printf("INPUT1: %s\n",input[1]);
            char *datenStudent;
            datenStudent=malloc(500);

            while((fscanf(pFile,"%500s",datenStudent)) != EOF)
				printf("%s\n",datenStudent);
            fclose(pFile);

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
            if(strcmp(input[2],student[2])==0)
			{
           		printf("PASSWORD SUCCESS!\n");
                if(strcmp(input[1],"admin")==0)
				{
                	printf("ADMIN LOGIN\n");
                    chdir("..");
                    return 1;
                }
         		chdir("..");
                return 0;
            }
            else
            {	printf("PASSWORD WRONG!"); }

            chdir("..");
		}
	}	
	printf("Benutzer existiert nicht.\n");
	return -1;
}

//Returns the groupsbest and average
char* gBestHelp(char* directory)
{
	static char bestReturn[MAXDATASIZE];

	if(strcmp(directory,"0") == 0)
	{	return "0"; }

	DIR *dir;
	struct dirent *dirzeiger;
 	/* das Verzeichnis öffnen */
	if((dir=opendir(directory)) == NULL) 
	{ 	printf("Fehler bei opendir\n"); return "0"; }

	off_t pos;
	printf("Im Studiengang %s sind folgende Studenten:\n", directory);
	
	char* bestsName;
	double bestAvg = 0.0;
	double compAvg = 0.0;
	double avg = 0.0;

	/* das komplette Verzeichnis auslesen */
	while((dirzeiger=readdir(dir)) != NULL)
	{	
		char* name = (*dirzeiger).d_name;
		if(strcmp(name,".")!=0 && strcmp(name,"..")!=0 && strcmp(name,".git")!=0 && strcmp(name,"Admin")!=0)
		{
			printf("%s\n",(*dirzeiger).d_name); pos=telldir(dir); 
			printf("Zeiger:%ld\n",pos); 

			FILE *pFile = NULL; 
			if((chdir(directory) == -1) || 
				((pFile = fopen(name, "r")) == NULL))     
			{
      			printf("Problem beim Öffnen des Ordners/Datei.\n");
				perror("chdir");
				perror("fopen");
				return "0";
			}    
			else
			{
				char *datenStudent;
				datenStudent=malloc(500);

				while((fscanf(pFile,"%500s",datenStudent)) != EOF)
				printf("%s\n",datenStudent);
				fclose(pFile);

				avg = average(datenStudent);
				printf("AVERAGE: %g\n", avg);
				if(avg != -1 && bestAvg == 0.0)
				{ 	bestAvg = avg; bestsName = name; }
				else if(avg != -1)
				{
					compAvg = avg;
					if(bestAvg > compAvg)
					{	bestAvg = compAvg; bestsName = name; }
				}				
				chdir("..");
			}
		}
	}
	/* Lesezeiger wieder schließen */
	if(closedir(dir) == -1)
	{	printf("Fehler beim Schließen von %s\n", directory); }

	if(bestsName == NULL)
	{	return "0"; }
		
	sprintf(bestReturn, "%s;%g", bestsName,bestAvg);
	return bestReturn;
}

//Sends the groups best and average to the client by using gBestHelp
int groupsBest(int fd)
{
	printf("groupsBest\n");
	char* directory;
    	directory = recMsg(fd);
	char* gBest;
	gBest = gBestHelp(directory);

	if(strcmp(gBest,"0") == 0)
	{ 	sendMsg(fd, "Keine Studenten vorhanden oder Fehler aufgetreten.\nBitte überprüfen Sie die Eingaben und versuchen es erneut.\n"); }
	else
	{
		char seps[]   = ";";
		char* token;
		int countSemikolon = 1;
		char* input[MAXDATASIZE];

   		token = strtok(gBest, seps);	
		while (token != NULL)
		{
			//save current token
			input[countSemikolon] = token;	
			printf("token: %s, attribute: %s\n", token, input[countSemikolon]);
      			// Get next token:
      			token = strtok( NULL, seps );

			countSemikolon++;
   		}
		char name_mark[MAXDATASIZE];
		sprintf(name_mark, "Bester Student der Gruppe:\n MNR: %s, Notendurchschnitt: %s\n", input[1], input[2]);
		sendMsg(fd, name_mark);	
	}
}

//Sends the best student's mNr and average to the client
int bestOfAll(int fd)
{
	char bestsName[10];
	char* currentGB;
	double bestAvg = 0.0;
	double compAvg = 0.0;

	char seps[]   = ";";
	char* token;
	int countSemikolon = 1;
	char* input[MAXDATASIZE];

	DIR *folder = opendir("./");
	DIR *sfolder;
	struct stat attribut;
	struct dirent *mainfile;
	struct dirent *subfile;
	while((mainfile=readdir(folder))!=NULL)
	{
		stat(mainfile->d_name,&attribut);
		if( attribut.st_mode & S_IFDIR)
		{
			char* name = mainfile->d_name;
			if(strcmp(name,".")!=0 && strcmp(name,"..")!=0 && strcmp(name,".git")!=0 && strcmp(name,"Admin")!=0)
			{
				countSemikolon = 1;
				currentGB = gBestHelp(name);
				if((strcmp(currentGB, "0")) !=0)
				{
					printf("\nCURRENT GBEST: %s\n\n", currentGB);
   					token = strtok(currentGB, seps);	
					while (token != NULL)
					{
						//save current token
						input[countSemikolon] = token;	
						printf("token: %s\n", token);
      						// Get next token:
      						token = strtok( NULL, seps );

						countSemikolon++;
			   		}
					if((strcmp(input[2], "-1")) != 0 && bestAvg == 0.0)
					{ 	strcpy(bestsName, input[1]); bestAvg = atof(input[2]); }
					else if((strcmp(input[2], "-1")) != 0)
					{
						compAvg = atof(input[2]);
						if(bestAvg > compAvg)
						{	strcpy(bestsName, input[1]); bestAvg = compAvg; }
					}
					else
					{ printf("Average ergab -1.\n\n"); }
				}
			}
		}
	}
	if(bestsName == NULL)
	{	sendMsg(fd, "Keine Noten vorhanden.\n"); return 7; }
	char name_mark[MAXDATASIZE];
	sprintf(name_mark, "Bester Student:\nMNR: %s und Notendurchschnitt: %g\n", bestsName, bestAvg);
	printf("%s",name_mark);
	sendMsg(fd,name_mark);
}

//Adds a mark to students data by taking the mNr
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
	char* path = getPath(input[1]);
	if(strcmp(path,"-1") != 0)
	{
		if(chdir(path) == -1) 
		{
			printf("Studiengang nicht vorhanden\n");
			sendMsg(fd, "\nStudiengang nicht gefunden.\n");
   		}
		else
   		{
			printf("Erfolgreich nach %s gewechselt!\n", input[1]);
	
			FILE *pFile = NULL;     
			if( (pFile = fopen(input[1], "a")) == NULL)
			{
	      			printf("Student kann nicht gefunden werden");
	      			sendMsg(fd, "\nStudent wurde nicht gefunden.\n");
			}
			else
			{
				fprintf(pFile, ";%s", input[2]); 
				fclose(pFile);
				printf("Note hinzugefügt");
				sendMsg(fd, "\nDie Note wurde hinzugefügt.\n");
			}
			chdir("..");
		}
	}
	else
	{	sendMsg(fd, "\nStudent existiert nicht.\n");}
	return;
}
// Sends all groups to the client
int showGroups(int fd) 
{
	printf("Show Groups\n");

	DIR *dir;
	struct dirent *dirzeiger;
 	/* das Verzeichnis öffnen */
	if((dir=opendir("./")) == NULL) 
	{	printf("Fehler bei opendir\n"); sendMsg(fd, "0"); }
	else
	{
		off_t pos;
		char groups[MAXDATASIZE];//TODO: exakte Länge angeben?
		/* das komplette Verzeichnis auslesen */
		while((dirzeiger=readdir(dir)) != NULL)
		{	
			char* name = dirzeiger->d_name;
			if(strchr(name,46) == NULL && strcmp(name,".git")!=0 && strcmp(name,"Admin")!=0 && strcmp(name,"MNR")!=0 && strcmp(name,"servern")!=0 && strcmp(name,"clientn")!=0 && strcmp(name,"makefile")!=0)
			{
				printf("%s\n",(*dirzeiger).d_name); pos=telldir(dir); 
				printf("Zeiger:%ld\n",pos); 
				sprintf(groups, "%s\n",name);
				sendMsg(fd, groups);
			}
		}	
		if(closedir(dir) == -1)
		{	printf("Fehler beim Schließen des Ordners"); }
	}

	sleep(1);
 	sendMsg(fd, "0");
	return;
}

// Sends all mNr of a group to the client
int findGroup(int fd) 
{
	printf("find Group\n");
	int found = 0;
    char* directory;
    directory = recMsg(fd);
	if(strcmp(directory,"0") == 0)
	{	sendMsg(fd, "\nFehler bei der Datenübertragung.\n"); return; }

	DIR *dir;
	struct dirent *dirzeiger;
 	/* das Verzeichnis öffnen */
	if((dir=opendir(directory)) == NULL) 
	{ printf("Fehler bei opendir\n"); sendMsg(fd, "\nStudiengang nicht gefunden.\n");}
	else
	{
		char students[MAXDATASIZE];
		//sprintf(students, "\nStudiengang %s gefunden. Folgende Studenten enthalten: \n", directory);
		off_t pos;
		printf("Im Studiengang %s sind folgende Studenten:\n", directory);
		/* das komplette Verzeichnis auslesen */
		
		while((dirzeiger=readdir(dir)) != NULL)
		{	
			char* name = dirzeiger->d_name;
			if(strcmp(name,".")!=0 && strcmp(name,"..")!=0 && strcmp(name,".git")!=0 && strcmp(name,"Admin")!=0)
			{
				found = 1;
				printf("%s\n",(*dirzeiger).d_name); pos=telldir(dir); 
				printf("Zeiger:%ld\n",pos); 
				sprintf(students, "%s\n",name);
				sendMsg(fd, students);
			}
		}
		
		/* Lesezeiger wieder schließen */
		if(closedir(dir) == -1)
		{	printf("Fehler beim Schließen von %s\n", directory); }

		if(found == 0)
		{	sendMsg(fd, "Keine Studenten enthalten.\n"); }
	}
	sleep(1);
 	sendMsg(fd, "0");
	return;
}

//creates a new Student in a given group directory
int createStudent(int fd)
{
	printf("create Student\n");
	char* student;
	student = recMsg(fd);
	printf("StudentRec: %s \n",student);
	if (strcmp(student,"0")==0){
        sendMsg(fd, "\nFehler bei der Datenübertragung.\n");
        return;
    } 
	
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
        mnrCounter[strlen(mnrCounter)-1]='\0';
		mnrCounterInt = atoi(mnrCounter);
		mnrCounterInt++;
		fclose(mnrFile); 
	}

	printf("Counter++: %i\n", mnrCounterInt); 
	
	if((mnrFile = fopen("MNR", "w")) == NULL)
	{	sendMsg(fd, "\nFehler in der MNR-Datei.\n"); return;}
	else
	{
		fprintf(mnrFile, "%i", mnrCounterInt); 
		fclose(mnrFile);
	}
	
	if(chdir(input[4]) == -1) //in den Studiengangsordner wechseln, falls vorhanden
	{ 
		printf("Studiengang %s nicht vorhanden\n", input[4]); 
		sendMsg(fd, "\nDer Studiengang ist nicht vorhanden.\n");
	}
	else //vorhanden -> neuen File erstellen
	{
		printf("Erfolgreich nach %s gewechselt!\n", input[4]);
		FILE *newFile = NULL;
		if((newFile = fopen(mnrCounter, "w")) == NULL)
		{
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
			sprintf(message, "\nStudent wurde erfolgreich erstellt\nMNR: %s\n",mnrCounter);
			
			sendMsg(fd, message);
		}			
		chdir("..");
	}
	return 1;
}

//Edits a Student
int editStudent(int fd)
{
	printf("Edit Student\n");
	char* mNr;
	int i;
	mNr = recMsg(fd);
	char message[MAXDATASIZE];
	char mNrCopy[10];
	strcpy(mNrCopy,mNr);
	printf("StudentRec: %s \n",mNr);
	if (strcmp(mNr,"0")==0)
	{
        	sendMsg(fd, "0");
        	return;
	} 
	
	char* path = getPath(mNr);
	if(strcmp(path,"-1") != 0)
	{
		if(chdir(path) == -1) //in den Studiengangsordner wechseln, falls vorhanden
		{	sendMsg(fd, "0"); return; }
		
		//vorhanden -> File bearbeiten
		printf("Erfolgreich nach %s gewechselt!\n", path);
		char *datenStudent;
   		datenStudent=malloc(500);
	
		FILE *editFile = NULL;
		if((editFile = fopen(mNr, "r")) == NULL)
		{	perror("fopen"); sendMsg(fd, "0"); return; }

   		while((fscanf(editFile,"%500s",datenStudent)) != EOF)
   	    		printf("%s\n",datenStudent);
   		fclose(editFile);
		
		sendMsg(fd,"1"); // Student vorhanden -> Client			
		char seps[]   = ";";
		char* token;
		int countSemikolon = 0;
		char* input[MAXDATASIZE];

		token = strtok(datenStudent, seps);	
		while (token != NULL)
		{
			countSemikolon++;
			//save current token
			input[countSemikolon] = token;	
			printf("token: %s, attribute: %s\n", token, input[countSemikolon]);
			// Get next token:
			token = strtok( NULL, seps );
   		}
		char* choice;
		choice = recMsg(fd);
		if(strcmp(choice,"1") == 0) //Passwort ändern
		{
			choice = recMsg(fd);
			input[2] = choice;
		}
		else
		{
			if(countSemikolon < 7)
			{
				sleep(1);
				sendMsg(fd, "Keine Noten vorhanden");
			}
			else
			{
				for(i = 7; i <= countSemikolon; i++)
				{
					sprintf(message,"Note %i: %s\n",countSemikolon-6,input[i]);
					sendMsg(fd, message);
				}
				sleep(1);
				sendMsg(fd, "0");
				choice = recMsg(fd);
				char* mark = recMsg(fd);
				input[atoi(choice)] = mark;
			}
		}	
		if((editFile = fopen(mNrCopy, "w")) == NULL)
		{	perror("fopen"); sendMsg(fd, "0"); return; }
		fprintf(editFile, "%s", input[1]);
		fclose(editFile);
		if((editFile = fopen(mNrCopy, "a")) == NULL)
		{	perror("fopen"); sendMsg(fd, "0"); return; }

		for(i = 2; i <= countSemikolon; i++)
			fprintf(editFile, ";%s", input[i]);
		fclose(editFile);
			
		sprintf(message, "\nStudent erfolgreich editiert\n");			
		sendMsg(fd, message);		
		chdir("..");
	}
	else
	{	sendMsg(fd, "0"); }
	return;
}

void handleMenu(int fd)
{
    char* auswahl;
    printf("Handle Menu\n");
    while(1)
	{
        auswahl = recMsg(fd);
        if(strcmp(auswahl,"1")==0){
            createStudent(fd);
        }
        if(strcmp(auswahl,"2")==0){           
		getSData(fd);
        }
        if(strcmp(auswahl,"3")==0){
            editStudent(fd);
        }
        if(strcmp(auswahl,"4")==0){
            createGroup(fd);
        }
        if(strcmp(auswahl,"5")==0){
            findGroup(fd);
        }
		if(strcmp(auswahl,"6")==0){
            showGroups(fd);
		}
        if(strcmp(auswahl,"7")==0){
            addMark(fd);
		}
        if(strcmp(auswahl,"8")==0){
            groupsBest(fd);
		}
        if(strcmp(auswahl,"9")==0){
            bestOfAll(fd);
        }
        if(strcmp(auswahl,"10")==0){
            return;
        }
        if(strcmp(auswahl,"11")==0){
            getSData(fd);
        }//TODO: return werte aller funktionen prüfen ob richtige zahl
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
    while(1){
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
        
        int valid = handleLogin(new_fd);
        if (valid==0){
            printf("UNVALID\n");
            while(valid==0){
                valid = handleLogin(new_fd);
                printf("VALID: %d\n",valid);
            }
        } 
        if (valid==1){
            handleMenu(new_fd);
        }
        
    }

    exit(0);
}
