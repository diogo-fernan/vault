#ifndef __VAULT_CLI__
#define __VAULT_CLI__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>

#include "crypto.h"
#include "db.h"

#define SUM_SIZE 16

void handleNewLine (
	char *
);

void printERROR (
	char*
);

void usage ();

int parser (
	char **,
	int,
	int
);

void partitioner (
	char *,
	char **,
	int *
);

#endif