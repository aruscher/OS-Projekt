/* add a group of students

quick and dirty

Anlegen eines Gruppenordners mit Studentendatei

Formatierung is noch müll :)
es fehlen noch Validierungen, angemessene Länge der String, etc
*/

#ifdef __linux__
   #include <sys/types.h>
   #include <sys/stat.h>
   #include <unistd.h>
   #define MODUS ,0711)
#elif _WIN32__ || _MS_DOS_
    #include <dir.h>
    #define MODUS )
#else
    #include <direct.h>
    #define MODUS )
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int main(void) {
   char pfadname[200];
	char student[200];
	char choice[5];

   printf("Wie soll die neue Gruppe heissen : ");
   scanf("%199s",pfadname);
   if(mkdir(pfadname MODUS == -1)
      printf("Erstellen der Gruppe nicht möglich\n");
   else {
      printf("Neue Gruppe namens %s erstellt\n", pfadname);
      printf(" --> (%s)\n", strerror(errno));
   }

   /* Jetzt wollen wir in das neue Verzeichnis wechseln. */
   if(chdir(pfadname) == -1) {
      printf("Konnte nicht in das Verzeichnis wechseln\n");
      return EXIT_FAILURE;
   }
   else{
      	printf("Erfolgreich nach %s gewechselt!\n", pfadname);
   	/* Studenten erstellen*/
	printf("Möchten Sie Studenten in der Gruppe erstellen? (j/n): ");
   	scanf("%199s", choice);
	//Hier wird noch nicht auf die Antwort Rücksicht genommen
/*
void String_Vergleich(char s1[], char s2[]) {
   int ret = strcmp (s1, s2);
   if(ret == 0)
      printf("%s == %s\n", s1, s2);
   else
      printf("%s %c %s\n",s1,( (ret < 0) ?'<' :'>'), s2);
*/

	if(choice)
	{
		printf("Matrikelnr des Studenten: ");
   		scanf("%199s", student);
		//Hier fehlt die Validierung if(student..?)
   		//fopen(student, "w");
		FILE *pFile = NULL;
		pFile = fopen(student, "a");
    		if(pFile)
    		{
				fprintf(pFile, "%s", student);
           		fputc(',', pFile);
				
				printf("Nachname des Studenten: ");
   				scanf("%199s", student);
				//Validierung !
       	    	fprintf(pFile, "%s", student);
            	fputc(',', pFile);
				
				printf("Vorname des Studenten: ");
   				scanf("%199s", student);
				//Validierung !
       	    	fprintf(pFile, "%s", student);
            	fputc(',', pFile);

        		fclose(pFile);
    		}
	}
	else {printf("Leider fehlgeschlagen");}}
   return EXIT_SUCCESS;
}
