#ifndef __VAULT_DB__
#define __VAULT_DB__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>

#include "sqlite3.h"
#include "crypto.h"

#define QUERY_SIZE 256

int open_db ();

int close_db ();

int registration ( 
	char *,
	unsigned char *
);

int login (
	char *,
	unsigned char *
);

int account (
	int,
	char *,
	char *,
	unsigned char *
);

int insert (
	int,
	char *,
	char *,
	char *
);

int peek (
	int,
	char *,
	int
);

int delete (
	char *,
	int,
	char *,
	char *,
	int
);

int modify (
	int,
	char *,
	char *,
	int
);

int print (
	char *,
	char *,
	char *,
	int,
	int
);

#endif