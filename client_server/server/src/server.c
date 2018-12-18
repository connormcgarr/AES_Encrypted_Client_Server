#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <string.h>
#include "../hdr/server.h"
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
  if(!(ctx = EVP_CIPHER_CTX_new())) 
{
	handleErrors();
}
  // Initialise the encryption operation. IMPORTANT - ensure you use a key
  // and IV size appropriate for your cipher
  // In this example we are using 256 bit AES (i.e. a 256 bit key). The
  // IV size for *most* modes is the same as the block size. For AES this
  //is 128 bits
  if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
  {
	  handleErrors();
  }
  //Provide the message to be encrypted, and obtain the encrypted output.
  //EVP_EncryptUpdate can be called multiple times if necessary
  if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
  {
	handleErrors();
  }
    ciphertext_len = len;

  //Finalise the encryption. Further ciphertext bytes may be written at
  //this stage.
  if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
 {   
	handleErrors();
 }
  ciphertext_len += len;

  //Clean up
  free(ctx);
  return ciphertext_len;

}

static int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
  unsigned char *iv, unsigned char *plaintext)
{
  EVP_CIPHER_CTX *ctx;
  int len;
  int plaintext_len;
  //Create and initialise the context
  if(!(ctx = EVP_CIPHER_CTX_new())) handleErrors();
  //Initialise the decryption operation. IMPORTANT - ensure you use a key
  //and IV size appropriate for your cipher
  //In this example we are using 256 bit AES (i.e. a 256 bit key). The
  //IV size for *most* modes is the same as the block size. For AES this
  // is 128 bits 
  if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
  {
	handleErrors();
  }
  //Provide the message to be decrypted, and obtain the plaintext output.
  //EVP_DecryptUpdate can be called multiple times if necessary
  if(1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
{
	handleErrors();
	plaintext_len = len;
}
  //Finalise the decryption. Further plaintext bytes may be written at
  //this stage.
  if(1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) handleErrors();
  plaintext_len += len;

  //Clean up
  free(ctx);
  return plaintext_len;
}
static int getStrLength(char *string_is_a_reserved_word)

{
	int i = 0; 
	while(string_is_a_reserved_word[i] != 0x00)
	{
		i++;
	}
	return i;
} 
static void search(char *targetstring, int client)
{
	int index=0;
	//Implement calloc
	char *return_str = calloc (128, sizeof(char));
	char z;
	int fileindex=1;
	//256 key calling from above AES.
	unsigned char *key = (unsigned char *)"01234567890123456789012345678901";
        //128 bit calling from abovr AES.
	unsigned char *iv = (unsigned char *)"0123456789012345";
	//Implement calloc
	unsigned char *ciphertext = calloc(128,sizeof(char));

	int lengthTarget = getStrLength(targetstring);
	printf(" Target String %s \n", targetstring);
	//Decaling FILE *inFilePtr for pattern matching.
	FILE *inFilePtr;
	printf("Opening %s\n", "license.txt");
	if((inFilePtr = fopen("license.txt", "r")) != NULL)
	{
		printf("Looking...\n");
		while((z = fgetc(inFilePtr)) != EOF)
		{
			
			if((char)z == targetstring[index]) 
			{
				index++;
			}
			else if( z == targetstring[0])
			{
				index = 1;
			}
			else 
			{
				index = 0;
			}
			if(index == lengthTarget)
			{
				(void)snprintf((char *)return_str, 128,"%s indexed! %d\n",targetstring, (fileindex-lengthTarget));
				printf("%s indexed! %d\n",targetstring, (fileindex-lengthTarget));
				//Implement strnlen				
				(void)encrypt ((unsigned char *)return_str, strnlen((char *)return_str, 128), key, iv, ciphertext);
				//Keep BIO_dump for debugging and making program look cool :)
				(void)BIO_dump_fp (stdout, (const char *)ciphertext, 48);
				//Sending the encrypted string.
				(void)send(client,ciphertext, 128, 0);
				//Implement sleep
				(void)sleep(1);
				index = 0;
				memset(return_str, 0x00, sizeof(*return_str));
				
			}
			fileindex++;
		}
		//Implement snprintf
		(void)snprintf(return_str, 127, "END OF FILE!!!!!!!!!!!!!");
		(void)encrypt ((unsigned char *)return_str, strnlen((char *)return_str, 128), key, iv, ciphertext);
				(void)BIO_dump_fp (stdout, (const char *)ciphertext, 48);
				(void)write(client,ciphertext, 128);
		
	}
	else
	{
		printf("File failed to open");
	}
	free (return_str);
	free (ciphertext);
}
static void socket_function()
{
  //256 Key. Calling from encrypt/decrypt
  unsigned char *key = (unsigned char *)"01234567890123456789012345678901";
  //128 bit. Calling from encrypt/decrypt
  unsigned char *iv = (unsigned char *)"0123456789012345";
  //Buffer for ciphertext. Ensure the buffer is long enough for the
  //ciphertext which may be longer than the plaintext, dependant on the
  //algorithm and mode
  //Implement calloc
  unsigned char *ciphertext = calloc(128,sizeof(char));
  //Buffer for the decrypted text 
  unsigned char *decryptedtext = calloc(128, sizeof(char));
  int ciphertext_len;
  //Declaring variables for the searching function
  unsigned char *targetstring = calloc(128, sizeof(char));

  printf("Ciphertext is:\n");

  //Show the decrypted text 
  printf("Decrypted text is:\n");
  printf("%s\n", (char *)decryptedtext);
   
    //Implement calloc   
    char *data_packet = calloc (128, sizeof(char));
    int incoming_client = 0, client = 0;
    int client_count = 0;
    struct sockaddr_in server_ip;
    //Declare socket
    incoming_client = socket(AF_INET, SOCK_STREAM, 0); 
    memset(&server_ip, 0x00, sizeof(server_ip));
    memset(data_packet, 0x00, 128);
    server_ip.sin_family = AF_INET;
    server_ip.sin_addr.s_addr = htonl(INADDR_ANY);
    server_ip.sin_port = htons(8888);
    (void)bind(incoming_client, (struct sockaddr*)&server_ip, sizeof(server_ip));
    (void)listen(incoming_client, 20);

    while(1)
    {
        printf("\n\nServer Listening for Client %d\n", client_count);
        client = accept(incoming_client, (struct sockaddr*)NULL, NULL);
        (void)read(client, ciphertext, 128);
	printf("Initializing Client...\n");
	ciphertext_len = decrypt(ciphertext, 16, key, iv, targetstring);
	targetstring[ciphertext_len]='\0';
	printf("%s", (char*)targetstring);
	search((char *)targetstring, client);
        (void)close(client);
        client_count++;
        (void)sleep(1);
	}


}

int main()
{
  socket_function();
  return 0;
}
