/* search students

quick and dirty



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

#define ZEILENLAENGE 80

int main(void) {
	char pfadname[200];
	char student[20];

   printf("In welcher Gruppe befindet sich der gesuchte Student?");
   scanf("%199s",pfadname);

   /* In Gruppen-Verzeichnis wechseln */
   if(chdir(pfadname) == -1) {
      printf("Gruppe nicht vorhanden\n");
      return EXIT_FAILURE;
   }
   else
	{
		FILE *pFile = NULL;
		char puffer[ZEILENLAENGE];

      		printf("Erfolgreich nach %s gewechselt!\n", pfadname);

		printf("Geben Sie die Matrikelnr des gesuchten Studenten an: ");
   		scanf("%s", student);
		if( (pFile = fopen(student, "r")) == NULL)
		{
      		printf("Matrikelnr kann nicht gefunden werden");
      		return EXIT_FAILURE;
		}
   		while(fgets(puffer, ZEILENLAENGE, pFile))
      		fputs(puffer, stdout);
		printf("\nYEAHYAA\n!");

		//fclose(pFile);

   		return EXIT_SUCCESS;
	}
   return EXIT_SUCCESS;
}
