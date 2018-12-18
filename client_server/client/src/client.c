#include<sys/types.h>
#include<netinet/in.h>
#include<netdb.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<arpa/inet.h>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <string.h>
#include "../hdr/client.h"
static void handleErrors()
{
  ERR_print_errors_fp(stderr);
  abort();
}

static int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
  unsigned char *iv, unsigned char *ciphertext)
{
  EVP_CIPHER_CTX *ctx;
  int len;
  int ciphertext_len;

  //Create and initialise the context
  if(!(ctx = EVP_CIPHER_CTX_new())) handleErrors();
  //Initialise the encryption operation. IMPORTANT - ensure you use a key
  //and IV size appropriate for your cipher
  //In this example we are using 256 bit AES (i.e. a 256 bit key). The
  //IV size for *most* modes is the same as the block size. For AES this
  //is 128 bits

  if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
    handleErrors();

  //Provide the message to be encrypted, and obtain the encrypted output.
  //EVP_EncryptUpdate can be called multiple times if necessary
  if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
    handleErrors();
  ciphertext_len = len;

  //Finalise the encryption. Further ciphertext bytes may be written at
  //this stage.
  if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len)) handleErrors();
  ciphertext_len += len;

  //Clean
  free(ctx);
  return ciphertext_len;
}

static int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
  unsigned char *iv, unsigned char *plaintext)
{
  EVP_CIPHER_CTX *ctx;

  int len, plaintext_len;

  //Create and initialise the context
  if(!(ctx = EVP_CIPHER_CTX_new())) handleErrors();

  ///Initialise the decryption operation. IMPORTANT - ensure you use a key
  //and IV size appropriate for your cipher
  //In this example we are using 256 bit AES (i.e. a 256 bit key). The
  //IV size for *most* modes is the same as the block size. For AES this
  //is 128 bits
  if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
    handleErrors();

  //Provide the message to be decrypted, and obtain the plaintext output.
  //EVP_DecryptUpdate can be called multiple times if necessary
  if(1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
    handleErrors();
  plaintext_len = len;

  //Finalize the decryption. Further plaintext bytes may be written at
  //this stage.
  if(1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) handleErrors();
  plaintext_len += len;

  //Clean up
  free(ctx);
  return plaintext_len;
}

static int client_function(int argc, char*argv[])
{
//Error for more than 15 characters. Implement strnlen
  if( (argc != 2) || ((size_t)strnlen(argv[1], 128) > 15))
  {
	printf("USAGE: ./build/file <STRING_TO_SEARCH>\n\nYou cannot enter more than 15 characters to search for security reasons.\n");
	exit(0);
  }

  //256 Key
  unsigned char *key = (unsigned char *)"01234567890123456789012345678901";

  //128 bit
  unsigned char *iv = (unsigned char *)"0123456789012345";

  //Message to be encrypted and implement calloc
  unsigned char *plaintext = calloc (128, sizeof(char));
  strncpy((char *)plaintext, *(argv+1), 128);

  //Buffer for ciphertext. Ensure the buffer is long enough for the
  //ciphertext which may be longer than the plaintext, dependant on the
  //algorithm and mode. 

  //Implementing calloc.
  unsigned char *ciphertext = calloc (128, sizeof(char));

  //Buffer for the decrypted text. Implenting calloc.
  unsigned char *decryptedtext = calloc (128, sizeof(char));

  int decryptedtext_len, ciphertext_len;

  //Encrypt the plaintext
  ciphertext_len = encrypt (plaintext, strnlen((char *)plaintext, 127), key, iv, ciphertext);

  //Print the ciphertext here
  printf("Ciphertext is:\n");
  (void)BIO_dump_fp (stdout, (const char *)ciphertext, ciphertext_len);

  //Decrypt the ciphertext
  decryptedtext_len = decrypt(ciphertext, ciphertext_len, key, iv,
    decryptedtext);

  //Add a NULL terminator. We are expecting printable text 
  decryptedtext[decryptedtext_len] = '\0';

  //Show the decrypted text
  printf("Decrypted text is:\n");
  printf("%s\n", (char*)decryptedtext);

	int client_socket=0,n=0;
	char *data_received = calloc (128, sizeof(char));
	memset(data_received, 0x00, sizeof(*data_received));
	struct sockaddr_in server_ip;	
	if((client_socket=socket(AF_INET, SOCK_STREAM, 0)) <0)
		{
		printf("Socket not created\n");
		free(plaintext);
		free(ciphertext);
		free(decryptedtext);
		free(data_received);
		return 1;
		}

	server_ip.sin_family=AF_INET;
	server_ip.sin_port=htons(8888);
	server_ip.sin_addr.s_addr=inet_addr("127.0.0.1");

	if(connect(client_socket, (struct sockaddr *)&server_ip, (socklen_t)sizeof(server_ip))<0)
	{
		printf("Connection failed. Check port and/or IP addressing.\n");
	free(plaintext);
	free(ciphertext);
	free(decryptedtext);
	free(data_received);
	return 1;
	}

	(void)write(client_socket, ciphertext, (size_t)strnlen((char *)ciphertext, 128));
	while((n=(int)read(client_socket, data_received, 128))>0)
	{

		ciphertext_len = decrypt((unsigned char *)data_received, 32, key, iv, plaintext);
		plaintext[ciphertext_len] = '\0';
		printf("%s", (char *)plaintext);
		printf("\n");
		memset(data_received, 0x00, sizeof(*data_received));
	}
		free(plaintext);
		free(ciphertext);
		free(decryptedtext);
		free(data_received);

	if(n<0)
	{
		printf("Standard input error\n");
	}
	return 0;
}

#ifndef TEST
int main(int argc, char *argv[])
{
	(void)client_function(argc, argv);
	return 0;
}
#endif
