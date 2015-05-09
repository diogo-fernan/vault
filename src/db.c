#include "db.h"

sqlite3 *VAULT;
sqlite3_stmt *STATEMENT;
char QUERY[QUERY_SIZE];

int	rc;
char *zErrMsg = 0;
const char *pzTail;

int CONNECTED = 0;
char *aux = NULL;

void printSQLERROR (char* msg) {
	fprintf (stderr, " SQL ERROR: %s \n", msg);
}

int open_db () {
	if (SQLITE_OK != sqlite3_open_v2 ("../db/vault.db", 
						&VAULT, SQLITE_OPEN_READWRITE, NULL)) {
		sqlite3_close (VAULT);
		printSQLERROR (sqlite3_errmsg (VAULT));
		return -1;
	}
	sqlite3_exec (VAULT, "PRAGMA foreign_keys = ON", NULL, 0, &zErrMsg);
	CONNECTED = 1;
	return 0;
}

int close_db () {
	if (CONNECTED) {
		sqlite3_close (VAULT);
	}
	return 0;
}


void execute (char *query, int (*callback)(void*, int, char**, char**)) {
	rc = sqlite3_exec (VAULT, query, callback, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		printSQLERROR (zErrMsg);
		sqlite3_free (zErrMsg);
	}
}

int prepare () {
	rc = sqlite3_prepare_v2 (VAULT, QUERY, -1, &STATEMENT, &pzTail);
	if (rc != SQLITE_OK) {
		memset (QUERY, '\0', QUERY_SIZE);
		printSQLERROR ("not successful preparing statement");
		return -1;
	}
	memset (QUERY, '\0', QUERY_SIZE);
	return 0;
}


int registration (char *name, unsigned char *passwordSum) {
	snprintf (QUERY, QUERY_SIZE, "SELECT MAX (id) FROM User");
	if (-1 == prepare ()) {
		return -1;
	}
	else {
		sqlite3_step (STATEMENT);
		int maxUser = sqlite3_column_int (STATEMENT, 0);

		snprintf (QUERY, QUERY_SIZE, 
			"INSERT INTO User VALUES (%d, '%s', UPPER(HEX('%s')))", 
			++maxUser, name, passwordSum);
		if (-1 == prepare ()) {
			return -1;
		}
		else {
			sqlite3_step (STATEMENT);
		}
	}
	return 0;
}

int login (char *name, unsigned char *passwordSum) {
	snprintf (QUERY, QUERY_SIZE, 
		"SELECT COUNT(*) FROM User WHERE name='%s' AND password=UPPER(HEX('%s'))", 
		name, passwordSum);
	if (-1 == prepare ()) {
		return -1;
	}
	else {
		sqlite3_step (STATEMENT);
		int lines = sqlite3_column_int (STATEMENT, 0);

		if (0 == lines || 1 < lines) {
			printSQLERROR ("not successful");
			return -1;
		}

		if (1 == lines) {
			snprintf (QUERY, QUERY_SIZE, 
				"SELECT id FROM User WHERE name='%s' AND password=UPPER(HEX('%s'))", 
				name, passwordSum);
			if (-1 == prepare ()) {
				return -1;
			}
			sqlite3_step (STATEMENT);
			fprintf (stdout, " hello %s \n\n", name);
			return sqlite3_column_int (STATEMENT, 0);
		}
	}
	return -1;
}

int account (int id, char *attribute, char *name, unsigned char *sum) {
	if (NULL == name) {
		snprintf (QUERY, QUERY_SIZE, 
			"UPDATE User SET %s = '%s' WHERE id = %d", 
			attribute, sum, id);
	}
	else {
		snprintf (QUERY, QUERY_SIZE, 
			"UPDATE User SET %s = '%s' WHERE id = %d", 
			attribute, name, id);
	}
	if (-1 == prepare ()) {
		return -1;
	}
	else {
		sqlite3_step (STATEMENT);
	}
	return 0;
}

int insert (int id, char *name, char *secret, char *designation) {
	snprintf (QUERY, QUERY_SIZE, "SELECT MAX (id) FROM Information");
	if (-1 == prepare ()) {
		return -1;
	}
	else {
		sqlite3_step (STATEMENT);
		int maxId = sqlite3_column_int (STATEMENT, 0);

		snprintf (QUERY, QUERY_SIZE, 
			"INSERT INTO Information VALUES (%d, '%s', '%s', '%s', %d)",
			++maxId, name, secret, designation, id);
		
		if (-1 == prepare ()) {
			return -1;
		}
		else {
			sqlite3_step (STATEMENT);
		}
	}
	return 0;
}

int modify (int id, char *column, char *value, int userId) {
	snprintf (QUERY, QUERY_SIZE, 
		"UPDATE Information SET %s = '%s' WHERE id = %d AND idUser = %d", 
		column, value, id, userId);
	fprintf (stdout, "%s\n", QUERY);
	if (-1 == prepare ()) {
		return -1;
	}
	else {
		sqlite3_step (STATEMENT);
	}
	return 0;
}

static int printPeek (void *NotUsed, int argc, char **args, char **azColName) {
	fprintf (stdout, "\n");
	int i;
	for (i = 0; i < argc; i++) {
		if (!strncmp(azColName[i], "secret", 6)) {
			if (args[i] != NULL) {
				char *password = (char *) malloc (strlen (args[i]) * sizeof (char));
				dec_AES_CBC (args[i], aux, password ); 
				fprintf (stdout, " %s: %s\n", azColName[i], password);
				free (password);
				password = NULL;
			}
		}
		else {
			fprintf (stdout, " %s: %s\n", azColName[i], args[i] ? args[i] : "NULL");
		}
	}
	return 0;
}

int peek (int id, char *secret, int IDUSER) {
	aux = secret;
	snprintf (QUERY, QUERY_SIZE, 
		"SELECT * FROM Information WHERE id = %d AND idUser = %d", 
		id, IDUSER);
	execute (QUERY, printPeek);
	aux = NULL;
	return 0;
}

int delete (char *table, int id, char *column, char *pattern, int userId) {
	if (0 == id && column == NULL && pattern == NULL) {
		printSQLERROR ("invalid paramters");
		return -1;
	}
	if (-1 == id) { // all
		snprintf (QUERY, QUERY_SIZE, "DELETE FROM %s", table);
	}
	if (0 < id) { // id id
		snprintf (QUERY, QUERY_SIZE, 
			"DELETE FROM %s WHERE id = %d AND idUser = %d", table, id, userId);
	}
	if (column != NULL && pattern != NULL) { // remaining cases
		snprintf (QUERY, QUERY_SIZE, 
			"DELETE FROM %s WHERE %s LIKE \"%%%s%%\" AND idUser = %d", 
			table, column, pattern, userId);
	}

	if (-1 == prepare ()) {
		return -1;
	}
	else  {
		sqlite3_step (STATEMENT);
	}
	return 0;
}

static int printRecords (void *NotUsed, int argc, char **args, char **azColName) {
	fprintf (stdout, "\n");
	int i;
	for (i=0; i<argc; i++) {
		fprintf (stdout, " %s: %s\n", azColName[i], args[i] ? args[i] : "NULL");
	}
	return 0;
}

int print (char *table, char *column, char *pattern, int flag, int IDUSER) {
	if (flag) {
		snprintf (QUERY, QUERY_SIZE, 
			"SELECT id, timedate, operation FROM Log WHERE name LIKE \"%s\"", table);
	}
	else {
		if (pattern != NULL && column != NULL) {
			snprintf (QUERY, QUERY_SIZE, 
				"SELECT * FROM %s WHERE %s LIKE \"%%%s%%\" AND idUser = %d", 
				table, column, pattern, IDUSER);
		}
		else {
			snprintf (QUERY, QUERY_SIZE, "SELECT * FROM %s", table);
		}
	}
	execute (QUERY, printRecords);
	return 0;
}
