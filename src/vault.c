#include "cli.h"
#include "crypto.h"
#include "db.h"

#if defined(__WIN32) || defined(_WIN64) || defined(__TOS_WIN__) || defined(__WINDOWS__)
	#define WINDOWS
#elif defined(unix) || defined(__unix__) || defined(__unix)
	#define UNIX
#endif

#define COMMAND_SIZE 1024

char binaries[9];
char MSG[100];

int USERID;
unsigned char *sum = NULL;

int main (int argc, char **args) {

#if defined(WINDOWS)
	strncpy (binaries, "vault.exe", 9);
#elif defined(UNIX)
	strncpy (binaries, "./vault", 7);
#endif

	if (argc < 4) {
		if (argc == 2) {
			if (!strncmp (args[1], "help", 4)) {
				usage ();
				return EXIT_SUCCESS;
			}
		}
		printERROR ("insufficient args");
		usage ();
		return EXIT_SUCCESS;
	}
	if (argc > 4) {
		printERROR ("exceeding args");
		usage ();
		return EXIT_SUCCESS;
	}

	if (-1 == open_db ()) {
		return EXIT_SUCCESS;
	}
	sum = (unsigned char *) malloc ((SUM_SIZE + 1) * sizeof (unsigned char));
	if (!strncmp (args[1], "register", 8)) {
		hash_MD5_loop (args[3], sum);
		registration (args[2], sum);
		close_db ();
		return EXIT_SUCCESS;
	}

	if (strncmp (args[1], "login", 5)) {
		snprintf (MSG, 25 + strlen( args[1] ), "unknown option: %s ", args[1]);
		printERROR (MSG);
		usage ();
		close_db ();
		return EXIT_SUCCESS;
	}

	hash_MD5_loop (args[3], sum);
	if (-1 == (USERID = login (args[2], sum))) {
		return EXIT_SUCCESS;
	}
	
	free (sum);
	args = NULL;
	
	char command[COMMAND_SIZE];
	char *commandArgs[64];
	int commandArgc;
	while (1) {
		fprintf (stdout, "vault$ ");
		fgets (command, COMMAND_SIZE, stdin);
		if (!strncmp (command, "\n", 1)) {
			continue;
		}
		else {
			handleNewLine (command);
			if (!strncmp (command, "exit", 4)) {
				break;
			}
			partitioner (command, commandArgs, &commandArgc);
			if (parser (commandArgs, commandArgc, USERID)) {
				break;
			}
		}
	}

	close_db ();
	return EXIT_SUCCESS;
}