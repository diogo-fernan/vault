#include "cli.h"

unsigned char *sum;

void handleNewLine (char *str) {
	str[strlen(str) - 1] = '\0';
}

void printERROR (char* msg) {
	fprintf (stderr, " ERROR: %s\n", msg);
}

void usage () {
	fprintf (stdout, " Usage: vault register/login name password\n"
		" vault$ command [options]\n\n"
		" command and [options] include:\n"
		"   : account\n"
		"    name name\n"
		"    password password\n"
		"   : remove\n"
		"    account/information name/designation pattern\n"
		"    account/information id id (if current account, will exit app)\n"
		"    account/information all (if account, will exit app)\n"
		"   : insert\n"
		"    entry name secret [rand] - if [rand], a pseudo-random password will used for encryption\n"
		"   : peek\n"
		"    entry id secret\n"
		"   : modify\n"
		"    entry id name/designation value\n"
		"   : print\n"
		"    accounts [name] [pattern] - if not [name] and [pattern], all entries are printed (hashed)\n"
		"    information [name/designation] [pattern] - if not [name/designation] and [pattern], all entries are printed (encrypted)\n"
		"    log account/information\n"
		"   : generate\n"
		"    password length [seed] - [seed] is the pseudo-random generator seed\n"
		"   : help - print this message\n");
}

int parser (char **args, int argc, int userid) {
	char table[12];
	char column[12];

	// account
	if (!strncmp (args[0], "account", 7)) {
		if (3 == argc) {
			if (!strncmp (args[1], "name", 4)) {
				account (userid, args[1], args[2], NULL);
				return 0;
			}
			else if (!strncmp( args[1], "password", 8)) {
				sum = (unsigned char *) malloc ((SUM_SIZE + 1) * sizeof (unsigned char));
				hash_MD5_loop (args[2], sum);
				account (userid, args[1], NULL, sum);
				free (sum);
				sum = NULL;
				return 0;
			}
		}
	}

	// remove
	if (!strncmp (args[0], "remove", 6)) {
		if (3 == argc) {
			if (!strncmp (args[1], "account", 7)) {
				if (!strncmp (args[2], "all", 3)) {
					strncpy (table, "User", 4 );
					table[4] = '\0';
					delete (table, -1, NULL, NULL, userid);
					return 1;
				}
			}
			if (!strncmp (args[1], "information", 11)) {
				if (!strncmp (args[2], "all", 3)) {
					strncpy (table, "Information", 11);
					table[11] = '\0';
					delete (table, -1, NULL, NULL, userid);
					return 0;
				}
			}
		}
		if (4 == argc) {
			if (!strncmp (args[1], "account", 7)) {
				strncpy (table, "User", 4);
				table[4] = '\0';
				if (!strncmp (args[2], "name", 4)) {
					strncpy (column, "name", 4);
					column[4] = '\0';
					delete (table, 0, column, args[3], userid);
					return 0;
				}
				if (!strncmp (args[2], "id", 2)) {
					int idToRemove = atoi (args[3]);
					delete (table, idToRemove, NULL, NULL, userid);
					return userid == idToRemove;
				}
			}
			if (!strncmp (args[1], "information", 11)) {
				strncpy (table, "Information", 11);
				table[11] = '\0';
				if (!strncmp (args[2], "name", 4)) {
					strncpy (column, "name", 4);
					column[4] = '\0';
					delete (table, 0, column, args[3], userid);
					return 0;
				}
				if (!strncmp (args[2], "designation", 3)) {
					strncpy (column, "designation", 11);
					column[11] = '\0';
					delete (table, 0, column, args[3], userid);
					return 0;
				}
				if (!strncmp( args[2], "id", 2)) {
					int idToRemove = atoi (args[3]);
					delete (table, idToRemove, NULL, NULL, userid);
					return 0;
				}
			}
		}
	}

	// insert
	if (!strncmp (args[0], "insert", 6) && !strncmp (args[1], "entry", 5)) {
		int flag = 0;
		char password[34];
		char designation[51];
		if (4 == argc) {
			while (1) {
				fprintf (stdout, "\n Password for encryption (16/24/32 bytes): ");
				fgets (password, 34, stdin);
				handleNewLine (password);	
				int length = strlen (password);
				if (16 == length || 24 == length || 32 == length) {
					break;
				}
			}
			flag = 1;
		}
		if (5 == argc && !strncmp (args[4], "rand", 4)) {
			generatePseudoRandomPassword (password, 16, time (NULL));
			fprintf (stdout, "\n Password: %s \n", password);
			flag = 1;
		}
		if (flag) {
			fprintf (stdout, "\n Designation: ");
			fgets (designation, 51, stdin);
			handleNewLine (designation);	
			fprintf (stdout, "\n");
			
			char *encryptedPassword = (char *) malloc (strlen (password) * sizeof (char));
			enc_AES_CBC (args[3], password, encryptedPassword); 
			insert (userid, args[2], encryptedPassword, designation);
			return 0;
		}
	}

	// peek
	if (!strncmp (args[0], "peek", 4)) {
		if (4 == argc) {
			if (!strncmp (args[1], "entry", 5)) {
				peek (atoi (args[2]), args[3], userid);
				fprintf (stdout, "\n");
				return 0;
			}
		}
	}

	// modify
	if (!strncmp (args[0], "modify", 6)) {
		if (5 == argc) {
			if (!strncmp (args[1], "entry", 7)) {
				if (!strncmp (args[3], "name", 4)) {
					strncpy (column, "name", 4 );
					column[4] = '\0';
					modify (atoi (args[2]), column, args[4], userid);
					return 0;
				}
				if (!strncmp (args[3], "designation", 11)) {
					strncpy (column, "designation", 11);
					column[11] = '\0';
					modify (atoi(args[2]), column, args[4], userid);
					return 0;
				}
			}
		}
	}
	// print
	if (!strncmp (args[0], "print", 5)) {
		if (2 == argc) {
			if (!strncmp (args[1], "accounts", 8)) {
				strncpy (table, "User", 4);
				table[4] = '\0';
				print (table, NULL, NULL, 0, userid);
				fprintf (stdout, "\n");
				return 0;
			}
			if (!strncmp (args[1], "information", 11)) {
				strncpy (table, "Information", 11);
				table[11] = '\0';
				print (table, NULL, NULL, 0, userid);
				fprintf (stdout, "\n");
				return 0;
			}
		}
		if (3 == argc) {
			if (!strncmp (args[1], "log", 3)) {
				if (!strncmp (args[2], "account", 7)) {
					strncpy (table, "User", 4);
					table[4] = '\0';
					print (table, NULL, NULL, 1, userid);
					fprintf (stdout, "\n");
					return 0;
				}
				if (!strncmp (args[2], "information", 11)) {
					strncpy (table, "Information", 11);
					table[11] = '\0';
					print (table, NULL, NULL, 1, userid);
					fprintf (stdout, "\n");
					return 0;
				}
			}
		}
		if (4 == argc) {
			if (!strncmp (args[1], "accounts", 8 ) && 
				!strncmp (args[2], "name", 4)) {
				strncpy (table, "User", 4);
				table[4] = '\0';
				strncpy (column, "name", 4);
				column[4] = '\0';
				print (table, column, args[3], 0, userid);
				fprintf (stdout, "\n");
				return 0;
			}
			if (!strncmp (args[1], "information", 11)) {
				int flag = 0;
				strncpy (table, "Information", 11);
				table[11] = '\0';
				if (!strncmp (args[2], "name", 4)) {
					strncpy (column, "name", 4);
					column[4] = '\0';
					flag = 1;
				}
				if (!strncmp (args[2], "designation", 11)) {
					strncpy (column, "designation", 11);
					column[11] = '\0';
					flag = 1;
				}
				if (flag) {
					print (table, column, args[3], 0, userid);
					fprintf (stdout, "\n");
					return 0;
				}
			}
		}
	}

	// generate
	if (!strncmp (args[0], "generate", 8)) {
		if (3 <= argc && 4 >= argc) {
			if (!strncmp( args[1], "password", 8)) {
				unsigned int seed;
				seed = (unsigned int) (3 == argc ? 
					time (NULL) : 
					strtol (args[3], NULL, 10));
				int length = strtol (args[2], NULL, 10);
				char *password = (char *) malloc ((length + 1) * sizeof (char));
				generatePseudoRandomPassword( password, length, seed);
				fprintf (stdout, "\n Password: %s\n\n", password);
				return 0;
			}
		}
	}

	printERROR ("command not found\n");
	usage ();
	return 0;
}

void partitioner (char *command, char **args, int *argc) {
	*argc = 0;

	while (*command != '\0')  {
		while (*command == ' ' || *command == '\t') {
			*command++ = '\0';
		}

		*args++ = command;
		(*argc)++;
		
		while (*command != '\0' && *command != ' ' && *command != '\t') {
			command++;
		}
	}

	*args = '\0';
}