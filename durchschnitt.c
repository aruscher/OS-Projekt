#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STUDENT 11

int main()
{
	//weißt den Datenspeicherplatz zu => anpassen an das zugelassene Maximum
	char *datenStudent;
	datenStudent=malloc(500);

	datenStudent="John;Wayne;123456789;Informatik;12.12.1212;2.0;1.3;5.0;2.0;1.0;5.0;1.0;1.0\0";

	char *notenStudent;
	notenStudent=malloc(500);

	int laenge;
    	int i;
	char datei[10]="123456789\0";
	char *geradeGelsenesChar;
	geradeGelsenesChar=malloc(2);
	int naechstesStudentenAttribut=0;
	int datenAnzahl=1;

	laenge = strlen(datenStudent);

	for(i=0; i<laenge; i++)
	{
		//Vergleich auf ASCII-Basis Simikolon => 59 ist ;
		if(datenStudent[i]==59)
		{
			datenAnzahl++;
		}
	}
	for(i=0; i<laenge; i++)
	{
		//Vergleich auf ASCII-Basis Simikolon => 59 ist ;
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
