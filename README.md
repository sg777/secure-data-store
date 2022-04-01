## Secure Data Store
It's a tool to store and retrieve the data securely on the local DB. Basically what this tool does is it allows the user to pick keys from the local key store and encrypt the data using AES-CBC of key length 256 bits.

## Design - The thought process
The main areas of focus in designing the secure data store are as follows:
* User authentication
* Key management & recovery
* Load balancing

### User authentication
The tool doesn't do any authentication of the users, any users can take this tool and run it locally. In production code, ideally user authentication can be done by establishing the secure communication channel with the cloud infrastructure. 

#### Authentication using public key approach
* During the registration process, users generate the public key pair and share the public key(in the form of CSR) with the data store owner.
* Date store validates the user and attests the public key, means it signs the CSR and provides the issues the certificate to the user.
* Later when user wants to communicate with the data store, the user should provide the certificate first and then using any of the challenge-response mechanism the data store server authenticates the user.
 #### Password based authentication
 In which over a secure channel using any of the passoword authentication mechanisms agreed upon, the server authenticates the users.
 
 ### Key Management
This is the main part of secure data store. How the keys are created, used and retired should be well defined here. The best way to keep the keys secure are inside the HSM, the idea is once the key is created it should never be exported outside the HSM box. Managing access to HSM's and autherization of operations on HSM is one more key area which needs lots of attention. I'm skipping the part of how HSM's should be managed securely here.

As we know each key is associated with the key_identifier inside the HSM's and these key identifiers are mapped to the users. So the idea is users can make call to HSM API's to perform the respective crypto operation. In case if the HSM's are not available we can use trustzone to perform the crypto operation securely. 


Here in this tool im using OpenSSL to perform the crypto operations. For the demo purposes I created the keys table and storing the preconfigured keys in it. 
The schema of keys table with some test data looks as follows:
```
sqlite> .schema keys
CREATE TABLE keys (key_id varchar(100) primary key,key varchar(100));
sqlite> select * from keys;
key_id|key
k1|01234567890123456789012345678901
k2|01234567890123456789012345678901
k3|01234567890123456789012345678901
k4|01234567890123456789012345678901
k5|01234567890123456789012345678901
k6|01234567890123456789012345678901
k7|01234567890123456789012345678901
k8|01234567890123456789012345678901
k9|01234567890123456789012345678901
k10|01234567890123456789012345678901
sqlite> 
```
#### Key rotation & expiring the keys
Each keys that we generated must come up with a shelf time. Incase of worst case scenario where in which if any information of keys get leaked, the system should be resilient against it. The common practise is creating the shelf time of the key and also keep track of all the applications and operations for which a specific key get used so that it would be easy to take a decision in the case of any adversaties. 


### Load balancing
There can be many bottlenecks that exist specially based on how we manage the keys across the apps. So its important to study the traffic on the apps and if we use HSM's then the throughput and performance of them matters. It's important to make a provision to operate the system in a distributed environment for better load balancing.


### Implementation
The entire code is written in C. The implementation in this repo is not fully developed its rather i say its the demo implementation of an abstract thought of how a data store can be designed. 
#### libs used
* **openssl** for encrypt/decrypt
* **sqlite3** to maintain local DB
#### Steps to compile
Simply run `./run.sh` or following command
```
g++ -o data_store data_store.c crypto.c storage.c misc.c -g -w -fpermissive -pthread -ldl -I/usr/local/openssl/include -L/usr/local/openssl/lib -lcrypto -lsqlite3
```

### API's exposed
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
