#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STUDENT 11

int main()
{
	system("clear");
	char mnr[10];
	char passwort[21];
	char vorname[21];
	char nachname[21];
	char bday[13];
	char fach1[21];
	char note1[4];
	char fach2[21];
	char note2[4];
	char fach3[21];
	char note3[4];
	

	char *datenStudent;
	//weißt den Datenspeicherplatz zu => anpassen an das zugelassene Maximum
	datenStudent=malloc(130);
	
	int laenge;
    	int i;
	char datei[10]="123456789\0";
	char *geradeGelsenesChar;
	geradeGelsenesChar=malloc(2);
	int naechstesStudentenAttribut=0;

	//r -> lesender Zugriff
	FILE *pFile = NULL;
	pFile = fopen(datei, "r");

	//Lade Studentendaten in datenStudenten mit anschließender Ausgabe
	//while((fscanf(pFile,"%500s",datenStudent)) != EOF)
	//printf("%s\n",datenStudent);
	//Für gleiche Testwerte Einlese aus Datei als fertiger String:
	datenStudent="123456789;JohnWayne;John;Wayne;12.12.1212;Skills;1.0;Versagen;6.0;Linux-Kentnisse;0.7\0";

	//Länge der StudentenDatenberechnen und ausgeben
	laenge = strlen(datenStudent);
	//printf("Der Student hat %d Zeichen\n", laenge);
	mnr[0]='\0';

	for(i=0; i<laenge; i++)
	{
		//Vergleich auf ASCII-Basis => 59 ist ;
		if(datenStudent[i]==59)
		{
			naechstesStudentenAttribut++;
		}
		else
		{
			*geradeGelsenesChar=datenStudent[i];

			// Je nachdem an welchem Teil der CSV-Datei gerade gelesen wird, wird es an eine andere Variable angehängt
			switch(naechstesStudentenAttribut) 
			{
				case 0:
					// Hier wird mnr ermittelt
					strcat(mnr, geradeGelsenesChar);
					passwort[0]='\0';
				break;

				case 1:
					// Hier wird passwort ermittelt
					strcat(passwort, geradeGelsenesChar);
					vorname[0]='\0';
				break;

				case 2:
					// Hier wird vorname ermittelt
					strcat(vorname, geradeGelsenesChar);
					nachname[0]='\0';
				break;

				case 3:
					// Hier wird nachname ermittelt
					strcat(nachname, geradeGelsenesChar);
					bday[0]='\0';
				break;

				case 4:
					// Hier wird bday ermittelt
					strcat(bday, geradeGelsenesChar);
					fach1[0]='\0';
				break;

				case 5:
					// Hier wird fach1 ermittelt
					strcat(fach1, geradeGelsenesChar);
					note1[0]='\0';
				break;

				case 6:
					// Hier wird note1 ermittelt
					strcat(note1, geradeGelsenesChar);
					fach2[0]='\0';
				break;

				case 7:
					// Hier wird fach2 ermittelt
					strcat(fach2, geradeGelsenesChar);
					note2[0]='\0';
				break;

				case 8:
					// Hier wird note2 ermittelt
					strcat(note2, geradeGelsenesChar);
					fach3[0]='\0';
				break;

				case 9:
					// Hier wird fach3 ermittelt
					strcat(fach3, geradeGelsenesChar);
					note3[0]='\0';
				break;

				case 10:
					// Hier wird note3 ermittelt SCHLUSS
					strcat(note3, geradeGelsenesChar);
				break;
			}
		}
	}
	fclose(pFile);
	printf("Das ist MNR: %s. \n", mnr);
	printf("Das ist Passwort: %s. \n", passwort);
	printf("Das ist Vorname: %s. \n", vorname);
	printf("Das ist Nachname: %s. \n", nachname);
	printf("Das ist BDay: %s. \n", bday);
	printf("Das ist Fach1: %s. \n", fach1);
	printf("Das ist note1: %s. \n", note1);
	printf("Das ist fach2: %s. \n", fach2);
	printf("Das ist note2: %s. \n", note2);
	printf("Das ist fach3: %s. \n", fach3);
	printf("Das ist note3: %s. \n", note3);
}
