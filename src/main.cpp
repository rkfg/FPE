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

int main(int argc, char* argv[]) {
	if (argc < 3) {
		std::cerr << "Usage: <-e|-d> text" << std::endl;
		return 1;
	}
	OPENSSL_init();
	OpenSSL_add_all_ciphers();
	AES aes("Test passwd Test", "1234567812345678");
	LimitedAES<Size> lim(aes, 95);
	std::vector<Size> dataVec;
	for (size_t i = 0; i < strlen(argv[2]); ++i) {
		dataVec.push_back(argv[2][i] - ' ');
	}
	std::cout << "Source: " << dataVec << " / " << dataVec.size() << std::endl;
	if (!strcmp(argv[1], "-e")) {
		auto enc = lim.encrypt(dataVec);
		std::cout << "Encrypted: " << enc << std::endl;
	}
	if (!strcmp(argv[1], "-d")) {
		auto dec = lim.decrypt(dataVec);
		std::cout << "Decrypted: " << dec << std::endl;
	}
	EVP_cleanup();
	return 0;
}
