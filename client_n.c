#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <ctype.h>

// the port client will be connecting to
#define MYPORT 3490

// max number of bytes we can get at once
#define MAXDATASIZE 300

//wrapper for sending messages
void sendMsg(int socket, char message[MAXDATASIZE]) {
	send(socket, message, strlen(message), 0);
}

//wrapper for recievinf messages
char* recMsg(int socket) {
	static char rec[MAXDATASIZE];
	int msg;
	msg = recv(socket, rec, MAXDATASIZE-1, 0);
	rec[msg]='\0';
	return rec;
}

//returns mNr if Student, 1 Admin, -1 for invalid
char* manageLogin(int socket) {
	char login[11];
	char password[11];
	char message[MAXDATASIZE];
	char* rec;
	printf("Anmeldung auf Notenserver\n");
	printf("#########################\n");
	printf("Benutzername(Mnr): >");
	scanf("%s", login);
	printf("\nPasswort: >");
	scanf("%s", password);
	sprintf(message, "%s;%s", login, password);
	sendMsg(socket, message);
	rec=recMsg(socket);
	printf("Rec after: %s\n", rec);
	if (strcmp(rec, "0")==0) {
		strcpy(rec, login);
	}
	return rec;
}

//Because of our CSV structure we allow no ";" in input strings
//returns 1 if valid, 0 unvalid
int validStudentInput(char* input) {
	if (strchr(input, ';')) {
		return 0;
	}
	return 1;
}

// The mark should look like x.y
// 1 is valid,0 unvalid
int validMarkInput(char* input) {
	if (isdigit(input[0])==0) {
		printf("Fehler in der Vorkommastelle\n");
		return 0;
	}
	if (isdigit(input[2])==0) {
		printf("Fehler in der Nachkommastelle\n");
		return 0;
	}
	if (input[1]!='.') {
		printf("Bitte Note in der Form x.y angeben\n");
		return 0;
	}
	return 1;
}

// The admin's menu
void showMainMenu() {
	printf("Menü\n----\n");
	printf("1)Student anlegen\n");
	printf("2)Student finden\n");
	printf("3)Student editieren\n");
	printf("4)Studiengang anlegen\n");
	printf("5)Studiengang Mitglieder anzeigen\n");
	printf("6)Studiengänge finden\n");
	printf("7)Note hinzufügen\n");
	printf("8)Studiengangsbesten anzeigen\n");
	printf("9)Gesamtbesten anzeigen\n");
	printf("10)Beenden\n");
	printf("---------------\n");
	printf("Bitte Nummer eingeben:\n >");
}

// Provides client-server-interaction to create a new student
void createStudent(int socket) {
	printf("Student anlegen\n");
	printf("---------------\n");
	printf("0 für Beenden\n");
	char passwort[11];
	char vorname[21];
	char nachname[21];
	char bday[11];
	char studiengang[21];
	char auswahl[2];

	// aks for all needed inputs
	printf("Vorname (max.20): >");
	scanf("%s", vorname);
	while (!validStudentInput(vorname)) {
		printf("Ungültige Eingabe mit ;\n");
		printf("Vorname (max.20): >");
		scanf("%s", vorname);
	}
	if (strcmp(vorname, "0")==0) {
		sendMsg(socket, "0");
		return;
	}

	printf("Nachname (max.20): >");
	scanf("%s", nachname);
	while (!validStudentInput(nachname)) {
		printf("Ungültige Eingabe mit ;\n");
		printf("Nachname (max.20): >");
		scanf("%s", nachname);
	}
	if (strcmp(nachname, "0")==0) {
		sendMsg(socket, "0");
		return;
	}

	printf("Geburtstag (dd.mm.yyyy): >");
	scanf("%s", bday);
	while (!validStudentInput(bday)) {
		printf("Ungültige Eingabe mit ;\n");
		printf("Geburtstag (dd.mm.yyyy): >");
		scanf("%s", bday);
	}
	if (strcmp(bday, "0")==0) {
		sendMsg(socket, "0");
		return;
	}

	printf("Studiengang (muss vorhanden sein, max.20): >");
	scanf("%s", studiengang);
	while (!validStudentInput(studiengang)) {
		printf("Ungültige Eingabe mit ;\n");
		printf("Studiengang (max.20): >");
		scanf("%s", studiengang);
	}
	if (strcmp(studiengang, "0")==0) {
		sendMsg(socket, "0");
		return;
	}

	printf("Passwort (max.10): >");
	scanf("%s", passwort);
	while (!validStudentInput(passwort)) {
		printf("Ungültige Eingabe mit ;\n");
		printf("Passwort (max.10): >");
		scanf("%s", passwort);
	}
	if (strcmp(passwort, "0")==0) {
		sendMsg(socket, "0");
		return;
	}

	// Summary to be confirmed
	printf("#############################\n");
	printf("Zusammenfassung:\n");
	printf("%s %s, Studiengang: %s, Geb.: %s, Passwort: %s\n", vorname,
			nachname, studiengang, bday, passwort);
	printf("Bestätigen(1) Abbruch(0)\n");
	printf(">");
	scanf("%s", auswahl);
	if (strcmp(auswahl, "0")==0) // Cancel
	{
		return;
	}

	// message to the server
	char message[MAXDATASIZE];
	sprintf(message, "%s;%s;%s;%s;%s", passwort, vorname, nachname,
			studiengang, bday);
	sendMsg(socket, message);
	// print answer from server
	char* rec;
	rec = recMsg(socket);
	printf("%s\n", rec);
	printf("---------------\n");
	printf("1 für Weiter\n");
	printf(">");
	char go[30];
	scanf("%s", go);
	while (strcmp(go, "1")!=0) {
		printf("Ungültige Eingabe\n");
		printf("1 für Weiter\n");
		printf(">");
		scanf("%s", go);
	}
	system("clear");

}

// Provides client-server-interaction to find and print a student's data
void findStudent(int socket) {
	printf("Student finden\n");
	printf("---------------\n");
	printf("0 für Beenden\n");
	char mNr[11];
	char auswahl[2];

	// Ask for the mNr to search for the student
	printf("Matrikelnummer (max.9): >");
	scanf("%s", mNr);
	if (strcmp(mNr, "0")==0) {
		sendMsg(socket, "0");
		return;
	}

	// Summary to be confirmed
	printf("#############################\n");
	printf("Sie suchen den folgenden Studenten:\n");
	printf("Mnr: %s\n", mNr);
	printf("Bestätigen(1) Abbruch(0)\n");
	printf(">");
	scanf("%s", auswahl);
	if (strcmp(auswahl, "0")==0) {
		return;
	}

	sendMsg(socket, mNr);
	// Get the student's data from server
	char* rec;
	while (strcmp(rec = recMsg(socket), "0") != 0) // last Message is "0"
	{
		printf("%s", rec);
	}
	printf("---------------\n");
	printf("1 für Weiter\n");
	printf(">");
	char go[30];
	scanf("%s", go);
	while (strcmp(go, "1")!=0) {
		printf("Ungültige Eingabe\n");
		printf("1 für Weiter\n");
		printf(">");
		scanf("%s", go);
	}
	system("clear");
	return;
}

// Provides client-server-interaction to add a new student's mark
void addMark(int socket) {
	printf("Note hinzufügen\n");
	printf("---------------\n");
	printf("0 für Beenden\n");
	char mNr[11];
	char note[4];
	char auswahl[2];

	// Ask for mNr and new mark
	printf("Matrikelnummer (max.9): >");
	scanf("%s", mNr);
	if (strcmp(mNr, "0")==0) {
		sendMsg(socket, "0");
		system("clear");
		return;
	}

	printf("Note (x.y): >");
	scanf("%3s/0", note);
	while (!validMarkInput(note)) {
		printf("Ungülite Eingabe\n");
		printf("Note (x.y): >");
		scanf("%3s/0", note);
	}
	if (strcmp(mNr, "0")==0) {
		sendMsg(socket, "0");
		system("clear");
		return;
	}

	//Summary to be confirmed
	printf("#############################\n");
	printf("Für Student mit Mnr: %s Note %s hinzufügen?\n", mNr, note);
	printf("Bestätigen(1) Abbruch(0)\n");
	printf(">");
	scanf("%s", auswahl);
	if (strcmp(auswahl, "0")==0) {
		return;
	}

	// Send data to server
	char message[MAXDATASIZE];
	sprintf(message, "%s;%s", mNr, note);
	sendMsg(socket, message);
	// Print message from server
	char* rec;
	rec = recMsg(socket);
	printf("---------------\n");
	printf("1 für Weiter\n");
	printf(">");
	char go[30];
	scanf("%s", go);
	while (strcmp(go, "1")!=0) {
		printf("Ungültige Eingabe\n");
		printf("1 für Weiter\n");
		printf(">");
		scanf("%s", go);
		system("clear");
		printf("%s\n", rec);
	}
}
// Provides client-server-interaction to edit a existing student
void editStudent(int socket) {
	printf("Student editieren\n");
	printf("---------------\n");
	printf("0 für Beenden\n");
	char mNr[10];
	char auswahl[2];

	// Ask for mNr to let the server check the existence
	printf("Matrikelnummer des zu editierenden Studenten (max.9): >");
	scanf("%s", mNr);
	while (!validStudentInput(mNr)) {
		printf("Ungültige Eingabe mit ;\n");
		printf("Matrikelnummer (max.9): >");
		scanf("%s", mNr);
	}
	sendMsg(socket, mNr);

	// get answer from server wether student exists
	char* rec;
	rec = recMsg(socket);
	if (strcmp(rec, "0") == 0) {
		printf("\nStudent nicht vorhanden\n");
		return; //Cancel if student does not exist
	}

	// Ask if password or mark should be edited
	printf("Passwort ändern(1) Note ändern (2) Abbruch(0)\n");
	printf(">");
	scanf("%s", auswahl);
	// Check if valid choice
	while ((strcmp(auswahl, "0") != 0) && (strcmp(auswahl, "1") != 0)
			&& (strcmp(auswahl, "2") != 0)) {
		printf("Ungültige Eingabe\n");
		printf("Passwort ändern(1) Note ändern (2) Abbruch(0)\n");
		printf(">");
		scanf("%s", auswahl);
	}
	if (strcmp(auswahl, "0")==0) {
		return;
	} //cancel
	sendMsg(socket, auswahl);

	// Password has to be edited
	if (strcmp(auswahl, "1") == 0) {
		char passwort[11];
		printf("Passwort (max.10): >");
		scanf("%s", passwort);
		while (!validStudentInput(passwort)) {
			printf("Ungültige Eingabe mit ;\n");
			printf("Passwort (max.10): >");
			scanf("%s", passwort);
		}
		if (strcmp(passwort, "0")==0) {
			sendMsg(socket, "0");
			system("clear");
			return;
		}
		printf("#############################\n");
		printf("Für Student mit Mnr: %s Passwort zu %s ändern?\n", mNr,
				passwort);
		printf("Bestätigen(1) Abbruch(0)\n");
		printf(">");
		scanf("%s", auswahl);
		if (strcmp(auswahl, "0")==0) {
			system("clear");
			return;
		}
		sendMsg(socket, passwort); //send new password
		rec = recMsg(socket);
		if (strcmp(rec, "0") == 0) {
			printf("Fehler beim Editieren. Bitte versuchen Sie es erneut.\n");
			printf("---------------\n");
			printf("1 für Weiter\n");
			printf(">");
			char go[30];
			scanf("%s", go);
			while (strcmp(go, "1")!=0) {
				printf("Ungültige Eingabe\n");
				printf("1 für Weiter\n");
				printf(">");
				scanf("%s", go);
			}
			system("clear");
			return;
		}
		printf("%s\n", rec);
		printf("---------------\n");
		printf("1 für Weiter\n");
		printf(">");
		char go[30];
		scanf("%s", go);
		while (strcmp(go, "1")!=0) {
			printf("Ungültige Eingabe\n");
			printf("1 für Weiter\n");
			printf(">");
			scanf("%s", go);
		}
		system("clear");

		return;
	} else // Note ändern
	{
		rec = recMsg(socket);
		if (strcmp(rec, "0") == 0) {
			printf("Keine Noten vorhanden\n");
			return;
		}
		printf("%s", rec); //Noten vorhanden
		int counter = 1; //Anzahl der Noten
		while (strcmp(rec = recMsg(socket), "0") != 0) {
			printf("%s", rec);
			counter++;
		}
		printf("---------------\n");
		printf("Die wievielte Note wollen Sie ändern?\n");
		printf(">");
		scanf("%s", auswahl);
		if (strcmp(auswahl, "0")==0) {
			return;
		}
		sendMsg(socket, auswahl);
		char mark[4];
		printf("Neue Note (x.y): >");
		scanf("%s/0", mark);
		while (!validMarkInput(mark)) {
			printf("Ungülite Eingabe\n");
			printf("Note (x.y): >");
			scanf("%s/0", mark);
		}
		if (strcmp(mark, "0")==0) {
			sendMsg(socket, "0");
			system("clear");
			return;
		}
		printf("#############################\n");
		printf("Für Student mit Mnr: %s Note %s zu %s ändern?\n", mNr, auswahl,
				mark);
		printf("Bestätigen(1) Abbruch(0)\n");
		printf(">");
		scanf("%s", auswahl);
		if (strcmp(auswahl, "0")==0) {
			system("clear");
			return;
		}
		sendMsg(socket, mark); //neue Note senden
		rec = recMsg(socket);
		if (strcmp(rec, "0") == 0) {
			printf("Fehler beim Editieren. Bitte versuche  Sie es erneut.\n");
			printf("---------------\n");
			printf("1 für Weiter\n");
			printf(">");
			char go[30];
			scanf("%s", go);
			while (strcmp(go, "1")!=0) {
				printf("Ungültige Eingabe\n");
				printf("1 für Weiter\n");
				printf(">");
				scanf("%s", go);
			}
			system("clear");

			return;
		}
		printf("%s\n", rec);
		return;
	}
	printf("---------------\n");
	printf("1 für Weiter\n");
	printf(">");
	char go[30];
	scanf("%s", go);
	while (strcmp(go, "1")!=0) {
		printf("Ungültige Eingabe\n");
		printf("1 für Weiter\n");
		printf(">");
		scanf("%s", go);
	}
	system("clear");
	return;
}

//findGroup Menu
void findGroup(int socket) {
	printf("Studiengangsmitglieder finden\n");
	printf("---------------\n");
	printf("0 für Beenden\n");
	char title[MAXDATASIZE];
	printf("Bitte geben Sie den gesuchten Studiengang an: >");
	scanf("%s", title);
	if (strcmp(title, "0")==0) {
		sendMsg(socket, "0");
		return;
	}
	sendMsg(socket, title);
	printf("Folgende Studenten enthalten:\n");
	char* rec;
	while (strcmp(rec = recMsg(socket), "0") != 0) {
		printf("%s\n", rec);
	}
	printf("---------------\n");
	printf("1 für Weiter\n");
	printf(">");
	char go[30];
	scanf("%s", go);
	while (strcmp(go, "1")!=0) {
		printf("Ungültige Eingabe\n");
		printf("1 für Weiter\n");
		printf(">");
		scanf("%s", go);
	}
	system("clear");

}

//showGroups Menu
void showGroups(int socket) {
	printf("Studiengänge finden\n");
	printf("---------------\n");
	printf("0 für Beenden\n");

	sendMsg(socket, "");
	printf("Folgende Studiengänge enthalten:\n");
	char* rec;
	while (strcmp(rec = recMsg(socket), "0") != 0) {
		printf("%s\n", rec);
	}
	printf("---------------\n");
	printf("1 für Weiter\n");
	printf(">");
	char go[30];
	scanf("%s", go);
	while (strcmp(go, "1")!=0) {
		printf("Ungültige Eingabe\n");
		printf("1 für Weiter\n");
		printf(">");
		scanf("%s", go);
	}
	system("clear");
}

//createGroup Menu
void createGroup(int socket) {
	printf("Studiengang anlegen\n");
	printf("---------------\n");
	printf("0 für Beenden\n");
	char title[MAXDATASIZE];
	printf("Bitte Titel eingeben: >");
	scanf("%s", title);
	if (strcmp(title, "0")==0) {
		sendMsg(socket, "0");
		return;
	}
	sendMsg(socket, title);
	char* rec;
	rec = recMsg(socket);
	printf("%s\n", rec);
	printf("---------------\n");
	printf("1 für Weiter\n");
	printf(">");
	char go[30];
	scanf("%s", go);
	while (strcmp(go, "1")!=0) {
		printf("Ungültige Eingabe\n");
		printf("1 für Weiter\n");
		printf(">");
		scanf("%s", go);
	}
	system("clear");

}

//groupsBest Menu
void groupsBest(int socket) {
	printf("Studiengangsbesten anzeigen\n");
	printf("---------------\n");
	printf("0 für Beenden\n");
	char title[MAXDATASIZE];
	printf("Bitte geben Sie den gesuchten Studiengang an: >");
	scanf("%s", title);
	sendMsg(socket, title);
	char* rec;
	rec = recMsg(socket);
	printf("%s\n", rec);
	printf("---------------\n");
	printf("1 für Weiter\n");
	printf(">");
	char go[30];
	scanf("%s", go);
	while (strcmp(go, "1")!=0) {
		printf("Ungültige Eingabe\n");
		printf("1 für Weiter\n");
		printf(">");
		scanf("%s", go);
	}
	system("clear");
}

//bestOfAll Menu
void bestOfAll(int socket) {
	printf("Gesamtbesten anzeigen\n");
	printf("---------------\n");

	char* rec;
	rec = recMsg(socket);
	printf("%s\n", rec);
	printf("---------------\n");
	printf("1 für Weiter\n");
	printf(">");
	char go[30];
	scanf("%s", go);
	while (strcmp(go, "1")!=0) {
		printf("Ungültige Eingabe\n");
		printf("1 für Weiter\n");
		printf(">");
		scanf("%s", go);
	}
	system("clear");
}

//close the Programm
void exitProgramm() {
	exit(0);
}
char MainMenu(int socket) {
	int option;
	showMainMenu();
	while (1) {
		scanf("%i", &option);
		switch (option) {
		case 0:
			system("clear");
			showMainMenu();
			break;
		case 1:
			system("clear");
			sendMsg(socket, "1");
			createStudent(socket);
			system("clear");
			return '1';
		case 2:
			system("clear");
			sendMsg(socket, "2");
			findStudent(socket);
			system("clear");
			return '2';
		case 3:
			system("clear");
			sendMsg(socket, "3");
			editStudent(socket);
			system("clear");
			return '3';
		case 4:
			system("clear");
			sendMsg(socket, "4");
			createGroup(socket);
			system("clear");
			return '4';
		case 5:
			system("clear");
			sendMsg(socket, "5");
			findGroup(socket);
			system("clear");
			return '5';
		case 6:
			system("clear");
			sendMsg(socket, "6");
			showGroups(socket);
			system("clear");
			return '6';
		case 7:
			system("clear");
			sendMsg(socket, "7");
			addMark(socket);
			system("clear");
			return '7';
		case 8:
			system("clear");
			sendMsg(socket, "8");
			groupsBest(socket);
			system("clear");
			return '8';
		case 9:
			system("clear");
			sendMsg(socket, "9");
			bestOfAll(socket);
			system("clear");
			return '9';
		case 10:
			sendMsg(socket, "10");
			exitProgramm();
			break;
		default:
			printf("Ungültige Nummer. 0 für Hauptmenu\n >");
			break;
		}

	}
	return 'X';
}

//show the Student Menu
void showSMenu() {
printf("Menu\n----\n");
printf("1)Daten anzeigen\n");
printf("2)Beenden\n");
printf("---------------\n");
printf("Bitte Nummer eingeben:\n >");
}

//getSData -> get StudentData from Server
void getSData(int socket, char* mNr) {
	char go[10];
	char mnr[10];
	strcpy(mnr, mNr);
	printf("Meine Daten\n");
	printf("----------------\n");
	sendMsg(socket, mNr);

	char* rec;
	while (strcmp(rec = recMsg(socket), "0") != 0) {
		printf("%s", rec);
	}
	printf("---------------\n");
	printf("1 für Weiter\n");
	printf(">");
	scanf("%s", go);
	while (strcmp(go, "1")!=0) {
		printf("Ungültige Eingabe\n");
		printf("1 für Weiter\n");
		printf(">");
		scanf("%s", go);
	}
	system("clear");
}

//the StudentMenu loop
char SMenu(int socket, char* mNr) {
	int option;
	char mnr[10];
	strcpy(mnr, mNr);
	showSMenu();
	while (1) {
		scanf("%i", &option);
		switch (option) {
		case 0:
			system("clear");
			showSMenu();
			system("clear");
			break;
		case 1:
			system("clear");
			sendMsg(socket, "11");
			getSData(socket, mNr);
			system("clear");
			return;
		case 2:
			sendMsg(socket, "10");
			exitProgramm();
		default:
			printf("Ungültige Nummer. 0 für Hauptmenu\n >");
			break;
		}

	}
	return 'X';
}
//Init MainLoop for Socketconnection
int main(int argc, char *argv[ ]) {

	// connectors address information
	struct sockaddr_in their_addr;

	//Container for serveraddr
	struct hostent *he;

	// create a new socket
	// AF_INET -> IPV4; SOCK_STREAM -> TCP
	int sock;
	sock = socket(AF_INET, SOCK_STREAM, 0);

	// checks for mistakes on socket creation
	if (sock==-1) {
		perror("CANT CREATE SOCKET");
		exit(1);
	} else {
		printf("SOCKET Creation successfull\n");
	}

	if (argv[1]==NULL) {
		perror("Wrong Syntax: client $IPAddr");
		exit(1);
	}

	/* connect to server */
	he = gethostbyname(argv[1]);
	//get host by name of the given ip
	if (he == NULL) {
		perror("CANT FIND SERVER");
		exit(1);
	} else {
		printf("Server found at %s\n", argv[1]);
	}

	// host byte order
	their_addr.sin_family = AF_INET;

	// short, network byte order
	their_addr.sin_port = htons(MYPORT);
	their_addr.sin_addr = *((struct in_addr *)he->h_addr);
	memset(&(their_addr.sin_zero), '\0', 8);
	printf("CONNECT to %s:%d\n", argv[1], MYPORT);
	if ((connect(sock, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)))
			==-1) {
		perror("CANT CONNECT");
		exit(1);
	} else {
		printf("Connected\n");
	}

	int run = 1;
	char* typ;
	typ = manageLogin(sock);
	char mnr[11];
	strcpy(mnr, typ);
	while (run) {
		if (strcmp(typ, "-1")==0) {
			printf("Login fehlerhaft\n");
			while (strcmp(typ, "-1")==0) {
				typ = manageLogin(sock);
			}
		} else if (strcmp(typ, "1")==0) {
			while (1) {
				//start AdminMenu
				MainMenu(sock);
			}
		} else if (strcmp(typ, "1")!=0 && strcmp(typ, "-1")!=0) {
			while (1) {
				//start StudentMenu
				SMenu(sock, typ);
			}
		}
	}
	return 0;
}
