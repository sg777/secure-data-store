#define aes_encrypt 0
#define aes_decrypt 1

int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
            unsigned char *iv, unsigned char *ciphertext);
int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
						unsigned char *iv, unsigned char *plaintext);			
void handleErrors(void);
unsigned char* save_data(char *plaintext, char *key_id);
unsigned char* retrieve_data(int data_id);
