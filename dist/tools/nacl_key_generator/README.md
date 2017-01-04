# Key-pair generator using [NaCL](https://nacl.cr.yp.to/)
This program will generate two key-pairs and store them as binary files, using
tweetNaCl package in RIOT

## Usage
To use, you should call `generate-nacl-keys` without any arguments

The results will be printed if the operation is successful, and four binary
files called "server_skey", "server_pkey.pub", "firmware_skey" and 
"firmware_pkey.pub" will be created.

Additionally, a "keys.c" file will be also generated containing "server_pkey"
and "firmware_skey" as unsigned char arrays.
