#!/bin/sh
g++ -o data_store data_store.c crypto.c storage.c misc.c -g -w -fpermissive -pthread -ldl -I/usr/local/openssl/include -L/usr/local/openssl/lib -lcrypto -lsqlite3

