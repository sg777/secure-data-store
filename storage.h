#include <sqlite3.h>

int load_keys();
void bet_create_schema();
unsigned char* get_key(char *key_id);
int store_enc_data(char *cipher, int cipher_data_len, char *key_id);
unsigned char* get_enc_data(int data_id, int *cipher_len, char **key_id) ;
void list_data_ids();
void list_keys();
int check_if_key_exists(char *key_id);
int check_if_data_exists(int data_id);
