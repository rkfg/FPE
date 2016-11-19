//============================================================================
// Name        : aes.cpp
// Author      : rkfg
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <openssl/crypto.h>
#include <openssl/evp.h>
#include "AES.h"
#include "LimitedAES.h"

typedef unsigned char Size;

std::ostream& operator<<(std::ostream& os, const std::vector<Size>& data) {
	for (auto c : data) {
		os << (char) (c + ' ');
	}
	return os;
}

int main() {
	OPENSSL_init();
	OpenSSL_add_all_ciphers();
	AES aes("Test passwd", "12345678");
	LimitedAES<Size> lim(aes, 95);
	const char data[] = "This is an experimental text";
	std::vector<Size> dataVec;
	for (size_t i = 0; i < sizeof(data) - 1; ++i) {
		dataVec.push_back(data[i] - ' ');
	}
	std::cout << "Source: " << dataVec << " / " << dataVec.size() << std::endl;
	auto enc = lim.encrypt(dataVec);
	std::cout << "Encrypted: " << enc << std::endl;
	auto dec = lim.decrypt(enc);
	std::cout << "Decrypted: " << dec << std::endl;
	EVP_cleanup();
	return 0;
}
