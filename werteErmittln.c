#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STUDENT 11

int main()
{
	system("clear");
	char vorname[21];
	char nachname[21];
	char mnr[10];
	char studiengang[21];
	char bday[13];

	//weißt den Datenspeicherplatz zu => anpassen an das zugelassene Maximum
	char *datenStudent;
	datenStudent=malloc(500);

	char *notenStudent;
	notenStudent=malloc(500);

	int laenge;
    	int i;
	char datei[10]="123456789\0";
	char *geradeGelsenesChar;
	geradeGelsenesChar=malloc(2);
	int naechstesStudentenAttribut=0;
	int datenAnzahl=1;
	

	//r -> lesender Zugriff
	FILE *pFile = NULL;
	pFile = fopen(datei, "r");

	//Lade Studentendaten in datenStudenten mit anschließender Ausgabe
	//while((fscanf(pFile,"%500s",datenStudent)) != EOF)
	//printf("%s\n",datenStudent);
	//Für gleiche Testwerte Einlese aus Datei als fertiger String:
	datenStudent="John;Wayne;123456789;Informatik;12.12.1212;2.0;1.3;5.0;2.0;1.0;5.0;1.0;1.0\0";

	//Länge der StudentenDatenberechnen und ausgeben
	laenge = strlen(datenStudent);
	//printf("Der Student hat %d Zeichen\n", laenge);

	for(i=0; i<laenge; i++)
	{
		//Vergleich auf ASCII-Basis => 59 ist ;
		if(datenStudent[i]==59)
		{
			datenAnzahl++;
		}
	}

	printf("Student besteht aus  %i Datenteilen\n", datenAnzahl);
	vorname[0]='\0';

	for(i=0; i<laenge; i++)
	{
		//Vergleich auf ASCII-Basis Simikolon => 59 ist ;
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
					// Hier wird Vorname ermittelt
					strcat(vorname, geradeGelsenesChar);
					nachname[0]='\0';
				break;

				case 1:
					// Hier wird passwort ermittelt
					strcat(nachname, geradeGelsenesChar);
					mnr[0]='\0';
				break;

				case 2:
					// Hier wird vorname ermittelt
					strcat(mnr, geradeGelsenesChar);
					studiengang[0]='\0';
				break;

				case 3:
					// Hier wird nachname ermittelt
					strcat(studiengang, geradeGelsenesChar);
					bday[0]='\0';
				break;

				case 4:
					// Hier wird bday ermittelt
					strcat(bday, geradeGelsenesChar);
					notenStudent[0]='\0';
				break;

				default:
					// Hier werden alle Noten des Studenten in notenStudent festgehalten
					strcat(notenStudent, geradeGelsenesChar);
				break;
			}
		}
	}
	fclose(pFile);
	printf("Das ist MNR: %s. \n", mnr);
	printf("Das ist Vorname: %s. \n", vorname);
	printf("Das ist Nachname: %s. \n", nachname);
	printf("Das ist BDay: %s. \n", bday);
	printf("Das ist Studiengang: %s. \n", studiengang);
	printf("Der Student hat %i Noten. \n", datenAnzahl-5);

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

	printf("Noten des Studenten: ");
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
	//auf 2 Stellen genau
	//notenDurchschnitt = (int)(notenDurchschnitt*100)/100.0;
	//auf 1 Stellen genau
	notenDurchschnitt = (int)(notenDurchschnitt*10)/10.0;
	printf("Durchschnitte %f\n",notenDurchschnitt);

}
