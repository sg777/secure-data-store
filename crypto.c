#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <string.h>

#include "crypto.h"
#include "storage.h"
#include "misc.h"

#define data_size 2048
unsigned char *iv = (unsigned char *)"0123456789012345";

void handleErrors(void)
{
    ERR_print_errors_fp(stderr);
    abort();
}

int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
            unsigned char *iv, unsigned char *ciphertext)
{
    EVP_CIPHER_CTX *ctx;

    int len;

    int ciphertext_len;

    if(!(ctx = EVP_CIPHER_CTX_new()))
        handleErrors();

    if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        handleErrors();

    if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
        handleErrors();
    ciphertext_len = len;
    if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
        handleErrors();
    ciphertext_len += len;

    EVP_CIPHER_CTX_free(ctx);

    return ciphertext_len;
}


int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
            unsigned char *iv, unsigned char *plaintext)
{
    EVP_CIPHER_CTX *ctx;

    int len;

    int plaintext_len;

    if(!(ctx = EVP_CIPHER_CTX_new()))
        handleErrors();
    if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        handleErrors();
    if(1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
        handleErrors();
    plaintext_len = len;

    if(1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len))
        handleErrors();
    plaintext_len += len;

    EVP_CIPHER_CTX_free(ctx);

    return plaintext_len;
}

unsigned char* save_data(char *plaintext, char *key_id)
{
	 unsigned char ciphertext[data_size], *cipher = NULL;
	 int ciphertext_len;

	cipher = calloc(data_size, sizeof(char));

	ciphertext_len = encrypt (plaintext, strlen ((char *)plaintext), get_key("k1"), iv,
							   ciphertext);
	
	str_to_hexstr(ciphertext,ciphertext_len, cipher);
	store_enc_data(cipher,ciphertext_len, key_id);

    printf("cipher\n");
    for(int i=0; i< ciphertext_len; i++) {
            printf("%02x ", ciphertext[i]);
    }

	if(cipher)
		free(cipher);

	return ciphertext;
	
}
unsigned char* retrieve_data(int data_id)
{
	
	int cipher_len, decryptedtext_len; 
	char *key_id = NULL, *enc_data = NULL;
	unsigned char *cipher = NULL, decryptedtext[data_size];

	key_id = calloc(20, sizeof(char));
	enc_data = get_enc_data(data_id,&cipher_len,&key_id);
	
	cipher = calloc(data_size, sizeof(char));
	hexstr_to_str(enc_data, cipher);

    decryptedtext_len = decrypt(cipher, cipher_len, get_key(key_id), iv, decryptedtext);

    decryptedtext[decryptedtext_len] = '\0';

    printf("\nDecrypted text is:\n");
    printf("%s\n", decryptedtext);

	if(key_id)
		free(key_id);
	if(cipher)
		free(cipher);
	
	return decryptedtext;
}

