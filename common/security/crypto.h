#ifndef _CRYPTO_H_
#define _CRYPTO_H_

#include <iostream>
using std::string ;
class Crypto
{
public:


	string rsa_pub_encrypt(const string &clearText,  
			const string &pubKeyPath="rsa_public.key") ;
	string rsa_pri_decrypt(const string &cipherText, 
			const string &priKeyPath = "./rsa_private.key");  
	string aes_decryptwithpkcs5padding(const string &cipherStr, 
			const string &keyStr) ;
	string aes_encryptwithpkcs5padding(const string &plainText, 
			const string& keyStr);
private:
	string priKeyPath = "./rsa_private.key" ;
	/* data */
};

#endif 
