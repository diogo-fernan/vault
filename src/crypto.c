#include "crypto.h"

void generatePseudoRandomPassword (char *random, int length, int seed) {
	char *charset = "0aAbB1cCdD1eEf2F2gG3hHiIjJk3K45lLmM9n5N6o4OpPqQ76rRsStT78uUvVwW98xXyY0zZ";
	srand (seed); // time( NULL )

	int i;
	for (i = 0; i < length; i++)  {
		random[i] = charset[rand() % 72];
	}

	random[length] = '\0';
}

void enc_AES_CBC (char *plaintext, char *key, char *cryptogram) {
	int klength = strlen (key);
	int plength = strlen (plaintext);
	const int bits = klength * 8;

	char *iv = (char *) malloc ((AES_BLOCK_SIZE + 1) * sizeof (char));
	generatePseudoRandomPassword (iv, AES_BLOCK_SIZE, 0);

	AES_KEY aes_key;
	AES_set_encrypt_key (key, bits, &aes_key);
	AES_cbc_encrypt (plaintext, cryptogram, plength, &aes_key, iv, 1);

	free (iv);
}

void dec_AES_CBC (char *cryptogram, char *key, char *plaintext) {
	int klength = strlen (key);
	int clength = strlen (cryptogram);
	const int bits = klength * 8;

	char *iv = (char *) malloc ((AES_BLOCK_SIZE + 1) * sizeof (char));
	generatePseudoRandomPassword (iv, AES_BLOCK_SIZE, 0);
	// char *plaintext = (char *) malloc( clength * sizeof( char ) );

	AES_KEY aes_key;
	AES_set_decrypt_key (key, bits, &aes_key);
	AES_cbc_encrypt (cryptogram, plaintext, clength, &aes_key, iv, 0);

	free (iv);
}

void hash_MD5 (char *str, unsigned char *sum) {
	unsigned int length = strlen (str);
	MD5_CTX mdContext;

	MD5Init (&mdContext);
	MD5Update (&mdContext, str, length);
	MD5Final (&mdContext);

	int i;
	for (i = 0; i < SUM_SIZE; i++) {
		sum[i] = mdContext.digest[i];
	}
	sum[SUM_SIZE] = '\0';
}

void hash_MD5_loop (char *str, unsigned char *sum) {
	unsigned char *sum2 = (unsigned char *) malloc((SUM_SIZE + 1) * sizeof(unsigned char));
	hash_MD5 (str, sum);
	int i, j;
	for (i=0; i<HASHES_LOOP; i++) {
		for (j=0; j<=SUM_SIZE; j++) {
			sum2[j] = sum[j];
		}
		hash_MD5 (sum2, sum);
	}
	free (sum2);
}