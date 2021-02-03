#include <iostream>
#include <stdio.h>
#include <string.h>
#include <openssl/aes.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <stdlib.h>

#include "crypto.h"

using namespace std;

// 公钥加密    
// RSA_PKCS1_PADDING 必须占用11Byte，公钥长度为256Byte, 每次加密的
// 明文长度最大为  256 - 11 = 245 Byte 
#define PKCS1_PLEN 11

static string longTextRsaEncrypt(const string &clearText, 
		const string &pubKeyPath){
	RSA* rsa ;
	FILE *filePtr ;
	filePtr = fopen(pubKeyPath.data(),"r") ;
	rsa = PEM_read_RSA_PUBKEY(filePtr, NULL, NULL, NULL);
	fclose(filePtr) ;
	if (!rsa) { return std::string(""); }

	int rsaKeyLen = RSA_size(rsa);  
	int blockLen = rsaKeyLen - PKCS1_PLEN ;

	int encrypTextLen = (clearText.size()/blockLen) * rsaKeyLen ;
	if(clearText.size() % blockLen) encrypTextLen += rsaKeyLen ;

	char *encryptedText = (char *)malloc(encrypTextLen);
	int curEncrypTextLen = 0 ;
	memset(encryptedText, 0 , encrypTextLen ) ;
	for(int i = 0 ;i < clearText.size(); i += blockLen) {
		int sliceLen = blockLen ;
		if(clearText.size() - i < blockLen) sliceLen = clearText.size() - i ;
		int ret = RSA_public_encrypt( sliceLen, 
				(const unsigned char*)clearText.c_str()+i, 
				(unsigned char*)(encryptedText + curEncrypTextLen),
				rsa, RSA_PKCS1_PADDING);  
		if(ret <= 0){ break ; }		
		curEncrypTextLen += ret ;
	}

	string strRet(encryptedText, curEncrypTextLen) ;
	// 释放内存  
	free(encryptedText);  
	RSA_free(rsa);

	return strRet;  
}
string  Crypto::rsa_pub_encrypt(const string &clearText,
		const string &pubKeyPath)  
{  
	return longTextRsaEncrypt(clearText, pubKeyPath) ;
}

static string longTextRsaDecrypt(const string &cipherText, 
		const string &priKeyPath){

	RSA *rsa ;  
	FILE *filePtr ;
	filePtr = fopen(priKeyPath.data(),"r") ;
	rsa = PEM_read_RSAPrivateKey(filePtr, NULL, NULL, NULL);
	fclose(filePtr) ;

	if (!rsa) { return std::string(""); }

	int rsaKeyLen = RSA_size(rsa);  
	int blockLen = rsaKeyLen - PKCS1_PLEN ;
	char *decryptedText = (char *)malloc(cipherText.size());
	int curDecrypTextLen = 0 ;
	memset(decryptedText,0,cipherText.size()) ;

	for (int i = 0; i < cipherText.size(); i+= rsaKeyLen) {
		int ret = RSA_private_decrypt(rsaKeyLen, 
				(const unsigned char*)cipherText.c_str()+i, 
				(unsigned char*)(decryptedText+curDecrypTextLen),
				rsa, RSA_PKCS1_PADDING);  
		if(ret <= 0){ break ; }		
		curDecrypTextLen += ret ;
	}

	string strRet(decryptedText, curDecrypTextLen) ;
	free(decryptedText);  
	RSA_free(rsa);  

	return strRet ;
}
// 私钥解密    
string Crypto::rsa_pri_decrypt(const string &cipherText, 
		const string &priKeyPath)  
{  
	return longTextRsaDecrypt(cipherText,priKeyPath) ;
}


string Crypto::aes_encryptwithpkcs5padding(const string &plainText, 
		const string& keyStr){
	AES_KEY aes ;
	unsigned char key[AES_BLOCK_SIZE] ;		// AES_BLOCK_SIZE = 16Byte
	//unsigned char iv[AES_BLOCK_SIZE];
	int encrypBufLen ;
	if((plainText.size()) % AES_BLOCK_SIZE == 0){
		encrypBufLen = plainText.size() + AES_BLOCK_SIZE ;
	}else{
		encrypBufLen = (plainText.size()/AES_BLOCK_SIZE+1)*AES_BLOCK_SIZE;
	}
	char paddingCode = encrypBufLen - plainText.size() ;

	char *inputBuf = (char*)calloc(encrypBufLen, sizeof(char)) ;
	memcpy(inputBuf, plainText.data(), plainText.size()) ;
	// PKCS7 padding 
	memset(inputBuf+encrypBufLen-paddingCode, paddingCode, paddingCode) ;

	memcpy(key, keyStr.data(), AES_BLOCK_SIZE) ;
	//memset(iv,0,AES_BLOCK_SIZE) ;

	//int encrypRet = AES_set_encrypt_key(key, 128, &aes) ;
	AES_set_encrypt_key(key, 128, &aes) ;
	// 128是key的bit数
	char *cipherBuf = (char*)calloc(encrypBufLen, sizeof(char)) ;
	for (int i = 0; i < encrypBufLen; i+=16) {
		AES_encrypt((unsigned char*)inputBuf+i,(unsigned char*)cipherBuf+i,&aes);
	}
	string cipherStr(cipherBuf,encrypBufLen) ;
	free(inputBuf) ;
	free(cipherBuf) ;
	return cipherStr ;
}

string Crypto::aes_decryptwithpkcs5padding(const string &cipherStr, 
		const string &keyStr){
	AES_KEY aes ;
	unsigned char key[AES_BLOCK_SIZE] ;		// AES_BLOCK_SIZE = 16Byte
	//unsigned char iv[AES_BLOCK_SIZE];
	memcpy(key, keyStr.data(), AES_BLOCK_SIZE) ;
	//memset(iv,0,AES_BLOCK_SIZE) ;
	//int encrypRet = AES_set_encrypt_key(key, 128, &aes) ;
	AES_set_encrypt_key(key, 128, &aes) ;
	int encrypBufLen = cipherStr.size() ;
	char *outputBuf = (char*)calloc(encrypBufLen, sizeof(char)) ;
	AES_set_decrypt_key(key, 128, &aes) ;
	for (int i = 0; i < encrypBufLen; i+=16) {
		AES_decrypt((unsigned char*)cipherStr.data()+i, 
				(unsigned char*)outputBuf+i, &aes) ;
	}
	char paddingC = outputBuf[encrypBufLen-1] ;
	string outputStr(outputBuf, encrypBufLen-paddingC) ;
	free(outputBuf) ;
	return outputStr ;
}
