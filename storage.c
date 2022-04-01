#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#include "storage.h"
#include "err.h"
#include "misc.h"

#define no_of_tables 2
#define sql_query_size 8192

#define no_of_keys 10

char *db_name = "data_store.db";

const char *table_names[no_of_tables] = { "keys",     "data_store"};

const char *schemas[no_of_tables] = {
	"(key_id varchar(100) primary key,key varchar(100))",
	"(data_id INTEGER primary key AUTOINCREMENT, cipher varchar(2048), cipher_len int, key_id varchar(100))"
};

const char *key_ids[no_of_keys] = {
	"k1", "k2", "k3", "k4", "k5", "k6", "k7", "k8", "k9", "k10"
};

const char *keys[no_of_keys] = {
	"01234567890123456789012345678901",
	"01234567890123456789012345678901",
	"01234567890123456789012345678901",
	"01234567890123456789012345678901",
	"01234567890123456789012345678901",
	"01234567890123456789012345678901",
	"01234567890123456789012345678901",
	"01234567890123456789012345678901",
	"01234567890123456789012345678901",
	"01234567890123456789012345678901"
};


sqlite3 *bet_get_db_instance()
{
	sqlite3 *db = NULL;
	int rc;

	rc = sqlite3_open(db_name, &db);
	if (rc) {
		printf("Can't open database: %s", sqlite3_errmsg(db));
		return (0);
	}
	sqlite3_busy_timeout(db, 1000);
	return db;
}

void bet_make_insert_query(int argc, char **argv, char **sql_query)
{
	char *null_str = "NULL";
	sprintf(*sql_query, "INSERT INTO %s values(", argv[0]);
	for (int32_t i = 1; i < argc; i++) {
		if (strlen(argv[i]) != 0) {
			strcat(*sql_query, argv[i]);		
		} else {
			strcat(*sql_query, null_str);
			printf("some arg is null");
		}
		if ((i + 1) < argc)
			strcat(*sql_query, ",");
		else
			strcat(*sql_query, ");");
	}
}

unsigned char* get_key(char *key_id) 
{
	sqlite3_stmt *stmt = NULL;
	sqlite3 *db = NULL;
	int rc;
	char *sql_query = NULL, *err_msg = NULL, *key = NULL;
	
	db = bet_get_db_instance();
	sql_query = calloc(sql_query_size, sizeof(char));
	
	sprintf(sql_query,"select key from keys where key_id=\'%s\'", key_id);
	
	rc = sqlite3_prepare_v2(db, sql_query, -1, &stmt, NULL);
	if (rc != SQLITE_OK) {
		printf("error_code :: %d, error msg ::%s, \n query ::%s", rc, sqlite3_errmsg(db), sql_query);
		goto end;
	}
	while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
		key = sqlite3_column_text(stmt, 0);		
	}
	end:
		if (sql_query)
			free(sql_query);
		sqlite3_close(db);
		return key;	
}

int load_keys()
{
	sqlite3 *db = NULL;
	int rc, argc = 3;
	char *sql_query = NULL, *err_msg = NULL, **argv = NULL;

	db = bet_get_db_instance();
	sql_query = calloc(sql_query_size, sizeof(char));

	alloc_args(argc,&argv);
	for (int i = 0; i < no_of_keys; i++) {
		strcpy(argv[0], "keys");
		sprintf(argv[1], "\'%s\'", key_ids[i]);
		sprintf(argv[2], "\'%s\'", keys[i]);
		bet_make_insert_query(argc, argv, &sql_query);
		rc = sqlite3_exec(db, sql_query, NULL, 0, &err_msg);
		if (rc != SQLITE_OK) {
			sqlite3_free(err_msg);
		}
		memset_args(argc,&argv);
		memset(sql_query, 0x00, sql_query_size);
	}
	if (sql_query)
		free(sql_query);
	sqlite3_close(db);
	dealloc_args(argc,&argv);
}

int32_t sqlite3_check_if_table_exists(sqlite3 *db, const char *table_name)
{
	sqlite3_stmt *stmt = NULL;
	char *sql_query = NULL;
	int rc, retval = 0;

	db = bet_get_db_instance();
	sql_query = calloc(sql_query_size, sizeof(char));

	sprintf(sql_query, "select name from sqlite_master where type = \"table\" and name =\"%s\";", table_name);
	rc = sqlite3_prepare_v2(db, sql_query, -1, &stmt, NULL);
	if (rc != SQLITE_OK) {
		printf("error_code :: %d, error msg ::%s, \n query ::%s", rc, sqlite3_errmsg(db), sql_query);
		goto end;
	}
	while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
		const char *name = (const char *)sqlite3_column_text(stmt, 0);
		if (strcmp(name, table_name) == 0) {
			retval = 1;
			break;
		}
	}
	sqlite3_finalize(stmt);
	sqlite3_close(db);
end:
	if (sql_query)
		free(sql_query);
	return retval;
}

void bet_create_schema()
{
	sqlite3 *db = NULL;
	int rc;
	char *sql_query = NULL, *err_msg = NULL;

	db = bet_get_db_instance();
	sql_query = calloc(sql_query_size, sizeof(char));
	for (int32_t i = 0; i < no_of_tables; i++) {
		if (sqlite3_check_if_table_exists(db, table_names[i]) == 0) {
			sprintf(sql_query, "CREATE TABLE %s %s;", table_names[i], schemas[i]);

			rc = sqlite3_exec(db, sql_query, NULL, 0, &err_msg);
			if (rc != SQLITE_OK) {
				printf("error_code :: %d, error msg ::%s, \n query ::%s", rc, sqlite3_errmsg(db),
					  sql_query);
				sqlite3_free(err_msg);
			}
			memset(sql_query, 0x00, sql_query_size);
		}
	}
	sqlite3_close(db);
	if (sql_query)
		free(sql_query);
	sqlite3_close(db);
}

int store_enc_data(char *cipher, int cipher_data_len, char *key_id)
{
	sqlite3 *db = NULL;
	int rc, argc;
	char *sql_query = NULL, *err_msg = NULL, **argv = NULL;

	db = bet_get_db_instance();
	sql_query = calloc(sql_query_size, sizeof(char));

	argc = 5;
	rc = alloc_args(argc,&argv);
	if(rc != OK) {
		printf("Error :: %d", rc);
	}

	strcpy(argv[0], "data_store");
	strcpy(argv[1], "NULL");
	sprintf(argv[2], "\'%s\'", cipher);
	sprintf(argv[3], "%d", cipher_data_len);
	sprintf(argv[4], "\'%s\'", key_id);
	bet_make_insert_query(argc, argv, &sql_query);
	rc = sqlite3_exec(db, sql_query, NULL, 0, &err_msg);
	if (rc != SQLITE_OK) {
		printf("error_code :: %d, error msg ::%s, \n query ::%s", rc, sqlite3_errmsg(db), sql_query);
		sqlite3_free(err_msg);
	}
	memset_args(argc,&argv);
	memset(sql_query, 0x00, sql_query_size);

	sqlite3_close(db);
	if (sql_query)
		free(sql_query);
	sqlite3_close(db);
	dealloc_args(argc,&argv);
}

unsigned char* get_enc_data(int data_id, int *cipher_len, char **key_id) 
{
	sqlite3_stmt *stmt = NULL;
	sqlite3 *db = NULL;
	int rc;
	char *sql_query = NULL, *err_msg = NULL, *cipher = NULL;
	
	db = bet_get_db_instance();
	sql_query = calloc(sql_query_size, sizeof(char));
	sprintf(sql_query,"select cipher, cipher_len, key_id from data_store where data_id=%d", data_id);
	rc = sqlite3_prepare_v2(db, sql_query, -1, &stmt, NULL);
	if (rc != SQLITE_OK) {
		printf("error_code :: %d, error msg ::%s, \n query ::%s", rc, sqlite3_errmsg(db), sql_query);
		goto end;
	}
	while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
		cipher = sqlite3_column_text(stmt, 0);		
		*cipher_len = sqlite3_column_int(stmt, 1);
		strcpy(*key_id, sqlite3_column_text(stmt, 2));
	}
	end:
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		if (sql_query)
			free(sql_query);
		return cipher;	
}

void list_data_ids()
{
	
	sqlite3_stmt *stmt = NULL;
	sqlite3 *db = NULL;
	int rc;
	char *sql_query = NULL, *err_msg = NULL;
	
	db = bet_get_db_instance();
	sql_query = calloc(sql_query_size, sizeof(char));
	sprintf(sql_query,"select data_id from data_store");
	rc = sqlite3_prepare_v2(db, sql_query, -1, &stmt, NULL);
	if (rc != SQLITE_OK) {
		printf("error_code :: %d, error msg ::%s, \n query ::%s", rc, sqlite3_errmsg(db), sql_query);
		goto end;
	}
	printf("\nHere is the list of data sets stored on the server");	
	while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
		 printf("\ndata_id :: %d", sqlite3_column_int(stmt, 0)); 
	}
	printf("\n");
	end:
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		if (sql_query)
			free(sql_query);
}

void list_keys()
{
	
	sqlite3_stmt *stmt = NULL;
	sqlite3 *db = NULL;
	int rc;
	char *sql_query = NULL, *err_msg = NULL;
	
	db = bet_get_db_instance();
	sql_query = calloc(sql_query_size, sizeof(char));
	sprintf(sql_query,"select key_id from keys");
	rc = sqlite3_prepare_v2(db, sql_query, -1, &stmt, NULL);
	if (rc != SQLITE_OK) {
		printf("error_code :: %d, error msg ::%s, \n query ::%s", rc, sqlite3_errmsg(db), sql_query);
		goto end;
	}
	printf("\nKey identifiers stored in the DB");
	while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
		 printf("\nkey_id :: %s", sqlite3_column_text(stmt, 0)); 
	}
	printf("\n");
	end:
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		if (sql_query)
			free(sql_query);
}

int check_if_key_exists(char *key_id)
{
	sqlite3_stmt *stmt = NULL;
	sqlite3 *db = NULL;
	int rc, retval = 0;
	char *sql_query = NULL, *err_msg = NULL;
	
	db = bet_get_db_instance();
	sql_query = calloc(sql_query_size, sizeof(char));
	sprintf(sql_query,"select key_id from keys");
	rc = sqlite3_prepare_v2(db, sql_query, -1, &stmt, NULL);
	if (rc != SQLITE_OK) {
		printf("error_code :: %d, error msg ::%s, \n query ::%s", rc, sqlite3_errmsg(db), sql_query);
		goto end;
	}
	while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
		if(strcmp(key_id, sqlite3_column_text(stmt, 0)) == 0){
			retval = 1;
			break;
		}
	}
	end:
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		if (sql_query)
			free(sql_query);
	return retval;	
}

int check_if_data_exists(int data_id)
{
	sqlite3_stmt *stmt = NULL;
	sqlite3 *db = NULL;
	int rc, retval = 0;
	char *sql_query = NULL, *err_msg = NULL;
	
	db = bet_get_db_instance();
	sql_query = calloc(sql_query_size, sizeof(char));
	sprintf(sql_query,"select data_id from data_store");
	rc = sqlite3_prepare_v2(db, sql_query, -1, &stmt, NULL);
	if (rc != SQLITE_OK) {
		printf("error_code :: %d, error msg ::%s, \n query ::%s", rc, sqlite3_errmsg(db), sql_query);
		goto end;
	}
	while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
		if(data_id == sqlite3_column_int(stmt, 0)){
			retval = 1;
			break;
		}
	}
	end:
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		if (sql_query)
			free(sql_query);
	return retval;	
}

