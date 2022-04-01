### Secure Data Store
A tool to store and retrieve the data securely on the local DB. The follows API's of the tool are exposed using which users can store and retrive the data securely.

#### save
```
Description: Encrypts the data provided with the key identifier accessable to the server and stores in the local DB
Usage
./data_store save data key_id
```
#### retrieve
```
Description: Retrives the encrypted data with the data identifier provided
Usage
./data_store data_id
```
#### list_keys
```
Description: Lists all the keys available in the server for the user
Usage
./data_store list_keys
```
#### list_data
```
Description: Lists all the encrypted data sets identifiers available in the server
Usage
./data_store list_data
```

### Examples
#### scenario1
Here user encrypts the data **this is test data** with the with the corresponding key identifier of **k1** and stores it in the local DB.
```
Command:
./data_store save "this is test data" k1
Console log:
cipher
ab 0d 52 23 4f 9c 9b 05 e3 3d 28 55 cd 4b a8 73 6b b7 1e 96 52 43 94 a7 7d 9a 2e 95 46 1b 53 25
```
#### scenario2
Each encrypted data is associated with the data id and the DB has the mapping of the key used to encrypt that data, so when the can simply retrieve the data by mentioned the data_id. Here in this example user running the command to retrieve the data with the data_id **1**.
```
 Command:
./data_store retrieve 1
Console log:
Decrypted text is:
this is test data 
 ```
