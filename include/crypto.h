#ifndef __VAULT_CRYPTO__
#define __VAULT_CRYPTO__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>

#include "aes.h"
#include "md5.h"

#define SUM_SIZE	16
#define HASHES_LOOP	4096

void generatePseudoRandomPassword (
	char *,
	int,
	int
);

void enc_AES_CBC (
	char *,
	char *,
	char *
);

void dec_AES_CBC (
	char *,
	char *,
	char *
);

void hash_MD5_loop (
	char *,
	unsigned char *
);

#endif