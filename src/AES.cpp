/*
 * AES.cpp
 *
 *  Created on: 19 Nov 2016
 *      Author: rkfg
 */

#include "AES.h"
#include <iostream>

using namespace std;

AES::AES(const string& key, const string& iv): key_(key), iv_(iv) {
	cipher_ = EVP_get_cipherbyname("aes-128-cbc");
}

vector<unsigned char> AES::encrypt(const std::vector<unsigned char>& data) {
	auto ctx = EVP_CIPHER_CTX_new();
	EVP_EncryptInit(ctx, cipher_, (const unsigned char*)key_.c_str(), (const unsigned char*)iv_.c_str());
	size_t len = data.size();
	unsigned char out[(len / 16  + 1) * 16];
	int outl;
	EVP_EncryptUpdate(ctx, out, &outl, (const unsigned char*)data.data(), len);
	vector<unsigned char> result(out, out + outl);
	EVP_EncryptFinal(ctx, out, &outl);
	result.insert(result.end(), out, out + outl);
	EVP_CIPHER_CTX_free(ctx);
	return result;
}

std::vector<unsigned char> AES::decrypt(const std::vector<unsigned char>& data) {
	auto ctx = EVP_CIPHER_CTX_new();
	EVP_DecryptInit(ctx, cipher_, (const unsigned char*)key_.c_str(), (const unsigned char*)iv_.c_str());
	size_t len = data.size();
	unsigned char out[(len / 16  + 1) * 16];
	int outl;
	EVP_DecryptUpdate(ctx, out, &outl, (const unsigned char*)data.data(), len);
	vector<unsigned char> result(out, out + outl);
	EVP_DecryptFinal(ctx, out, &outl);
	result.insert(result.end(), out, out + outl);
	EVP_CIPHER_CTX_free(ctx);
	return result;
}
