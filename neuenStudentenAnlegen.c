#include <stdio.h>
#include <stdlib.h>
//Globale Varialbe STUDENT mit Wert 11
#define STUDENT 11

int main()
{
	//hier können die 11 Eigenschaften festgelegt werden, die der Student besitzt
	char mnr[10]="123456789\0";
	char passwort[21]="JohnWayne\0";
	char vorname[21]="John\0";
	char nachname[21]="Wayne\0";
	char bday[13]="12.12.1212\0";
	char fach1[21]="Skills\0";
	char note1[4]="1.0\0";
	char fach2[21]="Versagen";
	char note2[4]="6.0\0";
	char fach3[21]="Linux-Kentnisse\0";
	char note3[4]="0.7\0";

// the value being pointed to can't be changed but the pointer can be
	const char *BeispielStudent[STUDENT] = {mnr, passwort, vorname, nachname, bday, fach1, note1, fach2, note2, fach3, note3};

//es wird die Datei erstellt, in der die Daten des Studenten liegen: Name: Matrikelnummer -> könnte man als Eigenschaft in der Datei weglassen (bei Bedarf)
	FILE *pFile = fopen(mnr, "a");

	if(pFile)
	{
	    	int i;
		//für alle 11 Eigenschaften, die den Studen ausmachen
		for(i = 0; i < (STUDENT-1); i++)
		{
			//schreibe die Eigenschaften in die Datei
			fprintf(pFile, "%s", BeispielStudent[i]);
			fputc(';', pFile);
		}
		fprintf(pFile, "%s", BeispielStudent[10]);
		//Zum Schluss noch einen Zeilenumbruch bei Bedarf
		//fprintf(pFile,"\n");
		fclose(pFile);
	}
}
