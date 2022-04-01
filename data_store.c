#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <string.h>

#include "crypto.h"
#include "storage.h"
#include "misc.h"

void ds_init()
{
	bet_create_schema();
	load_keys();
}

void display_usage()
{
	printf(
		"\n======save API======"
		"\nDescription: Encrypts the data provided with the key identifier accessable to the server and stores in the local DB"
		"\nUsage"
		"\n./data_store save data key_id"
		"\n======retrive API======"
		"\nDescription: Retrives the encrypted data with the data identifier provided"
		"\nUsage"
		"\n./data_store data_id"
		"\n======list_keys API======"
		"\nDescription: Lists all the keys available in the server for the user"
		"\nUsage"
		"\n./data_store list_keys"
		"\n======list_data API======"
		"\nDescription: Lists all the encrypted data sets identifiers available in the server"
		"\nUsage"
		"\n./data_store list_data\n"
		"\n======Examples======"
		"\n./data_store save \"this is test data \" k1"
		"\n ./data_store retrieve 1\n");
}
int main (int argc, char **argv)
{
	ds_init();
	
	if(argc >=2) {
		if(strcmp(argv[1], "save") == 0) {
			if(argc == 4) {
				if(check_if_key_exists(argv[3]) == 1) {
					save_data(argv[2],argv[3]);
				} else {
					printf("\nThe key identifier provided doesn't exists, here is the available list of key identifiers");
					list_keys();
				}
			} else {
				display_usage();
			}
		} else if(strcmp(argv[1], "retrieve") == 0) {
			if(argc == 3) {
				if(check_if_data_exists(atoi(argv[2])) == 1) {
					retrieve_data(atoi(argv[2]));
				} else {
					printf("The data set identifier provided doesn't exist, here is the available list of data identifiers");
					list_data_ids();
				}
			} else {
				display_usage();
			}
		} else if(strcmp(argv[1], "list_keys") == 0) {
			list_keys();
		} else if(strcmp(argv[1], "list_data") == 0) {
			list_data_ids();
		} else {
				display_usage();
		}
	} else {
				display_usage();
		}
	
    return 0;
}



